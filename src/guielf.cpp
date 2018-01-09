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
#include "guielf.h"

DEFINE_EVENT_TYPE(ON_UART)

BEGIN_EVENT_TABLE(GuiElf, GuiElf2K)

	EVT_TEXT(XRCID("MainRomElf"), GuiMain::onMainRom1Text)
	EVT_TEXT(XRCID("MainRomElfII"), GuiMain::onMainRom1Text)
	EVT_TEXT(XRCID("MainRomSuperElf"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomElf"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomElfII"), GuiMain::onMainRom1Text)
	EVT_COMBOBOX(XRCID("MainRomSuperElf"), GuiMain::onMainRom1Text)
	EVT_BUTTON(XRCID("RomButtonElf"), GuiMain::onMainRom1)
	EVT_BUTTON(XRCID("RomButtonElfII"), GuiMain::onMainRom1)
	EVT_BUTTON(XRCID("RomButtonSuperElf"), GuiMain::onMainRom1)

	EVT_TEXT(XRCID("IdeFileElf"), GuiMain::onIdeText)
	EVT_TEXT(XRCID("IdeFileElfII"), GuiMain::onIdeText)
	EVT_TEXT(XRCID("IdeFileSuperElf"), GuiMain::onIdeText)
	EVT_BUTTON(XRCID("IDE_ButtonElf"), GuiMain::onIde)
	EVT_BUTTON(XRCID("IDE_ButtonElfII"), GuiMain::onIde)
	EVT_BUTTON(XRCID("IDE_ButtonSuperElf"), GuiMain::onIde)
	EVT_BUTTON(XRCID("Eject_IDEElf"), GuiMain::onIdeEject)
	EVT_BUTTON(XRCID("Eject_IDEElfII"), GuiMain::onIdeEject)
	EVT_BUTTON(XRCID("Eject_IDESuperElf"), GuiMain::onIdeEject)

	EVT_TEXT(XRCID("KeyFileElf"), GuiMain::onKeyFileText)
	EVT_TEXT(XRCID("KeyFileElfII"), GuiMain::onKeyFileText)
	EVT_TEXT(XRCID("KeyFileSuperElf"), GuiMain::onKeyFileText)
	EVT_BUTTON(XRCID("KeyFileButtonElf"), GuiMain::onKeyFile)
	EVT_BUTTON(XRCID("KeyFileButtonElfII"), GuiMain::onKeyFile)
	EVT_BUTTON(XRCID("KeyFileButtonSuperElf"), GuiMain::onKeyFile)
	EVT_BUTTON(XRCID("EjectKeyFileElf"), GuiMain::onKeyFileEject)
	EVT_BUTTON(XRCID("EjectKeyFileElfII"), GuiMain::onKeyFileEject)
	EVT_BUTTON(XRCID("EjectKeyFileSuperElf"), GuiMain::onKeyFileEject)

//    EVT_CHECKBOX(XRCID("VtExternalElf"), GuiMain::onVtExternal)
//    EVT_CHECKBOX(XRCID("VtExternalElfII"), GuiMain::onVtExternal)
//    EVT_CHECKBOX(XRCID("VtExternalSuperElf"), GuiMain::onVtExternal)

	EVT_CHOICE(XRCID("VTTypeElf"), GuiMain::onVT100)
	EVT_CHOICE(XRCID("VTTypeElfII"), GuiMain::onVT100)
	EVT_CHOICE(XRCID("VTTypeSuperElf"), GuiMain::onVT100)

	EVT_CHOICE(XRCID("QsoundElf"), GuiElf::onQsound)
	EVT_CHOICE(XRCID("QsoundElfII"), GuiElf::onQsound)
	EVT_CHOICE(XRCID("QsoundSuperElf"), GuiElf::onQsound)

	EVT_SPIN_UP(XRCID("ZoomSpinElf"), GuiMain::onZoomUp)
	EVT_SPIN_UP(XRCID("ZoomSpinElfII"), GuiMain::onZoomUp)
	EVT_SPIN_UP(XRCID("ZoomSpinSuperElf"), GuiMain::onZoomUp)

	EVT_SPIN_DOWN(XRCID("ZoomSpinElf"), GuiMain::onZoomDown)
	EVT_SPIN_DOWN(XRCID("ZoomSpinElfII"), GuiMain::onZoomDown)
	EVT_SPIN_DOWN(XRCID("ZoomSpinSuperElf"), GuiMain::onZoomDown)

	EVT_TEXT(XRCID("ZoomValueElf"), GuiMain::onZoomValue)
	EVT_TEXT(XRCID("ZoomValueElfII"), GuiMain::onZoomValue)
	EVT_TEXT(XRCID("ZoomValueSuperElf"), GuiMain::onZoomValue)

	EVT_SPIN_UP(XRCID("ZoomSpinVtElf"), GuiMain::onZoomUpVt)
	EVT_SPIN_UP(XRCID("ZoomSpinVtElfII"), GuiMain::onZoomUpVt)
	EVT_SPIN_UP(XRCID("ZoomSpinVtSuperElf"), GuiMain::onZoomUpVt)

	EVT_SPIN_DOWN(XRCID("ZoomSpinVtElf"), GuiMain::onZoomDownVt)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVtElfII"), GuiMain::onZoomDownVt)
	EVT_SPIN_DOWN(XRCID("ZoomSpinVtSuperElf"), GuiMain::onZoomDownVt)

	EVT_TEXT(XRCID("ZoomValueVtElf"), GuiMain::onZoomValueVt)
	EVT_TEXT(XRCID("ZoomValueVtElfII"), GuiMain::onZoomValueVt)
	EVT_TEXT(XRCID("ZoomValueVtSuperElf"), GuiMain::onZoomValueVt)

	EVT_CHECKBOX(XRCID("InterlaceElf"), GuiMain::onInterlace)
	EVT_CHECKBOX(XRCID("InterlaceElfII"), GuiMain::onInterlace)
	EVT_CHECKBOX(XRCID("InterlaceSuperElf"), GuiMain::onInterlace)

	EVT_CHECKBOX(XRCID("StretchDotElf"), GuiMain::onStretchDot)
	EVT_CHECKBOX(XRCID("StretchDotElfII"), GuiMain::onStretchDot)
	EVT_CHECKBOX(XRCID("StretchDotSuperElf"), GuiMain::onStretchDot)

	EVT_BUTTON(XRCID("ScreenDumpFileButtonElf"), GuiMain::onScreenDumpFile)
	EVT_BUTTON(XRCID("ScreenDumpFileButtonElfII"), GuiMain::onScreenDumpFile)
	EVT_BUTTON(XRCID("ScreenDumpFileButtonSuperElf"), GuiMain::onScreenDumpFile)

	EVT_TEXT(XRCID("ScreenDumpFileElf"), GuiMain::onScreenDumpFileText)
	EVT_TEXT(XRCID("ScreenDumpFileElfII"), GuiMain::onScreenDumpFileText)
	EVT_TEXT(XRCID("ScreenDumpFileSuperElf"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileElf"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileElfII"), GuiMain::onScreenDumpFileText)
	EVT_COMBOBOX(XRCID("ScreenDumpFileSuperElf"), GuiMain::onScreenDumpFileText)

	EVT_BUTTON(XRCID("ScreenDumpF5Elf"), GuiMain::onScreenDump)
	EVT_BUTTON(XRCID("ScreenDumpF5ElfII"), GuiMain::onScreenDump)
	EVT_BUTTON(XRCID("ScreenDumpF5SuperElf"), GuiMain::onScreenDump)

	EVT_BUTTON(XRCID("SaveButtonElf"), GuiMain::onSaveButton)
	EVT_BUTTON(XRCID("SaveButtonElfII"), GuiMain::onSaveButton)
	EVT_BUTTON(XRCID("SaveButtonSuperElf"), GuiMain::onSaveButton)

	EVT_BUTTON(XRCID("DP_ButtonElf"), GuiMain::onDp)
	EVT_BUTTON(XRCID("DP_ButtonElfII"), GuiMain::onDp)
	EVT_BUTTON(XRCID("DP_ButtonSuperElf"), GuiMain::onDp)

	EVT_BUTTON(XRCID("VtSetupElf"), GuiMain::onVtSetup)
	EVT_BUTTON(XRCID("VtSetupElfII"), GuiMain::onVtSetup)
	EVT_BUTTON(XRCID("VtSetupSuperElf"), GuiMain::onVtSetup)

	EVT_BUTTON(XRCID("LoadButtonElf"), GuiMain::onLoadButton)
	EVT_BUTTON(XRCID("LoadButtonElfII"), GuiMain::onLoadButton)
	EVT_BUTTON(XRCID("LoadButtonSuperElf"), GuiMain::onLoadButton)

	EVT_BUTTON(XRCID("RunButtonElf"), GuiMain::onLoadRunButton)
	EVT_BUTTON(XRCID("RunButtonElfII"), GuiMain::onLoadRunButton)
	EVT_BUTTON(XRCID("RunButtonSuperElf"), GuiMain::onLoadRunButton)

	EVT_BUTTON(XRCID("DsaveButtonElf"), GuiMain::onDataSaveButton)
	EVT_BUTTON(XRCID("DsaveButtonElfII"), GuiMain::onDataSaveButton)
	EVT_BUTTON(XRCID("DsaveButtonSuperElf"), GuiMain::onDataSaveButton)

	EVT_CHECKBOX(XRCID("UseLocationElf"), GuiMain::onUseLocation)
	EVT_CHECKBOX(XRCID("UseLocationElfII"), GuiMain::onUseLocation)
	EVT_CHECKBOX(XRCID("UseLocationSuperElf"), GuiMain::onUseLocation)

	EVT_TEXT(XRCID("BeepFrequencyElf"), GuiMain::onBeepFrequency)
	EVT_TEXT(XRCID("BeepFrequencyElfII"), GuiMain::onBeepFrequency)
	EVT_TEXT(XRCID("BeepFrequencySuperElf"), GuiMain::onBeepFrequency)

	EVT_BUTTON(XRCID("Rom1Elf"), GuiElf::onRom1)
	EVT_BUTTON(XRCID("Rom1ElfII"), GuiElf::onRom1)
	EVT_BUTTON(XRCID("Rom1SuperElf"), GuiElf::onRom1)

	EVT_TEXT(XRCID("MainRom2Elf"), GuiMain::onMainRom2Text)
	EVT_TEXT(XRCID("MainRom2ElfII"), GuiMain::onMainRom2Text)
	EVT_TEXT(XRCID("MainRom2SuperElf"), GuiMain::onMainRom2Text)
	EVT_COMBOBOX(XRCID("MainRom2Elf"), GuiMain::onMainRom2Text)
	EVT_COMBOBOX(XRCID("MainRom2ElfII"), GuiMain::onMainRom2Text)
	EVT_COMBOBOX(XRCID("MainRom2SuperElf"), GuiMain::onMainRom2Text)
	EVT_BUTTON(XRCID("RomButton2Elf"), GuiMain::onMainRom2)
	EVT_BUTTON(XRCID("RomButton2ElfII"), GuiMain::onMainRom2)
	EVT_BUTTON(XRCID("RomButton2SuperElf"), GuiMain::onMainRom2)

	EVT_BUTTON(XRCID("Rom2Elf"), GuiElf::onRom2)
	EVT_BUTTON(XRCID("Rom2ElfII"), GuiElf::onRom2)
	EVT_BUTTON(XRCID("Rom2SuperElf"), GuiElf::onRom2)

	EVT_TEXT(XRCID("CharRomElf"), GuiMain::onCharRomText)
	EVT_TEXT(XRCID("CharRomElfII"), GuiMain::onCharRomText)
	EVT_TEXT(XRCID("CharRomSuperElf"), GuiMain::onCharRomText)
	EVT_COMBOBOX(XRCID("CharRomElf"), GuiMain::onCharRomText)
	EVT_COMBOBOX(XRCID("CharRomElfII"), GuiMain::onCharRomText)
	EVT_COMBOBOX(XRCID("CharRomSuperElf"), GuiMain::onCharRomText)
	EVT_BUTTON(XRCID("CharRomButtonElf"), GuiMain::onCharRom)
	EVT_BUTTON(XRCID("CharRomButtonElfII"), GuiMain::onCharRom)
	EVT_BUTTON(XRCID("CharRomButtonSuperElf"), GuiMain::onCharRom)

	EVT_TEXT(XRCID("VtCharRomElf"), GuiMain::onVtCharRomText)
	EVT_TEXT(XRCID("VtCharRomElfII"), GuiMain::onVtCharRomText)
	EVT_TEXT(XRCID("VtCharRomSuperElf"), GuiMain::onVtCharRomText)
	EVT_COMBOBOX(XRCID("VtCharRomElf"), GuiMain::onVtCharRomText)
	EVT_COMBOBOX(XRCID("VtCharRomElfII"), GuiMain::onVtCharRomText)
	EVT_COMBOBOX(XRCID("VtCharRomSuperElf"), GuiMain::onVtCharRomText)
	EVT_BUTTON(XRCID("VtCharRomButtonElf"), GuiMain::onVtCharRom)
	EVT_BUTTON(XRCID("VtCharRomButtonElfII"), GuiMain::onVtCharRom)
	EVT_BUTTON(XRCID("VtCharRomButtonSuperElf"), GuiMain::onVtCharRom)

	EVT_TEXT(XRCID("StartRamElf"), GuiElf::onStartRam)
	EVT_TEXT(XRCID("StartRamElfII"), GuiElf::onStartRam)
	EVT_TEXT(XRCID("StartRamSuperElf"), GuiElf::onStartRam)

	EVT_TEXT(XRCID("EndRamElf"), GuiElf::onEndRam)
	EVT_TEXT(XRCID("EndRamElfII"), GuiElf::onEndRam)
	EVT_TEXT(XRCID("EndRamSuperElf"), GuiElf::onEndRam)

	EVT_CHOICE(XRCID("VideoTypeElf"), GuiElf::onVideoType)
	EVT_CHOICE(XRCID("VideoTypeElfII"), GuiElf::onVideoType)
	EVT_CHOICE(XRCID("VideoTypeSuperElf"), GuiElf::onVideoType)

	EVT_CHOICE(XRCID("KeyboardElf"), GuiElf::onElfKeyboard)
	EVT_CHOICE(XRCID("KeyboardElfII"), GuiElf::onElfKeyboard)
	EVT_CHOICE(XRCID("KeyboardSuperElf"), GuiElf::onElfKeyboard)

	EVT_CHECKBOX(XRCID("UpperCaseElf"), GuiElf::onForceUpperCase)
	EVT_CHECKBOX(XRCID("UpperCaseElfII"), GuiElf::onForceUpperCase)
	EVT_CHECKBOX(XRCID("UpperCaseSuperElf"), GuiElf::onForceUpperCase)

    EVT_CHECKBOX(XRCID("GiantElfII"), GuiElf::onGiantBoardMapping)

    EVT_CHOICE(XRCID("DiskTypeElf"), GuiElf::onDiskType)
	EVT_CHOICE(XRCID("DiskTypeElfII"), GuiElf::onDiskType)
	EVT_CHOICE(XRCID("DiskTypeSuperElf"), GuiElf::onDiskType)

	EVT_CHOICE(XRCID("MemoryElf"), GuiElf::onMemory)
	EVT_CHOICE(XRCID("MemoryElfII"), GuiElf::onMemory)
	EVT_CHOICE(XRCID("MemorySuperElf"), GuiElf::onMemory)

	EVT_BUTTON(XRCID("FullScreenF3Elf"), GuiMain::onFullScreen)
	EVT_BUTTON(XRCID("FullScreenF3ElfII"), GuiMain::onFullScreen)
	EVT_BUTTON(XRCID("FullScreenF3SuperElf"), GuiMain::onFullScreen)

	EVT_TEXT(XRCID("PrintFileElf"), GuiMain::onPrintFileText)
	EVT_TEXT(XRCID("PrintFileElfII"), GuiMain::onPrintFileText)
	EVT_TEXT(XRCID("PrintFileSuperElf"), GuiMain::onPrintFileText)
	EVT_BUTTON(XRCID("PrintButtonElf"), GuiMain::onPrintButton)
	EVT_BUTTON(XRCID("PrintButtonElfII"), GuiMain::onPrintButton)
	EVT_BUTTON(XRCID("PrintButtonSuperElf"), GuiMain::onPrintButton)

	EVT_CHOICE(XRCID("PrintModeElf"), GuiMain::onPrintMode)
	EVT_CHOICE(XRCID("PrintModeElfII"), GuiMain::onPrintMode)
	EVT_CHOICE(XRCID("PrintModeSuperElf"), GuiMain::onPrintMode)

	EVT_BUTTON(XRCID("PrintFileButtonElf"), GuiMain::onPrintFile)
	EVT_BUTTON(XRCID("PrintFileButtonElfII"), GuiMain::onPrintFile)
	EVT_BUTTON(XRCID("PrintFileButtonSuperElf"), GuiMain::onPrintFile)
		
	EVT_CHOICE(XRCID("VTBaudTChoiceElf"), GuiMain::onBaudT)
	EVT_CHOICE(XRCID("VTBaudTChoiceElfII"), GuiMain::onBaudT)
	EVT_CHOICE(XRCID("VTBaudTChoiceSuperElf"), GuiMain::onBaudT)

	EVT_CHOICE(XRCID("VTBaudRChoiceElf"), GuiMain::onBaudR)
	EVT_CHOICE(XRCID("VTBaudRChoiceElfII"), GuiMain::onBaudR)
	EVT_CHOICE(XRCID("VTBaudRChoiceSuperElf"), GuiMain::onBaudR)

	EVT_COMMAND(wxID_ANY, OPEN_PRINTER_WINDOW, GuiMain::openPrinterFrame) 

	EVT_CHECKBOX(XRCID("UseLedModule"), GuiElf::onLedModule)

	EVT_CHECKBOX(XRCID("PortExtElf"), GuiElf::onUsePortExtender)
	EVT_CHECKBOX(XRCID("PortExtElfII"), GuiElf::onUsePortExtender)
	EVT_CHECKBOX(XRCID("PortExtSuperElf"), GuiElf::onUsePortExtender)

	EVT_CHECKBOX(XRCID("ControlWindowsElf"), GuiElf::onElfControlWindows)
	EVT_CHECKBOX(XRCID("ControlWindowsElfII"), GuiElf::onElfControlWindows)
	EVT_CHECKBOX(XRCID("ControlWindowsSuperElf"), GuiElf::onElfControlWindows)

	EVT_CHECKBOX(XRCID("AutoBootElf"), GuiElf::onAutoBoot)
	EVT_CHECKBOX(XRCID("AutoBootElfII"), GuiElf::onAutoBoot)
	EVT_CHECKBOX(XRCID("AutoBootSuperElf"), GuiElf::onAutoBoot)

	EVT_TEXT(XRCID("BootAddressElf"), GuiElf::onBootAddress)
	EVT_TEXT(XRCID("BootAddressElfII"), GuiElf::onBootAddress)
	EVT_TEXT(XRCID("BootAddressSuperElf"), GuiElf::onBootAddress)

	EVT_BUTTON(XRCID("KeyMapElf"), Main::onHexKeyDef)
	EVT_BUTTON(XRCID("KeyMapElfII"), Main::onHexKeyDef)
	EVT_BUTTON(XRCID("KeyMapSuperElf"), Main::onHexKeyDef)

	EVT_BUTTON(XRCID("CasButtonElf"), GuiMain::onCassette)
	EVT_BUTTON(XRCID("EjectCasElf"), GuiMain::onCassetteEject)
	EVT_TEXT(XRCID("WavFileElf"), GuiMain::onCassetteText)
	EVT_BUTTON(XRCID("RealCasLoadElf"), GuiMain::onRealCas)
	EVT_BUTTON(XRCID("CasLoadElf"), GuiMain::onCassetteLoad)
	EVT_BUTTON(XRCID("CasSaveElf"), GuiMain::onCassetteSave)
	EVT_BUTTON(XRCID("CasStopElf"), GuiMain::onCassetteStop)
	EVT_CHECKBOX(XRCID("TurboElf"), GuiMain::onTurbo)
	EVT_TEXT(XRCID("TurboClockElf"), GuiMain::onTurboClock)
	EVT_CHECKBOX(XRCID("AutoCasLoadElf"), GuiMain::onAutoLoad)
	EVT_BUTTON(XRCID("TapeElf"), GuiElf::onTape)

	EVT_BUTTON(XRCID("CasButtonElfII"), GuiMain::onCassette)
	EVT_BUTTON(XRCID("EjectCasElfII"), GuiMain::onCassetteEject)
	EVT_TEXT(XRCID("WavFileElfII"), GuiMain::onCassetteText)
	EVT_BUTTON(XRCID("RealCasLoadElfII"), GuiMain::onRealCas)
	EVT_BUTTON(XRCID("CasLoadElfII"), GuiMain::onCassetteLoad)
	EVT_BUTTON(XRCID("CasSaveElfII"), GuiMain::onCassetteSave)
	EVT_BUTTON(XRCID("CasStopElfII"), GuiMain::onCassetteStop)
	EVT_CHECKBOX(XRCID("TurboElfII"), GuiMain::onTurbo)
	EVT_TEXT(XRCID("TurboClockElfII"), GuiMain::onTurboClock)
	EVT_CHECKBOX(XRCID("AutoCasLoadElfII"), GuiMain::onAutoLoad)
	EVT_BUTTON(XRCID("TapeElfII"), GuiElf::onTape)

	EVT_BUTTON(XRCID("CasButtonSuperElf"), GuiMain::onCassette)
	EVT_BUTTON(XRCID("EjectCasSuperElf"), GuiMain::onCassetteEject)
	EVT_TEXT(XRCID("WavFileSuperElf"), GuiMain::onCassetteText)
	EVT_BUTTON(XRCID("RealCasLoadSuperElf"), GuiMain::onRealCas)
	EVT_BUTTON(XRCID("CasLoadSuperElf"), GuiMain::onCassetteLoad)
	EVT_BUTTON(XRCID("CasSaveSuperElf"), GuiMain::onCassetteSave)
	EVT_BUTTON(XRCID("CasStopSuperElf"), GuiMain::onCassetteStop)
	EVT_CHECKBOX(XRCID("TurboSuperElf"), GuiMain::onTurbo)
	EVT_TEXT(XRCID("TurboClockSuperElf"), GuiMain::onTurboClock)
	EVT_CHECKBOX(XRCID("AutoCasLoadSuperElf"), GuiMain::onAutoLoad)
	EVT_BUTTON(XRCID("TapeSuperElf"), GuiElf::onTape)

	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeElf"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeElf"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeElfII"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeElfII"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_THUMBTRACK(XRCID("VolumeSuperElf"), GuiMain::onVolume)
	EVT_COMMAND_SCROLL_CHANGED(XRCID("VolumeSuperElf"), GuiMain::onVolume)

    EVT_MENU(XRCID("MI_GameMode"), GuiElf::setGameModeConfig)
    EVT_MENU(XRCID("MI_ChipMode"), GuiElf::setChipModeConfig)
	EVT_MENU(XRCID("MI_GiantBoard"), GuiElf::setGiantBoard)
	EVT_MENU(XRCID("MI_QuestLoader"), GuiElf::setQuestLoader)
	EVT_MENU(XRCID("MI_SuperBasic"), GuiElf::setSuperBasic1)
	EVT_MENU(XRCID("MI_SuperBasic3"), GuiElf::setSuperBasic3)
	EVT_MENU(XRCID("MI_SuperBasic5"), GuiElf::setSuperBasic5)
	EVT_MENU(XRCID("MI_SuperBasic6"), GuiElf::setSuperBasic6)
	EVT_MENU(XRCID("MI_SuperBasicSerial"), GuiElf::setSuperBasicSerial)
	EVT_MENU(XRCID("MI_RcaBasic"), GuiElf::setRcaBasic)
	EVT_MENU(XRCID("MI_RcaBasicPixie"), GuiElf::setRcaBasicPixie)
	EVT_MENU(XRCID("MI_RcaBasicSerial"), GuiElf::setRcaBasicSerial)
	EVT_MENU(XRCID("MI_RcaBasicElfOsInstall"), GuiElf::setRcaBasicElfOsInstall)
    EVT_MENU(XRCID("MI_ROM_Mapper_Pixie"), GuiElf::setRomMapperPixie)
    EVT_MENU(XRCID("MI_ROM_Mapper_Serial"), GuiElf::setRomMapperSerial)
    EVT_MENU(XRCID("MI_TinyBasicPixie"), GuiElf::setTinyBasicPixie)
	EVT_MENU(XRCID("MI_TinyBasicSerial"), GuiElf::setTinyBasicSerial)
	EVT_MENU(XRCID("MI_TinyBasic6847"), GuiElf::setTinyBasic6847)
	EVT_MENU(XRCID("MI_SuperGoldMonitor"), GuiElf::setSuperGoldMonitor)
	EVT_MENU(XRCID("MI_MonitorBasic"), GuiElf::setMonitorBasic)
	EVT_MENU(XRCID("MI_FigForth"), GuiElf::setFigForth)
	EVT_MENU(XRCID("MI_Music"), GuiElf::setMusic)
	EVT_MENU(XRCID("MI_SetElfOsInstallConfig"), GuiElf::setElfOsInstallConfig)
	EVT_MENU(XRCID("MI_SetElfOsConfig"), GuiElf::setElfOsConfig)
	EVT_MENU(XRCID("MI_SetElfOsIoConfig"), GuiElf::setElfOsIoConfig)
	EVT_MENU(XRCID("MI_SetElfOsIoInstallConfigPixie"), GuiElf::setElfOsIoInstallConfigPixie)
	EVT_MENU(XRCID("MI_SetElfOsIoConfigPixie"), GuiElf::setElfOsIoConfigPixie)
	EVT_MENU(XRCID("MI_SetElfOsIoInstallConfig6845"), GuiElf::setElfOsIoInstallConfig6845)
	EVT_MENU(XRCID("MI_SetElfOsIoConfig6845"), GuiElf::setElfOsIoConfig6845)
	EVT_MENU(XRCID("MI_SetElfOsIoInstallConfig6847"), GuiElf::setElfOsIoInstallConfig6847)
	EVT_MENU(XRCID("MI_SetElfOsIoConfig6847"), GuiElf::setElfOsIoConfig6847)
    EVT_MENU(XRCID("MI_SetElfOsIoInstallConfigTms"), GuiElf::setElfOsIoInstallConfigTms)
    EVT_MENU(XRCID("MI_SetElfOsIoConfigTms"), GuiElf::setElfOsIoConfigTms)
    EVT_MENU(XRCID("MI_SetElfOsIoConfigTmsGra"), GuiElf::setElfOsIoConfigTmsGra)

	EVT_BUTTON(XRCID("ColoursElf"), Main::onColoursDef)
	EVT_BUTTON(XRCID("ColoursElfII"), Main::onColoursDef)
	EVT_BUTTON(XRCID("ColoursSuperElf"), Main::onColoursDef)

	EVT_TEXT(XRCID("ShowAddressElf"), GuiMain::onLedTimer)
	EVT_TEXT(XRCID("ShowAddressElfII"), GuiMain::onLedTimer)
	EVT_TEXT(XRCID("ShowAddressSuperElf"), GuiMain::onLedTimer)

	EVT_TEXT(XRCID("SaveStartElf"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveStartElfII"), GuiMain::onSaveStart)
	EVT_TEXT(XRCID("SaveStartSuperElf"), GuiMain::onSaveStart)

	EVT_TEXT(XRCID("SaveEndElf"), GuiMain::onSaveEnd)
	EVT_TEXT(XRCID("SaveEndElfII"), GuiMain::onSaveEnd)
	EVT_TEXT(XRCID("SaveEndElfSuperII"), GuiMain::onSaveEnd)

	EVT_TEXT(XRCID("SaveExecElf"), GuiMain::onSaveExec)
	EVT_TEXT(XRCID("SaveExecElfII10"), GuiMain::onSaveExec)
	EVT_TEXT(XRCID("SaveExecSuperElf"), GuiMain::onSaveExec)

	EVT_COMMAND(wxID_ANY, ON_UART, GuiElf::onUart) 

	END_EVENT_TABLE()

GuiElf::GuiElf(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiElf2K(title, pos, size, mode, dataDir, iniDir)
{
	tapeOnBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/tick.png", wxBITMAP_TYPE_PNG);
	tapeOffBitmap = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/minus.png", wxBITMAP_TYPE_PNG);
}

void GuiElf::readElfConfig(int elfType, wxString elfTypeStr)
{
	bool romMode;
	selectedComputer_ = elfType;

    readElfPortConfig(elfType, elfTypeStr);
    
	conf[elfType].volume_ = 0;

    conf[elfType].configurationDir_ = iniDir_ + "Configurations" + pathSeparator_ + "Elf" + pathSeparator_;
    conf[elfType].mainDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Main", dataDir_ + "Elf" + pathSeparator_);
	conf[elfType].romDir_[MAINROM1] = readConfigDir("/Dir/"+elfTypeStr+"/Main_Rom_1_File", dataDir_ + "Elf" + pathSeparator_);
	conf[elfType].romDir_[MAINROM2] = readConfigDir("/Dir/"+elfTypeStr+"/Main_Rom_2_File", dataDir_ + "Elf" + pathSeparator_);
	conf[elfType].charRomDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Font_Rom_File", dataDir_ + "Elf" + pathSeparator_);
	conf[elfType].vtCharRomDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Vt_Font_Rom_File", dataDir_ + "Elf" + pathSeparator_);
	conf[elfType].ramDir_ = readConfigDir("/Dir/"+elfTypeStr+"/SWD", dataDir_ + "Elf" + pathSeparator_);
	conf[elfType].ideDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Ide_File", dataDir_ + "Elf" + pathSeparator_);
	conf[elfType].keyFileDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Key_File", dataDir_ + "Elf" + pathSeparator_);
	conf[elfType].printFileDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Print_File", dataDir_ + "Elf" + pathSeparator_);
	conf[elfType].screenDumpFileDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Video_Dump_File", dataDir_ + "Elf" + pathSeparator_);
	conf[elfType].wavFileDir_ = readConfigDir("/Dir/"+elfTypeStr+"/Wav_File", dataDir_ + "Elf" + pathSeparator_);
	elfConfiguration[elfType].vtWavFileDir_ = readConfigDir("/Dir/" + elfTypeStr + "/Vt_Wav_File", dataDir_ + "Elf" + pathSeparator_);

	conf[elfType].videoMode_ = (int)configPointer->Read(elfTypeStr+"/Video_Type", 0l);

	conf[elfType].rom_[MAINROM1] = configPointer->Read(elfTypeStr+"/Main_Rom_1_File", "bios.1.0.4.rom");
	conf[elfType].rom_[MAINROM2] = configPointer->Read(elfTypeStr+"/Main_Rom_2_File", "");
	conf[elfType].ide_ = configPointer->Read(elfTypeStr+"/Ide_File", "vt100.ide");
	conf[elfType].keyFile_ = configPointer->Read(elfTypeStr+"/Key_File", "");
	conf[elfType].printFile_ = configPointer->Read(elfTypeStr+"/Print_File", "printerout.txt");
	conf[elfType].screenDumpFile_ = configPointer->Read(elfTypeStr+"/Video_Dump_File", "screendump.png");
	conf[elfType].wavFile_ = configPointer->Read(elfTypeStr+"/Wav_File", "");
	elfConfiguration[elfType].vtWavFile_ = configPointer->Read(elfTypeStr + "/Vt_Wav_File", "");
    elfConfiguration[elfType].serialPort_ = configPointer->Read(elfTypeStr + "/VtSerialPortChoice", "");

	conf[elfType].saveStartString_ = "";
	conf[elfType].saveEndString_ = "";
	conf[elfType].saveExecString_ = "";

	elfConfiguration[elfType].qSound_ = (int)configPointer->Read(elfTypeStr+"/Enable_Q_Sound", 1l);
	elfConfiguration[elfType].vtType = (int)configPointer->Read(elfTypeStr+"/VT_Type", 2l);
    elfConfiguration[elfType].vt52SetUpFeature_ = configPointer->Read(elfTypeStr+"/VT52Setup", 0x00004092l);
    elfConfiguration[elfType].vt100SetUpFeature_ = configPointer->Read(elfTypeStr+"/VT100Setup", 0x0000ca52l);
    elfConfiguration[elfType].vtExternalSetUpFeature_ = configPointer->Read(elfTypeStr+"/VTExternalSetup", 0x0000ca52l);
	elfConfiguration[elfType].baudR = (int)configPointer->Read(elfTypeStr+"/Vt_Baud_Receive", 0l);
	elfConfiguration[elfType].baudT = (int)configPointer->Read(elfTypeStr+"/Vt_Baud_Transmit", 0l);
	elfConfiguration[elfType].diskType = (int)configPointer->Read(elfTypeStr+"/Disk_Type", 2l);
	elfConfiguration[elfType].keyboardType = (int)configPointer->Read(elfTypeStr+"/Keyboard_Type", 0l);
	elfConfiguration[elfType].memoryType = (int)configPointer->Read(elfTypeStr+"/Memory_Type", 0l);

	elfConfiguration[elfType].bellFrequency_ = (int)configPointer->Read(elfTypeStr + "/Bell_Frequency", 800);
    configPointer->Read(elfTypeStr+"/SerialLog", &elfConfiguration[elfType].serialLog, false);
    configPointer->Read(elfTypeStr+"/Uart", &elfConfiguration[elfType].useUart, false);
	configPointer->Read(elfTypeStr+"/Enable_Auto_Boot", &elfConfiguration[elfType].autoBoot, true);
	configPointer->Read(elfTypeStr+"/Force_Uppercase", &elfConfiguration[elfType].forceUpperCase, true);
	configPointer->Read(elfTypeStr+"/Enable_Printer", &conf[elfType].printerOn_, false);
	configPointer->Read(elfTypeStr+"/Enable_Extended_Ports", &elfConfiguration[elfType].usePortExtender, false);
	configPointer->Read(elfTypeStr+"/Open_Control_Windows", &elfConfiguration[elfType].useElfControlWindows, false);
	configPointer->Read(elfTypeStr+"/Enable_Interlace", &conf[elfType].interlace_, true);
	configPointer->Read(elfTypeStr+"/Enable_Vt_Stretch_Dot", &conf[elfType].stretchDot_, false);
    configPointer->Read(elfTypeStr+"/Enable_Vt_External", &elfConfiguration[elfType].vtExternal, false);
	conf[elfType].printMode_ = (int)configPointer->Read(elfTypeStr+"/Print_Mode", 1l);

    configPointer->Read(elfTypeStr+"/GiantBoardMapping", &elfConfiguration[elfType].giantBoardMapping, false);
    
    wxString defaultZoom;
	defaultZoom.Printf("%2.2f", 2.0);
	conf[elfType].zoom_ = configPointer->Read(elfTypeStr+"/Zoom", defaultZoom);
	defaultZoom.Printf("%2.2f", 1.0);
	conf[elfType].zoomVt_ = configPointer->Read(elfTypeStr+"/Vt_Zoom", defaultZoom);
	wxString defaultScale;
	defaultScale.Printf("%i", 3);
	conf[elfType].xScale_ = configPointer->Read(elfTypeStr+"/Window_Scale_Factor_X", defaultScale);

	wxString defaultClock;
	defaultClock.Printf("%1.2f", 1.76);
	conf[elfType].clock_ = configPointer->Read(elfTypeStr+"/Clock_Speed", defaultClock);
	wxString defaultTimer;
	defaultTimer.Printf("%d", 100);
	conf[elfType].ledTime_ = configPointer->Read(elfTypeStr+"/Led_Update_Frequency", defaultTimer);

	conf[elfType].beepFrequency_ = (int)configPointer->Read(elfTypeStr+"/Beep_Frequency", 250);

	configPointer->Read(elfTypeStr+"/Enable_Turbo_Cassette", &conf[elfType].turbo_, true);
	conf[elfType].turboClock_ = configPointer->Read(elfTypeStr+"/Turbo_Clock_Speed", "15");
	configPointer->Read(elfTypeStr+"/Enable_Auto_Cassette", &conf[elfType].autoCassetteLoad_, true);
	configPointer->Read(elfTypeStr+"/Enable_Cassette", &elfConfiguration[elfType].useTape, false);
	configPointer->Read(elfTypeStr+"/Enable_Real_Cassette", &conf[elfType].realCassetteLoad_, false);
	conf[elfType].volume_ = (int)configPointer->Read(elfTypeStr+"/Volume", 25l);

	long value; 
	wxString bootAddress = configPointer->Read(elfTypeStr+"/Boot_Address", "FF00");
	if (!bootAddress.ToLong(&value, 16))
		value = 0x9000;
	conf[elfType].bootAddress_ = value;

	wxString startRam = configPointer->Read(elfTypeStr+"/Ram_Start_Address", "0");
	if (!startRam.ToLong(&value, 16))
		value = 0;
	startRam_[elfType] = value;

	wxString endRam = configPointer->Read(elfTypeStr+"/Ram_End_Address", "FFFF");
	if (!endRam.ToLong(&value, 16))
		value = 0xffff;
	endRam_[elfType] = value;

	if (mode_.gui)
		setBaudChoice(elfType);

	setVtType(elfTypeStr, elfType, elfConfiguration[elfType].vtType, false);
	setVideoType(elfTypeStr, elfType, conf[elfType].videoMode_);

	conf[elfType].charRom_ = configPointer->Read(elfTypeStr+"/Font_Rom_File", "super.video.bin");
	conf[elfType].vtCharRom_ = configPointer->Read(elfTypeStr+"/Vt_Font_Rom_File", "vt100.bin");

	if (mode_.gui)
	{
		setPrinterState(elfType);
		XRCCTRL(*this, "MainRom"+elfTypeStr, wxComboBox)->SetValue(conf[elfType].rom_[MAINROM1]);
		XRCCTRL(*this, "MainRom2"+elfTypeStr, wxComboBox)->SetValue(conf[elfType].rom_[MAINROM2]);
		XRCCTRL(*this, "CharRom"+elfTypeStr, wxComboBox)->SetValue(conf[elfType].charRom_);
		XRCCTRL(*this, "VtCharRom"+elfTypeStr, wxComboBox)->SetValue(conf[elfType].vtCharRom_);
		XRCCTRL(*this, "IdeFile"+elfTypeStr, wxTextCtrl)->SetValue(conf[elfType].ide_);
		XRCCTRL(*this, "KeyFile"+elfTypeStr, wxTextCtrl)->SetValue(conf[elfType].keyFile_);
		XRCCTRL(*this, "PrintFile"+elfTypeStr, wxTextCtrl)->SetValue(conf[elfType].printFile_);
		XRCCTRL(*this, "ScreenDumpFile"+elfTypeStr, wxComboBox)->SetValue(conf[elfType].screenDumpFile_);
		XRCCTRL(*this, "WavFile"+elfTypeStr, wxTextCtrl)->SetValue(conf[elfType].wavFile_);

		XRCCTRL(*this, "Qsound"+elfTypeStr, wxChoice)->SetSelection(elfConfiguration[elfType].qSound_);
		XRCCTRL(*this, "VTType"+elfTypeStr, wxChoice)->SetSelection(elfConfiguration[elfType].vtType);
        XRCCTRL(*this, "BeepFrequency"+elfTypeStr, wxTextCtrl)->Enable(elfConfiguration[elfType].qSound_ == QSOUNDEXT);

		XRCCTRL(*this, "VTBaudRChoice" + elfTypeStr, wxChoice)->SetSelection(elfConfiguration[elfType].baudR);
		XRCCTRL(*this, "VTBaudTChoice" + elfTypeStr, wxChoice)->SetSelection(elfConfiguration[elfType].baudT);
//		XRCCTRL(*this, "VTBaudRChoice" + elfTypeStr, wxChoice)->Enable((elfConfiguration[elfType].vtType != VTNONE) && elfConfiguration[elfType].useUart);
        XRCCTRL(*this, "BeepFrequencyText"+elfTypeStr, wxStaticText)->Enable(elfConfiguration[elfType].qSound_ == QSOUNDEXT);
        XRCCTRL(*this, "BeepFrequencyTextHz"+elfTypeStr, wxStaticText)->Enable(elfConfiguration[elfType].qSound_ == QSOUNDEXT);
//        XRCCTRL(*this, "VTBaudRText" + elfTypeStr, wxStaticText)->Enable((elfConfiguration[elfType].vtType != VTNONE) && elfConfiguration[elfType].useUart);
//        XRCCTRL(*this, "VTBaudTText" + elfTypeStr, wxStaticText)->Enable(elfConfiguration[elfType].vtType != VTNONE);
        XRCCTRL(*this,"AddressText1"+elfTypeStr, wxStaticText)->Enable(elfConfiguration[elfType].useElfControlWindows);
        XRCCTRL(*this,"AddressText2"+elfTypeStr, wxStaticText)->Enable(elfConfiguration[elfType].useElfControlWindows);
//        XRCCTRL(*this, "VTBaudTChoice" + elfTypeStr, wxChoice)->Enable(elfConfiguration[elfType].vtType != VTNONE);

		XRCCTRL(*this, "VideoType"+elfTypeStr, wxChoice)->SetSelection(conf[elfType].videoMode_);
		XRCCTRL(*this, "AutoBoot"+elfTypeStr, wxCheckBox)->SetValue(elfConfiguration[elfType].autoBoot);
		XRCCTRL(*this, "BootAddress"+elfTypeStr, wxTextCtrl)->SetValue(bootAddress);
		XRCCTRL(*this, "StartRam"+elfTypeStr, wxTextCtrl)->SetValue(startRam);
		XRCCTRL(*this, "EndRam"+elfTypeStr, wxTextCtrl)->SetValue(endRam);
		XRCCTRL(*this, "UpperCase"+elfTypeStr, wxCheckBox)->SetValue(elfConfiguration[elfType].forceUpperCase);
		XRCCTRL(*this, "DiskType"+elfTypeStr, wxChoice)->SetSelection(elfConfiguration[elfType].diskType);
		XRCCTRL(*this, "Memory"+elfTypeStr, wxChoice)->SetSelection(elfConfiguration[elfType].memoryType);
//		XRCCTRL(*this, "VtCharRomButton"+elfTypeStr, wxButton)->Enable(elfConfiguration[elfType].vtType != VTNONE);
//		XRCCTRL(*this, "VtCharRom"+elfTypeStr, wxComboBox)->Enable(elfConfiguration[elfType].vtType != VTNONE);
//		XRCCTRL(*this, "VtSetup"+elfTypeStr, wxButton)->Enable(elfConfiguration[elfType].vtType != VTNONE);
		XRCCTRL(*this, "Keyboard"+elfTypeStr, wxChoice)->SetSelection(elfConfiguration[elfType].keyboardType);
		XRCCTRL(*this, "ZoomValue"+elfTypeStr, wxTextCtrl)->ChangeValue(conf[elfType].zoom_);
		XRCCTRL(*this, "ZoomValueVt"+elfTypeStr, wxTextCtrl)->ChangeValue(conf[elfType].zoomVt_);
		XRCCTRL(*this, "PortExt"+elfTypeStr, wxCheckBox)->SetValue(elfConfiguration[elfType].usePortExtender);
		XRCCTRL(*this, "ControlWindows"+elfTypeStr, wxCheckBox)->SetValue(elfConfiguration[elfType].useElfControlWindows);
		XRCCTRL(*this, "Interlace"+elfTypeStr, wxCheckBox)->SetValue(conf[elfType].interlace_);
 //       XRCCTRL(*this, "VtExternal"+elfTypeStr, wxCheckBox)->SetValue(elfConfiguration[elfType].vtExternal);
        XRCCTRL(*this, "StretchDot"+elfTypeStr, wxCheckBox)->SetValue(conf[elfType].stretchDot_);

        if (elfType == ELFII)
            XRCCTRL(*this, "Giant"+elfTypeStr, wxCheckBox)->SetValue(elfConfiguration[elfType].giantBoardMapping);

		setPrinterState(elfType);
		XRCCTRL(*this, "PrintMode"+elfTypeStr, wxChoice)->SetSelection(conf[elfType].printMode_);
		setPrintMode();

		clockTextCtrl[elfType]->ChangeValue(conf[elfType].clock_);
		wxString beepFrequency;
		beepFrequency.Printf("%d", conf[elfType].beepFrequency_);
		XRCCTRL(*this, "BeepFrequency"+elfTypeStr, wxTextCtrl)->ChangeValue(beepFrequency);

		XRCCTRL(*this, "Turbo"+elfTypeStr, wxCheckBox)->SetValue(conf[elfType].turbo_);
		turboGui(elfTypeStr);

		XRCCTRL(*this, "TurboClock"+elfTypeStr, wxTextCtrl)->SetValue(conf[elfType].turboClock_);
		XRCCTRL(*this, "AutoCasLoad"+elfTypeStr, wxCheckBox)->SetValue(conf[elfType].autoCassetteLoad_);
		setTapeType(elfTypeStr, elfType);

		XRCCTRL(*this, "Volume"+elfTypeStr, wxSlider)->SetValue(conf[elfType].volume_);

		XRCCTRL(*this, "ShowAddress"+elfTypeStr, wxTextCtrl)->ChangeValue(conf[elfType].ledTime_);
		XRCCTRL(*this,"ShowAddress"+elfTypeStr, wxTextCtrl)->Enable(elfConfiguration[elfType].useElfControlWindows);

/*#ifdef __WXMAC__
        XRCCTRL(*this,"Eject_IDE"+elfTypeStr, wxBitmapButton)->SetSize(26, 27);
        XRCCTRL(*this,"EjectKeyFile"+elfTypeStr, wxBitmapButton)->SetSize(26, 27);
        XRCCTRL(*this,"Eject_IDE"+elfTypeStr, wxBitmapButton)->SetSize(26, 27);
        XRCCTRL(*this,"PrintButton"+elfTypeStr, wxBitmapButton)->SetSize(26, 27);
        XRCCTRL(*this,"ScreenDumpF5"+elfTypeStr, wxBitmapButton)->SetSize(26, 27);
        XRCCTRL(*this,"Tape"+elfTypeStr, wxBitmapButton)->SetSize(26, 27);
        XRCCTRL(*this,"EjectCas"+elfTypeStr, wxBitmapButton)->SetSize(26, 27);
#endif*/
    }

	setDiskType(elfTypeStr, elfType, elfConfiguration[elfType].diskType);
	setElfKeyboard(elfTypeStr, elfType, elfConfiguration[elfType].keyboardType);
	setRealCas(elfType);

	configPointer->Read(elfTypeStr+"/Load_Mode_Rom_1", &romMode, true);
	if (romMode)
	{
		loadromMode_[elfType][0] = ROM;
		if (mode_.gui)
		{
			XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetLabel("ROM 1");
			XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetToolTip("Browse for Elf ROM file");
		}
	}
	else
	{
		loadromMode_[elfType][0] = RAM;
		if (mode_.gui)
		{
			XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetLabel("RAM 1");
			XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetToolTip("Browse for Elf RAM file");
		}
	}

	configPointer->Read(elfTypeStr+"/Load_Mode_Rom_2", &romMode, false);
	if (romMode)
	{
		loadromMode_[elfType][1] = ROM;
		if (mode_.gui)
		{
			XRCCTRL(*this, "RomButton2"+elfTypeStr, wxButton)->SetLabel("ROM 2");
			XRCCTRL(*this, "RomButton2"+elfTypeStr, wxButton)->SetToolTip("Browse for Elf ROM file");
		}
	}
	else
	{
		loadromMode_[elfType][1] = RAM;
		if (mode_.gui)
		{
			XRCCTRL(*this, "RomButton2"+elfTypeStr, wxButton)->SetLabel("RAM 2");
			XRCCTRL(*this, "RomButton2"+elfTypeStr, wxButton)->SetToolTip("Browse for Elf RAM file");
		}
	}

    setMemory(elfTypeStr, elfType, elfConfiguration[elfType].memoryType);

	if (elfType == ELF)
	{
		configPointer->Read("/Elf/Enable_Led_Module", &elfConfiguration[ELF].useLedModule, false);
		if (mode_.gui)
		{
			XRCCTRL(*this, "UseLedModule", wxCheckBox)->SetValue(elfConfiguration[ELF].useLedModule);
			XRCCTRL(*this,"UseLedModule",wxCheckBox)->Enable(elfConfiguration[ELF].useElfControlWindows);
			if (!elfConfiguration[ELF].useElfControlWindows)
				XRCCTRL(*this,"UseLedModule",wxCheckBox)->SetValue(false);
		}
	}
	else
	{
		elfConfiguration[elfType].useLedModule = false;
	}

	conf[elfType].loadFileNameFull_ = "";
	conf[elfType].loadFileName_ = "";

	conf[elfType].pLoadSaveName_[0] = 'S';
	conf[elfType].pLoadSaveName_[1] = 'U';
	conf[elfType].pLoadSaveName_[2] = 'P';
	conf[elfType].pLoadSaveName_[3] = 'E';

	conf[elfType].defus_ = 0x2a81;
	conf[elfType].eop_ = 0x2a83;
	conf[elfType].string_ = 0x2a92;
	conf[elfType].arrayValue_ = 0x2a94;
	conf[elfType].eod_ = 0x2a99;
	conf[elfType].basicRamAddress_ = 0x2c00;
	conf[elfType].useLoadLocation_ = false;
}

void GuiElf::writeElfDirConfig(int elfType, wxString elfTypeStr)
{
	writeConfigDir("/Dir/" + elfTypeStr + "/Main", conf[elfType].mainDir_);
	writeConfigDir("/Dir/" + elfTypeStr + "/Main_Rom_1_File", conf[elfType].romDir_[MAINROM1]);
	writeConfigDir("/Dir/" + elfTypeStr + "/Main_Rom_2_File", conf[elfType].romDir_[MAINROM2]);
	writeConfigDir("/Dir/" + elfTypeStr + "/Font_Rom_File", conf[elfType].charRomDir_);
	writeConfigDir("/Dir/" + elfTypeStr + "/Vt_Font_Rom_File", conf[elfType].vtCharRomDir_);
	writeConfigDir("/Dir/" + elfTypeStr + "/Software_File", conf[elfType].ramDir_);
	writeConfigDir("/Dir/" + elfTypeStr + "/Ide_File", conf[elfType].ideDir_);
	writeConfigDir("/Dir/" + elfTypeStr + "/Key_File", conf[elfType].keyFileDir_);
    writeConfigDir("/Dir/" + elfTypeStr + "/Print_File", conf[elfType].printFileDir_);
	writeConfigDir("/Dir/" + elfTypeStr + "/Video_Dump_File", conf[elfType].screenDumpFileDir_);
	writeConfigDir("/Dir/" + elfTypeStr + "/Wav_File", conf[elfType].wavFileDir_);
	writeConfigDir("/Dir/" + elfTypeStr + "/Vt_Wav_File", elfConfiguration[elfType].vtWavFileDir_);
}
	
void GuiElf::writeElfConfig(int elfType, wxString elfTypeStr)
{
	wxString buffer;

	writeElfPortConfig(elfType, elfTypeStr);

	configPointer->Write(elfTypeStr + "/Main_Rom_1_File", conf[elfType].rom_[MAINROM1]);
	configPointer->Write(elfTypeStr+"/Main_Rom_2_File", conf[elfType].rom_[MAINROM2]);
	configPointer->Write(elfTypeStr+"/Font_Rom_File", conf[elfType].charRom_);
	configPointer->Write(elfTypeStr+"/Vt_Font_Rom_File", conf[elfType].vtCharRom_);
	configPointer->Write(elfTypeStr+"/Ide_File", conf[elfType].ide_);
	configPointer->Write(elfTypeStr+"/Key_File", conf[elfType].keyFile_);
	configPointer->Write(elfTypeStr+"/Print_File", conf[elfType].printFile_);
	configPointer->Write(elfTypeStr+"/Video_Dump_File", conf[elfType].screenDumpFile_);
	configPointer->Write(elfTypeStr+"/Wav_File", conf[elfType].wavFile_);
	configPointer->Write(elfTypeStr+"/Vt_Wav_File", elfConfiguration[elfType].vtWavFile_);
    configPointer->Write(elfTypeStr+"/VtSerialPortChoice", elfConfiguration[elfType].serialPort_);

	configPointer->Write(elfTypeStr+"/Load_Mode_Rom_1", (loadromMode_[elfType][0] == ROM));
	configPointer->Write(elfTypeStr+"/Load_Mode_Rom_2", (loadromMode_[elfType][1] == ROM));
	configPointer->Write(elfTypeStr+"/Disk_Type", elfConfiguration[elfType].diskType);
	configPointer->Write(elfTypeStr+"/Enable_Q_Sound", elfConfiguration[elfType].qSound_);
	configPointer->Write(elfTypeStr+"/Memory_Type", elfConfiguration[elfType].memoryType);
	buffer.Printf("%04X", (unsigned int)startRam_[elfType]);
	configPointer->Write(elfTypeStr+"/Ram_Start_Address", buffer);
	buffer.Printf("%04X", (unsigned int)endRam_[elfType]);
	configPointer->Write(elfTypeStr+"/Ram_End_Address", buffer);
	configPointer->Write(elfTypeStr+"/VT_Type", elfConfiguration[elfType].vtType);

    
    long value = elfConfiguration[elfType].vt52SetUpFeature_.to_ulong();
    configPointer->Write(elfTypeStr+"/VT52Setup", value);
    value = elfConfiguration[elfType].vt100SetUpFeature_.to_ulong();
    configPointer->Write(elfTypeStr+"/VT100Setup", value);
    value = elfConfiguration[elfType].vtExternalSetUpFeature_.to_ulong();
    configPointer->Write(elfTypeStr+"/VTExternalSetup", value);

	configPointer->Write(elfTypeStr+"/Vt_Baud_Receive", elfConfiguration[elfType].baudR);
	configPointer->Write(elfTypeStr+"/Vt_Baud_Transmit", elfConfiguration[elfType].baudT);
	configPointer->Write(elfTypeStr + "/Bell_Frequency", elfConfiguration[elfType].bellFrequency_);
    configPointer->Write(elfTypeStr+"/SerialLog", elfConfiguration[elfType].serialLog);
    configPointer->Write(elfTypeStr+"/Uart", elfConfiguration[elfType].useUart);
    configPointer->Write(elfTypeStr+"/Enable_Auto_Boot", elfConfiguration[elfType].autoBoot);
	buffer.Printf("%04X", (unsigned int)conf[elfType].bootAddress_);
	configPointer->Write(elfTypeStr+"/Boot_Address", buffer);
	configPointer->Write(elfTypeStr+"/Video_Type", conf[elfType].videoMode_);
	configPointer->Write(elfTypeStr+"/Keyboard_Type", elfConfiguration[elfType].keyboardType);
	configPointer->Write(elfTypeStr+"/Zoom", conf[elfType].zoom_);
	configPointer->Write(elfTypeStr+"/Vt_Zoom", conf[elfType].zoomVt_);
	configPointer->Write(elfTypeStr+"/Force_Uppercase", elfConfiguration[elfType].forceUpperCase);

    configPointer->Write(elfTypeStr+"/GiantBoardMapping", elfConfiguration[elfType].giantBoardMapping);

	configPointer->Write(elfTypeStr+"/Enable_Printer", conf[elfType].printerOn_);
	configPointer->Write(elfTypeStr + "/Print_Mode", conf[elfType].printMode_);

    configPointer->Write(elfTypeStr+"/Enable_Extended_Ports", elfConfiguration[elfType].usePortExtender);
	configPointer->Write(elfTypeStr+"/Open_Control_Windows", elfConfiguration[elfType].useElfControlWindows);
	configPointer->Write(elfTypeStr+"/Enable_Interlace", conf[elfType].interlace_);
	configPointer->Write(elfTypeStr+"/Enable_Vt_Stretch_Dot", conf[elfType].stretchDot_);
    configPointer->Write(elfTypeStr+"/Enable_Vt_External", elfConfiguration[elfType].vtExternal);

	configPointer->Write(elfTypeStr+"/Clock_Speed", conf[elfType].clock_);
	configPointer->Write(elfTypeStr+"/Beep_Frequency", conf[elfType].beepFrequency_);

	if (elfTypeStr == "Elf")
		configPointer->Write(elfTypeStr+"/Enable_Led_Module", elfConfiguration[elfType].useLedModule);

	configPointer->Write(elfTypeStr+"/Led_Update_Frequency", conf[elfType].ledTime_);
	configPointer->Write(elfTypeStr+"/Enable_Turbo_Cassette", conf[elfType].turbo_);
	configPointer->Write(elfTypeStr+"/Turbo_Clock_Speed", conf[elfType].turboClock_);
	configPointer->Write(elfTypeStr+"/Enable_Auto_Cassette", conf[elfType].autoCassetteLoad_);
	configPointer->Write(elfTypeStr+"/Enable_Real_Cassette", conf[elfType].realCassetteLoad_);
	configPointer->Write(elfTypeStr+"/Enable_Cassette", elfConfiguration[elfType].useTape);
	configPointer->Write(elfTypeStr+"/Volume", conf[elfType].volume_);
}

void GuiElf::readElfWindowConfig(int elfType, wxString elfTypeStr)
{
    conf[elfType].pixieX_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Pixie_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[elfType].pixieY_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Pixie_Y", mainWindowY_);
    conf[elfType].tmsX_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Tms_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[elfType].tmsY_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Tms_Y", mainWindowY_);
    conf[elfType].mc6845X_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_MC6845_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[elfType].mc6845Y_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_MC6845_Y", mainWindowY_);
    conf[elfType].mc6847X_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_MC6847_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[elfType].mc6847Y_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_MC6847_Y", mainWindowY_);
    conf[elfType].i8275X_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_I8275_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[elfType].i8275Y_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_I8275_Y", mainWindowY_);
    conf[elfType].vtX_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Vt_X", mainWindowX_+windowInfo.mainwX+windowInfo.xBorder);
    conf[elfType].vtY_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Vt_Y", mainWindowY_);
    conf[elfType].mainX_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_X", mainWindowX_);
    conf[elfType].mainY_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
    
    if (elfType == ELF)
    {
        ledModuleX_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Led_Module_X", mainWindowX_+346+windowInfo.xBorder2);
        ledModuleY_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Led_Module_Y", mainWindowY_+windowInfo.mainwY+229+windowInfo.yBorder2);
        conf[elfType].keypadX_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Keypad_X", mainWindowX_+346+windowInfo.xBorder2);
        conf[elfType].keypadY_ = (int)configPointer->Read(elfTypeStr+"/Window_Position_Keypad_Y", mainWindowY_+windowInfo.mainwY+windowInfo.yBorder);
    }
}

void GuiElf::writeElfWindowConfig(int elfType, wxString elfTypeStr)
{
	if (conf[elfType].mainX_ > 0)
		configPointer->Write(elfTypeStr + "/MainX", conf[elfType].mainX_);
	if (conf[elfType].mainY_ > 0)
		configPointer->Write(elfTypeStr + "/MainY", conf[elfType].mainY_);
	if (conf[elfType].pixieX_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_Pixie_X", conf[elfType].pixieX_);
	if (conf[elfType].pixieY_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_Pixie_Y", conf[elfType].pixieY_);
	if (conf[elfType].tmsX_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_Tms_X", conf[elfType].tmsX_);
	if (conf[elfType].tmsY_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_Tms_Y", conf[elfType].tmsY_);
	if (conf[elfType].mc6845X_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_MC6845_X", conf[elfType].mc6845X_);
	if (conf[elfType].mc6845Y_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_MC6845_Y", conf[elfType].mc6845Y_);
	if (conf[elfType].mc6847X_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_MC6847_X", conf[elfType].mc6847X_);
	if (conf[elfType].mc6847Y_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_MC6847_Y", conf[elfType].mc6847Y_);
	if (conf[elfType].i8275X_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_I8275_X", conf[elfType].i8275X_);
	if (conf[elfType].i8275Y_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_I8275_Y", conf[elfType].i8275Y_);
	if (conf[elfType].vtX_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_Vt_X", conf[elfType].vtX_);
	if (conf[elfType].vtY_ > 0)
		configPointer->Write(elfTypeStr + "/Window_Position_Vt_Y", conf[elfType].vtY_);

	if (elfType == ELF)
	{
		if (conf[elfType].keypadX_ > 0)
			configPointer->Write(elfTypeStr + "/Window_Position_Keypad_X", conf[elfType].keypadX_);
		if (conf[elfType].keypadY_ > 0)
			configPointer->Write(elfTypeStr + "/Window_Position_Keypad_Y", conf[elfType].keypadY_);
		if (ledModuleX_ > 0)
			configPointer->Write(elfTypeStr + "/Window_Position_Led_Module_X", ledModuleX_);
		if (ledModuleY_ > 0)
			configPointer->Write(elfTypeStr + "/Window_Position_Led_Module_Y", ledModuleY_);
	}
}

void GuiElf::onDiskType(wxCommandEvent&event)
{
	setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, event.GetSelection());
}

void GuiElf::onMemory(wxCommandEvent&event)
{
	setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, event.GetSelection());
}

void GuiElf::onAutoBoot(wxCommandEvent&event)
{
	elfConfiguration[selectedComputer_].autoBoot = event.IsChecked();
}

void GuiElf::onStartRam(wxCommandEvent&WXUNUSED(event))
{
	startRam_[selectedComputer_] = get16BitValue("StartRam"+computerInfo[selectedComputer_].gui);
	if (startRam_[selectedComputer_] == -1)  startRam_[selectedComputer_] = 0;
}

void GuiElf::onEndRam(wxCommandEvent&WXUNUSED(event))
{
	endRam_[selectedComputer_] = get16BitValue("EndRam"+computerInfo[selectedComputer_].gui);
	if (endRam_[selectedComputer_] == -1)  endRam_[selectedComputer_] = 0xffff;
}

void GuiElf::onVideoType(wxCommandEvent&event)
{
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, event.GetSelection());
}

void GuiElf::onElfKeyboard(wxCommandEvent&event)
{
	setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, event.GetSelection());
}

void GuiElf::onForceUpperCase(wxCommandEvent&event)
{
	elfConfiguration[selectedComputer_].forceUpperCase = event.IsChecked();
	switch(runningComputer_)
	{
		case ELF:
			p_Elf->setForceUpperCase(event.IsChecked());
		break;

		case ELFII:
			p_Elf2->setForceUpperCase(event.IsChecked());
		break;

		case SUPERELF:
			p_Super->setForceUpperCase(event.IsChecked());
		break;
	}
}

void GuiElf::onGiantBoardMapping(wxCommandEvent&event)
{
    elfConfiguration[selectedComputer_].giantBoardMapping = event.IsChecked();
}

void GuiElf::setMemory(wxString elfTypeStr, int elfType, int Selection)
{
    if (elfConfiguration[selectedComputer_].memoryType == 3)
    {
        if (mode_.gui)
        {
            if (loadromMode_[elfType][0] == ROM)
            {
                XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetLabel("ROM 1");
                XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetToolTip("Browse for Elf ROM file");
            }
            else
            {
                XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetLabel("RAM 1");
                XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetToolTip("Browse for Elf RAM file");
            }
            XRCCTRL(*this, "Rom1"+elfTypeStr, wxBitmapButton)->Enable(true);
        }
    }
    
    elfConfiguration[selectedComputer_].memoryType = Selection;
	switch(Selection)
	{
		case MEM_64K:
			elfConfiguration[elfType].useEms = false;
            elfConfiguration[elfType].usePager = false;
            elfConfiguration[elfType].useRomMapper = false;
			if (mode_.gui)
			{
				XRCCTRL(*this, "StartRamText"+elfTypeStr, wxStaticText)->Enable(true);
				XRCCTRL(*this, "StartRam"+elfTypeStr, wxTextCtrl)->Enable(true);
				XRCCTRL(*this, "EndRamText"+elfTypeStr, wxStaticText)->Enable(true);
				XRCCTRL(*this, "EndRam"+elfTypeStr, wxTextCtrl)->Enable(true);
				XRCCTRL(*this, "PortExt"+elfTypeStr, wxCheckBox)->Enable(true);
			}
		break;

		case MEM_EMS:
			elfConfiguration[elfType].useEms = true;
			elfConfiguration[elfType].usePager = false;
            elfConfiguration[elfType].useRomMapper = false;
			if (mode_.gui)
			{
				XRCCTRL(*this, "StartRamText"+elfTypeStr, wxStaticText)->Enable(false);
				XRCCTRL(*this, "StartRam"+elfTypeStr, wxTextCtrl)->Enable(false);
				XRCCTRL(*this, "EndRamText"+elfTypeStr, wxStaticText)->Enable(false);
				XRCCTRL(*this, "EndRam"+elfTypeStr, wxTextCtrl)->Enable(false);
				XRCCTRL(*this, "PortExt"+elfTypeStr, wxCheckBox)->Enable(true);
			}
		break;

		case MEM_MAP:
			elfConfiguration[elfType].useEms = false;
			elfConfiguration[elfType].usePager = true;
			elfConfiguration[elfType].usePortExtender = true;
            elfConfiguration[elfType].useRomMapper = false;
			if (mode_.gui)
			{
				XRCCTRL(*this, "PortExt"+elfTypeStr, wxCheckBox)->Enable(false);
				XRCCTRL(*this, "PortExt"+elfTypeStr, wxCheckBox)->SetValue(true);
				XRCCTRL(*this, "StartRamText"+elfTypeStr, wxStaticText)->Enable(false);
				XRCCTRL(*this, "StartRam"+elfTypeStr, wxTextCtrl)->Enable(false);
				XRCCTRL(*this, "EndRamText"+elfTypeStr, wxStaticText)->Enable(false);
				XRCCTRL(*this, "EndRam"+elfTypeStr, wxTextCtrl)->Enable(false);
			}
		break;

        case MEM_ROM_MAP:
            elfConfiguration[elfType].useEms = false;
            elfConfiguration[elfType].usePager = false;
            elfConfiguration[elfType].useRomMapper = true;
            if (mode_.gui)
            {
                XRCCTRL(*this, "StartRamText"+elfTypeStr, wxStaticText)->Enable(false);
                XRCCTRL(*this, "StartRam"+elfTypeStr, wxTextCtrl)->Enable(false);
                XRCCTRL(*this, "EndRamText"+elfTypeStr, wxStaticText)->Enable(false);
                XRCCTRL(*this, "EndRam"+elfTypeStr, wxTextCtrl)->Enable(false);
                XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetLabel("Mapper");
                XRCCTRL(*this, "RomButton"+elfTypeStr, wxButton)->SetToolTip("Browse for Elf ROM Mapper file");
                XRCCTRL(*this, "Rom1"+elfTypeStr, wxBitmapButton)->Enable(false);
                XRCCTRL(*this, "EndRam"+elfTypeStr, wxTextCtrl)->SetValue("7FFF");
            }
        break;
    }
}

void GuiElf::onUsePortExtender(wxCommandEvent&event)
{
	elfConfiguration[selectedComputer_].usePortExtender = event.IsChecked();
}

void GuiElf::onLedModule(wxCommandEvent&event)
{
	elfConfiguration[selectedComputer_].useLedModule = event.IsChecked();
}

void GuiElf::onElfControlWindows(wxCommandEvent&event)
{
	elfConfiguration[selectedComputer_].useElfControlWindows = event.IsChecked();

	if (mode_.gui)
	{
		XRCCTRL(*this,"ShowAddress"+computerInfo[selectedComputer_].gui,wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
        XRCCTRL(*this,"AddressText1"+computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
		XRCCTRL(*this,"AddressText2"+computerInfo[selectedComputer_].gui,wxStaticText)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
		if (selectedComputer_ == ELF && (runningComputer_ != ELF))
		{
			XRCCTRL(*this,"UseLedModule",wxCheckBox)->Enable(elfConfiguration[selectedComputer_].useElfControlWindows);
			if (!elfConfiguration[selectedComputer_].useElfControlWindows)
				XRCCTRL(*this,"UseLedModule",wxCheckBox)->SetValue(false);
		}
	}

	switch (runningComputer_)
	{
		case ELF:
			p_Elf->Show(elfConfiguration[runningComputer_].useElfControlWindows);
			p_Elf->showModules(elfConfiguration[runningComputer_].useElfControlWindows);
		break;

		case ELFII:
			p_Elf2->Show(elfConfiguration[runningComputer_].useElfControlWindows);
		break;

		case SUPERELF:
			p_Super->Show(elfConfiguration[runningComputer_].useElfControlWindows);
		break;

	}
}

void GuiElf::onRom1(wxCommandEvent&WXUNUSED(event))
{
	if (loadromMode_[selectedComputer_][0] == ROM)
	{
		loadromMode_[selectedComputer_][0] = RAM;
		XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 1");
		XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetToolTip("Browse for Elf RAM file");
	}
	else
	{
		loadromMode_[selectedComputer_][0] = ROM;
		XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("ROM 1");
		XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetToolTip("Browse for Elf ROM file");
	}
}

void GuiElf::onRom2(wxCommandEvent&WXUNUSED(event))
{
	if (loadromMode_[selectedComputer_][1] == ROM)
	{
		loadromMode_[selectedComputer_][1] = RAM;
		XRCCTRL(*this, "RomButton2"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 2");
		XRCCTRL(*this, "RomButton2"+computerInfo[selectedComputer_].gui, wxButton)->SetToolTip("Browse for Elf RAM file");
	}
	else
	{
		loadromMode_[selectedComputer_][1] = ROM;
		XRCCTRL(*this, "RomButton2"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("ROM 2");
		XRCCTRL(*this, "RomButton2"+computerInfo[selectedComputer_].gui, wxButton)->SetToolTip("Browse for Elf ROM file");
	}
}

void GuiElf::setGameModeConfig(wxCommandEvent&WXUNUSED(event))
{
    XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("elfinvad.bin");
    XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("");
    XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
    XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKNONE);
    setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKNONE);
    if (elfConfiguration[selectedComputer_].useUart)
    {
        elfConfiguration[selectedComputer_].useUart = false;
        switchUart();
    }
    configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
    XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VTNONE);
    setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VTNONE, true);
    elfConfiguration[selectedComputer_].autoBoot = true;
    XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
    XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
    XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEOPIXIE);
    setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEOPIXIE);
    XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARD_HEX_EF);
    setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARD_HEX_EF);
    conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_ + "Games" + pathSeparator_;
    
    loadromMode_[selectedComputer_][0] = RAM;
    XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 1");
    
    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
    XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
    XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FFFF");
    XRCCTRL(*this, "UpperCase"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(true);
    
    conf[selectedComputer_].clock_.Printf("%1.2f", 1.76);
    clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);
    
    conf[selectedComputer_].printerOn_ = false;
    setConfigItem(computerInfo[selectedComputer_].gui+"/Printer", conf[selectedComputer_].printerOn_);
    setPrinterState(selectedComputer_);
}

void GuiElf::setChipModeConfig(wxCommandEvent&WXUNUSED(event))
{
    XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("chip8.hex");
    XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("Blinky [Hans Christian Egeberg, 1991].ch8");
    XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
    XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKNONE);
    setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKNONE);
    if (elfConfiguration[selectedComputer_].useUart)
    {
        elfConfiguration[selectedComputer_].useUart = false;
        switchUart();
    }
    configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
    XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VTNONE);
    setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VTNONE, true);
    elfConfiguration[selectedComputer_].autoBoot = true;
    XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
    XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
    XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEOPIXIE);
    setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEOPIXIE);
    XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARD_HEX_EF);
    setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARD_HEX_EF);
    conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_;
    conf[selectedComputer_].romDir_[MAINROM2] = dataDir_ + "Chip-8" + pathSeparator_ + "Chip-8 Games" + pathSeparator_;
    
    loadromMode_[selectedComputer_][0] = RAM;
    XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 1");
    
    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
    XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
    XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FFFF");
    XRCCTRL(*this, "UpperCase"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(true);
    
    conf[selectedComputer_].clock_.Printf("%1.2f", 1.76);
    clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);
    
    conf[selectedComputer_].printerOn_ = false;
    setConfigItem(computerInfo[selectedComputer_].gui+"/Printer", conf[selectedComputer_].printerOn_);
    setPrinterState(selectedComputer_);
}

void GuiElf::setGiantBoard(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("giantboard.hex");
	XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
	XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKNONE);
	setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKNONE);
	if (elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].useUart = false;
		switchUart();
	}
	configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
	XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VTNONE);
	setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VTNONE, true);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("F000");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEONONE);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEONONE);
	XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARDNONE);
	setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARDNONE);
	elfConfiguration[selectedComputer_].useTape = true;
	setTapeType(computerInfo[selectedComputer_].gui, selectedComputer_);
	conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_;

	loadromMode_[selectedComputer_][0] = ROM;
	XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("ROM 1");

    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
	XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FFFF");

	conf[selectedComputer_].clock_.Printf("%1.2f", 1.76);
	clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);

	elfConfiguration[selectedComputer_].elfPortConf.tapeEf = 2;
}

void GuiElf::setQuestLoader(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("quest.tape.loader.hex");
	XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
	XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKNONE);
	setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKNONE);
	if (elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].useUart = false;
		switchUart();
	}
	configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
	XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VTNONE);
	setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VTNONE, true);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("F000");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEOS100);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEOS100);
	XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARD_ASCII_HEX_EF);
	setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARD_ASCII_HEX_EF);
	elfConfiguration[selectedComputer_].useTape = true;
	setTapeType(computerInfo[selectedComputer_].gui, selectedComputer_);
	conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_;

	loadromMode_[selectedComputer_][0] = RAM;
	XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 1");

    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
	XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("7FFF");

	conf[selectedComputer_].clock_.Printf("%1.2f", 1.76);
	clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);

	elfConfiguration[selectedComputer_].elfPortConf.hexOutput = 4;
	elfConfiguration[selectedComputer_].elfPortConf.hexInput = 4;
	elfConfiguration[selectedComputer_].elfPortConf.hexEf = 3;

	elfConfiguration[selectedComputer_].elfPortConf.keyboardInput = 5;
	elfConfiguration[selectedComputer_].elfPortConf.keyboardEf = 2;
	elfConfiguration[selectedComputer_].elfPortConf.tapeEf = 3;

	conf[selectedComputer_].printerOn_ = true;
	setConfigItem(computerInfo[selectedComputer_].gui+"/Printer", conf[selectedComputer_].printerOn_);
	elfConfiguration[selectedComputer_].elfPortConf.printerOutput = 3;
	setPrinterState(selectedComputer_);
}

void GuiElf::setSuperBasic1(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("superbasic.1.4.bin");
	XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FFFF");
	setSuperBasic();
}

void GuiElf::setSuperBasic3(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("superbasic.3.0.bin");
	XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("DFFF");
	setSuperBasic();
}

void GuiElf::setSuperBasic5(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("superbasic.5.0.bin");
	XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("DFFF");
	setSuperBasic();
}

void GuiElf::setSuperBasic6(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("superbasic.6.0.bin");
	XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("DFFF");
	setSuperBasic();
}

void GuiElf::setSuperBasic()
{
	XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
	setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKNONE);
	XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKNONE);
	if (elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].useUart = false;
		switchUart();
	}
	configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
	XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VTNONE);
	setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VTNONE, true);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEOS100);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEOS100);
	XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARD_ASCII_HEX_EF);
	setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARD_ASCII_HEX_EF);
	elfConfiguration[selectedComputer_].useTape = true;
	setTapeType(computerInfo[selectedComputer_].gui, selectedComputer_);
	conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_;

	loadromMode_[selectedComputer_][0] = RAM;
	XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 1");

    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
	XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");

	conf[selectedComputer_].clock_.Printf("%1.2f", 1.76);
	clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);

	elfConfiguration[selectedComputer_].elfPortConf.hexOutput = 4;
	elfConfiguration[selectedComputer_].elfPortConf.hexInput = 4;
	elfConfiguration[selectedComputer_].elfPortConf.hexEf = 3;

	elfConfiguration[selectedComputer_].elfPortConf.keyboardInput = 5;
	elfConfiguration[selectedComputer_].elfPortConf.keyboardEf = 2;
	elfConfiguration[selectedComputer_].elfPortConf.tapeEf = 3;

	conf[selectedComputer_].printerOn_ = true;
	setConfigItem(computerInfo[selectedComputer_].gui+"/Printer", conf[selectedComputer_].printerOn_);
	elfConfiguration[selectedComputer_].elfPortConf.printerOutput = 3;
	setPrinterState(selectedComputer_);
}

void GuiElf::setSuperBasicSerial(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("superbasic.1.4.bin");
	XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
	setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKNONE);
	XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKNONE);
	if (elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].useUart = false;
		switchUart();
	}
	configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
	XRCCTRL(*this, "VTBaudRChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(0);
	XRCCTRL(*this, "VTBaudTChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(0);
	elfConfiguration[selectedComputer_].baudR = 0;
	elfConfiguration[selectedComputer_].baudT = 0;
	XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VT100);
	setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VT100, true);
	elfConfiguration[selectedComputer_].autoBoot = false;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEONONE);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEONONE);
	XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARDNONE);
	setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARDNONE);
	elfConfiguration[selectedComputer_].useTape = true;
	setTapeType(computerInfo[selectedComputer_].gui, selectedComputer_);
	conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_;

	loadromMode_[selectedComputer_][0] = RAM;
	XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 1");

    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
	XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FFFF");

	conf[selectedComputer_].clock_.Printf("%1.2f", 1.76);
	clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);

    elfConfiguration[selectedComputer_].elfPortConf.vt100Output = 5;
	elfConfiguration[selectedComputer_].elfPortConf.vt100Ef = 2;
	elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseEf = 1;
	elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseQ = 0;
	elfConfiguration[selectedComputer_].elfPortConf.tapeEf = 3;

	conf[selectedComputer_].printerOn_ = true;
	setConfigItem(computerInfo[selectedComputer_].gui+"/Printer", conf[selectedComputer_].printerOn_);
	elfConfiguration[selectedComputer_].elfPortConf.printerOutput = 3;
	setPrinterState(selectedComputer_);
}

void GuiElf::setRcaBasic(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("rca.basic3.v1.1.hex");
	XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("bios+biosiov93.rom");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
	setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKNONE);
	XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKNONE);
	if (elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].useUart = false;
		switchUart();
	}
	configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
	XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VTNONE);
	setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VTNONE, true);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("2200");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEOS100);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEOS100);
	XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARD_ASCII_HEX_EF);
	setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARD_ASCII_HEX_EF);
	elfConfiguration[selectedComputer_].useTape = false;
	setTapeType(computerInfo[selectedComputer_].gui, selectedComputer_);
	conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_;
	conf[selectedComputer_].romDir_[MAINROM2] = dataDir_ + "Elf" + pathSeparator_;

	loadromMode_[selectedComputer_][0] = RAM;
	XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 1");
	loadromMode_[selectedComputer_][1] = RAM;
	XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 2");

    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
	XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FFFF");

	conf[selectedComputer_].clock_.Printf("%1.2f", 1.76);
	clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);

	elfConfiguration[selectedComputer_].elfPortConf.hexOutput = 4;
	elfConfiguration[selectedComputer_].elfPortConf.hexInput = 4;
	elfConfiguration[selectedComputer_].elfPortConf.hexEf = 3;

	elfConfiguration[selectedComputer_].elfPortConf.keyboardInput = 5;
	elfConfiguration[selectedComputer_].elfPortConf.keyboardEf = 4;

	conf[selectedComputer_].printerOn_ = true;
	setConfigItem(computerInfo[selectedComputer_].gui+"/Printer", conf[selectedComputer_].printerOn_);
	elfConfiguration[selectedComputer_].elfPortConf.printerOutput = 3;
	setPrinterState(selectedComputer_);
}

void GuiElf::setRcaBasicPixie(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("rca.basic3.v1.1.hex");
	XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("bios+biosiov93.rom");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
	setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKNONE);
	XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKNONE);
	if (elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].useUart = false;
		switchUart();
	}
	configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
	XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VTNONE);
	setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VTNONE, true);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("2200");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEOPIXIE);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEOPIXIE);
	XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARD_ASCII_HEX_EF);
	setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARD_ASCII_HEX_EF);
	elfConfiguration[selectedComputer_].useTape = false;
	setTapeType(computerInfo[selectedComputer_].gui, selectedComputer_);
	conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_;
	conf[selectedComputer_].romDir_[MAINROM2] = dataDir_ + "Elf" + pathSeparator_;

	loadromMode_[selectedComputer_][0] = RAM;
	XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 1");
	loadromMode_[selectedComputer_][1] = RAM;
	XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 2");

    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
	XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FFFF");

	conf[selectedComputer_].clock_.Printf("%1.2f", 1.76);
	clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);

	elfConfiguration[selectedComputer_].elfPortConf.hexOutput = 4;
	elfConfiguration[selectedComputer_].elfPortConf.hexInput = 4;
	elfConfiguration[selectedComputer_].elfPortConf.hexEf = 3;

	elfConfiguration[selectedComputer_].elfPortConf.keyboardInput = 7;
	elfConfiguration[selectedComputer_].elfPortConf.keyboardEf = 3;
}

void GuiElf::setRcaBasicSerial(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("rca.basic3.v1.1.hex");
	XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("bios+biosiov93.rom");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
	setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKNONE);
	XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKNONE);
	if (elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].useUart = false;
		switchUart();
	}
	configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
	XRCCTRL(*this, "VTBaudRChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(0);
	XRCCTRL(*this, "VTBaudTChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(0);
	elfConfiguration[selectedComputer_].baudR = 0;
	elfConfiguration[selectedComputer_].baudT = 0;
	XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VT100);
	setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VT100, true);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("2200");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEONONE);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEONONE);
	XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARDNONE);
	setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARDNONE);
	elfConfiguration[selectedComputer_].useTape = false;
	setTapeType(computerInfo[selectedComputer_].gui, selectedComputer_);
	conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_;
	conf[selectedComputer_].romDir_[MAINROM2] = dataDir_ + "Elf" + pathSeparator_;

	loadromMode_[selectedComputer_][0] = RAM;
	XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 1");
	loadromMode_[selectedComputer_][1] = RAM;
	XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 2");

    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
	XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FFFF");

	conf[selectedComputer_].clock_.Printf("%1.2f", 1.76);
	clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);

	elfConfiguration[selectedComputer_].elfPortConf.vt100Output = 5;
	elfConfiguration[selectedComputer_].elfPortConf.vt100Ef = 2;
	elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseEf = 1;
	elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseQ = 0;

	conf[selectedComputer_].printerOn_ = true;
	setConfigItem(computerInfo[selectedComputer_].gui+"/Printer", conf[selectedComputer_].printerOn_);
	elfConfiguration[selectedComputer_].elfPortConf.printerOutput = 3;
	setPrinterState(selectedComputer_);
}

void GuiElf::setRcaBasicElfOsInstall(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("bios+biosiov93.rom");
	XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("rca.basic4.v1.1.install.hex");
	if (elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].useUart = false;
		switchUart();
	}
	configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
	XRCCTRL(*this, "VTBaudRChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(0);
	XRCCTRL(*this, "VTBaudTChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(0);
	elfConfiguration[selectedComputer_].baudR = 0;
	elfConfiguration[selectedComputer_].baudT = 0;
	XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VT100);
	setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VT100, true);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FF00");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEONONE);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEONONE);
	XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARDNONE);
	setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARDNONE);
	conf[selectedComputer_].romDir_[MAINROM2] = dataDir_ + "Elf" + pathSeparator_;

    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
	elfConfiguration[selectedComputer_].elfPortConf.vt100Output = 7;
	elfConfiguration[selectedComputer_].elfPortConf.vt100Ef = 2;
	elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseEf = 1;
	elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseQ = 0;

	conf[selectedComputer_].printerOn_ = true;
	setConfigItem(computerInfo[selectedComputer_].gui+"/Printer", conf[selectedComputer_].printerOn_);
	elfConfiguration[selectedComputer_].elfPortConf.printerOutput = 3;
	setPrinterState(selectedComputer_);

	setElfOsConfigMain();
}

void GuiElf::setTinyBasicPixie(wxCommandEvent&WXUNUSED(event))
{
    XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("giantboard.hex");
    XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("netronic.tinyBasic.hex");
    XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
    XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKNONE);
    setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKNONE);
    if (elfConfiguration[selectedComputer_].useUart)
    {
        elfConfiguration[selectedComputer_].useUart = false;
        switchUart();
    }
    configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
    XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VTNONE);
    setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VTNONE, true);
    elfConfiguration[selectedComputer_].autoBoot = true;
    XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
    XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
    XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEOPIXIE);
    setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEOPIXIE);
    XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARD_ASCII_HEX_EF);
    setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARD_ASCII_HEX_EF);
    elfConfiguration[selectedComputer_].useTape = true;
    setTapeType(computerInfo[selectedComputer_].gui, selectedComputer_);
    conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_;
    conf[selectedComputer_].romDir_[MAINROM2] = dataDir_ + "Elf" + pathSeparator_;
    
    loadromMode_[selectedComputer_][0] = ROM;
    XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("ROM 1");
    loadromMode_[selectedComputer_][1] = RAM;
    XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 2");
    
    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
    XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
    XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FFFF");
    
    conf[selectedComputer_].clock_.Printf("%1.2f", 1.76);
    clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);
    
    elfConfiguration[selectedComputer_].elfPortConf.hexOutput = 4;
    elfConfiguration[selectedComputer_].elfPortConf.hexInput = 4;
    elfConfiguration[selectedComputer_].elfPortConf.hexEf = 3;
    
    elfConfiguration[selectedComputer_].elfPortConf.keyboardInput = 7;
    elfConfiguration[selectedComputer_].elfPortConf.keyboardEf = 3;
    elfConfiguration[selectedComputer_].elfPortConf.tapeEf = 2;
}

void GuiElf::setRomMapperPixie(wxCommandEvent&WXUNUSED(event))
{
    XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("rommapper.bin");
    XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("");
    XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
    XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKNONE);
    setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKNONE);
    if (elfConfiguration[selectedComputer_].useUart)
    {
        elfConfiguration[selectedComputer_].useUart = false;
        switchUart();
    }
    configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
    XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VTNONE);
    setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VTNONE, true);
    elfConfiguration[selectedComputer_].autoBoot = true;
    XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
    XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("8080");
    XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEOPIXIE);
    setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEOPIXIE);
    XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARD_ASCII_HEX_EF);
    setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARD_ASCII_HEX_EF);
    elfConfiguration[selectedComputer_].useTape = true;
    setTapeType(computerInfo[selectedComputer_].gui, selectedComputer_);
    conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_;
    conf[selectedComputer_].romDir_[MAINROM2] = dataDir_ + "Elf" + pathSeparator_;

    loadromMode_[selectedComputer_][1] = RAM;
    XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 2");
    
    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_ROM_MAP);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_ROM_MAP);

    XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
    
    conf[selectedComputer_].clock_.Printf("%1.2f", 1.76);
    clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);
    
    elfConfiguration[selectedComputer_].elfPortConf.hexOutput = 4;
    elfConfiguration[selectedComputer_].elfPortConf.hexInput = 4;
    elfConfiguration[selectedComputer_].elfPortConf.hexEf = 3;
    
    elfConfiguration[selectedComputer_].elfPortConf.keyboardInput = 7;
    elfConfiguration[selectedComputer_].elfPortConf.keyboardEf = 3;
    elfConfiguration[selectedComputer_].elfPortConf.tapeEf = 2;
}

void GuiElf::setRomMapperSerial(wxCommandEvent&WXUNUSED(event))
{
    XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("rommapper.bin");
    XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("");
    XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
    XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKNONE);
    setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKNONE);
    if (elfConfiguration[selectedComputer_].useUart)
    {
        elfConfiguration[selectedComputer_].useUart = false;
        switchUart();
    }
    configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
    XRCCTRL(*this, "VTBaudRChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(0);
    XRCCTRL(*this, "VTBaudTChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(0);
    elfConfiguration[selectedComputer_].baudR = 0;
    elfConfiguration[selectedComputer_].baudT = 0;
    XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VT100);
    setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VT100, true);
    elfConfiguration[selectedComputer_].autoBoot = true;
    XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
    XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("8080");
    XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEOPIXIE);
    setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEOPIXIE);
    XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARD_ASCII_HEX_EF);
    setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARD_ASCII_HEX_EF);
    elfConfiguration[selectedComputer_].useTape = false;
    setTapeType(computerInfo[selectedComputer_].gui, selectedComputer_);
    conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_;
    conf[selectedComputer_].romDir_[MAINROM2] = dataDir_ + "Elf" + pathSeparator_;
    
    loadromMode_[selectedComputer_][1] = RAM;
    XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 2");
    
    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_ROM_MAP);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_ROM_MAP);
    
    XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
    
    conf[selectedComputer_].clock_.Printf("%1.2f", 1.76);
    clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);
    
    elfConfiguration[selectedComputer_].elfPortConf.vt100Output = 6;
    elfConfiguration[selectedComputer_].elfPortConf.vt100Ef = 2;
    elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseEf = 1;
    elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseQ = 0;

    elfConfiguration[selectedComputer_].elfPortConf.hexOutput = 4;
    elfConfiguration[selectedComputer_].elfPortConf.hexInput = 4;
    elfConfiguration[selectedComputer_].elfPortConf.hexEf = 3;
    
    elfConfiguration[selectedComputer_].elfPortConf.keyboardInput = 7;
    elfConfiguration[selectedComputer_].elfPortConf.keyboardEf = 3;

    elfConfiguration[selectedComputer_].elfPortConf.tapeEf = 2;
}

void GuiElf::setTinyBasicSerial(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("giantboard.hex");
	XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("netronic.tinyBasic.hex");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
	XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKNONE);
	setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKNONE);
	if (elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].useUart = false;
		switchUart();
	}
	configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
	XRCCTRL(*this, "VTBaudRChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(6);
	XRCCTRL(*this, "VTBaudTChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(6);
	elfConfiguration[selectedComputer_].baudR = 6;
	elfConfiguration[selectedComputer_].baudT = 6;
	XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VT100);
	setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VT100, true);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEONONE);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEONONE);
	XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARDNONE);
	setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARDNONE);
	elfConfiguration[selectedComputer_].useTape = true;
	setTapeType(computerInfo[selectedComputer_].gui, selectedComputer_);
	conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_;
	conf[selectedComputer_].romDir_[MAINROM2] = dataDir_ + "Elf" + pathSeparator_;

	loadromMode_[selectedComputer_][0] = ROM;
	XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("ROM 1");
	loadromMode_[selectedComputer_][1] = RAM;
	XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 2");

    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
	XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FFFF");

	conf[selectedComputer_].clock_.Printf("%1.2f", 1.76);
	clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);

	elfConfiguration[selectedComputer_].elfPortConf.vt100Output = 7;
	elfConfiguration[selectedComputer_].elfPortConf.vt100Ef = 4;
	elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseEf = 1;
	elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseQ = 0;
	elfConfiguration[selectedComputer_].elfPortConf.tapeEf = 2;
}

void GuiElf::setTinyBasic6847(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("tiny.basic.interpreter.v2.hex");
	XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
	setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKNONE);
	XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKNONE);
	if (elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].useUart = false;
		switchUart();
	}
	configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
	XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VTNONE);
	setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VTNONE, true);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEO6847);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEO6847);
	XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARD_ASCII_HEX_EF);
	setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARD_ASCII_HEX_EF);
	elfConfiguration[selectedComputer_].useTape = false;
	setTapeType(computerInfo[selectedComputer_].gui, selectedComputer_);
	conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_;

	loadromMode_[selectedComputer_][0] = ROM;
	XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("ROM 1");

    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
	XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FFFF");

	conf[selectedComputer_].clock_.Printf("%1.2f", 1.76);
	clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);

	reset6847ConfigItem(2);
	elfConfiguration[selectedComputer_].elfPortConf.mc6847dd6=2;

	elfConfiguration[selectedComputer_].elfPortConf.hexOutput=4;
	elfConfiguration[selectedComputer_].elfPortConf.hexInput=4;
	elfConfiguration[selectedComputer_].elfPortConf.hexEf=3;

	elfConfiguration[selectedComputer_].elfPortConf.keyboardInput=5;
	elfConfiguration[selectedComputer_].elfPortConf.keyboardEf=2;
}

void GuiElf::setSuperGoldMonitor(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("super.gold.monitor.hex");
	XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
	setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKNONE);
	XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKNONE);
	if (elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].useUart = false;
		switchUart();
	}
	configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
	XRCCTRL(*this, "VTBaudRChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(1);
	XRCCTRL(*this, "VTBaudTChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(1);
	elfConfiguration[selectedComputer_].baudR = 1;
	elfConfiguration[selectedComputer_].baudT = 1;
	XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VT100);
	setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VT100, true);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("C0C0");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEONONE);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEONONE);
	XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARDNONE);
	setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARDNONE);
	elfConfiguration[selectedComputer_].useTape = true;
	setTapeType(computerInfo[selectedComputer_].gui, selectedComputer_);
	conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_;

	loadromMode_[selectedComputer_][0] = ROM;
	XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("ROM 1");

    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
	XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("BFFF");

	conf[selectedComputer_].clock_.Printf("%1.2f", 1.76);
	clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);

	elfConfiguration[selectedComputer_].elfPortConf.vt100Output=5;
	elfConfiguration[selectedComputer_].elfPortConf.vt100Ef=4;
	elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseEf=0;
	elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseQ=0;
	elfConfiguration[selectedComputer_].elfPortConf.tapeEf=2;
}

void GuiElf::setMonitorBasic(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("mini.monitor.and.super.basic.hex");
	XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
	setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKNONE);
	XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKNONE);
	if (elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].useUart = false;
		switchUart();
	}
	configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
	XRCCTRL(*this, "VTBaudRChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(1);
	XRCCTRL(*this, "VTBaudTChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(1);
	elfConfiguration[selectedComputer_].baudR = 1;
	elfConfiguration[selectedComputer_].baudT = 1;
	XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VT100);
	setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VT100, true);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("C0C0");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEONONE);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEONONE);
	XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARDNONE);
	setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARDNONE);
	elfConfiguration[selectedComputer_].useTape = true;
	setTapeType(computerInfo[selectedComputer_].gui, selectedComputer_);
	conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_;

	loadromMode_[selectedComputer_][0] = ROM;
	XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("ROM 1");

    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
	XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("BFFF");

	conf[selectedComputer_].clock_.Printf("%1.2f", 1.76);
	clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);

	elfConfiguration[selectedComputer_].elfPortConf.vt100Output=5;
	elfConfiguration[selectedComputer_].elfPortConf.vt100Ef=4;
	elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseEf=0;
	elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseQ=0;
	elfConfiguration[selectedComputer_].elfPortConf.tapeEf=2;

	conf[selectedComputer_].printerOn_ = true;
	setConfigItem(computerInfo[selectedComputer_].gui+"/Printer", conf[selectedComputer_].printerOn_);
	elfConfiguration[selectedComputer_].elfPortConf.printerOutput=3;
	setPrinterState(selectedComputer_);
}

void GuiElf::setFigForth(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("fig.forth.1802.hex");
	XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
	setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKNONE);
	XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKNONE);
	if (!elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].useUart = true;
		switchUart();
	}
	configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
	XRCCTRL(*this, "VTBaudRChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(0);
	XRCCTRL(*this, "VTBaudTChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(0);
	elfConfiguration[selectedComputer_].baudR = 0;
	elfConfiguration[selectedComputer_].baudT = 0;
	XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VT100);
	setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VT100, true);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEONONE);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEONONE);
	XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARDNONE);
	setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARDNONE);
	elfConfiguration[selectedComputer_].useTape = false;
	setTapeType(computerInfo[selectedComputer_].gui, selectedComputer_);
	conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_;

	loadromMode_[selectedComputer_][0] = RAM;
	XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 1");

    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
	XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FFFF");

	conf[selectedComputer_].clock_.Printf("%1.2f", 1.76);
	clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);

	elfConfiguration[selectedComputer_].elfPortConf.uartOut=2;
	elfConfiguration[selectedComputer_].elfPortConf.uartIn=2;
	elfConfiguration[selectedComputer_].elfPortConf.uartControl=3;
	elfConfiguration[selectedComputer_].elfPortConf.uartStatus=3;
}

void GuiElf::setMusic(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("Music Machine.hex");
	XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("O Christmas Tree.hex");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("");
	XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKNONE);
	setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKNONE);
	if (elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].useUart = false;
		switchUart();
	}
	configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
	XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VTNONE);
	setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VTNONE, true);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEONONE);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEONONE);
	XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARDNONE);
	setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARDNONE);
	conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_ + "Music" + pathSeparator_;
	conf[selectedComputer_].romDir_[MAINROM2] = dataDir_ + "Elf" + pathSeparator_ + "Music" + pathSeparator_;

	loadromMode_[selectedComputer_][0] = RAM;
	XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 1");
	loadromMode_[selectedComputer_][1] = RAM;
	XRCCTRL(*this, "RomButton2"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 2");

    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
	XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FFFF");

	conf[selectedComputer_].clock_ = "1";
	clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);
}

void GuiElf::setElfOsInstallConfig(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("bios.1.0.4.rom");
	XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("elfos.0.2.8.hex");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("vt100.ide");
	if (elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].useUart = false;
		switchUart();
	}
	configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
	XRCCTRL(*this, "VTBaudRChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(0);
	XRCCTRL(*this, "VTBaudTChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(0);
	elfConfiguration[selectedComputer_].baudR = 0;
	elfConfiguration[selectedComputer_].baudT = 0;
	XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VT100);
	setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VT100, true);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("9000");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEONONE);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEONONE);
	XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARDNONE);
	setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARDNONE);
	conf[selectedComputer_].romDir_[MAINROM2] = dataDir_ + "Elf" + pathSeparator_;

	elfConfiguration[selectedComputer_].elfPortConf.vt100Output=7;
	elfConfiguration[selectedComputer_].elfPortConf.vt100Ef=2;
	elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseEf=1;
	elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseQ=0;

	loadromMode_[selectedComputer_][1] = RAM;
	XRCCTRL(*this, "RomButton2"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("RAM 2");

    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
	setElfOsConfigMain();
}

void GuiElf::setElfOsConfig(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("bios.1.0.4.rom");
	XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("vt100.ide");
	if (elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].useUart = false;
		switchUart();
	}
	configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
	XRCCTRL(*this, "VTBaudRChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(0);
	XRCCTRL(*this, "VTBaudTChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(0);
	elfConfiguration[selectedComputer_].baudR = 0;
	elfConfiguration[selectedComputer_].baudT = 0;
	XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VT100);
	setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VT100, true);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FF00");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEONONE);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEONONE);
	XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARDNONE);
	setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARDNONE);

	elfConfiguration[selectedComputer_].elfPortConf.vt100Output=7;
	elfConfiguration[selectedComputer_].elfPortConf.vt100Ef=2;
	elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseEf=1;
	elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseQ=0;

	setElfOsConfigMain();
}

void GuiElf::setElfOsIoInstallConfigPixie(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("biosio.elfos.0.2.5.rom");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("pixie.ide");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEOPIXIE);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEOPIXIE);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("DFA0");

	elfConfiguration[selectedComputer_].elfPortConf.pixieInput=1;
	elfConfiguration[selectedComputer_].elfPortConf.pixieOutput=1;
	elfConfiguration[selectedComputer_].elfPortConf.pixieEf=1;

	setElfOsIoConfigMain();
}

void GuiElf::setElfOsIoInstallConfig6845(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("biosio.elfos.0.2.5.rom");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("mc6845.ide");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEO6845);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEO6845);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("DFA3");

	setElfOsIoConfigMain();

	elfConfiguration[selectedComputer_].elfPortConf.hexOutput=4;
	elfConfiguration[selectedComputer_].elfPortConf.hexInput=4;
	elfConfiguration[selectedComputer_].elfPortConf.hexEf=3;

	elfConfiguration[selectedComputer_].elfPortConf.keyboardInput=5;
	elfConfiguration[selectedComputer_].elfPortConf.keyboardEf=4;
}

void GuiElf::setElfOsIoInstallConfig6847(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("biosio.elfos.0.2.5_32.column.rom");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("mc6847.ide");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEO6847);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEO6847);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("DFA3");

	setElfOsIoConfigMain();

	reset6847ConfigItem(2);
	elfConfiguration[selectedComputer_].elfPortConf.mc6847dd6=2;

	elfConfiguration[selectedComputer_].elfPortConf.hexOutput=4;
	elfConfiguration[selectedComputer_].elfPortConf.hexInput=4;
	elfConfiguration[selectedComputer_].elfPortConf.hexEf=3;

	elfConfiguration[selectedComputer_].elfPortConf.keyboardInput=5;
	elfConfiguration[selectedComputer_].elfPortConf.keyboardEf=4;
}

void GuiElf::setElfOsIoInstallConfigTms(wxCommandEvent&WXUNUSED(event))
{
    XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("biosio.elfos.0.2.5.rom");
    XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("tms.ide");
    XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEOTMS);
    setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEOTMS);
    elfConfiguration[selectedComputer_].autoBoot = true;
    XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
    XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("DFA6");
    
    elfConfiguration[selectedComputer_].elfPortConf.tmsModeHighOutput=5;
    elfConfiguration[selectedComputer_].elfPortConf.tmsModeLowOutput=6;
    
    setElfOsIoConfigMain();
}

void GuiElf::setElfOsIoConfig(wxCommandEvent&WXUNUSED(event))
{
    XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("bios+biosiov93.rom");
    XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("");
    XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("serial.ide");
    if (elfConfiguration[selectedComputer_].useUart)
    {
        elfConfiguration[selectedComputer_].useUart = false;
        switchUart();
    }
    configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
    XRCCTRL(*this, "VTBaudRChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(0);
    XRCCTRL(*this, "VTBaudTChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(0);
    elfConfiguration[selectedComputer_].baudR = 0;
    elfConfiguration[selectedComputer_].baudT = 0;
    XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VT100);
    setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VT100, true);
    elfConfiguration[selectedComputer_].autoBoot = true;
    XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
    XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FF00");
    XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEONONE);
    setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEONONE);
    XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARDNONE);
    setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARDNONE);
    
    elfConfiguration[selectedComputer_].elfPortConf.vt100Output=7;
    elfConfiguration[selectedComputer_].elfPortConf.vt100Ef=2;
    elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseEf=1;
    elfConfiguration[selectedComputer_].elfPortConf.vt100ReverseQ=0;
	elfConfiguration[selectedComputer_].elfPortConf.ef3default=0;

    setElfOsConfigMain();
}

void GuiElf::setElfOsIoConfigPixie(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("bios+biosiov93.rom");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("pixie.ide");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEOPIXIE);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEOPIXIE);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FF00");

	elfConfiguration[selectedComputer_].elfPortConf.pixieInput=1;
	elfConfiguration[selectedComputer_].elfPortConf.pixieOutput=1;
	elfConfiguration[selectedComputer_].elfPortConf.pixieEf=1;

	setElfOsIoConfigMain();
}

void GuiElf::setElfOsIoConfig6845(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("bios+biosiov93.rom");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("mc6845.ide");

	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEO6845);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEO6845);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FF00");

	setElfOsIoConfigMain();

	elfConfiguration[selectedComputer_].elfPortConf.hexOutput=4;
	elfConfiguration[selectedComputer_].elfPortConf.hexInput=4;
	elfConfiguration[selectedComputer_].elfPortConf.hexEf=3;

	elfConfiguration[selectedComputer_].elfPortConf.keyboardInput=5;
	elfConfiguration[selectedComputer_].elfPortConf.keyboardEf=4;
}

void GuiElf::setElfOsIoConfig6847(wxCommandEvent&WXUNUSED(event))
{
	XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("bios+biosiov93_32.column.rom");
	XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("mc6847.ide");
	XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEO6847);
	setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEO6847);
	elfConfiguration[selectedComputer_].autoBoot = true;
	XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
	XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FF00");

	setElfOsIoConfigMain();

	reset6847ConfigItem(2);
	elfConfiguration[selectedComputer_].elfPortConf.mc6847dd6=2;

	elfConfiguration[selectedComputer_].elfPortConf.hexOutput=4;
	elfConfiguration[selectedComputer_].elfPortConf.hexInput=4;
	elfConfiguration[selectedComputer_].elfPortConf.hexEf=3;

	elfConfiguration[selectedComputer_].elfPortConf.keyboardInput=5;
	elfConfiguration[selectedComputer_].elfPortConf.keyboardEf=4;
}

void GuiElf::setElfOsIoConfigTms(wxCommandEvent&WXUNUSED(event))
{
    XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("bios+biosiov93.rom");
    XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("tms.ide");
    XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEOTMS);
    setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEOTMS);
    elfConfiguration[selectedComputer_].autoBoot = true;
    XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
    XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FF00");
    
    elfConfiguration[selectedComputer_].elfPortConf.tmsModeHighOutput=5;
    elfConfiguration[selectedComputer_].elfPortConf.tmsModeLowOutput=6;
    
    setElfOsIoConfigMain();
}

void GuiElf::setElfOsIoConfigTmsGra(wxCommandEvent&WXUNUSED(event))
{
    XRCCTRL(*this, "MainRom"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("bios+biosiov93.rom");
    XRCCTRL(*this, "IdeFile"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("gra.ide");
    XRCCTRL(*this, "VideoType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VIDEOTMS);
    setVideoType(computerInfo[selectedComputer_].gui, selectedComputer_, VIDEOTMS);
    elfConfiguration[selectedComputer_].autoBoot = true;
    XRCCTRL(*this, "AutoBoot"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(elfConfiguration[selectedComputer_].autoBoot);
    XRCCTRL(*this, "BootAddress"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FF00");
    
    elfConfiguration[selectedComputer_].elfPortConf.tmsModeHighOutput=5;
    elfConfiguration[selectedComputer_].elfPortConf.tmsModeLowOutput=6;
    
    setElfOsIoConfigMain();
}

void GuiElf::setElfOsConfigMain()
{
	XRCCTRL(*this, "DiskType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(DISKIDE);
	setDiskType(computerInfo[selectedComputer_].gui, selectedComputer_, DISKIDE);
	XRCCTRL(*this, "StartRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("0");
	XRCCTRL(*this, "EndRam"+computerInfo[selectedComputer_].gui, wxTextCtrl)->SetValue("FFFF");
	XRCCTRL(*this, "UpperCase"+computerInfo[selectedComputer_].gui, wxCheckBox)->SetValue(true);
	conf[selectedComputer_].romDir_[MAINROM1] = dataDir_ + "Elf" + pathSeparator_;
	if (selectedComputer_ != ELF)
	{
		elfConfiguration[selectedComputer_].useTape = false;
		setTapeType(computerInfo[selectedComputer_].gui, selectedComputer_);
	}
	elfConfiguration[selectedComputer_].elfPortConf.ideTracks=512;
	elfConfiguration[selectedComputer_].elfPortConf.ideHeads=4;
	elfConfiguration[selectedComputer_].elfPortConf.ideSectors=26;
	elfConfiguration[selectedComputer_].elfPortConf.ideSelectOutput=2;
	elfConfiguration[selectedComputer_].elfPortConf.ideWriteOutput=3;
	elfConfiguration[selectedComputer_].elfPortConf.ideInput=3;

	loadromMode_[selectedComputer_][0] = ROM;
	XRCCTRL(*this, "RomButton"+computerInfo[selectedComputer_].gui, wxButton)->SetLabel("ROM 1");

    XRCCTRL(*this, "Memory"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(MEM_64K);
    setMemory(computerInfo[selectedComputer_].gui, selectedComputer_, MEM_64K);
    
	conf[selectedComputer_].clock_.Printf("%1.2f", 1.76);
	clockTextCtrl[selectedComputer_]->ChangeValue(conf[selectedComputer_].clock_);

	conf[selectedComputer_].printerOn_ = false;
	setConfigItem(computerInfo[selectedComputer_].gui+"/Printer", conf[selectedComputer_].printerOn_);
	setPrinterState(selectedComputer_);
}

void GuiElf::setElfOsIoConfigMain()
{
	XRCCTRL(*this, "MainRom2"+computerInfo[selectedComputer_].gui, wxComboBox)->SetValue("");
	XRCCTRL(*this, "Keyboard"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(KEYBOARD_ASCII_HEX_EF);
	setElfKeyboard(computerInfo[selectedComputer_].gui, selectedComputer_, KEYBOARD_ASCII_HEX_EF);
	if (elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].useUart = false;
		switchUart();
	}
	configPointer->Write(computerInfo[selectedComputer_].gui+"/Uart", elfConfiguration[selectedComputer_].useUart);
	XRCCTRL(*this, "VTType"+computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(VTNONE);
	setVtType(computerInfo[selectedComputer_].gui, selectedComputer_, VTNONE, true);

	elfConfiguration[selectedComputer_].elfPortConf.hexOutput=4;
	elfConfiguration[selectedComputer_].elfPortConf.hexInput=4;
	elfConfiguration[selectedComputer_].elfPortConf.hexEf=3;

	elfConfiguration[selectedComputer_].elfPortConf.keyboardInput=7;
	elfConfiguration[selectedComputer_].elfPortConf.keyboardEf=3;

	setElfOsConfigMain();
}

int GuiElf::getLoadromMode(int elfType, int num)
{
	return loadromMode_[elfType][num];
}

long GuiElf::getStartRam(wxString elfTypeStr, int elfType)
{
	startRam_[elfType] &= 0xff00;
	if (mode_.gui)
	{
		wxString address;
		address.Printf("%04X", (unsigned int)startRam_[elfType]);
		XRCCTRL(*this,"StartRam"+elfTypeStr, wxTextCtrl)->ChangeValue(address);
	}
	return startRam_[elfType];
}

long GuiElf::getEndRam(wxString elfTypeStr, int elfType)
{
	endRam_[elfType] &= 0xff00;
	endRam_[elfType] |= 0xff;
	if (mode_.gui)
	{
		wxString address;
		address.Printf("%04X", (unsigned int)endRam_[elfType]);
		XRCCTRL(*this,"EndRam"+elfTypeStr, wxTextCtrl)->ChangeValue(address);
	}
	return endRam_[elfType];
}

void GuiElf::setElfKeyboard(wxString elfTypeStr, int elfType, int Selection)
{
	elfConfiguration[elfType].keyboardType = Selection;

	switch(Selection)
	{
		case KEYBOARDNONE:
			elfConfiguration[elfType].useHexKeyboard = false;
			elfConfiguration[elfType].useHexKeyboardEf3 = false;
			elfConfiguration[elfType].useKeyboard = false;
			elfConfiguration[elfType].UsePS2 = false;
			elfConfiguration[elfType].usePs2gpio = false;
			if (mode_.gui)
			{
				XRCCTRL(*this,"KeyFileButton"+elfTypeStr, wxButton)->Enable(false);
				XRCCTRL(*this,"KeyFile"+elfTypeStr, wxTextCtrl)->Enable(false);
				XRCCTRL(*this,"EjectKeyFile"+elfTypeStr, wxBitmapButton)->Enable(false);
			}
		break;
		case KEYBOARD_HEX_EF:
			elfConfiguration[elfType].useHexKeyboard = true;
			elfConfiguration[elfType].useHexKeyboardEf3 = true;
			elfConfiguration[elfType].useKeyboard = false;
			elfConfiguration[elfType].UsePS2 = false;
			elfConfiguration[elfType].usePs2gpio = false;
			if (mode_.gui)
			{
				XRCCTRL(*this,"KeyFileButton"+elfTypeStr, wxButton)->Enable(false);
				XRCCTRL(*this,"KeyFile"+elfTypeStr, wxTextCtrl)->Enable(false);
				XRCCTRL(*this,"EjectKeyFile"+elfTypeStr, wxBitmapButton)->Enable(false);
			}
		break;
		case KEYBOARD_HEX:
			elfConfiguration[elfType].useHexKeyboard = true;
			elfConfiguration[elfType].useHexKeyboardEf3 = false;
			elfConfiguration[elfType].useKeyboard = false;
			elfConfiguration[elfType].UsePS2 = false;
			elfConfiguration[elfType].usePs2gpio = false;
			if (mode_.gui)
			{
				XRCCTRL(*this,"KeyFileButton"+elfTypeStr, wxButton)->Enable(false);
				XRCCTRL(*this,"KeyFile"+elfTypeStr, wxTextCtrl)->Enable(false);
				XRCCTRL(*this,"EjectKeyFile"+elfTypeStr, wxBitmapButton)->Enable(false);
			}
		break;
		case KEYBOARD_ASCII:
			elfConfiguration[elfType].useHexKeyboard = false;
			elfConfiguration[elfType].useHexKeyboardEf3 = false;
			elfConfiguration[elfType].useKeyboard = true;
			elfConfiguration[elfType].UsePS2 = false;
			elfConfiguration[elfType].usePs2gpio = false;
			if (mode_.gui)
			{
				XRCCTRL(*this, "KeyFileButton" + elfTypeStr, wxButton)->Enable(true);
				XRCCTRL(*this, "KeyFile" + elfTypeStr, wxTextCtrl)->Enable(true);
				XRCCTRL(*this, "EjectKeyFile" + elfTypeStr, wxBitmapButton)->Enable(true);
			}
			break;
		case KEYBOARD_ASCII_HEX_EF:
			elfConfiguration[elfType].useHexKeyboard = true;
			elfConfiguration[elfType].useHexKeyboardEf3 = true;
			elfConfiguration[elfType].useKeyboard = true;
			elfConfiguration[elfType].UsePS2 = false;
			elfConfiguration[elfType].usePs2gpio = false;
			if (mode_.gui)
			{
				XRCCTRL(*this,"KeyFileButton"+elfTypeStr, wxButton)->Enable(true);
				XRCCTRL(*this,"KeyFile"+elfTypeStr, wxTextCtrl)->Enable(true);
				XRCCTRL(*this,"EjectKeyFile"+elfTypeStr, wxBitmapButton)->Enable(true);
			}
		break;
		case KEYBOARD_PS2:
			elfConfiguration[elfType].useHexKeyboard = false;
			elfConfiguration[elfType].useHexKeyboardEf3 = false;
			elfConfiguration[elfType].useKeyboard = false;
			elfConfiguration[elfType].UsePS2 = true;
			elfConfiguration[elfType].ps2Interrupt = false;
			elfConfiguration[elfType].usePs2gpio = false;
			if (mode_.gui)
			{
				XRCCTRL(*this,"KeyFileButton"+elfTypeStr, wxButton)->Enable(false);
				XRCCTRL(*this,"KeyFile"+elfTypeStr, wxTextCtrl)->Enable(false);
				XRCCTRL(*this,"EjectKeyFile"+elfTypeStr, wxBitmapButton)->Enable(false);
			}
		break;
		case KEYBOARD_PS2_INT:
			elfConfiguration[elfType].useHexKeyboard = false;
			elfConfiguration[elfType].useHexKeyboardEf3 = false;
			elfConfiguration[elfType].useKeyboard = false;
			elfConfiguration[elfType].UsePS2 = true;
			elfConfiguration[elfType].ps2Interrupt = true;
			elfConfiguration[elfType].usePs2gpio = false;
			if (mode_.gui)
			{
				XRCCTRL(*this,"KeyFileButton"+elfTypeStr, wxButton)->Enable(false);
				XRCCTRL(*this,"KeyFile"+elfTypeStr, wxTextCtrl)->Enable(false);
				XRCCTRL(*this,"EjectKeyFile"+elfTypeStr, wxBitmapButton)->Enable(false);
			}
		break;
	}
}

void GuiElf::setVideoType(wxString elfTypeStr, int elfType, int Selection)
{
	conf[elfType].videoMode_ = Selection;

	switch(Selection)
	{
		case VIDEONONE:
			elfConfiguration[elfType].usePixie = false;
			elfConfiguration[elfType].use6845 = false;
			elfConfiguration[elfType].useS100 = false;
			elfConfiguration[elfType].use6847 = false;
			elfConfiguration[elfType].useTMS9918 = false;
			elfConfiguration[elfType].use8275 = false;
			if (mode_.gui)
			{
				XRCCTRL(*this, "ZoomSpin"+elfTypeStr, wxSpinButton)->Enable(false);
				XRCCTRL(*this, "ZoomValue"+elfTypeStr, wxTextCtrl)->Enable(false);
				XRCCTRL(*this, "ZoomText"+elfTypeStr, wxStaticText)->Enable(false);
				XRCCTRL(*this, "Interlace"+elfTypeStr, wxCheckBox)->Enable(false);
				XRCCTRL(*this,"CharRomButton"+elfTypeStr, wxButton)->Enable(false);
				XRCCTRL(*this,"CharRom"+elfTypeStr, wxComboBox)->Enable(false);
			}
		break;

		case VIDEOPIXIE:
			elfConfiguration[elfType].usePixie = true;
			elfConfiguration[elfType].use6845 = false;
			elfConfiguration[elfType].useS100 = false;
			elfConfiguration[elfType].use6847 = false;
			elfConfiguration[elfType].useTMS9918 = false;
			elfConfiguration[elfType].use8275 = false;
			if (mode_.gui)
			{
				XRCCTRL(*this, "ZoomSpin"+elfTypeStr, wxSpinButton)->Enable(true);
				XRCCTRL(*this, "ZoomValue"+elfTypeStr, wxTextCtrl)->Enable(true);
				XRCCTRL(*this, "ZoomText"+elfTypeStr, wxStaticText)->Enable(true);
                XRCCTRL(*this, "Interlace"+elfTypeStr, wxCheckBox)->Enable(false);
				XRCCTRL(*this,"CharRomButton"+elfTypeStr, wxButton)->Enable(false);
				XRCCTRL(*this,"CharRom"+elfTypeStr, wxComboBox)->Enable(false);
			}
		break;

		case VIDEOS100:
			elfConfiguration[elfType].usePixie = false;
			elfConfiguration[elfType].use6845 = false;
			elfConfiguration[elfType].useS100 = true;
			elfConfiguration[elfType].use6847 = false;
			elfConfiguration[elfType].charLine = 64;
			elfConfiguration[elfType].useTMS9918 = false;
			elfConfiguration[elfType].use8275 = false;
			if (mode_.gui)
			{
				XRCCTRL(*this, "ZoomSpin"+elfTypeStr, wxSpinButton)->Enable(true);
				XRCCTRL(*this, "ZoomValue"+elfTypeStr, wxTextCtrl)->Enable(true);
				XRCCTRL(*this, "ZoomText"+elfTypeStr, wxStaticText)->Enable(true);
                XRCCTRL(*this, "Interlace"+elfTypeStr, wxCheckBox)->Enable(true);
				XRCCTRL(*this, "CharRomButton"+elfTypeStr, wxButton)->Enable(true);
				XRCCTRL(*this, "CharRom"+elfTypeStr, wxComboBox)->Enable(true);
				XRCCTRL(*this, "CharRom"+elfTypeStr, wxComboBox)->SetValue("super.video.bin");
			}
		break;

		case VIDEO6845:
			elfConfiguration[elfType].usePixie = false;
			elfConfiguration[elfType].use6845 = true;
			elfConfiguration[elfType].useS100 = false;
			elfConfiguration[elfType].use6847 = false;
			elfConfiguration[elfType].charLine = 64;
			elfConfiguration[elfType].useTMS9918 = false;
			elfConfiguration[elfType].use8275 = false;
			if (mode_.gui)
			{
				XRCCTRL(*this, "ZoomSpin"+elfTypeStr, wxSpinButton)->Enable(true);
				XRCCTRL(*this, "ZoomValue"+elfTypeStr, wxTextCtrl)->Enable(true);
				XRCCTRL(*this, "ZoomText"+elfTypeStr, wxStaticText)->Enable(true);
                XRCCTRL(*this, "Interlace"+elfTypeStr, wxCheckBox)->Enable(true);
				XRCCTRL(*this, "CharRomButton"+elfTypeStr, wxButton)->Enable(true);
				XRCCTRL(*this, "CharRom"+elfTypeStr, wxComboBox)->Enable(true);
				XRCCTRL(*this, "CharRom"+elfTypeStr, wxComboBox)->SetValue("mc6845.comx.bin");
			}
		break;

		case VIDEO6847:
			elfConfiguration[elfType].usePixie = false;
			elfConfiguration[elfType].use6845 = false;
			elfConfiguration[elfType].useS100 = false;
			elfConfiguration[elfType].use6847 = true;
			elfConfiguration[elfType].charLine = 32;
			elfConfiguration[elfType].screenHeight6847 = 192;
			elfConfiguration[elfType].useTMS9918 = false;
			elfConfiguration[elfType].use8275 = false;
			if (mode_.gui)
			{
				XRCCTRL(*this, "ZoomSpin"+elfTypeStr, wxSpinButton)->Enable(true);
				XRCCTRL(*this, "ZoomValue"+elfTypeStr, wxTextCtrl)->Enable(true);
				XRCCTRL(*this, "ZoomText"+elfTypeStr, wxStaticText)->Enable(true);
                XRCCTRL(*this, "Interlace"+elfTypeStr, wxCheckBox)->Enable(false);
				XRCCTRL(*this, "CharRomButton"+elfTypeStr, wxButton)->Enable(true);
				XRCCTRL(*this, "CharRom"+elfTypeStr, wxComboBox)->Enable(true);
				XRCCTRL(*this, "CharRom"+elfTypeStr, wxComboBox)->SetValue("mc6847.ntsc.round.bin");
			}
		break;

		case VIDEOTMS:
			elfConfiguration[elfType].usePixie = false;
			elfConfiguration[elfType].use6845 = false;
			elfConfiguration[elfType].useS100 = false;
			elfConfiguration[elfType].use6847 = false;
			elfConfiguration[elfType].useTMS9918 = true;
			elfConfiguration[elfType].use8275 = false;
			if (mode_.gui)
			{
				XRCCTRL(*this, "ZoomSpin"+elfTypeStr, wxSpinButton)->Enable(true);
				XRCCTRL(*this, "ZoomValue"+elfTypeStr, wxTextCtrl)->Enable(true);
				XRCCTRL(*this, "ZoomText"+elfTypeStr, wxStaticText)->Enable(true);
				XRCCTRL(*this, "Interlace"+elfTypeStr, wxCheckBox)->Enable(false);
				XRCCTRL(*this,"CharRomButton"+elfTypeStr, wxButton)->Enable(false);
				XRCCTRL(*this,"CharRom"+elfTypeStr, wxComboBox)->Enable(false);
			}
		break;

		case VIDEOI8275:
			elfConfiguration[elfType].usePixie = false;
			elfConfiguration[elfType].use6845 = false;
			elfConfiguration[elfType].useS100 = false;
			elfConfiguration[elfType].use6847 = false;
			elfConfiguration[elfType].useTMS9918 = false;
			elfConfiguration[elfType].use8275 = true;
			if (mode_.gui)
			{
				XRCCTRL(*this, "ZoomSpin"+elfTypeStr, wxSpinButton)->Enable(true);
				XRCCTRL(*this, "ZoomValue"+elfTypeStr, wxTextCtrl)->Enable(true);
				XRCCTRL(*this, "ZoomText"+elfTypeStr, wxStaticText)->Enable(true);
				XRCCTRL(*this, "Interlace"+elfTypeStr, wxCheckBox)->Enable(true);
				XRCCTRL(*this, "CharRomButton"+elfTypeStr, wxButton)->Enable(true);
				XRCCTRL(*this, "CharRom"+elfTypeStr, wxComboBox)->Enable(true);
				XRCCTRL(*this, "CharRom"+elfTypeStr, wxComboBox)->SetValue("intel8275.bin");
			}
		break;
	}
}

void GuiElf::setDiskType(wxString elfTypeStr, int elfType, int Selection)
{
	elfConfiguration[elfType].diskType = Selection;

	switch(Selection)
	{
		case DISKNONE:
			elfConfiguration[elfType].ideEnabled = false;
			elfConfiguration[elfType].fdcEnabled = false;
			if (mode_.gui)
			{
				XRCCTRL(*this, "IDE_Button"+elfTypeStr, wxButton)->Enable(false);
				XRCCTRL(*this, "IdeFile"+elfTypeStr, wxTextCtrl)->Enable(false);
				XRCCTRL(*this, "Eject_IDE"+elfTypeStr, wxButton)->Enable(false);
			}
		break;

		case DISKFDC:
			elfConfiguration[elfType].ideEnabled = false;
			elfConfiguration[elfType].fdcEnabled = true;
			if (mode_.gui)
			{
				XRCCTRL(*this, "IDE_Button"+elfTypeStr, wxButton)->Enable(false);
				XRCCTRL(*this, "IdeFile"+elfTypeStr, wxTextCtrl)->Enable(false);
				XRCCTRL(*this, "Eject_IDE"+elfTypeStr, wxButton)->Enable(false);
			}
		break;

		case DISKIDE:
			elfConfiguration[elfType].ideEnabled = true;
			elfConfiguration[elfType].fdcEnabled = false;
			if (mode_.gui)
			{
				XRCCTRL(*this, "IDE_Button"+elfTypeStr, wxButton)->Enable(true);
				XRCCTRL(*this, "IdeFile"+elfTypeStr, wxTextCtrl)->Enable(true);
				XRCCTRL(*this, "Eject_IDE"+elfTypeStr, wxButton)->Enable(true);
			}
		break;
    }
}

wxPoint GuiElf::getLedModulePos()
{
	return wxPoint(ledModuleX_, ledModuleY_);
}

void GuiElf::setLedModulePos(wxPoint position)
{
	if (!mode_.window_position_fixed)
	{
		ledModuleX_ = -1;
		ledModuleY_ = -1;
	}
	else
	{
		if (position.x > 0)
			ledModuleX_ = position.x;
		if (position.y > 0)
			ledModuleY_ = position.y;
	}
}

void GuiElf::reset6847ConfigItem(int num)
{
	if (getConfigItem(computerInfo[selectedComputer_].gui+"/MC6847-DD7", 1) == num)
		elfConfiguration[selectedComputer_].elfPortConf.mc6847dd7 = 0;
	if (getConfigItem(computerInfo[selectedComputer_].gui+"/MC6847-DD6", 0) == num)
		elfConfiguration[selectedComputer_].elfPortConf.mc6847dd6 = 0;
	if (getConfigItem(computerInfo[selectedComputer_].gui+"/MC6847-B7", 0) == num)
		elfConfiguration[selectedComputer_].elfPortConf.mc6847b7 = 0;
	if (getConfigItem(computerInfo[selectedComputer_].gui+"/MC6847-B6", 0) == num)
		elfConfiguration[selectedComputer_].elfPortConf.mc6847b6 = 0;
	if (getConfigItem(computerInfo[selectedComputer_].gui+"/MC6847-B5", 0) == num)
		elfConfiguration[selectedComputer_].elfPortConf.mc6847b5 = 0;
	if (getConfigItem(computerInfo[selectedComputer_].gui+"/MC6847-B4", 0) == num)
		elfConfiguration[selectedComputer_].elfPortConf.mc6847b4 = 0;
	if (getConfigItem(computerInfo[selectedComputer_].gui+"/MC6847-B3", 3) == num)
		elfConfiguration[selectedComputer_].elfPortConf.mc6847b3 = 0;
	if (getConfigItem(computerInfo[selectedComputer_].gui+"/MC6847-B2", 4) == num)
		elfConfiguration[selectedComputer_].elfPortConf.mc6847b2 = 0;
	if (getConfigItem(computerInfo[selectedComputer_].gui+"/MC6847-B1", 6) == num)
		elfConfiguration[selectedComputer_].elfPortConf.mc6847b1 = 0;
	if (getConfigItem(computerInfo[selectedComputer_].gui+"/MC6847-B0", 5) == num)
		elfConfiguration[selectedComputer_].elfPortConf.mc6847b0 = 0;
}

void GuiElf::onTape(wxCommandEvent&WXUNUSED(event))
{
	elfConfiguration[selectedComputer_].useTape = !elfConfiguration[selectedComputer_].useTape;
	setTapeType(computerInfo[selectedComputer_].gui, selectedComputer_);
}

void GuiElf::setTapeType(wxString elfTypeStr, int elfType)
{
	if (elfConfiguration[elfType].useTape)
	{
		XRCCTRL(*this, "Tape"+elfTypeStr, wxBitmapButton)->SetBitmapLabel(tapeOnBitmap);
		XRCCTRL(*this, "Tape"+elfTypeStr, wxBitmapButton)->SetBitmapDisabled (tapeOnBitmap);
		XRCCTRL(*this, "Tape"+elfTypeStr, wxBitmapButton)->SetToolTip("Disable cassette support");
	}
	else
	{
		XRCCTRL(*this, "Tape"+elfTypeStr, wxBitmapButton)->SetBitmapLabel(tapeOffBitmap);
		XRCCTRL(*this, "Tape"+elfTypeStr, wxBitmapButton)->SetBitmapDisabled (tapeOffBitmap);
		XRCCTRL(*this, "Tape"+elfTypeStr, wxBitmapButton)->SetToolTip("Enable cassette support");
	}
	enableTapeGui(elfConfiguration[elfType].useTape, elfType);
}

void GuiElf::onQsound(wxCommandEvent&event)
{
	elfConfiguration[selectedComputer_].qSound_ = event.GetSelection();
	if ((selectedComputer_ == runningComputer_) && computerRunning_)
		p_Computer->setQsound(elfConfiguration[selectedComputer_].qSound_);

	XRCCTRL(*this, "BeepFrequencyText"+computerInfo[selectedComputer_].gui, wxStaticText)->Enable(elfConfiguration[selectedComputer_].qSound_ == QSOUNDEXT);
    XRCCTRL(*this, "BeepFrequencyTextHz"+computerInfo[selectedComputer_].gui, wxStaticText)->Enable(elfConfiguration[selectedComputer_].qSound_ == QSOUNDEXT);
	XRCCTRL(*this, "BeepFrequency"+computerInfo[selectedComputer_].gui, wxTextCtrl)->Enable(elfConfiguration[selectedComputer_].qSound_ == QSOUNDEXT);
}

void GuiElf::onUart(wxCommandEvent&WXUNUSED(event))
{
//	elfConfiguration[selectedComputer_].useUart = p_Main->getConfigBool(computerInfo[selectedComputer_].gui+"/Uart", false);
	switchUart();
}

void GuiElf::switchUart()
{
	setBaudChoice(selectedComputer_);

	if (elfConfiguration[selectedComputer_].useUart)
	{
		elfConfiguration[selectedComputer_].baudR += 3;
		elfConfiguration[selectedComputer_].baudT += 3;
		XRCCTRL(*this, "VTBaudRChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(elfConfiguration[selectedComputer_].baudR);
		XRCCTRL(*this, "VTBaudTChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(elfConfiguration[selectedComputer_].baudT);
	}
	else
	{
		if (elfConfiguration[selectedComputer_].baudT < 3)  elfConfiguration[selectedComputer_].baudT = 3;
		if (elfConfiguration[selectedComputer_].baudT > 9)  elfConfiguration[selectedComputer_].baudT = 9;
		elfConfiguration[selectedComputer_].baudT -= 3;
		elfConfiguration[selectedComputer_].baudR = elfConfiguration[selectedComputer_].baudT;
		XRCCTRL(*this, "VTBaudRChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(elfConfiguration[selectedComputer_].baudR);
		XRCCTRL(*this, "VTBaudTChoice" + computerInfo[selectedComputer_].gui, wxChoice)->SetSelection(elfConfiguration[selectedComputer_].baudT);
	}
}

