#ifndef DEBUG_H
#define DEBUG_H

#include "wx/listctrl.h"
#include "wx/imaglist.h"

#include "guicomx.h"

class InsertPointer
{
public:
	Word address;
	Byte instruction;
	Word branchAddress;
	bool insert;
	InsertPointer *next;
};

#define TREG_D 0
#define TREG_P 1
#define TREG_X 2
#define TREG_B 3
#define TREG_T 4
#define TREG_DF 5
#define TREG_Q 6
#define TREG_CH 7
#define TREG_CNTR 8
#define TREG_R0 9
#define TREG_R1 10
#define TREG_R2 11
#define TREG_R3 12
#define TREG_R4 13
#define TREG_R5 14
#define TREG_R6 15
#define TREG_R7 16
#define TREG_R8 17
#define TREG_R9 18
#define TREG_RA 19
#define TREG_RB 20
#define TREG_RC 21
#define TREG_RD 22
#define TREG_RE 23
#define TREG_RF 24
#define TREG_FAULT 25

#define CPU_MEMORY 0
#define CPU_TYPE 1
#define CPU_PROFILER 2
#define CDP_1870_P 3
#define CDP_1870_C 4
#define CDP_1870_COLOUR 5
#define V_6845 6
#define V_6847 7
#define V_6847_RAM 8
#define TMS_MEMORY 9
#define VT_RAM 10
#define CDP_1864 11
#define I_8275 12
#define I_8275_RAM 13
#define VIP2KSEQUENCER 14
#define RTCRAM 15

#define TEXT_ASSEMBLER true
#define DIRECT_ASSEMBLER false

#define MEM_LABEL_TYPE true
#define FIND_BRANCH false

#define PSEUDO_DETAILS_X 0
#define PSEUDO_DETAILS_I 1
#define PSEUDO_DETAILS_MI 2
#define PSEUDO_DETAILS_R 3
#define PSEUDO_DETAILS_MR 4

class PseudoCodeDetails
{
public:
    long command;
    int length;
    wxString subCommand;
    wxString commandText;
    wxString parameterText;
};

enum
{
    DUMMY,
    ADD_VX_VY_VZ,
    ADD_VX_KK,
    ADD8_VX_VY_N,
    BEEP_F_KK_N,
    CALL_MMM,
    DRW_VX_KK,
    DRW_VX_VY_N,
    DRW_VX_L_N,
    JP_MMM,
    JZ_VX_KK,
    JNZ_VX_KK,
    JE_I_VX_KK,
    JNE_I_VX_KK,
    LD_B_VX_VY,
    LD_M6AA_VX,
    LD_M8AA_VX,
    LD_RA_MMM,
    LD_RB_MMM,
    LD_I_MMMM,
    LD_VX_KK,
    LD_VX_27KK,
    LD_27KK_VX,
    RND_VX_KK,
    SE_VX_KK,
    SE_VX_M6AA,
    SNE_VX_KK,
    SNE_VX_VY,
    SNE_VX_M6AA,
    SNE_VX_M8AA,
    SNE_X_KK,
    SUB_VX_VY_VZ,
    SUB_VY_VX_VZ,
    SYS_MMM,
    SYS1_AA,
    TAPE_KK,
    TONE_VX_VY,
    FEL1_COMMAND_0,
    FEL1_COMMAND_7,
    FEL1_COMMAND_E,
    FEL2_COMMAND_C,
    FEL2_COMMAND_E,
    FEL3_COMMAND_4,
    FEL3_COMMAND_6,
    FEL3_COMMAND_8,
    FEL3_COMMAND_C,
    FEL3_COMMAND_E,
    STIV_COMMAND_4,
    STIV_COMMAND_5,
    STIV_COMMAND_6,
    STIV_COMMAND_7,
    FPL_COMMAND_C,
    FPL_COMMAND_E,
    CARDTRAN_COMMAND,
    LAST_COMMAND
};

class AssInput
{
public:
	wxString command;
	wxString commandSeperator;
	wxString parameterString[8];
	long parameterValue[8];
	int parameterType[8];
	wxString seperator[8];
	int errorCode;
	int numberOfParameters;
};

class LabelInfo
{
public:
    bool labelDefined;
    bool branchToFound;
    bool outOfRange;
    Word outOfRangeAddress;
};

class DebugWindow : public GuiComx 
{
public:
	DebugWindow(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir);
	~DebugWindow();

	void readDebugConfig();
	void writeDebugConfig();

	void enableDebugGuiMemory();
	void enableChip8DebugGui(bool status);
	void enableDebugGui(bool status);
	void updateAssTabCheck(Word address);
    void cyclePseudoDebug();
    bool checkSingleCommand(Byte command);
    bool checkTrippleCommand(Byte command);
    bool checkQuadrupleCommand(Byte command);
	bool chip8BreakPointCheck();
	void showInstructionTrace();
	void cycleDebug();
	void updateChip8Window(); 
	void updateWindow(); 
	void resetDisplay();
	void assemblerDisplay(wxString buffer);
//	void disassemblerDisplay(wxString buffer);
    void debugTrace(wxString buffer);
	void chip8DebugTrace(wxString buffer);
//	void onEnter(wxCommandEvent& event);
//	void onDebugDis(wxCommandEvent&event);
	void setPauseState();
	void SetDebugMode();
	void SetChip8DebugMode();

	void onLog(wxCommandEvent&event);
	void onClear(wxCommandEvent&event);
    void onTrace(wxCommandEvent&event);
    void onTrace(bool state);
	void onTraceDma(wxCommandEvent&event);
	void onTraceInt(wxCommandEvent&event);
	void onChip8TraceInt(wxCommandEvent&event);
	void onTraceTrap(wxCommandEvent&event);

    void onInt(wxCommandEvent&event);
    void onReset(wxCommandEvent&event);

	void onPauseButton(wxCommandEvent&event);
	void onStepButton(wxCommandEvent&event);
	void onRunButton(wxCommandEvent&event);
	void onRunAddress(wxCommandEvent&event);
	void onBreakPointAddress(wxCommandEvent&event);
	void onTregValue(wxCommandEvent&event);
	void onNumberOfSteps(wxCommandEvent&event);

	void onBreakPointSet(wxCommandEvent&event);
	void onTregSet(wxCommandEvent&event);
	void onTrapSet(wxCommandEvent&event);

	void deleteBreakPoint(wxListEvent&event);
	void deleteTreg(wxListEvent&event);
	void deleteTrap(wxListEvent&event);

	void editBreakPoint(wxListEvent&event);
	void editTreg(wxListEvent&event);
	void editTrap(wxListEvent&event);

	void switchBreakPoint(int item);
	void selectBreakPoint(wxListEvent&event);
	void deselectBreakPoint(wxListEvent&event);
	void keyBreakPoint(wxListEvent&event);

	void switchTreg(int item);
	void selectTreg(wxListEvent&event);
	void deselectTreg(wxListEvent&event);
	void keyTreg(wxListEvent&event);

	void switchTrap(int item);
	void selectTrap(wxListEvent&event);
	void deselectTrap(wxListEvent&event);
	void keyTrap(wxListEvent&event);

	void switchChip8BreakPoint(int item);
	void selectChip8BreakPoint(wxListEvent&event);
	void deselectChip8BreakPoint(wxListEvent&event);
	void keyChip8BreakPoint(wxListEvent&event);

	void onTrapCommand(wxCommandEvent&event);

	void O1(wxCommandEvent&event);
	void O2(wxCommandEvent&event);
	void O3(wxCommandEvent&event);
	void O4(wxCommandEvent&event);
	void O5(wxCommandEvent&event);
	void O6(wxCommandEvent&event);
	void O7(wxCommandEvent&event);

	void D(wxCommandEvent&event);
	void P(wxCommandEvent&event);
	void X(wxCommandEvent&event);
    void T(wxCommandEvent&event);
    void B(wxCommandEvent&event);
    void CH(wxCommandEvent&event);
    void CNTR(wxCommandEvent&event);
	void DF(wxCommandEvent&event);
	void Q(wxCommandEvent&event);
	void IE(wxCommandEvent&event);
    void CIE(wxCommandEvent&event);
	void EF1(wxCommandEvent&event);
	void EF2(wxCommandEvent&event);
	void EF3(wxCommandEvent&event);
	void EF4(wxCommandEvent&event);

	void chip8I(wxCommandEvent&event);
	void Vx(wxCommandEvent&event);
	void R0(wxCommandEvent&event);
	void R1(wxCommandEvent&event);
	void R2(wxCommandEvent&event);
	void R3(wxCommandEvent&event);
	void R4(wxCommandEvent&event);
	void R5(wxCommandEvent&event);
	void R6(wxCommandEvent&event);
	void R7(wxCommandEvent&event);
	void R8(wxCommandEvent&event);
	void R9(wxCommandEvent&event);
	void RA(wxCommandEvent&event);
	void RB(wxCommandEvent&event);
	void RC(wxCommandEvent&event);
	void RD(wxCommandEvent&event);
	void RE(wxCommandEvent&event);
	void RF(wxCommandEvent&event);

	void directAss();
    void setProfileColor(Byte executedColor);
    void onProfilerType(wxCommandEvent&event);
    void onProfilerCounter(wxCommandEvent&event);
    void onProfilerClear(wxCommandEvent&event);
    int getProfilerType()  {return profilerType_;};
    int getProfilerCounter()  {return profilerCounter_;};
	void drawAssCharacter(Word address, int line, int count);
	void onAssEnter(wxCommandEvent&event);
	int setMemLabel(Word address, bool removeMemLabel);
    void onAssAddress(wxCommandEvent&event);
    void onProfilerAddress(wxCommandEvent&event);
    void onAssThumbTrack(wxScrollEvent&event);
    void onProfilerThumbTrack(wxScrollEvent&event);
    void onAssSpinUp(wxSpinEvent&event);
    void onAssSpinUp(wxScrollEvent&event);
    void onProfilerSpinUp(wxScrollEvent&event);
    void assSpinUp();
    void assSpinUpScroll();
    void onAssSpinDown(wxSpinEvent&event);
    void onAssSpinDown(wxScrollEvent&event);
    void onProfilerSpinDown(wxScrollEvent&event);
    void assSpinDown();
    void assSpinDownScroll();
    void onAssSpinPageUp(wxSpinEvent&event);
    void onAssSpinPageUp(wxScrollEvent&event);
    void onProfilerSpinPageUp(wxScrollEvent&event);
	void onAssSpinPageUp();
    void onAssSpinPageDown(wxSpinEvent&event);
    void onAssSpinPageDown(wxScrollEvent&event);
    void onProfilerSpinPageDown(wxScrollEvent&event);
	void onAssSpinPageDown();
	void checkSlotAddressWarning(Word branchAddress);
	bool slotAddress(Word branchAddress);
	void assErrorDisplay(wxString buffer);
	void onAssRangeSpinUp(wxSpinEvent&event);
	void onAssRangeSpinDown(wxSpinEvent&event);
	void showConfiguration();
	void onAssMark(wxCommandEvent&event);
	int markType(long *address, int type);
	void onSaveDebugFile(wxCommandEvent&event);
    void onLaptimeTrigger(wxCommandEvent&event);
    bool getSaveDebugFile() {return saveDebugFile_;};
    int getLapTimeTrigger() {return lapTimeTrigger_;};
	void onClearErrorLog(wxCommandEvent&event);
	Byte getOut1();
	void setOut1(Byte out1);
    void onAssFrom(wxCommandEvent&event);
    void onAssFrom();
    void onAssFromL(wxCommandEvent&event);
    void onAssFromL();
    void onAssFromV(wxCommandEvent&event);
    void onAssFromV();
    void onAssFromAll(wxCommandEvent&event);
    void checkBranch(bool function, Word checkAddress);
	void checkLoadL(bool function, Word checkAddress);
	void checkLoadV();
	void onInsert(wxCommandEvent&event);
	bool branchChangeNeeded(int range, Word address, Word branchAddr);
	void insertByte(Word address, Byte instruction, int branchAddress, bool secondCardtranInsert);
	void onDelete(wxCommandEvent&event);
	void deleteByte(Word address, bool secondCardtranDelete);
	void shortLongBranch();
	void correctionList();
	void changeBranch(Word address, Word branchAddr);
	void onAssSave(wxCommandEvent&event);
	void onAssSave(int range);
	bool onAssLoad(int range);
	void onAssLoadAll(wxCommandEvent&event);
	void loadAll(wxString configFile);
	void onAssSaveSb(wxCommandEvent&event);
	void onAssSaveAll(wxCommandEvent&event);
	void onAssNew(wxCommandEvent&event);
    void assNew(int range);
    void assDefault(wxString fileName, Word start, Word end);
    void scrtValues(bool status, bool Scrt, long CallReg, long CallAddress, long RetReg, long RetAddress);
    void setScrtValues(bool Scrt, long CallReg, long CallAddress, long RetReg, long RetAddress, wxString Game);
    void saveScrtValues(wxString Game);
    void onDebugScrt(wxCommandEvent&event);
    void onDebugCallReg(wxCommandEvent&event);
    void onDebugCallAddress(wxCommandEvent&event);
    void onDebugRetReg(wxCommandEvent&event);
    void onDebugRetAddress(wxCommandEvent&event);
    void onAssStore(wxCommandEvent&event);
    void onAssStore();
    void onAssDir(wxCommandEvent&event);
	bool saveAll(wxString configFile);
	void saveAll();
    void onAssDataView(wxCommandEvent&event);
    void onProfilerDataView(wxCommandEvent&event);
	void AssAddConfig(wxString dir, wxString name, Word programStart, Word programEnd, Word dataEnd, Byte slot);
	void AssStoreConfig(int range, wxString dir, wxString name, Word programStart, Word programEnd, Word dataEnd, Byte slot);
    void AssInitConfig();
    void AssInitLog();
	void onAssCopy(wxCommandEvent&event);
	void onAssDis(wxCommandEvent&event);
    void assDirOld(wxString fileName, long start, long end);
    void assLog(Byte value);
    void addressLog(Word value);
	void stopAssLog();
    void onAssTextChange(wxCommandEvent&event);
	bool findWorkingRang();
    void paintDebugBackground();
    void changeNumberOfDebugLines(int height);

	void onDebugDisplayPage(wxCommandEvent&event); 
	void onDebugDisplayPageSpinUp(wxSpinEvent&event);
	void debugDisplayPageSpinUp(); 
	void onDebugDisplayPageSpinDown(wxSpinEvent&event);
    void DebugDisplayPage();
    void DebugDisplayProfiler();
	void ShowCharacters(Word address, int y);
	void DebugDisplayMap();
	void DebugDisplay();
	void onDebugMemType(wxCommandEvent&event);
	void onDebugExpansionSlot(wxCommandEvent&event);
	void onDebugExpansionRam(wxCommandEvent&event);
	void onDebugExpansionEprom(wxCommandEvent&event);
	void onDebugEmsPage(wxCommandEvent&event);
	void onDebugPager(wxCommandEvent&event);
	void onDebugPortExtender(wxCommandEvent&event);
	void onDebugSaveDump(wxCommandEvent&event);
	void onDebugCopy(wxCommandEvent&event);
	void onDebugCopyStart(wxCommandEvent&event);
	void onDebugCopyEnd(wxCommandEvent&event);
	void onDebugCopyTo(wxCommandEvent&event);
//	void onDebugAssemblerAddress(wxCommandEvent&event);
//	void onDebugDisStart(wxCommandEvent&event);
//	void onDebugDisEnd(wxCommandEvent&event);
//	void onDebugDisLog(wxCommandEvent&event);
	void onDebugDisChip8(wxCommandEvent&event);

	void onEditMemory(wxCommandEvent&event);

	void setMemoryType(int id, int setType);
    void memoryDisplaySetGuiSize(int offset);
	void memoryDisplay();
	Word getAddressMask();
    void DebugDisplayRtcRam();
    void DebugDisplayVip2kSequencer();
    void DebugDisplay1870VideoRam();
    void DebugDisplay1870ColourRam();
	void DebugDisplay1864ColorRam();
    void DebugDisplay8275CharRom();
    void DebugDisplay8275VideoRam();
	void DebugDisplay6845CharRom();
	void DebugDisplay6847CharRom();
	void DebugDisplay6847VideoRam();
	void DebugDisplayTmsRam();
	void DebugDisplayVtRam();

	void onProtectedMode(wxCommandEvent& event);

	Byte debugReadMem(Word address);
	void debugWriteMem(Word address, Byte value);
    void setSwName(wxString swName);
    void updateTitle();
	void updateDebugMenu(bool debugMode);
	void onDebugMode(wxCommandEvent& event);
	void onFxDebugMode();

	void updateChip8DebugMenu(bool debugMode);
	void onChip8DebugMode(wxCommandEvent& event);
	void onChip8PauseButton(wxCommandEvent&event);
	void setChip8PauseState();
	void onChip8StepButton(wxCommandEvent&event);
    void pseudoTrace(Word address);
	wxString getPseudoDefinition(Word* pseudoBaseVar, Word* pseudoMainLoop, bool* chip8register12bit, bool* pseudoLoaded);
    void forcePseudoDefinition(wxString pseudoType, wxString filename, wxString pseudoName);
    void definePseudoCommands();
    wxString pseudoDisassemble(Word address, bool includeDetails, bool showOpcode);
    wxString addDetails();
	void onChip8Trace(wxCommandEvent&event);
	void onChip8ProtectedMode(wxCommandEvent&event);
	void onChip8Log(wxCommandEvent&event);
	void onChip8Clear(wxCommandEvent&event);

	void onChip8BreakPointAddress(wxCommandEvent&event);
	void onChip8BreakPointSet(wxCommandEvent&event);
	void deleteChip8BreakPoint(wxListEvent&event);
	void editChip8BreakPoint(wxListEvent&event);

	void onPercentageClock(wxScrollEvent&event);
	void onChip8PercentageClock(wxScrollEvent&event);

    bool getDebugScrtMode(int computerType) {return conf[computerType].scrtMode_;};
    Byte getDebugCallReg(int computerType) {return conf[computerType].debugCallReg_;};
    Word getDebugCallAddress(int computerType) {return conf[computerType].debugCallAddress_;};
    Byte getDebugRetReg(int computerType) {return conf[computerType].debugRetReg_;};
    Word getDebugRetAddress(int computerType) {return conf[computerType].debugRetAddress_;};

protected:
	void trace();

    wxTextCtrl *traceWindowPointer;
	wxTextCtrl *assInputWindowPointer;
	wxTextCtrl *assErrorWindowPointer;
	wxListCtrl *breakPointWindowPointer;
	wxListCtrl *chip8BreakPointWindowPointer;
	wxListCtrl *tregWindowPointer;
	wxListCtrl *trapWindowPointer;
	wxTextCtrl *registerTextPointer[16];
	wxTextCtrl *chip8varTextPointer[16];
	wxTextCtrl *chip8TraceWindowPointer;
	wxTextCtrl *outTextPointer[8];
	wxTextCtrl *inTextPointer[8];
	wxTextCtrl *efTextPointer[5];
	wxTextCtrl *dfTextPointer;
	wxTextCtrl *qTextPointer;
    wxTextCtrl *ieTextPointer;
    wxTextCtrl *cieTextPointer;
    wxTextCtrl *chTextPointer;
    wxTextCtrl *counterTextPointer;
	wxTextCtrl *dTextPointer;
	wxTextCtrl *pTextPointer;
	wxTextCtrl *xTextPointer;
    wxTextCtrl *tTextPointer;
    wxTextCtrl *bTextPointer;

	bool traceChip8Int_;
	bool traceInt_;
	bool traceDma_;
	bool traceTrap_;
	bool trace_;
	bool chip8Trace_;
	bool xmlLoaded_;
	Word memoryStart_;
	bool chip8ProtectedMode_;
	long chip8Steps_;
	bool performChip8Step_;
	bool additionalChip8Details_;
    int additionalChip8DetailsType_;
    Word additionalDetailsAddress_;
    Word additionalDetailsAddressV2_;
    wxString additionalDetailsPrintStr_;
    wxString additionalDetailsPrintStrV2_;

	double percentageClock_;
	bool saveDebugFile_;
    int lapTimeTrigger_;
    wxString traceString_;
    wxString chipTraceString_;

    wxString dirAssNewDir_;
    bool pseudoLoaded_;

    int profilerType_;
    int profilerCounter_;

private:
	wxMemoryDC dcLine, dcChar, dcAss;
	bool updatingTraceString_;

	wxBitmap *lineBmp[16];
    wxBitmap *assBmp;
    wxBitmap *profilerBmp;
	wxString extractWord(wxString *buffer);
	wxString extractNextWord(wxString *buffer, wxString *seperator);
	void addBreakPoint(); 
	void addChip8BreakPoint(); 
	void addTrap(); 
	void addTreg(); 
	wxString cdp1802disassemble(Word* address, bool includeDetails, bool showOpcode, bool textAssembler, Word start, Word end);
    wxString getShortAddressOrLabel(Word address, bool textAssembler, Word start, Word end);
    wxString getLongAddressOrLabel(Word address, bool textAssembler, Word start, Word end);
    wxString getSubAddressOrLabel(Word address, bool textAssembler, Word start, Word end);
    wxString getLoadAddressOrLabel(Word address, Word start, Word end);
    wxString getReversedLoadAddressOrLabel(Word address, Word start, Word end);
    wxString getLoadAddress(Word address);
    wxString getCurrentAddresssLabel(Word address);
    wxString getHexByte(Word address, bool textAssembler);
    int assemblePseudo(wxString *buffer, Byte* b1, Byte* b2, Byte* b3, Byte* b4);
	AssInput getAssInput(wxString buffer);
	int checkParameterPseudo(AssInput assInput, int32_t* pseudoCode);
	Byte getCardtranAddress(long address);
	int assemble(wxString *buffer, Byte* b1, Byte* b2, Byte* b3, Byte* b4, Byte* b5, Byte* b6, Byte* b7, bool allowX);
	int getByte(AssInput assInput, Byte* b2, bool allowX);
	int getSlot(AssInput assInput, Byte* b2);
	int getWord(AssInput assInput, Byte* b2, Byte* b3, bool allowX);
	int getWordPar2(AssInput assInput, Byte* b2, Byte* b3, bool allowX);
	int getRegisterNumber(AssInput assInput, long* registerNumber, Byte* b4, bool allowX);
	int translateChipParameter(wxString buffer, long* registerNumber, int* type);
	int getRegister(wxString buffer); 

	Word chip8BreakPoints_[64];
	bool chip8BreakPointsSelected_[64];
	int numberOfChip8BreakPoints_;

	Word breakPoints_[64];
	bool breakPointsSelected_[64];
	int numberOfBreakPoints_;

	Byte traps_[64][9];
	bool trapsSelected_[64];
	int numberOfTraps_;

	Word tregs_[64][2];
	bool tregsSelected_[64];
	int numberOfTregs_;

	Word lastR_[16];
	Word lastI_;
	Word lastPC_;
	Byte lastD_;
	Byte lastP_;
	Byte lastX;
    Byte lastT_;
    Byte lastB_;
	Byte lastDf_;
	Byte lastQ_;
    Byte lastIe_;
    Byte lastCie_;
    Byte lastCh_;
    Byte lastCounter_;
	Byte lastEf1_;
	Byte lastEf2_;
	Byte lastEf3_;
	Byte lastEf4_;
	Word lastOut_[8];
	Byte lastIn_[8];

	long debugAddress_;
	bool protectedMode_;
	bool performStep_;
	long steps_;
	wxString debugDir_;

	wxBitmap pauseOnBitmap;
	wxBitmap pauseOffBitmap;
	wxString swName_;

	bool spinning_;

	wxString pseudoType_;

	bool showInstructionTrap_;
    bool dataViewDump;
    bool dataViewProfiler;
	Word showInstructionTrapAddress_;
	
	int selectedBreakPoint_;
	int selectedTrap_;
	int selectedTreg_;
	int selectedChip8BreakPoint_;

	wxBitmap uncheckBitmap_;
	wxBitmap checkedBitmap_;
	int uncheckButton_;
	int checkedButton_;
	wxImageList *imageList_;

	wxFile dirAssLogFile_;
	bool dirAssLog_;
    bool writingToLog_;
	int lastLogValue_;

	Word dirAssStart_;
	Word dirAssEnd_;
	Word dirAssAddress_;
	int	workingRange_;
	int	shownRange_;
	int	lastRange_;

	wxString dirAssConfigFile_;
	wxString dirAssConfigFileDir_;

	wxString lastAssError_;

	Word branchAddressTable[65536];
	bool branchAddressTableCorrection[65536];
    
    int disassemblePass_;
    bool disassembleAgain_;
    
    LabelInfo labelInfo_[65536];

	int numberOfDebugLines_;
    int lineSpace_;
    int charWidth_;
    int assWidth_;
    int profilerWidth_;
    
	wxTextFile inFile;
    wxTextFile outputTextFile;
    
    size_t psuedoNumber_;
    vector<PseudoCodeDetails> pseudoCodeDetails_;

    size_t singleByteCommandNumber_;
    vector<Byte> singleByteCommand_;

    size_t trippleByteCommandNumber_;
    vector<Byte> trippleByteCommand_;

    size_t quadrupleByteCommandNumber_;
    vector<Byte> quadrupleByteCommand_;

    size_t jumpCommandNumber_;
    vector<Byte> jumpCommand_;
    vector<Word> jumpMask_;
    vector<Word> jumpOffset_;
    
    size_t branchCommandNumber_;
    vector<Byte> branchCommand_;
    vector<Byte> branchMask_;

    size_t decimalBranchCommandNumber_;
    vector<Byte> decimalBranchCommand_;    
    
	wxString commandSyntaxFile_;
	DECLARE_EVENT_TABLE()
};

#endif  // DEBUG_H

