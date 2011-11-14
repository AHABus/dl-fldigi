/* 
 * Copyright (C) 2011 James Coxon, Daniel Richman, Robert Harrison,
 *                    Philip Heron, Adam Greig, Simrun Basuita
 * License: GNU GPL 3
 */

using namespace std;

namespace dl_fldigi {
namespace gps {

static GPSThread *gps_thread;

void cleanup()
{
    if (gps_thread)
        gps_thread->shutdown();

    while (gps_thread)
        Fl::wait();
}

void configure_gps()
{
    if (gps_thread != NULL)
    {
        gps_thread->shutdown();

        /* We will have to wait for the current thread to shutdown before
         * we can start it up again. thread_death will call this function. */
        return;
    }

    if (gps_thread == NULL &&
        current_location_mode == LOC_GPS &&
        progdefaults.gps_device.size() && progdefaults.gps_speed)
    {
        gps_thread = new GPSThread(progdefaults.gps_device,
                                   progdefaults.gps_speed);
        gps_thread->start();
    }
}

static void gps_thread_death(void *what)
{
    if (what != gps_thread)
    {
        LOG_ERROR("unknown thread");
        return;
    }

    LOG_INFO("cleaning up");

    gps_thread->join();
    delete gps_thread;
    gps_thread = 0;

    if (!shutting_down)
        configure_gps();
}

class GPSThread : public EZ::SimpleThread
{
    const string device;
    const int baud;
    bool term;

#ifdef __MINGW32__
    HANDLE handle;
#endif
    int fd;
    FILE *f;
    int wait_exp;

    void prepare_signals();
    void send_signal();
    bool check_term();
    void set_term();
    void wait();

    void setup();
    void cleanup();
    void log(const string &message);
    void warning(const string &message);

    void read();
    void upload(int h, int m, int s, double lat, double lon, double alt);

public:
    GPSThread(const string &d, int b)
        : device(d), baud(b), term(false),
#ifdef __MINGW32__
          handle(INVALID_HANDLE_VALUE),
#endif
          fd(-1), f(NULL), wait_exp(0) {};
    ~GPSThread() {};
    void *run();
    void shutdown();
};

/* How does online/offline work? if online() is false, uthr->settings() will
 * reset the UploaderThread, leaving it unintialised */

/* On Windows we have to wait for a timeout to wake the GPS thread up
 * to terminate it. This sucks. But on systems that support signals,
 * it will cut short the last timeout */
#ifndef __MINGW32__
void GPSThread::prepare_signals()
{
    sigset_t usr2;

    sigemptyset(&usr2);
    sigaddset(&usr2, SIGUSR2);
    pthread_sigmask(SIG_UNBLOCK, &usr2, NULL);
}

void GPSThread::send_signal()
{
    pthread_kill(thread, SIGUSR2);
}
#else
void GPSThread::prepare_signals() {}
void GPSThread::send_signal()
{
    /* TODO: HABITAT does this work as expected? */
    pthread_cancel(thread);
}
#endif

void GPSThread::wait()
{
    /* On error. Wait for 1, 2, 4... 64 seconds */
    sleep(1 << wait_exp);

    if (wait_exp < 6)
        wait_exp++;
}

void GPSThread::shutdown()
{
    set_term();
    send_signal();
}

void GPSThread::set_term()
{
    EZ::MutexLock lock(mutex);
    term = true;
}

bool GPSThread::check_term()
{
    bool b;
    EZ::MutexLock lock(mutex);
    b = term;
    if (b)
        log("term = true");
    return b;
}

void *GPSThread::run()
{
    prepare_signals();

    while (!check_term())
    {
        try
        {
            setup();
            log("Opened device " + device);
            while (!check_term())
            {
                read();
                /* Success? reset wait */
                wait_exp = 0;
            }
        }
        catch (runtime_error e)
        {
            warning(e.what());
            cleanup();
            wait();
        }
    }

    Fl::awake(thread_death, this);
    return NULL;
}

void GPSThread::warning(const string &message)
{
    Fl_AutoLock lock;
    LOG_WARN("hbtGPS %s", message.c_str());

    string temp = "WARNING GPS Error " + message;
    put_status_safe(temp.c_str(), 10);
    last_warn = time(NULL);
}

void GPSThread::log(const string &message)
{
    Fl_AutoLock lock;
    LOG_DEBUG("hbtGPS %s", message.c_str());
}

static bool check_gpgga(const vector<string> &parts)
{
    if (parts.size() < 7)
        return false;

    if (parts[0] != "$GPGGA")
        return false;

    /* Fix quality field */
    if (parts[6] == "0")
        return false;

    for (int i = 1; i < 7; i++)
        if (!parts[i].size())
            return false;

    return true;
}

static void split_nmea(const string &data, vector<string> &parts)
{
    size_t a = 0, b = 0;

    while (b != string::npos)
    {
        string part;
        b = data.find_first_of(",*", a);

        if (b == string::npos)
            part = data.substr(a);
        else
            part = data.substr(a, b - a);

        parts.push_back(part);
        a = b + 1;
    }
}

static double parse_ddm(string part, const string &dirpart)
{
    double degrees, mins;
    size_t pos = part.find('.');
    if (pos == string::npos || pos < 3)
        throw runtime_error("Bad DDM");

    /* Split degrees and minutes parts */
    part.insert(pos - 2, " ");

    istringstream tmp(part);
    tmp.exceptions(istringstream::failbit | istringstream::badbit);

    tmp >> degrees;
    tmp >> mins;

    double value = degrees + mins / 60;

    if (dirpart == "S" || dirpart == "W")
        return -value;
    else
        return value;
}

static void parse_hms(string part, int &hour, int &minute, int &second)
{
    /* Split HH MM SS with spaces */
    part.insert(6, " ");
    part.insert(4, " ");
    part.insert(2, " ");

    istringstream tmp(part);
    tmp.exceptions(istringstream::failbit | istringstream::badbit);

    tmp >> hour;
    tmp >> minute;
    tmp >> second;
}

static double parse_alt(const string &part, const string &unit_part)
{
    if (unit_part != "M")
        throw runtime_error("altitude units are not M");

    istringstream tmp;
    double value;

    tmp.exceptions(istringstream::failbit | istringstream::badbit);
    tmp.str(part);
    tmp >> value;

    return value;
}

void GPSThread::read()
{
    /* Read until a newline */
    char buf[100];
    char *result;
    result = fgets(buf, sizeof(buf), f);

    if (result != buf)
        throw runtime_error("fgets read no data: EOF or error");

    /* Find the $ (i.e., discard garbage before the $) */
    char *start = strchr(buf, '$');

    if (!start)
        throw runtime_error("Did not find start delimiter");

    string data(start);
    data.erase(data.end() - 1);

    log("Read line: " + data);

    vector<string> parts;
    split_nmea(data, parts);

    if (!check_gpgga(parts))
        return;

    int hour, minute, second;
    double latitude, longitude, altitude;

    try
    {
        parse_hms(parts[1], hour, minute, second);
        latitude = parse_ddm(parts[2], parts[3]);
        longitude = parse_ddm(parts[4], parts[5]);
        altitude = parse_alt(parts[9], parts[10]);
    }
    catch (out_of_range e)
    {
        throw runtime_error("Failed to parse data (oor)");
    }
    catch (istringstream::failure e)
    {
        throw runtime_error("Failed to parse data (fail)");
    }

    upload(hour, minute, second, latitude, longitude, altitude);
}

void GPSThread::upload(int hour, int minute, int second,
                       double latitude, double longitude, double altitude)
{
    Fl_AutoLock lock;

    /* Data OK? upload. */
    if (current_location_mode != LOC_GPS)
        throw runtime_error("GPS mode disabled mid-line");

    listener_valid = true;
    listener_latitude = latitude;
    listener_longitude = longitude;
    update_distance_bearing();

    Json::Value data(Json::objectValue);
    data["time"] = Json::Value(Json::objectValue);
    Json::Value &time = data["time"];
    time["hour"] = hour;
    time["minute"] = minute;
    time["second"] = second;

    data["latitude"] = latitude;
    data["longitude"] = longitude;
    data["altitude"] = altitude;

    uthr->listener_telemetry(data);
}

/* The open() functions for both platforms were originally written by
 * Robert Harrison */
#ifndef __MINGW32__
void GPSThread::setup()
{
    /* Open the serial port without blocking. Rely on cleanup() */
    fd = open(device.c_str(), O_RDONLY | O_NOCTTY | O_NONBLOCK);
    if (fd == -1)
        throw runtime_error("open() failed");

    f = fdopen(fd, "r");
    if (f == NULL)
        throw runtime_error("fdopen() failed");

    /* Linux requires baudrates be given as a constant */
    speed_t baudrate = B4800;
    if (baud == 9600)           baudrate = B9600;
    else if (baud == 19200)     baudrate = B19200;
    else if (baud == 38400)     baudrate = B38400;
    else if (baud == 57600)     baudrate = B57600;
    else if (baud == 115200)    baudrate = B115200;
    else if (baud == 230400)    baudrate = B230400;

    /* Set all the weird arcane settings Linux demands (boils down to 8N1) */
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

    /* Set raw input output */
    port_settings.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    port_settings.c_oflag &= ~OPOST;

    /* Ignore CR in NMEA's CRLF */
    port_settings.c_iflag |= (IGNCR);

    /* Blocking read until 1 character arrives */
    port_settings.c_cc[VMIN] = 1;

    /* Re enable blocking for reading. */
    int set = fcntl(fd, F_SETFL, 0);
    if (set == -1)
        throw runtime_error("fcntl() failed");

    /* All baud settings */
    set = tcsetattr(fd, TCSANOW, &port_settings);
    if (set == -1)
        throw runtime_error("tcsetattr() failed");
}

void GPSThread::cleanup()
{
    /* The various things will close their underlying fds or handles (w32).
     * Close the last thing we managed to open */
    if (f)
        fclose(f);
    else if (fd != -1)
        close(fd);

    f = NULL;
    fd = -1;
}
#else
void GPSThread::setup()
{
    HANDLE handle = CreateFile(device, GENERIC_READ, 0, 0,
                               OPEN_EXISTING, 0, 0);
    if (handle == INVALID_HANDLE_VALUE)
        throw runtime_error("CreateFile() failed");

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(handle, &dcbSerialParams))
        throw runtime_error("GetCommState() failed");

    dcbSerialParams.BaudRate = baud;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(handle, &dcbSerialParams))
        throw runtime_error("GetCommState() failed");

    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout            = 2000;
    timeouts.ReadTotalTimeoutMultiplier     = 0;
    timeouts.ReadTotalTimeoutConstant       = 5000;
    timeouts.WriteTotalTimeoutMultiplier    = 0;
    timeouts.WriteTotalTimeoutConstant      = 0;

    if (!SetCommTimeouts(handle, &timeouts))
        throw runtime_error("SetCommTimeouts() failed");

    fd = _open_osfhandle((intptr_t) serial_port_handle, _O_RDONLY);
    if (fd == -1)
        throw runtime_error("_open_osfhandle() failed");

    f = fdopen(fd, "r");
    if (!f)
        throw runtime_error("fdopen() failed");
}

void GPSThread::cleanup()
{
    if (f)
        fclose(f);
    else if (fd != -1)
        close(fd);
    else if (handle != INVALID_HANDLE_VALUE)
        CloseHandle(handle);

    f = NULL;
    fd = -1;
    handle = INVALID_HANDLE_VALUE;
}
#endif

} /* namespace gps */
} /* namespace dl_fldigi */