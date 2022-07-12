#ifndef PRINTER_H
#define PRINTER_H

#include "wx/textfile.h"

#define STANDBY 0
#define LEFT 1
#define RIGHT 2
#define BREAK 3

class PrinterCanvas : public wxScrolledWindow
{
public:
    PrinterCanvas(wxWindow *parent, wxWindowID, const wxPoint &pos, const wxSize &size, long style, const wxString& title);
    ~PrinterCanvas();

    void onClose(wxCloseEvent &event);
    void onPaint(wxPaintEvent &event);

    void init();
    Byte inThermal();
    void outThermal(Byte value, Byte Qflag);
    bool cycleThermal(bool ef4);

    void readPlotterRom(wxString DirName, wxString fileName);
    void readPlotterRomExtension(wxString DirName, wxString fileName);
    void completePage(wxDC& dc, int page);
    int getNumberofPages();
    void thermalLine(Byte value);
    void thermalLine2();
    void plotterLine(wxString printline);
    void setFont(int selection);
    void matrixLine(wxString printline);
    void doGraphics();
    void doText();
    void printCharacter(Byte CharNumber);
    void drawCharLine(int x, int y, int xDest, int yDest, int drawmode);
    int readInt();
    void checkXandY();
    void changeSize(int x);
    void checkX();
    void checkY();
    void checkThermalY();
    void printFinished();

private:
    wxBitmap *printerBitmapPointer;
    wxImage PrinterImage;
    wxMemoryDC dcPreview;
    wxFont *printFontPointer;

    int paperWidth_;
    wxString printLine_;
    int maxY_;
    int pageSize_;

    bool plotterTextMode_;
    bool plotterSolidLine_;
    bool plotterTextDrawn_;
    int plotterStringPosition_;
    int plotterCharSize_;
    int plotterTextSize_;
    int plotterColour_;
    int plotterX_;
    int plotterY_;
    int plotterOriginX_;
    int plotterOriginY_;
    int plotterAngle_;
    int plotterCharWidth_;
    int plotterCharHeight_;
    int plotterGraphCharWidth_;
    int plotterGraphCharHeight_;
    int plotterLineSpace_;
    int plotterCharSet_;
    int plotterLineScale_;
    int plotterCharacterSets_[8][256];
    int plotterNumberOfSets_;
    wxDash plotterDash_[16][4];
    Byte plotterRom_[16385];

    int matrixStringPosition_;
    int matrixX_;
    int matrixY_;
    int matrixCharWidth_;
    int matrixCharHeight_;

    int thermalX_;
    int thermalY_;
    int thermalCommand_;
    int thermalCycleValue_;
    int thermalLineFeed_;

    DECLARE_EVENT_TABLE()
};

class PrinterFrame : public wxFrame
{
public:
    PrinterFrame(const wxString& title, const wxPoint& pos, const wxSize& size, int printerType);
    ~PrinterFrame();

    void onClose(wxCloseEvent&event );

    void init();
    Byte inThermal();
    void outThermal(Byte value, Byte Qflag);
    bool cycleThermal(bool ef4);

    void onFont(wxCommandEvent &event);
    void onRows(wxCommandEvent &event);
    void onPlotterRomExtensionText(wxCommandEvent &event);
    void onPlotterRomText(wxCommandEvent &event);
    void onRomButton(wxCommandEvent &event);
    void onExtButton(wxCommandEvent &event);
    void onPrint(wxCommandEvent &event);
    void onPreview(wxCommandEvent &event);
    void onPageSetup(wxCommandEvent &event);

    void matrixLine(wxString printline);
    void plotterLine(wxString printline);
    void printFinished();
    void completePage(wxDC& dc, int page);
    int getNumberofPages();
    void showPlotterRomFile(wxString fileName);
    void showPlotterRomExtensionFile(wxString fileName);
    wxString getPlotterRomFile();
    wxString getPlotterRomExtensionFile();

private:
    PrinterCanvas *printerCanvasPointer;

    wxString plotterRomDir_;
    wxString plotterRomFile_;
    wxString plotterRomExtensionFile_;
    int printerType_;
    int paperWidth_;

    DECLARE_EVENT_TABLE()
};

class Printer : public wxObject
{
public:
    Printer();
    ~Printer();

    void configureElfPrinter(ElfPortConfiguration portConf);
    void configureThermalPrinter();
    void init(Printer *pointer, wxString computerName, int printerType);
    void initComx(Printer *pointer);
    void initTelmac(Printer *pointer);
    void initPecom(Printer *pointer);
    void initMS2000(Printer *pointer);
    void initVip(Printer *pointer);
    void initElf(Printer *pointer, wxString computerName);
    Byte inParallel();
    Byte inSerial();
    Byte inThermal();
    void openPrintFile();
    void printerOut(Byte value); 
    void startPecomChar(); 
    void outParallel(Byte value);
    void outSerial(Byte value);
    void outThermal(Byte value, Byte Qflag);
    void cycleThermal();

    void setComxPrintMode(int mode);
    void setPrintMode(int mode);
    bool getThermalEf4();
    void closeFrames();
    void closePreviewWindow();

    void onF4Parallel();
    void onF4Serial();
    void onF4Thermal();
    void onF4();
    void onComxF4();
    void onComxF4Printer();
    void onComxF4Plotter();

    void printFinished();
    void enableToolbar(bool status);
    void showPlotterRomFile(wxString fileName);
    void showPlotterRomExtensionFile(wxString fileName);
    wxString getPlotterRomFile();
    wxString getPlotterRomExtensionFile();

    void completePage(wxDC& dc, int page);
    int getNumberofPages();

    void setPrintfileName(wxString fileName);
    void printOut();
    void printPreview();

private:
    Printer *printerPointer;
    PrinterFrame *printerFramePointer;
    wxStaticText* fontTextPointer;
    wxButton* printerPlotterButtonPointer;
    wxButton* plotterRomPointer;
    wxButton* plotterExtensionRomPointer;
    wxComboBox* plotterRomTextPointer;
    wxComboBox* plotterExtensionRomTextPointer;
    wxChoice* printerFontPointer;
    wxChoice* printerRowsPointer;

    int printerType_;
    wxString computerName_;

    wxString line_;
    bool printEscape_;

    wxString printFileFullPath_;
    wxString printFileName_;
    bool newPrintFile_;

    wxFile printFile_;
    bool printFileOpen_;

    int bit_;
    int stopBit_;
    int dataBits_;
    int printValue_;

    bool parallelFrameOpen_;
    bool serialFrameOpen_;
    bool thermalFrameOpen_;
    bool printerFrameOpen_;

    bool printStarted_;
    bool thermalEf4_;
    int comxPrintMode_;
    int printMode_;
};

class ComxPrintout: public wxPrintout
{
public:
    ComxPrintout(const wxString& title, Printer *pointer);
    ~ComxPrintout() {};

    bool OnPrintPage(int page);
    bool OnBeginDocument(int startPage, int endPage);
    void GetPageInfo(int *minPage, int *maxPage, int *selPageFrom, int *selPageTo);
    bool HasPage(int page);

private:
    Printer *printerPointer;

};

#endif  // PRINTER_H
