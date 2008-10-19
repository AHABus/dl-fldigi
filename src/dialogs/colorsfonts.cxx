// generated by Fast Light User Interface Designer (fluid) version 1.0108

#include "colorsfonts.h"
#include <config.h>
#include "fl_digi.h"
#include "configuration.h"

void selectColorsFonts() {
  if (!dlgColorFont)
    make_colorsfonts();
dlgColorFont->show();
}

static void choose_color(Fl_Color & c) {
  unsigned char r, g, b;
Fl::get_color(c, r, g, b);
if (fl_color_chooser("Font color", r, g, b))
    c = fl_rgb_color(r, g, b);
}

static void adjust_label(Fl_Widget* w) {
  w->labelcolor(fl_contrast(w->labelcolor(), w->color()));
}

static void cbRxFontBrowser(Fl_Widget* w, void* arg) {
  Font_Browser *fb= (Font_Browser*)w;
    Fl_Font font = fb->fontNumber();
    int size = fb->fontSize();
    Fl_Color color = fb->fontColor();

    RxText->textfont(font);
    RxText->textsize(size);
    RxText->textcolor(color);
    RxText->redraw();

    progdefaults.RxFontnbr = font;
    progdefaults.RxFontsize = size;
    progdefaults.RxFontcolor = color;

    ReceiveText->setFont(font);
    ReceiveText->setFontSize(size);
    ReceiveText->setFontColor(progdefaults.RxFontcolor, FTextBase::RECV);

    fb->hide();

    progdefaults.changed = true;
}

static void cbTxFontBrowser(Fl_Widget* w, void* arg) {
  Font_Browser *fb= (Font_Browser*)w;
    Fl_Font font = fb->fontNumber();
    int size = fb->fontSize();
    Fl_Color color = fb->fontColor();

    TxText->textfont(font);
    TxText->textsize(size);
    TxText->textcolor(color);
    TxText->redraw();

    progdefaults.TxFontnbr = font;
    progdefaults.TxFontsize = size;
    progdefaults.TxFontcolor = color;

    TransmitText->setFont(font);
    TransmitText->setFontSize(size);
    TransmitText->setFontColor(progdefaults.TxFontcolor, FTextBase::RECV);

    fb->hide();

    progdefaults.changed = true;
}

void cbWaterfallFontBrowser(Font_Browser*, void* v) {
  Font_Browser *ft= (Font_Browser*)v;
    Fl_Font fnt = ft->fontNumber();
    int size = ft->fontSize();

    progdefaults.WaterfallFontnbr = fnt;
    progdefaults.WaterfallFontsize = size;
    progdefaults.changed = true;

    ft->hide();
}

Fl_Double_Window *dlgColorFont=(Fl_Double_Window *)0;

Fl_Button *btnClrFntClose=(Fl_Button *)0;

static void cb_btnClrFntClose(Fl_Button* o, void*) {
  o->window()->hide();
}

Fl_Box *FDdisplay=(Fl_Box *)0;

Fl_Button *btnBackgroundColor=(Fl_Button *)0;

static void cb_btnBackgroundColor(Fl_Button*, void*) {
  uchar r, g, b;
    r = progdefaults.FDbackground.R;
    g = progdefaults.FDbackground.G;
    b = progdefaults.FDbackground.B;

    if (!fl_color_chooser("Background", r, g, b))
        return;

    progdefaults.FDbackground.R = r;
    progdefaults.FDbackground.G = g;
    progdefaults.FDbackground.B = b;
    
    FDdisplay->color(fl_rgb_color(r,g,b));
    FDdisplay->redraw();
    
    qsoFreqDisp->SetONOFFCOLOR(
          fl_rgb_color(	progdefaults.FDforeground.R,
                        progdefaults.FDforeground.G,
                        progdefaults.FDforeground.B),
          fl_rgb_color(	progdefaults.FDbackground.R,
                        progdefaults.FDbackground.G,
                        progdefaults.FDbackground.B));
    qsoFreqDisp->redraw();

    progdefaults.changed = true;
}

Fl_Button *btnForegroundColor=(Fl_Button *)0;

static void cb_btnForegroundColor(Fl_Button*, void*) {
  uchar r, g, b;
    r = progdefaults.FDforeground.R;
    g = progdefaults.FDforeground.G;
    b = progdefaults.FDforeground.B;

    if (!fl_color_chooser("Foreground", r, g, b))
        return;

    progdefaults.FDforeground.R = r;
    progdefaults.FDforeground.G = g;
    progdefaults.FDforeground.B = b;
    
    FDdisplay->labelcolor(fl_rgb_color(r,g,b));
    FDdisplay->redraw();
    
    qsoFreqDisp->SetONOFFCOLOR(
          fl_rgb_color(	progdefaults.FDforeground.R,
                        progdefaults.FDforeground.G,
                        progdefaults.FDforeground.B),
          fl_rgb_color(	progdefaults.FDbackground.R,
                        progdefaults.FDbackground.G,
                        progdefaults.FDbackground.B));
    qsoFreqDisp->redraw();

    progdefaults.changed = true;
}

Fl_Button *btnFD_SystemColor=(Fl_Button *)0;

static void cb_btnFD_SystemColor(Fl_Button*, void*) {
  uchar r, g, b;
    Fl_Color clr = FL_BACKGROUND2_COLOR;

    Fl::get_color(clr, r, g, b);

    progdefaults.FDbackground.R = r;
    progdefaults.FDbackground.G = g;
    progdefaults.FDbackground.B = b;
   
    FDdisplay->color(clr);
    
    clr = FL_FOREGROUND_COLOR;
    Fl::get_color(clr, r, g, b);
    
    progdefaults.FDforeground.R = r;
    progdefaults.FDforeground.G = g;
    progdefaults.FDforeground.B = b;

    qsoFreqDisp->SetONOFFCOLOR(
          fl_rgb_color(	progdefaults.FDforeground.R,
                        progdefaults.FDforeground.G,
                        progdefaults.FDforeground.B),
          fl_rgb_color(	progdefaults.FDbackground.R,
                        progdefaults.FDbackground.G,
                        progdefaults.FDbackground.B));
     
    progdefaults.changed = true;
}

Fl_Check_Button *btnUseGroupColors=(Fl_Check_Button *)0;

static void cb_btnUseGroupColors(Fl_Check_Button* o, void*) {
  progdefaults.useGroupColors = o->value();
    colorize_macros();
    progdefaults.changed = true;
}

Fl_Button *btnGroup1=(Fl_Button *)0;

static void cb_btnGroup1(Fl_Button* o, void*) {
  uchar r, g, b;
    r = progdefaults.btnGroup1.R;
    g = progdefaults.btnGroup1.G;
    b = progdefaults.btnGroup1.B;

    if (fl_color_chooser("Group 1", r, g, b) == 0)
        return;
    progdefaults.btnGroup1.R = r;
    progdefaults.btnGroup1.G = g;
    progdefaults.btnGroup1.B = b;
    o->color(fl_rgb_color(r,g,b));
    colorize_macros();
    
    progdefaults.changed = true;
}

Fl_Button *btnGroup2=(Fl_Button *)0;

static void cb_btnGroup2(Fl_Button* o, void*) {
  uchar r, g, b;
    r = progdefaults.btnGroup2.R;
    g = progdefaults.btnGroup2.G;
    b = progdefaults.btnGroup2.B;

    if (fl_color_chooser("Group 2", r, g, b) == 0)
        return;
    progdefaults.btnGroup2.R = r;
    progdefaults.btnGroup2.G = g;
    progdefaults.btnGroup2.B = b;
    o->color(fl_rgb_color(r,g,b));
    colorize_macros();

    progdefaults.changed = true;
}

Fl_Button *btnGroup3=(Fl_Button *)0;

static void cb_btnGroup3(Fl_Button* o, void*) {
  uchar r, g, b;
    r = progdefaults.btnGroup3.R;
    g = progdefaults.btnGroup3.G;
    b = progdefaults.btnGroup3.B;

    if (fl_color_chooser("Group 3", r, g, b) == 0)
        return;
    progdefaults.btnGroup3.R = r;
    progdefaults.btnGroup3.G = g;
    progdefaults.btnGroup3.B = b;
    o->color(fl_rgb_color(r,g,b));

    colorize_macros();
    progdefaults.changed = true;
}

Fl_Button *btnFkeyTextColor=(Fl_Button *)0;

static void cb_btnFkeyTextColor(Fl_Button* o, void*) {
  uchar r, g, b;
    r = progdefaults.btnFkeyTextColor.R;
    g = progdefaults.btnFkeyTextColor.G;
    b = progdefaults.btnFkeyTextColor.B;

    if (fl_color_chooser("Fkey Text", r, g, b) == 0)
        return;

    progdefaults.btnFkeyTextColor.R = r;
    progdefaults.btnFkeyTextColor.G = g;
    progdefaults.btnFkeyTextColor.B = b;
    o->color(fl_rgb_color(r,g,b));
    adjust_label(o);
    btnGroup1->labelcolor(fl_rgb_color(r,g,b));
    btnGroup2->labelcolor(fl_rgb_color(r,g,b));
    btnGroup3->labelcolor(fl_rgb_color(r,g,b));
    btnGroup1->redraw_label();
    btnGroup2->redraw_label();
    btnGroup3->redraw_label();
    progdefaults.changed = true;
    colorize_macros();
}

Fl_Button *btnFkeyDEfaults=(Fl_Button *)0;

static void cb_btnFkeyDEfaults(Fl_Button*, void*) {
  uchar r, g, b;
    Fl_Color clr;

    r = 80; g = 144; b = 144;
    clr = fl_rgb_color(r,g,b);
    btnGroup1->color(clr);
    progdefaults.btnGroup1.R = r;
    progdefaults.btnGroup1.G = g;
    progdefaults.btnGroup1.B = b;

    r = 144; g = 80; b = 80;
    clr = fl_rgb_color(r,g,b);
    btnGroup2->color(clr);
    progdefaults.btnGroup2.R = r;
    progdefaults.btnGroup2.G = g;
    progdefaults.btnGroup2.B = b;

    r = 80; g = 80; b = 144;
    clr = fl_rgb_color(r,g,b);
    btnGroup3->color(clr);
    progdefaults.btnGroup3.R = r;
    progdefaults.btnGroup3.G = g;
    progdefaults.btnGroup3.B = b;

    r = 255; g = 255; b = 255;
    clr = fl_rgb_color(r,g,b);
    btnFkeyTextColor->color(clr);
    btnFkeyTextColor->redraw_label();
    progdefaults.btnFkeyTextColor.R = r;
    progdefaults.btnFkeyTextColor.G = g;
    progdefaults.btnFkeyTextColor.B = b;

    btnGroup1->labelcolor(clr);
    btnGroup1->redraw_label();

    btnGroup2->labelcolor(clr);
    btnGroup2->redraw_label();

    btnGroup3->labelcolor(clr);
    btnGroup3->redraw_label();

    colorize_macros();

progdefaults.changed = true;
}

Fl_Input *RxText=(Fl_Input *)0;

Fl_Button *btnRxColor=(Fl_Button *)0;

static void cb_btnRxColor(Fl_Button*, void*) {
  uchar r, g, b;
    r = progdefaults.RxColor.R;
    g = progdefaults.RxColor.G;
    b = progdefaults.RxColor.B;

    if (!fl_color_chooser("Rx Color", r, g, b))
        return;

    progdefaults.RxColor.R = r;
    progdefaults.RxColor.G = g;
    progdefaults.RxColor.B = b;
    RxText->color(fl_rgb_color(r,g,b));
    ReceiveText->color(RxText->color());
    RxText->redraw();
    ReceiveText->redraw();

    progdefaults.changed = true;
}

Fl_Button *btnTxColor=(Fl_Button *)0;

static void cb_btnTxColor(Fl_Button*, void*) {
  uchar r, g, b;
    r = progdefaults.TxColor.R;
    g = progdefaults.TxColor.G;
    b = progdefaults.TxColor.B;

    if (!fl_color_chooser("Tx Color", r, g, b))
        return;

    progdefaults.TxColor.R = r;
    progdefaults.TxColor.G = g;
    progdefaults.TxColor.B = b;
    TxText->color(fl_rgb_color(r,g,b));
    TransmitText->color(TxText->color());
    TxText->redraw();
    TransmitText->redraw();

progdefaults.changed = true;
}

Fl_Input *TxText=(Fl_Input *)0;

Fl_Button *btnRxFont=(Fl_Button *)0;

static void cb_btnRxFont(Fl_Button*, void*) {
  static Font_Browser* b = 0;
if (!b) {
    b = new Font_Browser;
    b->fontNumber(progdefaults.RxFontnbr);
    b->fontSize(progdefaults.RxFontsize);
    b->fontColor(progdefaults.RxFontcolor);
    b->callback(cbRxFontBrowser);
}
b->show();
}

Fl_Button *btnTxFont=(Fl_Button *)0;

static void cb_btnTxFont(Fl_Button*, void*) {
  static Font_Browser* b = 0;
    if (!b) {
        b = new Font_Browser;
        b->fontNumber(progdefaults.TxFontnbr);
        b->fontSize(progdefaults.TxFontsize);
        b->fontColor(progdefaults.TxFontcolor);
        b->callback(cbTxFontBrowser);
    }
    b->show();
}

Fl_Button *btnXMIT=(Fl_Button *)0;

static void cb_btnXMIT(Fl_Button*, void*) {
  choose_color(progdefaults.XMITcolor);
    btnXMIT->color( progdefaults.XMITcolor );
    TransmitText->setFontColor(progdefaults.XMITcolor, FTextBase::XMIT);
    ReceiveText->setFontColor(progdefaults.XMITcolor, FTextBase::XMIT);

    progdefaults.changed = true;
}

Fl_Button *btnCTRL=(Fl_Button *)0;

static void cb_btnCTRL(Fl_Button*, void*) {
  choose_color(progdefaults.CTRLcolor);
    btnCTRL->color( progdefaults.CTRLcolor );
    TransmitText->setFontColor(progdefaults.CTRLcolor, FTextBase::CTRL);
    ReceiveText->setFontColor(progdefaults.CTRLcolor, FTextBase::CTRL);

    progdefaults.changed = true;
}

Fl_Button *btnSKIP=(Fl_Button *)0;

static void cb_btnSKIP(Fl_Button*, void*) {
  choose_color(progdefaults.SKIPcolor);
    btnSKIP->color( progdefaults.SKIPcolor );
    TransmitText->setFontColor(progdefaults.SKIPcolor, FTextBase::SKIP);
    ReceiveText->setFontColor(progdefaults.SKIPcolor, FTextBase::SKIP);

    progdefaults.changed = true;
}

Fl_Button *btnALTR=(Fl_Button *)0;

static void cb_btnALTR(Fl_Button*, void*) {
  choose_color(progdefaults.ALTRcolor);
    btnALTR->color( progdefaults.ALTRcolor );
    TransmitText->setFontColor(progdefaults.ALTRcolor, FTextBase::ALTR);
    ReceiveText->setFontColor(progdefaults.ALTRcolor, FTextBase::ALTR);

    progdefaults.changed = true;
}

Fl_Button *btnNoTextColor=(Fl_Button *)0;

static void cb_btnNoTextColor(Fl_Button*, void*) {
  uchar r, g, b;
    Fl_Color clr = FL_BACKGROUND2_COLOR;

    Fl::get_color(clr, r, g, b);

    progdefaults.TxFontcolor = FL_BLACK;
    progdefaults.RxFontcolor = FL_BLACK;
    progdefaults.XMITcolor = FL_RED;
    progdefaults.CTRLcolor = FL_DARK_GREEN;
    progdefaults.SKIPcolor = FL_BLUE;
    progdefaults.ALTRcolor = FL_DARK_MAGENTA;
    btnXMIT->color(progdefaults.XMITcolor);
    btnCTRL->color(progdefaults.CTRLcolor);
    btnSKIP->color(progdefaults.SKIPcolor);
    btnALTR->color(progdefaults.ALTRcolor);
    btnXMIT->redraw();
    btnCTRL->redraw();
    btnSKIP->redraw();
    btnALTR->redraw();

    progdefaults.RxColor.R = r;
    progdefaults.RxColor.G = g;
    progdefaults.RxColor.B = b;

    clr = fl_rgb_color(r,g,b);
    RxText->color(clr);
    RxText->textcolor(progdefaults.RxFontcolor);
    RxText->redraw();

    ReceiveText->color(clr);
    ReceiveText->setFontColor(progdefaults.RxFontcolor, FTextBase::RECV);
    ReceiveText->setFontColor(progdefaults.XMITcolor, FTextBase::XMIT);
    ReceiveText->setFontColor(progdefaults.CTRLcolor, FTextBase::CTRL);
    ReceiveText->setFontColor(progdefaults.SKIPcolor, FTextBase::SKIP);
    ReceiveText->setFontColor(progdefaults.ALTRcolor, FTextBase::ALTR);
    ReceiveText->redraw();

    progdefaults.TxColor.R = r;
    progdefaults.TxColor.G = g;
    progdefaults.TxColor.B = b;

    TxText->color(clr);
    TxText->textcolor(progdefaults.TxFontcolor);
    TxText->redraw();

    TransmitText->color(clr);
    TransmitText->setFontColor(progdefaults.TxFontcolor, FTextBase::RECV);
    TransmitText->setFontColor(progdefaults.XMITcolor, FTextBase::XMIT);
    TransmitText->setFontColor(progdefaults.CTRLcolor, FTextBase::CTRL);
    TransmitText->setFontColor(progdefaults.SKIPcolor, FTextBase::SKIP);
    TransmitText->setFontColor(progdefaults.ALTRcolor, FTextBase::ALTR);
    TransmitText->redraw();

    progdefaults.changed = true;
}

Fl_Button *btnTextDefaults=(Fl_Button *)0;

static void cb_btnTextDefaults(Fl_Button*, void*) {
  uchar r, g, b;
    Fl_Color clr;

    progdefaults.TxFontcolor = FL_BLACK;
    progdefaults.RxFontcolor = FL_BLACK;
    progdefaults.XMITcolor = FL_RED;
    progdefaults.CTRLcolor = FL_DARK_GREEN;
    progdefaults.SKIPcolor = FL_BLUE;
    progdefaults.ALTRcolor = FL_DARK_MAGENTA;
    btnXMIT->color(progdefaults.XMITcolor);
    btnCTRL->color(progdefaults.CTRLcolor);
    btnSKIP->color(progdefaults.SKIPcolor);
    btnALTR->color(progdefaults.ALTRcolor);
    btnXMIT->redraw();
    btnCTRL->redraw();
    btnSKIP->redraw();
    btnALTR->redraw();

    r = 255; g = 242; b = 190;
    progdefaults.RxColor.R = r;
    progdefaults.RxColor.G = g;
    progdefaults.RxColor.B = b;

    clr = fl_rgb_color(r,g,b);
    RxText->color(clr);
    RxText->textcolor(progdefaults.RxFontcolor);
    RxText->redraw();

    ReceiveText->color(clr);
    ReceiveText->setFontColor(progdefaults.RxFontcolor, FTextBase::RECV);
    ReceiveText->setFontColor(progdefaults.XMITcolor, FTextBase::XMIT);
    ReceiveText->setFontColor(progdefaults.CTRLcolor, FTextBase::CTRL);
    ReceiveText->setFontColor(progdefaults.SKIPcolor, FTextBase::SKIP);
    ReceiveText->setFontColor(progdefaults.ALTRcolor, FTextBase::ALTR);
    ReceiveText->redraw();

    r = 200; g = 235; b = 255;
    progdefaults.TxColor.R = r;
    progdefaults.TxColor.G = g;
    progdefaults.TxColor.B = b;

    clr = fl_rgb_color(r,g,b);
    TxText->color(clr);
    TxText->textcolor(progdefaults.TxFontcolor);
    TxText->redraw();

    TransmitText->color(clr);
    TransmitText->setFontColor(progdefaults.TxFontcolor, FTextBase::RECV);
    TransmitText->setFontColor(progdefaults.XMITcolor, FTextBase::XMIT);
    TransmitText->setFontColor(progdefaults.CTRLcolor, FTextBase::CTRL);
    TransmitText->setFontColor(progdefaults.SKIPcolor, FTextBase::SKIP);
    TransmitText->setFontColor(progdefaults.ALTRcolor, FTextBase::ALTR);
    TransmitText->redraw();

    progdefaults.changed = true;
}

Fl_Choice *mnuScheme=(Fl_Choice *)0;

static void cb_mnuScheme(Fl_Choice* o, void*) {
  progdefaults.ui_scheme = o->text();
    Fl::scheme(progdefaults.ui_scheme.c_str());

    progdefaults.changed = true;
}

Fl_Double_Window* make_colorsfonts() {
  { dlgColorFont = new Fl_Double_Window(375, 220, "Colors and Fonts");
    { btnClrFntClose = new Fl_Button(296, 190, 75, 25, "Close");
      btnClrFntClose->callback((Fl_Callback*)cb_btnClrFntClose);
    } // Fl_Button* btnClrFntClose
    { Fl_Tabs* o = new Fl_Tabs(0, 5, 375, 179);
      { Fl_Group* o = new Fl_Group(5, 30, 365, 150, "Freq Display");
        { Fl_Box* o = FDdisplay = new Fl_Box(100, 45, 45, 67, "8");
          FDdisplay->box(FL_DOWN_BOX);
          FDdisplay->color((Fl_Color)55);
          FDdisplay->labelfont(4);
          FDdisplay->labelsize(48);
          o->color(fl_rgb_color(progdefaults.FDbackground.R,progdefaults.FDbackground.G,progdefaults.FDbackground.B));
          o->labelcolor(fl_rgb_color(progdefaults.FDforeground.R,progdefaults.FDforeground.G,progdefaults.FDforeground.B));
        } // Fl_Box* FDdisplay
        { btnBackgroundColor = new Fl_Button(165, 45, 100, 30, "Background");
          btnBackgroundColor->callback((Fl_Callback*)cb_btnBackgroundColor);
        } // Fl_Button* btnBackgroundColor
        { btnForegroundColor = new Fl_Button(165, 85, 100, 30, "Foreground");
          btnForegroundColor->callback((Fl_Callback*)cb_btnForegroundColor);
        } // Fl_Button* btnForegroundColor
        { btnFD_SystemColor = new Fl_Button(165, 125, 100, 30, "System");
          btnFD_SystemColor->callback((Fl_Callback*)cb_btnFD_SystemColor);
        } // Fl_Button* btnFD_SystemColor
        o->end();
      } // Fl_Group* o
      { Fl_Group* o = new Fl_Group(5, 30, 365, 150, "Func keys");
        o->hide();
        { btnUseGroupColors = new Fl_Check_Button(10, 40, 165, 20, "Use colored buttons");
          btnUseGroupColors->down_box(FL_DOWN_BOX);
          btnUseGroupColors->callback((Fl_Callback*)cb_btnUseGroupColors);
          btnUseGroupColors->value(progdefaults.useGroupColors);
        } // Fl_Check_Button* btnUseGroupColors
        { btnGroup1 = new Fl_Button(10, 70, 75, 20, "Group 1");
          btnGroup1->tooltip("Background color for Function key group 1");
          btnGroup1->callback((Fl_Callback*)cb_btnGroup1);
          btnGroup1->color(fl_rgb_color(progdefaults.btnGroup1.R, progdefaults.btnGroup1.G,progdefaults.btnGroup1.B));
          btnGroup1->labelcolor(fl_rgb_color(progdefaults.btnFkeyTextColor.R, progdefaults.btnFkeyTextColor.R, progdefaults.btnFkeyTextColor.R));
        } // Fl_Button* btnGroup1
        { btnGroup2 = new Fl_Button(101, 70, 75, 20, "Group 2");
          btnGroup2->tooltip("Background color for Function key group 2");
          btnGroup2->callback((Fl_Callback*)cb_btnGroup2);
          btnGroup2->color(fl_rgb_color(progdefaults.btnGroup2.R, progdefaults.btnGroup2.G,progdefaults.btnGroup2.B));
          btnGroup2->labelcolor(fl_rgb_color(progdefaults.btnFkeyTextColor.R, progdefaults.btnFkeyTextColor.R, progdefaults.btnFkeyTextColor.R));
        } // Fl_Button* btnGroup2
        { btnGroup3 = new Fl_Button(193, 70, 75, 20, "Group 3");
          btnGroup3->tooltip("Background color for Function key group 3");
          btnGroup3->callback((Fl_Callback*)cb_btnGroup3);
          btnGroup3->color(fl_rgb_color(progdefaults.btnGroup3.R, progdefaults.btnGroup3.G,progdefaults.btnGroup3.B));
          btnGroup3->labelcolor(fl_rgb_color(progdefaults.btnFkeyTextColor.R, progdefaults.btnFkeyTextColor.R, progdefaults.btnFkeyTextColor.R));
        } // Fl_Button* btnGroup3
        { btnFkeyTextColor = new Fl_Button(193, 40, 75, 20, "Label text");
          btnFkeyTextColor->callback((Fl_Callback*)cb_btnFkeyTextColor);
          btnFkeyTextColor->color(fl_rgb_color(progdefaults.btnFkeyTextColor.R, progdefaults.btnFkeyTextColor.G, progdefaults.btnFkeyTextColor.B));
          adjust_label(btnFkeyTextColor);
        } // Fl_Button* btnFkeyTextColor
        { btnFkeyDEfaults = new Fl_Button(285, 70, 75, 20, "Defaults");
          btnFkeyDEfaults->callback((Fl_Callback*)cb_btnFkeyDEfaults);
        } // Fl_Button* btnFkeyDEfaults
        o->end();
      } // Fl_Group* o
      { Fl_Group* o = new Fl_Group(5, 30, 365, 150, "Text Ctrls");
        o->hide();
        { RxText = new Fl_Input(15, 43, 165, 35);
          RxText->value("Receive Text");
          RxText->color(fl_rgb_color(progdefaults.RxColor.R, progdefaults.RxColor.G, progdefaults.RxColor.B));
          RxText->textfont(progdefaults.RxFontnbr); RxText->textsize(progdefaults.RxFontsize); RxText->textcolor(progdefaults.RxFontcolor);
          RxText->type(FL_MULTILINE_INPUT_WRAP);
        } // Fl_Input* RxText
        { btnRxColor = new Fl_Button(190, 50, 75, 20, "Rx bkgnd");
          btnRxColor->callback((Fl_Callback*)cb_btnRxColor);
        } // Fl_Button* btnRxColor
        { btnTxColor = new Fl_Button(190, 90, 75, 20, "Tx bkgnd");
          btnTxColor->callback((Fl_Callback*)cb_btnTxColor);
        } // Fl_Button* btnTxColor
        { TxText = new Fl_Input(15, 85, 165, 35);
          TxText->value("Transmit Text");
          TxText->color(fl_rgb_color(progdefaults.TxColor.R, progdefaults.TxColor.G, progdefaults.TxColor.B));
          TxText->textfont(progdefaults.TxFontnbr); TxText->textsize(progdefaults.TxFontsize); TxText->textcolor(progdefaults.TxFontcolor);
          TxText->type(FL_MULTILINE_INPUT_WRAP);
        } // Fl_Input* TxText
        { btnRxFont = new Fl_Button(275, 50, 75, 20, "Rx font");
          btnRxFont->callback((Fl_Callback*)cb_btnRxFont);
        } // Fl_Button* btnRxFont
        { btnTxFont = new Fl_Button(275, 90, 75, 20, "Tx font");
          btnTxFont->callback((Fl_Callback*)cb_btnTxFont);
        } // Fl_Button* btnTxFont
        { btnXMIT = new Fl_Button(15, 130, 40, 20, "XMIT");
          btnXMIT->tooltip("Sent chars in Rx/Tx pane");
          btnXMIT->callback((Fl_Callback*)cb_btnXMIT);
          btnXMIT->align(FL_ALIGN_BOTTOM);
          btnXMIT->color(progdefaults.XMITcolor);
        } // Fl_Button* btnXMIT
        { btnCTRL = new Fl_Button(66, 130, 40, 20, "CTRL");
          btnCTRL->tooltip("Control chars in Rx/Tx pane");
          btnCTRL->callback((Fl_Callback*)cb_btnCTRL);
          btnCTRL->align(FL_ALIGN_BOTTOM);
          btnCTRL->color(progdefaults.CTRLcolor);
        } // Fl_Button* btnCTRL
        { btnSKIP = new Fl_Button(118, 130, 40, 20, "SKIP");
          btnSKIP->tooltip("Skipped chars in Tx pane\n(Tx on/off in CW)");
          btnSKIP->callback((Fl_Callback*)cb_btnSKIP);
          btnSKIP->align(FL_ALIGN_BOTTOM);
          btnSKIP->color(progdefaults.SKIPcolor);
        } // Fl_Button* btnSKIP
        { btnALTR = new Fl_Button(170, 130, 40, 20, "ALTR");
          btnALTR->tooltip("Quick view chars in Rx pane");
          btnALTR->callback((Fl_Callback*)cb_btnALTR);
          btnALTR->align(FL_ALIGN_BOTTOM);
          btnALTR->color(progdefaults.ALTRcolor);
        } // Fl_Button* btnALTR
        { btnNoTextColor = new Fl_Button(219, 130, 70, 20, "System");
          btnNoTextColor->callback((Fl_Callback*)cb_btnNoTextColor);
        } // Fl_Button* btnNoTextColor
        { btnTextDefaults = new Fl_Button(296, 130, 70, 20, "Defaults");
          btnTextDefaults->callback((Fl_Callback*)cb_btnTextDefaults);
        } // Fl_Button* btnTextDefaults
        o->end();
      } // Fl_Group* o
      { Fl_Group* o = new Fl_Group(5, 30, 365, 150, "User Interface");
        o->hide();
        { mnuScheme = new Fl_Choice(120, 50, 80, 25, "UI scheme");
          mnuScheme->down_box(FL_BORDER_BOX);
          mnuScheme->callback((Fl_Callback*)cb_mnuScheme);
          mnuScheme->align(FL_ALIGN_RIGHT);
          mnuScheme->add("base");
          mnuScheme->add("gtk+");
          mnuScheme->add("plastic");
          mnuScheme->value(mnuScheme->find_item(progdefaults.ui_scheme.c_str()));
        } // Fl_Choice* mnuScheme
        o->end();
      } // Fl_Group* o
      o->end();
    } // Fl_Tabs* o
    dlgColorFont->xclass(PACKAGE_TARNAME);
    dlgColorFont->end();
  } // Fl_Double_Window* dlgColorFont
  return dlgColorFont;
}
