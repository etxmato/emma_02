#ifndef USBCARD_H
#define USBCARD_H

#include "wx/dir.h"
#include "wx/textfile.h"

using namespace std;

#include <vector>

class Radio
{
public:
    wxString name_;
    size_t linkNumber_;

    size_t numberOfRadioValues_;
    size_t radioValueNumber_;
    vector<wxString> value_;
};

class InputInfo
{
public:
    int formNumber;
    int inputType;
};

class HtmlFormInfo
{
public:
    wxString formName;
    wxString formAction;
};

class UrlScreens
{
public:
    Byte screenData[0xe00];

    int numberOfLinks;
    vector<InputInfo> inputInfo;
    vector<wxString> urlLinks;

    size_t shapeLocation_;
    size_t shapeNumber_;
    size_t shape_[256];

    bool invalidCharFound_;
    bool textInputFound_;

    size_t numberOfInvalidChar_;
    vector<int> invalidCharValue_;

    Byte numberOfRadios_;
    vector<Radio> radio_;
};

class CellContent
{
public:
    wxString cellText;

    int currentLink;
    int numberOfLinks; 
    int screenLinkNumber;
    vector<InputInfo> inputInfo;
    vector<wxString> urlLinks;
};

class UrlTable
{
public:
    size_t column;
    size_t numberOfRows;
    size_t numberOfColumns;
    size_t tableWidth;
    vector<size_t> numberOfColumnsCurrentRow;
    vector<size_t> columnWidth;
    vector<Byte> urlLinkCell;
    vector<Byte> shapeColourCell;

    CellContent caption;
    vector< CellContent > row;
    vector< vector<CellContent> > cellContent;
    size_t footStartRow;
    size_t footEndRow;
};

class Tag
{
public:
    wxString element;
    int attributeNumber;
    vector<wxString> attribute;
    vector<wxString> attributeValue;
};

class Usbcard
{
public:
    Usbcard ();
    ~Usbcard ();

    Byte usbIn();
    Byte usbIn6();
    void usbOut(Byte value);
    void usbOutNoEfCheck(Byte value);

    Byte usbInVspe();
    void usbOutVspe(Byte value);
    void storeClockValue(Byte value);
    void storeDateFormat(wxString value);
    void storeTimeFormat(wxString value);
    wxString convertCase(wxString strValue, int comxCaseType, int convertType);

    bool usb_ef();
    void dir();
    void save();
    void load();
    void getTrack();
    void putTrack();
    void testImg();
    void writeDumpFile(wxTextFile* file);
    void deleteBackup(wxFileName dir, wxString name, wxString ext, int flags);
    void deleteAllBackup(wxString dir, bool sub);
    void convertSyncToScores();
    int getSwId(int sw_id);
    wxString getName(int pos, int length);
    wxString convertString(wxString string);
    void setName(wxString name, int pos, int length);
    int getScore1(int pos, int length, char addFactor);
    int getScore2(int pos, int length, int factor);
    int getScore3(int pos);
    int getScore4(int pos, Byte nibbleMask);
    void setScore1(long score, int pos, int length, char addFactor);
    void setScore2(long score, int pos, int length, int factor);
    void setScore3(long score, int pos);
    void setScore4(long score, int pos, Byte nibbleMask);
    Byte getNibble(int *pos, Byte *nibbleMask, bool increase=true);
    void setNibble(Byte nibble, int *pos, Byte *nibbleMask);
    void sendEmail();
    void getPage();
    void getHtmlTag();
    wxString getTag(size_t *pos);
    wxString getTagString();
    void findHtmlCloseTag(wxString htmlCode);
    void findHtmlTag(wxString htmlCode);
    wxString getNextWord(size_t pos);
    void writeText(wxString text, bool link, bool highColour, bool newline);
    void writeInputText(wxString text, bool highColour, bool radioInput, int radioNumber, bool newline, bool breakline);
    void writeChar(wchar_t character, bool link, bool highColour);
    void writeLine(bool trim=true);
    wxString convertUtf8(wxString text);
    void newLine();
    void hr();
    void checkUrlScreenPos();
    void convertRemainingSpecialChar();
    void tab();
    void getText();
    void startTable();
    void writeTable();
    void writeRow(size_t row);
    void adjustColumnWidth();
    wxString shapeHtmlCharacter(wxString str);
    wxString replaceHtmlCharacter(wxString str);
    void writeFirstCellLine(size_t row, size_t col, size_t cellWidth, size_t *remove, Byte *urlLink, Byte *shapeColour, bool lastCol, bool caption);
    void regularCell();
    void getHelp();
    void getEmailHelp();
    wxString getForm();
    wxString getEmailForm(wxString *error);
    void email(wxString message);
    void email_error(wxString message);
    void setLinkInfo(bool increaseLinkSize, wxString urlLink, int inputType, int formNumber);

    void aTag();
    void formTag();
    void olTag();
    void liTag();
    void sourceTag();
    void inputTag();
    void imgTag();
    void iframeTag();
    void comxTag();
    void showAttribute(wxString attribute);
    int checkLinkNumber();

private:
    Byte *rawUrlData;
    size_t rawPos;
    size_t rawUrlDataSize;

    vector<HtmlFormInfo> htmlFormInfo;

    Tag htmlTag;
    int numberOfUrlScreens_;
    int currentUrlScreen_;
    int currentUrlLinePos_;
    size_t currentUrlPos_;
    size_t numberOfForms_;
    Byte urlLink_;
    bool urlInput_;
    Byte shapeColour_;
    Byte colourInfo_;
    Byte reshape_;
    Byte reshapeInfo_;
    wxString urlServer_;
    wxString urlFullPath_;
    wxString urlFragment_;
    wxString urlPath_;
    wxString previousServerName_; 
    wxString baseName_;
    int charSet_;
    bool preTag_; 
    bool hrefTag_; 
    bool captionTag_;
    bool olTag_[31]; 
    int olNumber_[31];
    int olType_[31];
    bool olReversed_[31];
    wxString nameTag_; 
    int liNum_; 
    int emptyLines_;
    bool centerTag_;
    int tableTag_;
    bool tdTag_;
    wxString textLine_;
    int linkLine_[40];
    bool continueLink_;
    int lastLink_;
    int shapeLine_[40];
    vector<wxString> urlHistory;
    size_t urlHistorySize_;
    int urlRedirectCounter_;

    Byte header[128];
    char buffer_[65536];
    int networkError_;

    int tracks_;
    int sides_;
    int bank_;

    int bufferPos_;
    int bufferFlush_;
    wxString rootDirectory_;
    wxString fileNameOnly_;
    wxString extensionOnly_;
    wxString fileName_;
    wxString catFileName_;
    wxFileName additionalDir_;
    wxDir *catDir_;
    bool contCat_;
    bool dirSearch_;

    Word start_;
    Word end_;
    Word length_;
    Word saveLength_;
    int number_;

    int mode_;
    int charType_;
    bool ef_;

    int fwVersion_;

    vector<UrlScreens> urlScreens;
    UrlTable urlTable;
};

#endif  // USBCARD_H
