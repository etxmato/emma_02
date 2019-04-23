/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/
 
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#ifdef __WXMSW__
#include "wx/msw/private.h"
#endif

#if !wxUSE_COMBOCTRL
    #error "Please set wxUSE_COMBOCTRL to 1 and rebuild the library."
#endif

#include "wx/xrc/xmlres.h"
#include "wx/spinctrl.h"

#include "main.h"
#include "guifred.h"
#include "pixie.h"
#include "http.h"
#include "wx/wfstream.h"

enum
{
	FRED1_GAME_NONE,
    FRED1_GAME_21,
    FRED1_GAME_21_II,
    FRED1_GAME_ACEY_DUECY,
    FRED1_GAME_ADD_DRILL,
    FRED1_GAME_ANIMATE,
    FRED1_GAME_BINARY,
    FRED1_GAME_BOWLING,
    FRED1_GAME_CALCULATE,
    FRED1_GAME_CARDTRAN,
    FRED1_GAME_CLUE,
    FRED1_GAME_DEDUCE,
    FRED1_GAME_DEDUCE_LEDS,
    FRED1_GAME_DEMO,
    FRED1_GAME_DRAW,
    FRED1_GAME_ERASE,
    FRED1_GAME_ESP,
    FRED1_GAME_ESP_II,
    FRED1_GAME_FLIP,
    FRED1_GAME_FLIP_II,
    FRED1_GAME_HILO,
    FRED1_GAME_JACKPOT,
    FRED1_GAME_LIFE,
    FRED1_GAME_LUCKYPATH,
    FRED1_GAME_MATCH,
    FRED1_GAME_MATCH_II,
    FRED1_GAME_MEMORY_TEST,
    FRED1_GAME_MINIKRIEG,
    FRED1_GAME_NIMNET,
    FRED1_GAME_PATTERN_PUZZLE,
    FRED1_GAME_PROG_APT,
    FRED1_GAME_SLIDE_PUZZLE_I,
    FRED1_GAME_SLIDE_PUZZLE_II,
	FRED1_GAME_SPACE_WAR,
    FRED1_GAME_SPOOK,
    FRED1_GAME_SPOT_SPEEDWAY_1,
    FRED1_GAME_SPOT_SPEEDWAY_2,
    FRED1_GAME_TIC_TAC_TOE,
    FRED1_GAME_TARGET_1,
	FRED1_GAME_END
};

wxString gameList[] =
{
    "",
    "21 I.bin",
    "21 II.bin",
    "Acey Duecy.bin",
    "Add Drill.bin",
    "Animate Demo.bin",
    "Binary.bin",
    "Bowling.bin",
    "Calculate.bin",
    "Cardtran.bin",
    "Clue.bin",
    "Deduce.bin",
    "Deduce-leds.bin",
    "Demo.bin",
    "Draw.bin",
    "Erase.bin",
    "ESP I.bin",
    "ESP II.bin",
    "Flip I.bin",
    "Flip II.bin",
    "Hi-Lo.bin",
    "Jackpot.bin",
    "Life.bin",
    "Luckypath.bin",
    "Match I.bin",
    "Match II.bin",
    "Memory Test.bin",
    "Minikrieg.bin",
    "Nimnet.bin",
    "Pattern Puzzle I.bin",
    "Prog. Apt. Test.bin",
    "Slide Puzzle I.bin",
    "Slide Puzzle II.bin",
    "Space War.bin",
    "Spook.bin",
    "Spot Speedway 1.bin",
    "Spot Speedway 2.bin",
    "Target",
    "Tic-Tac-Toe.bin",
	""
};

int cardValue[FRED1_GAME_END+1][FRED1_CARDS] =
{
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // no game
    { 0xbd, 0xcc, 0x78, 0x73, 0xb1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // 21 I
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // 21 II
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Acey Duecy
    { 0xFe, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1  }, // Add Drill
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1  }, // Animate Demo
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Binary
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Bowling
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Calculate
    { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Cardtran
    { 0, 1, 2, 3, 4, 5, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Clue
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Deduce
    { 0, 0x11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Deduce-leds
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Demo
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Draw
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Erase
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // ESP I
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // ESP II
    { 0, 0x6d, 0x97, 0x9e, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Flip
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Flip II
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Hi-Lo
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Jackpot
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Life
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Luckypath
    { 0xff, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Match
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Match II
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Memory Test
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Minikrieg
    { 0x1a1, 0xa2, 0xb1, 0xb2, 0xb3, 0xb4,  0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xe1, 0xe2, 0xe3, 0xe4, 0xf1, 0xf2, -1, -1 }, // Nimnet
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Pattern Puzzle
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Prog. Apt. Test
    { 0, 0x31, 0x33, 0x35, -1, -1, 0x6, -1, -1, -1, -1, -1, 0x1, 0xff, -1, -1, -1, -1, 0xfa, -1, -1, -1, -1, -1 }, // Slide Puzzle I
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Slide Puzzle II
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Space War
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Spook
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Spot Speedway 1
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Spot Speedway 2
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Tic Tac Toe
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }, // Target 1
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }  // no game
};

BEGIN_EVENT_TABLE(GuiFred, GuiVip)
    EVT_TEXT(XRCID("RamSWFRED1"), GuiMain::onRamSWText)
    EVT_COMBOBOX(XRCID("RamSWFRED1"), GuiMain::onRamSWText)
    EVT_BUTTON(XRCID("RamSWButtonFRED1"), GuiMain::onRamSW)
    EVT_BUTTON(XRCID("CardButtonFRED1_00"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_01"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_02"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_03"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_04"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_05"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_06"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_07"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_08"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_09"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_10"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_11"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_12"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_13"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_14"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_15"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_16"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_17"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_18"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_19"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_20"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_21"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_22"), GuiFred::onCardButton)
    EVT_BUTTON(XRCID("CardButtonFRED1_23"), GuiFred::onCardButton)

    EVT_BUTTON(XRCID("ScreenDumpFileButtonFRED1"), GuiMain::onScreenDumpFile)
    EVT_TEXT(XRCID("ScreenDumpFileFRED1"), GuiMain::onScreenDumpFileText)
    EVT_COMBOBOX(XRCID("ScreenDumpFileFRED1"), GuiMain::onScreenDumpFileText)

    EVT_BUTTON(XRCID("CasButtonFRED1"), GuiMain::onCassette)
    EVT_BUTTON(XRCID("EjectCasFRED1"), GuiMain::onCassetteEject)
    EVT_TEXT(XRCID("WavFileFRED1"), GuiMain::onCassetteText)

    EVT_SPIN_UP(XRCID("ZoomSpinFRED1"), GuiMain::onZoomUp)
    EVT_SPIN_DOWN(XRCID("ZoomSpinFRED1"), GuiMain::onZoomDown)
    EVT_TEXT(XRCID("ZoomValueFRED1"), GuiMain::onZoomValue)
    EVT_BUTTON(XRCID("FullScreenF3FRED1"), GuiMain::onFullScreen)
    EVT_BUTTON(XRCID("ScreenDumpF5FRED1"), GuiMain::onScreenDump)
    EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeFRED1"), GuiMain::onVolume)
    EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeFRED1"), GuiMain::onVolume)
    EVT_BUTTON(XRCID("KeyMapFRED1"), Main::onHexKeyDef)
    EVT_BUTTON(XRCID("ColoursFRED1"), Main::onColoursDef)
    EVT_TEXT(XRCID("BeepFrequencyFRED1"), GuiMain::onBeepFrequency)
    EVT_CHOICE(XRCID("RamFRED1"), GuiMain::onChoiceRam)
    EVT_CHOICE(XRCID("KeyPadModeFRED1"), GuiFred::onChoiceKeyPadMode)
    EVT_CHOICE(XRCID("TapeFormatFRED1"), GuiFred::onTapeFormat)

    EVT_BUTTON(XRCID("SaveButtonFRED1"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("LoadButtonFRED1"), GuiMain::onLoadButton)
    EVT_TEXT(XRCID("SaveStartFRED1"), GuiMain::onSaveStart)
    EVT_TEXT(XRCID("SaveEndFRED1"), GuiMain::onSaveEnd)
    EVT_CHECKBOX(XRCID("ControlWindowsFRED1"), GuiFred::onFred2ControlWindows)
    EVT_CHECKBOX(XRCID("AutoBootFRED1"), GuiFred::onAutoBoot)
    EVT_CHECKBOX(XRCID("stopToneFRED1"), GuiFred::onStopTone)
    EVT_CHECKBOX(XRCID("TapeStartFRED1"), GuiFred::onTapeStart)
    EVT_CHECKBOX(XRCID("InterlaceFRED1"), GuiMain::onInterlace)

    EVT_CHECKBOX(XRCID("TurboFRED1"), GuiMain::onTurbo)
    EVT_TEXT(XRCID("TurboClockFRED1"), GuiMain::onTurboClock)
    EVT_CHECKBOX(XRCID("AutoCasLoadFRED1"), GuiMain::onAutoLoad)
    EVT_BUTTON(XRCID("CasLoadFRED1"), GuiMain::onCassetteLoad)
    EVT_BUTTON(XRCID("CasSaveFRED1"), GuiMain::onCassetteSave)
    EVT_BUTTON(XRCID("CasStopFRED1"), GuiMain::onCassetteStop)
    EVT_BUTTON(XRCID("CasPauseFRED1"), GuiMain::onCassettePause)
    EVT_BUTTON(XRCID("RealCasLoadFRED1"), GuiMain::onRealCas)

    EVT_TEXT(XRCID("ShowAddressFRED1"), GuiMain::onLedTimer)
    EVT_TEXT(XRCID("BootAddressFRED1"), GuiElf::onBootAddress)

    EVT_TEXT(XRCID("RamSWFRED2"), GuiMain::onRamSWText)
    EVT_COMBOBOX(XRCID("RamSWFRED2"), GuiFred::onRamSWText)
    EVT_BUTTON(XRCID("RamSWButtonFRED2"), GuiMain::onRamSW)

    EVT_BUTTON(XRCID("ScreenDumpFileButtonFRED2"), GuiMain::onScreenDumpFile)
    EVT_TEXT(XRCID("ScreenDumpFileFRED2"), GuiMain::onScreenDumpFileText)
    EVT_COMBOBOX(XRCID("ScreenDumpFileFRED2"), GuiMain::onScreenDumpFileText)

    EVT_BUTTON(XRCID("CasButtonFRED2"), GuiMain::onCassette)
    EVT_BUTTON(XRCID("EjectCasFRED2"), GuiMain::onCassetteEject)
    EVT_TEXT(XRCID("WavFileFRED2"), GuiMain::onCassetteText)

    EVT_SPIN_UP(XRCID("ZoomSpinFRED2"), GuiMain::onZoomUp)
    EVT_SPIN_DOWN(XRCID("ZoomSpinFRED2"), GuiMain::onZoomDown)
    EVT_TEXT(XRCID("ZoomValueFRED2"), GuiMain::onZoomValue)
    EVT_BUTTON(XRCID("FullScreenF3FRED2"), GuiMain::onFullScreen)
    EVT_BUTTON(XRCID("ScreenDumpF5FRED2"), GuiMain::onScreenDump)
    EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeFRED2"), GuiMain::onVolume)
    EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeFRED2"), GuiMain::onVolume)
    EVT_BUTTON(XRCID("KeyMapFRED2"), Main::onHexKeyDef)
    EVT_BUTTON(XRCID("ColoursFRED2"), Main::onColoursDef)
    EVT_TEXT(XRCID("BeepFrequencyFRED2"), GuiMain::onBeepFrequency)
    EVT_CHOICE(XRCID("RamFRED2"), GuiMain::onChoiceRam)
    EVT_CHOICE(XRCID("KeyPadModeFRED2"), GuiFred::onChoiceKeyPadMode)
    EVT_CHOICE(XRCID("TapeFormatFRED2"), GuiFred::onTapeFormat)

    EVT_BUTTON(XRCID("SaveButtonFRED2"), GuiMain::onSaveButton)
    EVT_BUTTON(XRCID("LoadButtonFRED2"), GuiMain::onLoadButton)
    EVT_TEXT(XRCID("SaveStartFRED2"), GuiMain::onSaveStart)
    EVT_TEXT(XRCID("SaveEndFRED2"), GuiMain::onSaveEnd)
    EVT_CHECKBOX(XRCID("ControlWindowsFRED2"), GuiFred::onFred2ControlWindows)
    EVT_CHECKBOX(XRCID("AutoBootFRED2"), GuiFred::onAutoBoot)
    EVT_CHECKBOX(XRCID("CoinArcadeControlFRED2"), GuiFred::onCoinArcadeControl)
    EVT_CHECKBOX(XRCID("stopToneFRED2"), GuiFred::onStopTone)
    EVT_CHECKBOX(XRCID("TapeStartFRED2"), GuiFred::onTapeStart)
    EVT_CHECKBOX(XRCID("InterlaceFRED2"), GuiMain::onInterlace)

    EVT_CHECKBOX(XRCID("TurboFRED2"), GuiMain::onTurbo)
    EVT_TEXT(XRCID("TurboClockFRED2"), GuiMain::onTurboClock)
    EVT_CHECKBOX(XRCID("AutoCasLoadFRED2"), GuiMain::onAutoLoad)
    EVT_BUTTON(XRCID("CasLoadFRED2"), GuiMain::onCassetteLoad)
    EVT_BUTTON(XRCID("CasSaveFRED2"), GuiMain::onCassetteSave)
    EVT_BUTTON(XRCID("CasStopFRED2"), GuiMain::onCassetteStop)
    EVT_BUTTON(XRCID("CasPauseFRED2"), GuiMain::onCassettePause)
    EVT_BUTTON(XRCID("RealCasLoadFRED2"), GuiMain::onRealCas)

    EVT_TEXT(XRCID("ShowAddressFRED2"), GuiMain::onLedTimer)
    EVT_TEXT(XRCID("BootAddressFRED2"), GuiElf::onBootAddress)

END_EVENT_TABLE()

GuiFred::GuiFred(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiVip(title, pos, size, mode, dataDir, iniDir)
{
}

void GuiFred::readFred1Config()
{
    selectedComputer_ = FRED1;
    
    conf[FRED1].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "FRED1" + pathSeparator_;
    conf[FRED1].mainDir_ = readConfigDir("/Dir/FRED1/Main", dataDir_ + "FRED1" + pathSeparator_);
    
    conf[FRED1].ramDir_ = readConfigDir("/Dir/FRED1/Main_Ram_File", dataDir_ + "FRED1"  + pathSeparator_);
    conf[FRED1].screenDumpFileDir_ = readConfigDir("/Dir/FRED1/Video_Dump_File", dataDir_ + "FRED1" + pathSeparator_);
    conf[FRED1].wavFileDir_[0] = readConfigDir("/Dir/FRED1/Wav_File", dataDir_ + "FRED1" + pathSeparator_);

    conf[FRED1].ram_ = configPointer->Read("/FRED1/Main_Ram_File", "Space War.bin");
    conf[FRED1].screenDumpFile_ = configPointer->Read("/FRED1/Video_Dump_File", "screendump.png");
    conf[FRED1].wavFile_[0] = configPointer->Read("/FRED1/Wav_File", "");

    wxString defaultZoom;
    defaultZoom.Printf("%2.2f", 2.0);
    conf[FRED1].zoom_ = configPointer->Read("/FRED1/Zoom", defaultZoom);
    wxString defaultClock;
    defaultClock.Printf("%1.2f", 1.0);
    conf[FRED1].clock_ = configPointer->Read("/FRED1/Clock_Speed", defaultClock);
    
    wxString defaultTimer;
    defaultTimer.Printf("%d", 100);
    conf[FRED1].ledTime_ = configPointer->Read("/FRED1/Led_Update_Frequency", defaultTimer);

    conf[FRED1].beepFrequency_ = 640;
    conf[FRED1].turboClock_ = configPointer->Read("/FRED1/Turbo_Clock_Speed", "15");
    configPointer->Read("/FRED1/Enable_Turbo_Cassette", &conf[FRED1].turbo_, true);
    configPointer->Read("/FRED1/Enable_Auto_Cassette", &conf[FRED1].autoCassetteLoad_, true);
    configPointer->Read("/FRED1/Enable_Real_Cassette", &conf[FRED1].realCassetteLoad_, false);
    conf[FRED1].volume_ = (int)configPointer->Read("/FRED1/Volume", 25l);
    conf[FRED1].ramType_ = (int)configPointer->Read("/FRED1/Ram_Type", 0l);
    elfConfiguration[FRED1].keyboardType = (int)configPointer->Read("/FRED1/KeyPadMode", 0l);
    elfConfiguration[FRED1].tapeFormat_ = (int)configPointer->Read("/FRED1/TapeFormat", 0l);
    configPointer->Read("/FRED1/Open_Control_Windows", &elfConfiguration[FRED1].useElfControlWindows, true);
    configPointer->Read("/FRED1/Enable_Interlace", &conf[FRED1].interlace_, false);

    wxString defaultScale;
    defaultScale.Printf("%i", 3);
    conf[FRED1].xScale_ = configPointer->Read("/FRED1/Window_Scale_Factor_X", defaultScale);
    
	elfConfiguration[FRED1].coinArcadeControl_ = false;
    configPointer->Read("/FRED1/Enable_Auto_Boot", &elfConfiguration[FRED1].autoBoot, true);
    configPointer->Read("/FRED1/Enable_Stop_Tone", &elfConfiguration[FRED1].stopTone, true);
    configPointer->Read("/FRED1/Enable_Tape_Start", &elfConfiguration[FRED1].tapeStart, true);

    long value;
    wxString bootAddress = configPointer->Read("/FRED1/Boot_Address", "0001");
    if (!bootAddress.ToLong(&value, 16))
        value = 0x1;
    conf[FRED1].bootAddress_ = value;

    conf[FRED1].gameId_ = -1;
    guiStartup_ = true;

    if (mode_.gui)
    {
        XRCCTRL(*this, "RamSWFRED1", wxComboBox)->SetValue(conf[FRED1].ram_);
        XRCCTRL(*this, "ScreenDumpFileFRED1", wxComboBox)->SetValue(conf[FRED1].screenDumpFile_);
        XRCCTRL(*this, "ZoomValueFRED1", wxTextCtrl)->ChangeValue(conf[FRED1].zoom_);
        clockTextCtrl[FRED1]->ChangeValue(conf[FRED1].clock_);
        XRCCTRL(*this, "TurboClockFRED1", wxTextCtrl)->SetValue(conf[FRED1].turboClock_);
        XRCCTRL(*this, "TurboFRED1", wxCheckBox)->SetValue(conf[FRED1].turbo_);
        turboGui("FRED1");
        XRCCTRL(*this, "AutoCasLoadFRED1", wxCheckBox)->SetValue(conf[FRED1].autoCassetteLoad_);
        
        XRCCTRL(*this, "VolumeFRED1", wxSlider)->SetValue(conf[FRED1].volume_);
        XRCCTRL(*this, "WavFileFRED1", wxTextCtrl)->SetValue(conf[FRED1].wavFile_[0]);
        XRCCTRL(*this, "RamFRED1", wxChoice)->SetSelection(conf[FRED1].ramType_);
        XRCCTRL(*this, "KeyPadModeFRED1", wxChoice)->SetSelection(elfConfiguration[FRED1].keyboardType);
        XRCCTRL(*this, "TapeFormatFRED1", wxChoice)->SetSelection(elfConfiguration[FRED1].tapeFormat_);
        XRCCTRL(*this,"AddressText1FRED1", wxStaticText)->Enable(elfConfiguration[FRED1].useElfControlWindows);
        XRCCTRL(*this,"AddressText2FRED1", wxStaticText)->Enable(elfConfiguration[FRED1].useElfControlWindows);
        XRCCTRL(*this, "ControlWindowsFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].useElfControlWindows);
        XRCCTRL(*this, "ControlWindowsFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].useElfControlWindows);
        XRCCTRL(*this, "ShowAddressFRED1", wxTextCtrl)->ChangeValue(conf[FRED1].ledTime_);
        XRCCTRL(*this,"ShowAddressFRED1", wxTextCtrl)->Enable(elfConfiguration[FRED1].useElfControlWindows);
        XRCCTRL(*this, "AutoBootFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].autoBoot);
        XRCCTRL(*this, "stopToneFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].stopTone);
        XRCCTRL(*this, "TapeStartFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].tapeStart);
        XRCCTRL(*this, "InterlaceFRED1", wxCheckBox)->SetValue(conf[FRED1].interlace_);
        XRCCTRL(*this, "BootAddressFRED1", wxTextCtrl)->SetValue(bootAddress);
    }
    
    setGameId(conf[FRED1].ram_);
	guiStartup_ = false;
}

void GuiFred::writeFred1DirConfig()
{
    writeConfigDir("/Dir/FRED1/Main", conf[FRED1].mainDir_);
    writeConfigDir("/Dir/FRED1/Main_Ram_File", conf[FRED1].ramDir_);
    writeConfigDir("/Dir/FRED1/Video_Dump_File", conf[FRED1].screenDumpFileDir_);
    writeConfigDir("/Dir/FRED1/Wav_File", conf[FRED1].wavFileDir_[0]);
}

void GuiFred::writeFred1Config()
{
    wxString buffer;

    configPointer->Write("/FRED1/Main_Ram_File", conf[FRED1].ram_);
    configPointer->Write("/FRED1/Video_Dump_File", conf[FRED1].screenDumpFile_);
    configPointer->Write("/FRED1/Wav_File", conf[FRED1].wavFile_[0]);

    configPointer->Write("/FRED1/Zoom", conf[FRED1].zoom_);
    configPointer->Write("/FRED1/Clock_Speed", conf[FRED1].clock_);
    configPointer->Write("/FRED1/Turbo_Clock_Speed", conf[FRED1].turboClock_);
    configPointer->Write("/FRED1/Enable_Turbo_Cassette", conf[FRED1].turbo_);
    configPointer->Write("/FRED1/Enable_Auto_Cassette", conf[FRED1].autoCassetteLoad_);
    configPointer->Write("/FRED1/Enable_Real_Cassette", conf[FRED1].realCassetteLoad_);
    configPointer->Write("/FRED1/Volume", conf[FRED1].volume_);
    configPointer->Write("/FRED1/Ram_Type", conf[FRED1].ramType_);
    configPointer->Write("/FRED1/KeyPadMode", elfConfiguration[FRED1].keyboardType);
    configPointer->Write("/FRED1/TapeFormat", elfConfiguration[FRED1].tapeFormat_);
    configPointer->Write("/FRED1/Open_Control_Windows", elfConfiguration[FRED1].useElfControlWindows);
    configPointer->Write("/FRED1/Led_Update_Frequency", conf[FRED1].ledTime_);
    configPointer->Write("/FRED1/Enable_Auto_Boot", elfConfiguration[FRED1].autoBoot);
    configPointer->Write("/FRED1/Enable_Stop_Tone", elfConfiguration[FRED1].stopTone);
    configPointer->Write("/FRED1/Enable_Tape_Start", elfConfiguration[FRED1].tapeStart);
    configPointer->Write("/FRED1/Enable_Interlace", conf[FRED1].interlace_);
    buffer.Printf("%04X", (unsigned int)conf[FRED1].bootAddress_);
    configPointer->Write("/FRED1/Boot_Address", buffer);
}

void GuiFred::readFred1WindowConfig()
{
    conf[FRED1].pixieX_ = (int)configPointer->Read("/FRED1/Window_Position_Pixie_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[FRED1].pixieY_ = (int)configPointer->Read("/FRED1/Window_Position_Pixie_Y", mainWindowY_);
    conf[FRED1].mainX_ = (int)configPointer->Read("/FRED1/Window_Position_X",  mainWindowX_);
    conf[FRED1].mainY_ = (int)configPointer->Read("/FRED1/Window_Position_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
}

void GuiFred::writeFred1WindowConfig()
{
    if (conf[FRED1].pixieX_ > 0)
        configPointer->Write("/FRED1/Window_Position_Pixie_X", conf[FRED1].pixieX_);
    if (conf[FRED1].pixieY_ > 0)
        configPointer->Write("/FRED1/Window_Position_Pixie_Y", conf[FRED1].pixieY_);
    if (conf[FRED1].mainX_ > 0)
        configPointer->Write("/FRED1/Window_Position_X", conf[FRED1].mainX_);
    if (conf[FRED1].mainY_ > 0)
        configPointer->Write("/FRED1/Window_Position_Y", conf[FRED1].mainY_);
}

void GuiFred::onFred1ControlWindows(wxCommandEvent&event)
{
    elfConfiguration[FRED1].useElfControlWindows = event.IsChecked();
    
    if (mode_.gui)
    {
        XRCCTRL(*this,"ShowAddressFRED1",wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText1FRED1",wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText2FRED1",wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
    }
    
    if (runningComputer_ == FRED1)
        p_Fred->Show(elfConfiguration[FRED1].useElfControlWindows);
}

void GuiFred::readFred2Config()
{
    selectedComputer_ = FRED2;
    
    conf[FRED2].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "FRED2" + pathSeparator_;
    conf[FRED2].mainDir_ = readConfigDir("/Dir/FRED2/Main", dataDir_ + "FRED2" + pathSeparator_);
    
    conf[FRED2].ramDir_ = readConfigDir("/Dir/FRED2/Main_Ram_File", dataDir_ + "FRED2"  + pathSeparator_);
    conf[FRED2].screenDumpFileDir_ = readConfigDir("/Dir/FRED2/Video_Dump_File", dataDir_ + "FRED2" + pathSeparator_);
    conf[FRED2].wavFileDir_[0] = readConfigDir("/Dir/FRED2/Wav_File", dataDir_ + "FRED2" + pathSeparator_);
    
    conf[FRED2].ram_ = configPointer->Read("/FRED2/Main_Ram_File", "Curses Foiled Again.bin");
    conf[FRED2].screenDumpFile_ = configPointer->Read("/FRED2/Video_Dump_File", "screendump.png");
    conf[FRED2].wavFile_[0] = configPointer->Read("/FRED2/Wav_File", "");
    
    wxString defaultZoom;
    defaultZoom.Printf("%2.2f", 2.0);
    conf[FRED2].zoom_ = configPointer->Read("/FRED2/Zoom", defaultZoom);
    wxString defaultClock;
    defaultClock.Printf("%1.2f", 1.0);
    conf[FRED2].clock_ = configPointer->Read("/FRED2/Clock_Speed", defaultClock);
    
    wxString defaultTimer;
    defaultTimer.Printf("%d", 100);
    conf[FRED2].ledTime_ = configPointer->Read("/FRED2/Led_Update_Frequency", defaultTimer);
    
    conf[FRED2].beepFrequency_ = 640;
    conf[FRED2].turboClock_ = configPointer->Read("/FRED2/Turbo_Clock_Speed", "15");
    configPointer->Read("/FRED2/Enable_Turbo_Cassette", &conf[FRED2].turbo_, true);
    configPointer->Read("/FRED2/Enable_Auto_Cassette", &conf[FRED2].autoCassetteLoad_, true);
    configPointer->Read("/FRED2/Enable_Real_Cassette", &conf[FRED2].realCassetteLoad_, false);
    conf[FRED2].volume_ = (int)configPointer->Read("/FRED2/Volume", 25l);
    conf[FRED2].ramType_ = (int)configPointer->Read("/FRED2/Ram_Type", 2l);
    elfConfiguration[FRED2].keyboardType = (int)configPointer->Read("/FRED2/KeyPadMode", 0l);
    elfConfiguration[FRED2].tapeFormat_ = (int)configPointer->Read("/FRED2/TapeFormat", 0l);
    configPointer->Read("/FRED2/Open_Control_Windows", &elfConfiguration[FRED2].useElfControlWindows, true);
    configPointer->Read("/FRED2/Enable_Interlace", &conf[FRED2].interlace_, false);
    
    wxString defaultScale;
    defaultScale.Printf("%i", 3);
    conf[FRED2].xScale_ = configPointer->Read("/FRED2/Window_Scale_Factor_X", defaultScale);
    
    configPointer->Read("/FRED1/Enable_CoinArcadeControl", &elfConfiguration[FRED2].coinArcadeControl_, true);
    configPointer->Read("/FRED2/Enable_Auto_Boot", &elfConfiguration[FRED2].autoBoot, true);
    configPointer->Read("/FRED2/Enable_Stop_Tone", &elfConfiguration[FRED2].stopTone, true);
    configPointer->Read("/FRED2/Enable_Tape_Start", &elfConfiguration[FRED2].tapeStart, true);
    
    long value;
    wxString bootAddress = configPointer->Read("/FRED2/Boot_Address", "0001");
    if (!bootAddress.ToLong(&value, 16))
        value = 0;
    conf[FRED2].bootAddress_ = value;
    
    conf[FRED2].gameId_ = -1;

    if (mode_.gui)
    {
        XRCCTRL(*this, "RamSWFRED2", wxComboBox)->SetValue(conf[FRED2].ram_);
        XRCCTRL(*this, "ScreenDumpFileFRED2", wxComboBox)->SetValue(conf[FRED2].screenDumpFile_);
        XRCCTRL(*this, "ZoomValueFRED2", wxTextCtrl)->ChangeValue(conf[FRED2].zoom_);
        clockTextCtrl[FRED2]->ChangeValue(conf[FRED2].clock_);
        XRCCTRL(*this, "TurboClockFRED2", wxTextCtrl)->SetValue(conf[FRED2].turboClock_);
        XRCCTRL(*this, "TurboFRED2", wxCheckBox)->SetValue(conf[FRED2].turbo_);
        turboGui("FRED2");
        XRCCTRL(*this, "AutoCasLoadFRED2", wxCheckBox)->SetValue(conf[FRED2].autoCassetteLoad_);
        
        XRCCTRL(*this, "VolumeFRED2", wxSlider)->SetValue(conf[FRED2].volume_);
        XRCCTRL(*this, "WavFileFRED2", wxTextCtrl)->SetValue(conf[FRED2].wavFile_[0]);
        XRCCTRL(*this, "RamFRED2", wxChoice)->SetSelection(conf[FRED2].ramType_);
        XRCCTRL(*this, "KeyPadModeFRED2", wxChoice)->SetSelection(elfConfiguration[FRED2].keyboardType);
        XRCCTRL(*this, "TapeFormatFRED2", wxChoice)->SetSelection(elfConfiguration[FRED2].tapeFormat_);
        XRCCTRL(*this,"AddressText1FRED2", wxStaticText)->Enable(elfConfiguration[FRED2].useElfControlWindows);
        XRCCTRL(*this,"AddressText2FRED2", wxStaticText)->Enable(elfConfiguration[FRED2].useElfControlWindows);
        XRCCTRL(*this, "ControlWindowsFRED2", wxCheckBox)->SetValue(elfConfiguration[FRED2].useElfControlWindows);
        XRCCTRL(*this, "ControlWindowsFRED2", wxCheckBox)->SetValue(elfConfiguration[FRED2].useElfControlWindows);
        XRCCTRL(*this, "ShowAddressFRED2", wxTextCtrl)->ChangeValue(conf[FRED2].ledTime_);
        XRCCTRL(*this,"ShowAddressFRED2", wxTextCtrl)->Enable(elfConfiguration[FRED2].useElfControlWindows);
        XRCCTRL(*this, "AutoBootFRED2", wxCheckBox)->SetValue(elfConfiguration[FRED2].autoBoot);
//        XRCCTRL(*this, "CoinArcadeControlFRED2", wxCheckBox)->SetValue(elfConfiguration[FRED2].coinArcadeControl_);
        XRCCTRL(*this, "stopToneFRED2", wxCheckBox)->SetValue(elfConfiguration[FRED2].stopTone);
        XRCCTRL(*this, "TapeStartFRED2", wxCheckBox)->SetValue(elfConfiguration[FRED2].tapeStart);
        XRCCTRL(*this, "InterlaceFRED2", wxCheckBox)->SetValue(conf[FRED2].interlace_);
        XRCCTRL(*this, "BootAddressFRED2", wxTextCtrl)->SetValue(bootAddress);
    }

    conf[FRED2].gameId_ = 1;
//    checkGameFred2(conf[FRED2].ram_);
}

void GuiFred::writeFred2DirConfig()
{
    writeConfigDir("/Dir/FRED2/Main", conf[FRED2].mainDir_);
    writeConfigDir("/Dir/FRED2/Main_Ram_File", conf[FRED2].ramDir_);
    writeConfigDir("/Dir/FRED2/Video_Dump_File", conf[FRED2].screenDumpFileDir_);
    writeConfigDir("/Dir/FRED2/Wav_File", conf[FRED2].wavFileDir_[0]);
}

void GuiFred::writeFred2Config()
{
    wxString buffer;
    
    configPointer->Write("/FRED2/Main_Ram_File", conf[FRED2].ram_);
    configPointer->Write("/FRED2/Video_Dump_File", conf[FRED2].screenDumpFile_);
    configPointer->Write("/FRED2/Wav_File", conf[FRED2].wavFile_[0]);
    
    configPointer->Write("/FRED2/Zoom", conf[FRED2].zoom_);
    configPointer->Write("/FRED2/Clock_Speed", conf[FRED2].clock_);
    configPointer->Write("/FRED2/Turbo_Clock_Speed", conf[FRED2].turboClock_);
    configPointer->Write("/FRED2/Enable_Turbo_Cassette", conf[FRED2].turbo_);
    configPointer->Write("/FRED2/Enable_Auto_Cassette", conf[FRED2].autoCassetteLoad_);
    configPointer->Write("/FRED2/Enable_Real_Cassette", conf[FRED2].realCassetteLoad_);
    configPointer->Write("/FRED2/Volume", conf[FRED2].volume_);
    configPointer->Write("/FRED2/Ram_Type", conf[FRED2].ramType_);
    configPointer->Write("/FRED2/KeyPadMode", elfConfiguration[FRED2].keyboardType);
    configPointer->Write("/FRED2/TapeFormat", elfConfiguration[FRED2].tapeFormat_);
    configPointer->Write("/FRED2/Open_Control_Windows", elfConfiguration[FRED2].useElfControlWindows);
    configPointer->Write("/FRED2/Led_Update_Frequency", conf[FRED2].ledTime_);
    configPointer->Write("/FRED2/Enable_CoinArcadeControl", elfConfiguration[FRED2].coinArcadeControl_);
    configPointer->Write("/FRED2/Enable_Auto_Boot", elfConfiguration[FRED2].autoBoot);
    configPointer->Write("/FRED2/Enable_Stop_Tone", elfConfiguration[FRED2].stopTone);
    configPointer->Write("/FRED2/Enable_Tape_Start", elfConfiguration[FRED2].tapeStart);
    configPointer->Write("/FRED2/Enable_Interlace", conf[FRED2].interlace_);
    buffer.Printf("%04X", (unsigned int)conf[FRED2].bootAddress_);
    configPointer->Write("/FRED2/Boot_Address", buffer);
}

void GuiFred::readFred2WindowConfig()
{
    conf[FRED2].pixieX_ = (int)configPointer->Read("/FRED2/Window_Position_Pixie_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[FRED2].pixieY_ = (int)configPointer->Read("/FRED2/Window_Position_Pixie_Y", mainWindowY_);
    conf[FRED2].mainX_ = (int)configPointer->Read("/FRED2/Window_Position_X",  mainWindowX_);
    conf[FRED2].mainY_ = (int)configPointer->Read("/FRED2/Window_Position_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
}

void GuiFred::writeFred2WindowConfig()
{
    if (conf[FRED2].pixieX_ > 0)
        configPointer->Write("/FRED2/Window_Position_Pixie_X", conf[FRED2].pixieX_);
    if (conf[FRED2].pixieY_ > 0)
        configPointer->Write("/FRED2/Window_Position_Pixie_Y", conf[FRED2].pixieY_);
    if (conf[FRED2].mainX_ > 0)
        configPointer->Write("/FRED2/Window_Position_X", conf[FRED2].mainX_);
    if (conf[FRED2].mainY_ > 0)
        configPointer->Write("/FRED2/Window_Position_Y", conf[FRED2].mainY_);
}

void GuiFred::onFred2ControlWindows(wxCommandEvent&event)
{
    elfConfiguration[FRED2].useElfControlWindows = event.IsChecked();
    
    if (mode_.gui)
    {
        XRCCTRL(*this,"ShowAddressFRED2",wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText1FRED2",wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText2FRED2",wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
    }
    
    if (runningComputer_ == FRED2)
        p_Fred->Show(elfConfiguration[FRED2].useElfControlWindows);
}

bool GuiFred::getUseFredControlWindows()
{
    return elfConfiguration[selectedComputer_].useElfControlWindows;
}

void GuiFred::onAutoBoot(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].autoBoot = event.IsChecked();
}

void GuiFred::onCoinArcadeControl(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].coinArcadeControl_ = event.IsChecked();
}

void GuiFred::onStopTone(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].stopTone = event.IsChecked();
}

void GuiFred::onTapeStart(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].tapeStart = event.IsChecked();
}

void GuiFred::onChoiceKeyPadMode(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].keyboardType = event.GetSelection();
    
    if (runningComputer_ == FRED1)
        p_Fred->setKeyPadMode(elfConfiguration[FRED1].keyboardType);
    
    if (runningComputer_ == FRED2)
        p_Fred->setKeyPadMode(elfConfiguration[FRED2].keyboardType);
}

void GuiFred::onTapeFormat(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].tapeFormat_ = event.GetSelection();
    
    if (runningComputer_ == FRED1)
        p_Fred->setTapeFormat(elfConfiguration[FRED1].tapeFormat_);
    
    if (runningComputer_ == FRED2)
        p_Fred->setTapeFormat(elfConfiguration[FRED2].tapeFormat_);
}

void GuiFred::onCardButton(wxCommandEvent&event)
{
    long id;
    wxString idReference, buttonNumber;
    
    idReference = wxWindow::FindWindowById(event.GetId())->GetName();
    buttonNumber = idReference.Right(2);

    if (!buttonNumber.ToLong(&id, 10))
        return;
    
    if (runningComputer_ == FRED1 && currentCardValue[id] != -1)
        p_Fred->cardButton(currentCardValue[id]);
}

void GuiFred::checkGameFred2(wxString gameName)
{
    if (gameName == "Fred Demo.bin")
	{
        elfConfiguration[FRED2].autoBoot = false;
        XRCCTRL(*this,"AutoBootFRED2", wxCheckBox)->SetValue(elfConfiguration[FRED2].autoBoot);
        elfConfiguration[FRED2].keyboardType = FRED_HEX_MODE;
        XRCCTRL(*this,"KeyPadModeFRED2", wxChoice)->SetSelection(elfConfiguration[FRED2].keyboardType);
        conf[FRED2].turbo_ = false;
        XRCCTRL(*this, "TurboFRED2", wxCheckBox)->SetValue(conf[FRED2].turbo_);
        turboGui("FRED2");
        conf[FRED2].wavFile_[0] = "Fred Demo.wav";
        XRCCTRL(*this, "WavFileFRED2", wxTextCtrl)->SetValue(conf[FRED2].wavFile_[0]);
        downloadWavFiles(FRED2);
	}
}

void GuiFred::setGameId(wxString gameName)
{
    conf[FRED1].gameId_ = 1;
    
    while (conf[FRED1].gameId_ != FRED1_GAME_END && gameList[conf[FRED1].gameId_] != gameName)
        conf[FRED1].gameId_++;
    
    if (conf[FRED1].gameId_ == FRED1_GAME_END)
        conf[FRED1].gameId_ = FRED1_GAME_NONE;
    
    if (mode_.gui)
        setGame();
}

void GuiFred::setGame()
{
    wxString buttonText;
    
    XRCCTRL(*this,"CardTextFRED1", wxStaticText)->Hide();
    for (int i=0; i<FRED1_CARDS; i++)
    {
    	buttonText.Printf("CardButtonFRED1_%02d",i);
    	if (cardValue[conf[FRED1].gameId_][i] == -1)
    		XRCCTRL(*this,buttonText, wxButton)->Hide();
    	else
    	{
    		XRCCTRL(*this,buttonText, wxButton)->Show();
    		XRCCTRL(*this,"CardTextFRED1", wxStaticText)->Show();
    	}
    }
    
    setCurrentCardValue();
    
	if (guiStartup_)
		return;

    switch (conf[FRED1].gameId_)
    {
        case FRED1_GAME_21:
        case FRED1_GAME_21_II:
        case FRED1_GAME_ACEY_DUECY:
        case FRED1_GAME_CALCULATE:
        case FRED1_GAME_DEDUCE:
        case FRED1_GAME_DRAW:
        case FRED1_GAME_ESP:
        case FRED1_GAME_ESP_II:
        case FRED1_GAME_FLIP:
        case FRED1_GAME_FLIP_II:
        case FRED1_GAME_HILO:
        case FRED1_GAME_JACKPOT:
        case FRED1_GAME_LIFE:
        case FRED1_GAME_MATCH:
        case FRED1_GAME_MATCH_II:
        case FRED1_GAME_MINIKRIEG:
        case FRED1_GAME_NIMNET:
        case FRED1_GAME_PATTERN_PUZZLE:
        case FRED1_GAME_SLIDE_PUZZLE_II:
    	case FRED1_GAME_SPACE_WAR:
        case FRED1_GAME_SPOOK:
        case FRED1_GAME_SPOT_SPEEDWAY_1:
        case FRED1_GAME_SPOT_SPEEDWAY_2:
        case FRED1_GAME_TIC_TAC_TOE:
        case FRED1_GAME_TARGET_1:
            elfConfiguration[FRED1].autoBoot = true;
    		XRCCTRL(*this,"AutoBootFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].autoBoot);
    		elfConfiguration[FRED1].keyboardType = FRED_HEX_MODE;
    		XRCCTRL(*this,"KeyPadModeFRED1", wxChoice)->SetSelection(elfConfiguration[FRED1].keyboardType);
    	break;
    	    	
        case FRED1_GAME_LUCKYPATH:
        case FRED1_GAME_SLIDE_PUZZLE_I:
        case FRED1_GAME_CARDTRAN:
        case FRED1_GAME_MEMORY_TEST:
            elfConfiguration[FRED1].autoBoot = true;
            XRCCTRL(*this,"AutoBootFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].autoBoot);
            elfConfiguration[FRED1].keyboardType = FRED_BYTE_MODE;
            XRCCTRL(*this,"KeyPadModeFRED1", wxChoice)->SetSelection(elfConfiguration[FRED1].keyboardType);
        break;
            
        case FRED1_GAME_ERASE:
            elfConfiguration[FRED1].autoBoot = true;
            XRCCTRL(*this,"AutoBootFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].autoBoot);
            elfConfiguration[FRED1].keyboardType = FRED_HEX_PULSE_MODE;
            XRCCTRL(*this,"KeyPadModeFRED1", wxChoice)->SetSelection(elfConfiguration[FRED1].keyboardType);
        break;

        case FRED1_GAME_ADD_DRILL:
            elfConfiguration[FRED1].autoBoot = false;
            XRCCTRL(*this,"AutoBootFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].autoBoot);
            elfConfiguration[FRED1].keyboardType = FRED_HEX_MODE;
            XRCCTRL(*this,"KeyPadModeFRED1", wxChoice)->SetSelection(elfConfiguration[FRED1].keyboardType);
            conf[FRED1].turbo_ = false;
            XRCCTRL(*this, "TurboFRED1", wxCheckBox)->SetValue(conf[FRED1].turbo_);
            turboGui("FRED1");
            conf[FRED1].wavFile_[0] = "Add Drill.wav";
            XRCCTRL(*this, "WavFileFRED1", wxTextCtrl)->SetValue(conf[FRED1].wavFile_[0]);
            downloadWavFiles(FRED1);
        break;

        case FRED1_GAME_DEMO:
            elfConfiguration[FRED1].autoBoot = false;
            XRCCTRL(*this,"AutoBootFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].autoBoot);
            elfConfiguration[FRED1].keyboardType = FRED_HEX_MODE;
            XRCCTRL(*this,"KeyPadModeFRED1", wxChoice)->SetSelection(elfConfiguration[FRED1].keyboardType);
            conf[FRED1].turbo_ = false;
            XRCCTRL(*this, "TurboFRED1", wxCheckBox)->SetValue(conf[FRED1].turbo_);
            turboGui("FRED1");
            conf[FRED1].wavFile_[0] = "Demo.wav";
            XRCCTRL(*this, "WavFileFRED1", wxTextCtrl)->SetValue(conf[FRED1].wavFile_[0]);
            downloadWavFiles(FRED1);
        break;

        case FRED1_GAME_BOWLING:
            elfConfiguration[FRED1].autoBoot = true;
            XRCCTRL(*this,"AutoBootFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].autoBoot);
            elfConfiguration[FRED1].keyboardType = FRED_HEX_MODE;
            XRCCTRL(*this,"KeyPadModeFRED1", wxChoice)->SetSelection(elfConfiguration[FRED1].keyboardType);
            conf[FRED1].turbo_ = false;
            XRCCTRL(*this, "TurboFRED1", wxCheckBox)->SetValue(conf[FRED1].turbo_);
            turboGui("FRED1");
            conf[FRED1].wavFile_[0] = "Bowling.wav";
            XRCCTRL(*this, "WavFileFRED1", wxTextCtrl)->SetValue(conf[FRED1].wavFile_[0]);
            downloadWavFiles(FRED1);
        break;

        case FRED1_GAME_DEDUCE_LEDS:
            elfConfiguration[FRED1].autoBoot = false;
            XRCCTRL(*this,"AutoBootFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].autoBoot);
            elfConfiguration[FRED1].keyboardType = FRED_HEX_MODE;
            XRCCTRL(*this,"KeyPadModeFRED1", wxChoice)->SetSelection(elfConfiguration[FRED1].keyboardType);
            conf[FRED1].turbo_ = false;
            XRCCTRL(*this, "TurboFRED1", wxCheckBox)->SetValue(conf[FRED1].turbo_);
            turboGui("FRED1");
            conf[FRED1].wavFile_[0] = "Deduce-leds.wav";
            XRCCTRL(*this, "WavFileFRED1", wxTextCtrl)->SetValue(conf[FRED1].wavFile_[0]);
            downloadWavFiles(FRED1);
        break;
        
        case FRED1_GAME_CLUE:
            elfConfiguration[FRED1].autoBoot = false;
            XRCCTRL(*this,"AutoBootFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].autoBoot);
            elfConfiguration[FRED1].keyboardType = FRED_HEX_MODE;
            XRCCTRL(*this,"KeyPadModeFRED1", wxChoice)->SetSelection(elfConfiguration[FRED1].keyboardType);
            conf[FRED1].turbo_ = false;
            XRCCTRL(*this, "TurboFRED1", wxCheckBox)->SetValue(conf[FRED1].turbo_);
            turboGui("FRED1");
            conf[FRED1].wavFile_[0] = "Clue.wav";
            XRCCTRL(*this, "WavFileFRED1", wxTextCtrl)->SetValue(conf[FRED1].wavFile_[0]);
            downloadWavFiles(FRED1);
        break;
        
        case FRED1_GAME_PROG_APT:
            elfConfiguration[FRED1].autoBoot = false;
            XRCCTRL(*this,"AutoBootFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].autoBoot);
            elfConfiguration[FRED1].keyboardType = FRED_HEX_MODE;
            XRCCTRL(*this,"KeyPadModeFRED1", wxChoice)->SetSelection(elfConfiguration[FRED1].keyboardType);
            conf[FRED1].turbo_ = false;
            XRCCTRL(*this, "TurboFRED1", wxCheckBox)->SetValue(conf[FRED1].turbo_);
            turboGui("FRED1");
            conf[FRED1].wavFile_[0] = "Prog. Apt. Test.wav";
            XRCCTRL(*this, "WavFileFRED1", wxTextCtrl)->SetValue(conf[FRED1].wavFile_[0]);
            downloadWavFiles(FRED1);
        break;

        case FRED1_GAME_ANIMATE:
            elfConfiguration[FRED1].autoBoot = true;
            XRCCTRL(*this,"AutoBootFRED1", wxCheckBox)->SetValue(elfConfiguration[FRED1].autoBoot);
            elfConfiguration[FRED1].keyboardType = FRED_HEX_MODE;
            XRCCTRL(*this,"KeyPadModeFRED1", wxChoice)->SetSelection(elfConfiguration[FRED1].keyboardType);
            conf[FRED1].turbo_ = false;
            XRCCTRL(*this, "TurboFRED1", wxCheckBox)->SetValue(conf[FRED1].turbo_);
            turboGui("FRED1");
            conf[FRED1].wavFile_[0] = "Animate Demo Data.wav";
            XRCCTRL(*this, "WavFileFRED1", wxTextCtrl)->SetValue(conf[FRED1].wavFile_[0]);
            downloadWavFiles(FRED1);
        break;
            
        default:
    	break;
    }
}

void GuiFred::downloadWavFiles(int computer)
{
    if (wxFile::Exists(conf[computer].wavFileDir_[0] + conf[computer].wavFile_[0]))
        return;
    
    int answer = wxMessageBox("Additional wav file required: " + conf[computer].wavFile_[0], "Download file?", wxICON_EXCLAMATION | wxYES_NO);
    if (answer == wxYES)
    {
        wxString fileName = conf[computer].wavFileDir_[0] + conf[computer].wavFile_[0];
        wxFileOutputStream html_stream(fileName);

        wxCurlHTTP http("https://www.emma02.hobby-site.com/wave/" + conf[computer].wavFile_[0]);
    
        if (!http.Get(html_stream))
            wxMessageBox( "Download failed", "Emma 02", wxICON_ERROR | wxOK );
    }
}

void GuiFred::setCurrentCardValue()
{
    int last=FRED1_CARDS-1;
    for (int i=(FRED1_CARDS-1); i>=0; i--)
    {
        currentCardValue[i] = cardValue[conf[FRED1].gameId_][i];
        if (currentCardValue[i] == -1)
            last--;
    }
    
    if (cardValue[conf[FRED1].gameId_][0] != -1 && (cardValue[conf[FRED1].gameId_][0] & 0x100) == 0x100)
    {
        currentCardValue[0] = currentCardValue[0] & 0xff;
        for (int i=last; i!=0; i--)
        {
            using std::swap;
            int j = rand() % (last+1);
            swap(currentCardValue[i], currentCardValue[j]);
        }
    }
}
