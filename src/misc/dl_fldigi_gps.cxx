// TODO: Windoze it up
#ifndef __MINGW32__

#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "configuration.h"
#include "util.h"
#include "fl_digi.h"
#include "dl_fldigi.h"
#include "dl_fldigi_gps.h"
#include "util.h"
#include "confdialog.h"
#include "fl_digi.h"
#include "main.h"
#include "threads.h"
#include "qrunner.h"

/* Is the pointer volatile, or the stuff it points to volatile? */
static volatile char *serial_port, *serial_identity;
static volatile int serial_baud;
static volatile int serial_updated;
static volatile int serial_ready;
static pthread_t serial_thread_id;
static pthread_mutex_t serial_ready_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  serial_ready_cond  = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t serial_info_mutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  serial_info_cond   = PTHREAD_COND_INITIALIZER;

static void *serial_thread(void *a);
static void dl_fldigi_gps_post(float lat, float lon, int alt, char *identity);
static FILE *dl_fldigi_open_serial_port(const char *port, int baud);

struct gps_data
{
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
	float lat;
	unsigned int lat_d;
	float lat_m;
	char lat_ns;
	float lon;
	unsigned int lon_d;
	float lon_m;
	char lon_we;
	unsigned int sats;
	unsigned int alt;
};

void dl_fldigi_gps_init()
{
	#ifdef DL_FLDIGI_DEBUG
		fprintf(stderr, "dl_fldigi: dl_fldigi_gps init() creating thread\n");
	#endif

	pthread_mutex_lock(&serial_ready_mutex);

	if (pthread_create(&serial_thread_id, NULL, serial_thread, NULL) != 0)
	{
		perror("dl_fldigi: ext_gps pthread_create");
		exit(EXIT_FAILURE);
	}

	#ifdef DL_FLDIGI_DEBUG
		fprintf(stderr, "dl_fldigi: dl_fldigi_gps init created thread, waiting for it to become ready\n");
	#endif

	while (!serial_ready)
		pthread_cond_wait(&serial_ready_cond, &serial_ready_mutex);

	pthread_mutex_unlock(&serial_ready_mutex);

	#ifdef DL_FLDIGI_DEBUG
		fprintf(stderr, "dl_fldigi: dl_fldigi_gps init thread creation done, setting default settings\n");
	#endif

	dl_fldigi_gps_update_ports(0, 0);
	dl_fldigi_gps_setup_fromprogdefaults();
}

void dl_fldigi_gps_update_ports(int rescan, int update)
{
	const Fl_Menu_Item *item;

	#ifdef DL_FLDIGI_DEBUG
		fprintf(stderr, "dl_fldigi: dl_fldigi_gps update ports: rescan = %i\n", rescan);
	#endif

	if (rescan)
		progdefaults.testCommPorts();

	item = inpGPSdev->find_item(progdefaults.gpsDevice.c_str());

	#ifdef DL_FLDIGI_DEBUG
		fprintf(stderr, "dl_fldigi: dl_fldigi_gps update ports: item = %p\n", item);
	#endif

	if (item != NULL)
	{
		inpGPSdev->value(item);
	}

	inpGPSdev->redraw();
}

void dl_fldigi_gps_setup_fromprogdefaults()
{
        dl_fldigi_gps_setup(progdefaults.gpsDevice.c_str(),
	                    progdefaults.gpsSpeed,
                            progdefaults.gpsIdentity.c_str());
}

void dl_fldigi_gps_setup(const char *port, int baud, const char *identity)
{
	#ifdef DL_FLDIGI_DEBUG
		fprintf(stderr, "dl_fldigi: dl_fldigi_gps setup begin\n");
	#endif

	pthread_mutex_lock(&serial_info_mutex);

	serial_port = (port != NULL ? strdup(port) : NULL);
	serial_baud = baud;
	serial_identity = (identity != NULL ? strdup(identity) : NULL);
	serial_updated = 1;

	full_memory_barrier();

	pthread_kill(serial_thread_id, SIGUSR2);
	pthread_cond_signal(&serial_info_cond);  /* Not really required since the signal will have caused a spurious wakeup */

	pthread_mutex_unlock(&serial_info_mutex);

	#ifdef DL_FLDIGI_DEBUG
		fprintf(stderr, "dl_fldigi: dl_fldigi_gps setup done, sent signal & cond_signal\n");
	#endif
}

static void *serial_thread(void *a)
{
	char *port, *identity;
	int baud;
	FILE *f;
	struct gps_data fix;
	int i, c;
	time_t last_post;
	struct timespec abstime;
	time_t retry_time;
	sigset_t usr2;

	sigemptyset(&usr2);
	sigaddset(&usr2, SIGUSR2);
	pthread_sigmask(SIG_UNBLOCK, &usr2, NULL);

	memset(&abstime, 0, sizeof(abstime));
	retry_time = 0;
	port = NULL;
	identity = NULL;
	baud = 0;

	#ifdef DL_FLDIGI_DEBUG
		fprintf(stderr, "dl_fldigi: dl_fldigi_gps thread: start\n");
	#endif

	pthread_mutex_lock(&serial_ready_mutex);
	serial_ready = 1;
	full_memory_barrier();
	pthread_cond_signal(&serial_ready_cond);
	pthread_mutex_unlock(&serial_ready_mutex);

	#ifdef DL_FLDIGI_DEBUG
		fprintf(stderr, "dl_fldigi: dl_fldigi_gps thread: signaled ready\n");
	#endif

	for (;;)
	{
		pthread_mutex_lock(&serial_info_mutex);

		if (retry_time == 0)
		{
			#ifdef DL_FLDIGI_DEBUG
				fprintf(stderr, "dl_fldigi: dl_fldigi_gps thread: sleeping on serial_updated\n");
			#endif

			while (!serial_updated)
				pthread_cond_wait(&serial_info_cond, &serial_info_mutex);
		}
		else
		{
			#ifdef DL_FLDIGI_DEBUG
				fprintf(stderr, "dl_fldigi: dl_fldigi_gps thread: sleeping on serial_updated with a timeout in %is\n", (int) (retry_time - time(NULL)));
			#endif

			abstime.tv_sec = retry_time;

			while (!serial_updated && (time(NULL) < retry_time))
				pthread_cond_timedwait(&serial_info_cond, &serial_info_mutex, &abstime);
		}

		#ifdef DL_FLDIGI_DEBUG
			fprintf(stderr, "dl_fldigi: dl_fldigi_gps thread: woke up\n");
		#endif

		if (serial_updated)
		{
			#ifdef DL_FLDIGI_DEBUG
				fprintf(stderr, "dl_fldigi: dl_fldigi_gps thread: updating thread's serial config\n");
			#endif

			if (port != NULL)     free(port);
			if (identity != NULL) free(identity);

			port = (char *) serial_port;
			baud = (int) serial_baud;
			identity = (char *) serial_identity;

			serial_updated = 0;
		}

		pthread_mutex_unlock(&serial_info_mutex);

		if (port == NULL || port[0] == '\0' || baud == 0)
		{
			#ifdef DL_FLDIGI_DEBUG
				fprintf(stderr, "dl_fldigi: dl_fldigi_gps thread: empty config, restarting\n");
			#endif

			retry_time = 0;
			continue;
		}

		f = dl_fldigi_open_serial_port(port, baud);
	
		if (f == NULL)
		{
			#ifdef DL_FLDIGI_DEBUG
				fprintf(stderr, "dl_fldigi: dl_fldigi_gps thread: restarting\n");
			#endif

			retry_time = time(NULL) + 10;
			continue;
		}

		last_post = 0;

		#ifdef DL_FLDIGI_DEBUG
			fprintf(stderr, "dl_fldigi: dl_fldigi_gps thread: beginning processing loop\n");
		#endif

		for (;;)
		{
			do
			{
				c = fgetc(f);
			}
			while (c != '$' && c != EOF);

			if (c == EOF)
			{
				fprintf(stderr, "dl-fldigi: fgetc returned EOF; feof() == %i, ferror() == %i\n", feof(f), ferror(f));
				break;
			}

			/* $GPGGA,123519.00,4807.0381,N,00056.0002,W,1,08,0.29,00545,M,046,M,,*76 */
			i = fscanf(f, "GPGGA,%2u%2u%2u%*[^,],%2u%f,%c,%3u%f,%c,%*u,%u,%*f,%u,M,%*u,%*c,,*%*2x\n",
				   &fix.hour, &fix.minute, &fix.second,
				   &fix.lat_d, &fix.lat_m, &fix.lat_ns,
				   &fix.lon_d, &fix.lon_m, &fix.lon_we,
				   &fix.sats, &fix.alt);

			if (i == 11)
			{
				fix.lat = fix.lat_d + (fix.lat_m / 60);
				fix.lon = fix.lon_d + (fix.lon_m / 60);

				if (fix.lat_ns == 'S')  fix.lat = -fix.lat;
				else if (fix.lat_ns != 'N')  continue;

				if (fix.lon_we == 'W')  fix.lon = -fix.lon;
				else if (fix.lon_we != 'E')  continue;

				if (time(NULL) - last_post > 5)
				{
					last_post = time(NULL);
					dl_fldigi_gps_post(fix.lat, fix.lon, fix.alt, identity);
				}
			}
			else if (i == EOF)
			{
				perror("dl-fldigi: fscanf gps");
				break;
			}
		}

		fclose(f);

		retry_time = time(NULL) + 10;

		#ifdef DL_FLDIGI_DEBUG
			fprintf(stderr, "dl_fldigi: dl_fldigi_gps thread: restart\n");
		#endif
	}

	return NULL;
}

static void dl_fldigi_gps_post(float lat, float lon, int alt, char *identity)
{
	char rx_chase [200];

	snprintf(rx_chase, sizeof(rx_chase), "ZC,%s,%6f,%6f,%d",
		 (identity ? identity : "UNKNOWN"),
		 lat, lon, alt);

	#ifdef DL_FLDIGI_DEBUG
		fprintf(stderr, "dl_fldigi: dl_fldigi_gps post string '%s'\n", rx_chase);
	#endif

	/* XXX is progdefaults thread safe? */
	dl_fldigi_post(rx_chase, progdefaults.myCall.empty() ? "UNKNOWN" : progdefaults.myCall.c_str());
}

/**
* Open the serial port and check it succeeded.
* The named port is opened at the given baud rate using 8N1
* with no hardware flow control.
* \param port The serial port to use, e.g. COM8 or /dev/ttyUSB0
* \param baud The baud rate to use, e.g. 9600 or 4800. On linux this
* is restricted to a specific range of common values.
*/
static FILE *dl_fldigi_open_serial_port(const char *port, int baud)
{
	#ifdef DL_FLDIGI_DEBUG
		fprintf(stderr, "dl_fldigi: Attempting to open serial port '%s' at %i baud.\n", port, baud);
	#endif

	//Open the serial port
	int serial_port = open(port, O_RDONLY | O_NOCTTY | O_NDELAY);
	if( serial_port == -1 ) {
		fprintf(stderr, "dl_fldigi: Error opening serial port.\n");
		return NULL;
	}

	FILE *f = fdopen(serial_port, "r");
	if (f == NULL)
	{
		fprintf(stderr, "dl_fldigi: Error fdopening serial port as a FILE\n");
		close(serial_port);
		return NULL;
	}

	//Initialise the port
	int serial_port_set = fcntl(serial_port, F_SETFL, 0);
	if( serial_port_set == -1 ) {
		fprintf(stderr, "dl_fldigi: Error initialising serial port.\n");
		fclose(f);
		return NULL;
	}

	//Linux requires baudrates be given as a constant
	speed_t baudrate = B4800;
	if( baud == 9600 ) baudrate = B9600;
	else if( baud == 19200 ) baudrate = B19200;
	else if( baud == 38400 ) baudrate = B38400;
	else if( baud == 57600 ) baudrate = B57600;
	else if( baud == 115200 ) baudrate = B115200;
	else if( baud == 230400 ) baudrate = B230400;

	//Set all the weird arcane settings Linux demands (boils down to 8N1)
	struct termios port_settings;
	memset(&port_settings, 0, sizeof(port_settings));

	cfsetispeed(&port_settings, baudrate);
	cfsetospeed(&port_settings, baudrate);

	/* Enable the reciever and set local */
	port_settings.c_cflag |= (CLOCAL | CREAD);

	/* Set 8N1 */
	port_settings.c_cflag &= ~PARENB;
	port_settings.c_cflag &= ~CSTOPB;
	port_settings.c_cflag &= ~CSIZE;
	port_settings.c_cflag |= CS8;

	/* Set raw input */
	port_settings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

	/* Ignore CR NMEA does CR LF at end of each sentence */
	port_settings.c_iflag |= (IGNCR);

	/* Set raw output (mute point as we don't send stuff to gps in here) */
	port_settings.c_oflag &= ~OPOST;

	/* Blocking read until 1 character arrives */
	port_settings.c_cc[VMIN] = 1;

	//Apply settings
	serial_port_set = tcsetattr(serial_port, TCSANOW, &port_settings);
	if( serial_port_set == -1 ) {
		fprintf(stderr, "dl_fldigi: Error configuring serial port.\n");
		fclose(f);
		return NULL;
	}

	#ifdef DL_FLDIGI_DEBUG
		fprintf(stderr, "dl_fldigi: Serial port '%s' opened successfully as %p (%i == %i).\n", port, f, fileno(f), serial_port);
	#endif

	return f;
}

#else /* __MINGW32__ */

void dl_fldigi_gps_init()
{
	fprintf(stderr, "dl_fldigi: Not yet implemented on windows: dl_fldigi_gps_init\n");
}

void dl_fldigi_gps_setup_fromprogdefaults()
{

}

void dl_fldigi_gps_setup(const char *port, int baud, const char *identity)
{
	fprintf(stderr, "dl_fldigi: Not yet implemented on windows: dl_fldigi_gps_setup\n");
}

#endif /* __MINGW32__ */