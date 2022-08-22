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
#include "guixml.h"

BEGIN_EVENT_TABLE(GuiXml, GuiXml)

END_EVENT_TABLE()

GuiXml::GuiXml(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiMain(title, pos, size, mode, dataDir, iniDir)
{
}

void GuiXml::parseXmlFile(int computer, wxString xmlDir, wxString xmlFile)
{
    wxString tagList[]=
    {
        "system",
        "bootstrap",
        "frontpanel",
        "memory",
        "comment",
        "disk",
        "video",
        "keyboard",
        "hex",
        "vt",
        "printer",
        "cassette",
        "xmodem",
        "efbuttons",
        "sound",
        "undefined"
    };

    enum
    {
        TAG_SYSTEM,
        TAG_BOOTSTRAP,
        TAG_PANEL,
        TAG_MEMORY,
        TAG_COMMENT,
        TAG_DISK,
        TAG_VIDEO,
        TAG_KEYBOARD,
        TAG_HEX,
        TAG_VT,
        TAG_PRINTER,
        TAG_CASSETTE,
        TAG_XMODEM,
        TAG_EFBUTTONS,
        TAG_SOUND,
        TAG_UNDEFINED
    };
    
    if (!wxFile::Exists(xmlDir + xmlFile))
    {
        conf[computer].memConfigNumber_ = 1;
        conf[computer].memConfig_.resize(1);
        conf[computer].memConfig_[0].filename = "";
        return;
    }
        
    wxFileName xmlFileName = wxFileName(xmlDir + xmlFile);
    wxDateTime newDate = xmlFileName.GetModificationTime();
    
    if (oldXmlFileName_ == xmlDir + xmlFile)
    {
        
        if (newDate.IsEqualTo(oldXmlDate_))
            return;
    }

    computerInfo[computer].gui = "Diy";
    
    elfConfiguration[computer].autoBoot = false;
    conf[computer].bootAddress_ = 0;

    elfConfiguration[computer].bootStrap = false;

    conf[computer].memConfigNumber_ = 1;
    conf[computer].memConfig_.resize(1);
    conf[computer].memConfig_[0].filename = "";
    conf[computer].mainDir_ =  dataDir_;
    conf[computer].emsConfigNumber_ = 0;
    elfConfiguration[computer].usePager = false;
    elfConfiguration[computer].usePortExtender = false;
    elfConfiguration[computer].useEms = false;
    elfConfiguration[computer].fdcEnabled = false;
    elfConfiguration[computer].ideEnabled = false;
    
    elfConfiguration[computer].usePixie = false;
    elfConfiguration[computer].use6845 = false;
    elfConfiguration[computer].use6847 = false;
    elfConfiguration[computer].useTMS9918 = false;
    elfConfiguration[computer].use8275 = false;
    elfConfiguration[computer].useS100 = false;
    
    elfConfiguration[computer].useHexKeyboard = false;
    elfConfiguration[computer].useHexKeyboardEf3 = false;
    elfConfiguration[computer].useKeyboard = false;
    elfConfiguration[computer].UsePS2 = false;
    elfConfiguration[computer].ps2Interrupt = false;
    elfConfiguration[computer].usePs2gpio = false;
    elfConfiguration[computer].forceUpperCase = false;

    elfConfiguration[computer].vtType = VTNONE;
    elfConfiguration[computer].vtExternal = false;
    elfConfiguration[computer].useUart = false;
    elfConfiguration[computer].useUart16450 = false;
    elfConfiguration[computer].baudR = 4;
    elfConfiguration[computer].baudT = 4;
    elfConfiguration[computer].serialPort_ = "";
    elfConfiguration[computer].vt52CharRom_ = "vt52.a.bin";
    elfConfiguration[computer].vt52CharRomDir_ = conf[computer].mainDir_ ;
    elfConfiguration[computer].vt100CharRom_ = "vt100.bin";
    elfConfiguration[computer].vt100CharRomDir_ = conf[computer].mainDir_ ;
    elfConfiguration[computer].vtWavFile_ = "";
    elfConfiguration[computer].vtWavFileDir_ = conf[computer].mainDir_ ;

    conf[computer].printerOn_ = false;
    elfConfiguration[computer].elfPortConf.printerEf = 0;

    elfConfiguration[computer].useTape = false;
    elfConfiguration[computer].useXmodem = false;

    elfConfiguration[computer].qSound_ = QSOUNDOFF;

    elfConfiguration[computer].efButtons = false;
    elfConfiguration[computer].panelType_ = PANEL_NONE;

    oldXmlFileName_ = xmlDir + xmlFile;
    oldXmlDate_ = newDate;

    int tagTypeInt;

    warningText_ = "";
    
    wxXmlDocument doc;
    
    if (!doc.Load(xmlDir + xmlFile))
        return;
    
    // start processing the XML file
    if (doc.GetRoot()->GetName() != "emmaconfig")
        return;
    
    wxXmlNode *child = doc.GetRoot()->GetChildren();
    while (child)
    {
        wxString childName = child->GetName();
        
        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_SYSTEM:
                parseXml_System (computer, *child);
            break;
                
            case TAG_BOOTSTRAP:
                parseXml_Bootstrap (computer, *child);
            break;
                
            case TAG_PANEL:
                if (child->GetNodeContent() == "elf")
                    elfConfiguration[computer].panelType_ = PANEL_ELF;
                if (child->GetNodeContent() == "elfii")
                    elfConfiguration[computer].panelType_ = PANEL_ELFII;
                if (child->GetNodeContent() == "super")
                    elfConfiguration[computer].panelType_ = PANEL_SUPER;
            break;
                
            case TAG_MEMORY:
                parseXml_Memory (computer, *child);
            break;

            case TAG_DISK:
                if (child->GetAttribute("type") == "1793")
                    parseXml_FdcDisk (computer, *child);
                if (child->GetAttribute("type") == "ide")
                    parseXml_IdeDisk (computer, *child);
            break;
                
            case TAG_VIDEO:
                if (child->GetAttribute("type") == "pixie")
                    parseXml_PixieVideo (computer, *child);
                if (child->GetAttribute("type") == "mc6845")
                    parseXml_MC6845Video (computer, *child);
                if (child->GetAttribute("type") == "mc6847")
                    parseXml_MC6847Video (computer, *child);
                if (child->GetAttribute("type") == "tms9918")
                    parseXml_TMS9918Video (computer, *child);
                if (child->GetAttribute("type") == "intel8275")
                    parseXml_Intel8275Video (computer, *child);
                if (child->GetAttribute("type") == "super")
                    parseXml_SuperVideo (computer, *child);
            break;

            case TAG_KEYBOARD:
                if (child->GetAttribute("type") == "ps2")
                {
                   elfConfiguration[computer].ps2Interrupt = false;
                   parseXml_Ps2Keyboard (computer, *child);
                }
                if (child->GetAttribute("type") == "ps2-interrupt")
                {
                   elfConfiguration[computer].ps2Interrupt = true;
                   parseXml_Ps2Keyboard (computer, *child);
                }
                if (child->GetAttribute("type") == "ascii")
                   parseXml_AsciiKeyboard (computer, *child);
            break;

            case TAG_HEX:
                  if (child->GetAttribute("type") == "elf")
                    parseXml_ElfHexKeypad (computer, *child);
            break;

            case TAG_VT:
                if (child->GetAttribute("type") == "vt100")
                    elfConfiguration[computer].vtType = VT100;
                if (child->GetAttribute("type") == "vt52")
                    elfConfiguration[computer].vtType = VT52;
                if (child->GetAttribute("type") == "external")
                {
                    elfConfiguration[computer].vtExternal = true;
                    elfConfiguration[computer].vtType = VTNONE;
                }
                if (child->GetAttribute("connection") == "serial")
                    parseXml_SerialVt (computer, *child);
                if (child->GetAttribute("connection") == "uart1854")
                {
                    elfConfiguration[computer].useUart = true;
                    parseXml_UartVt (computer, *child, false);
                }
                if (child->GetAttribute("connection") == "uart16450")
                {
                    elfConfiguration[computer].useUart16450 = true;
                    parseXml_UartVt (computer, *child, true);
                }
            break;

            case TAG_PRINTER:
                  parseXml_Printer (computer, *child);
            break;

            case TAG_CASSETTE:
                   parseXml_Cassette (computer, *child);
            break;

            case TAG_SOUND:
                if (child->GetAttribute("type") == "q")
                    parseXml_QSound (computer, *child);
            break;

            case TAG_XMODEM:
                elfConfiguration[computer].useTape = false;
                elfConfiguration[computer].useXmodem = true;
            break;

            case TAG_EFBUTTONS:
                elfConfiguration[computer].efButtons = true;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
       
        child = child->GetNext();
    }
    
    if (warningText_ != "")
        wxMessageBox(warningText_, "Warning list xml parser", wxICON_EXCLAMATION);
}

void GuiXml::parseXml_System(int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "name",
        "clock",
        "boot",
        "init",
        "dirname",
        "pload",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_NAME,
        TAG_CLOCK,
        TAG_BOOT,
        TAG_INIT,
        TAG_DIRNAME,
        TAG_PLOAD,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_NAME:
                computerInfo[computer].name = child->GetNodeContent();
            break;

            case TAG_CLOCK:
                conf[computer].clock_ = getDoubleString(child->GetNodeContent(), childName, 500, "500");
            break;
                
            case TAG_BOOT:
                elfConfiguration[computer].autoBoot = true;
                conf[computer].bootAddress_ = getHexDec(child->GetNodeContent());
            break;
                
            case TAG_INIT:
                if (child->GetAttribute("type") == "ef1")
                    elfConfiguration[computer].elfPortConf.ef1default = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "ef2")
                    elfConfiguration[computer].elfPortConf.ef2default = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "ef3")
                    elfConfiguration[computer].elfPortConf.ef3default = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "ef4")
                    elfConfiguration[computer].elfPortConf.ef4default = (int)parseXml_Number(*child);
            case TAG_DIRNAME:
                conf[computer].mainDir_ =  dataDir_ + child->GetNodeContent();
                if (conf[computer].mainDir_.Right(1) != pathSeparator_)
                    conf[computer].mainDir_ += pathSeparator_;
                
                elfConfiguration[computer].vt52CharRomDir_ = conf[computer].mainDir_ ;
                elfConfiguration[computer].vt100CharRomDir_ = conf[computer].mainDir_ ;
                elfConfiguration[computer].vtWavFileDir_ = conf[computer].mainDir_ ;
            break;
                
            case TAG_PLOAD:
                computerInfo[computer].ploadExtension = child->GetNodeContent();
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_Bootstrap(int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "address",
        "in",
        "out",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_ADDRESS,
        TAG_IN,
        TAG_OUT,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    elfConfiguration[computer].bootStrap = true;
    elfConfiguration[computer].elfPortConf.bootStrapType = BOOTSTRAPREAD;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_ADDRESS:
                elfConfiguration[computer].strapAddress = getHexDec(child->GetNodeContent());
            break;

            case TAG_OUT:
                elfConfiguration[computer].elfPortConf.bootStrapType = BOOTSTRAPOUT;
                elfConfiguration[computer].elfPortConf.bootStrapIo = (int)parseXml_Number(*child);
            break;
                
            case TAG_IN:
                elfConfiguration[computer].elfPortConf.bootStrapType = BOOTSTRAPIN;
                elfConfiguration[computer].elfPortConf.bootStrapIo = (int)parseXml_Number(*child);
            break;
                
            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_IdeDisk(int computer, wxXmlNode &node)
{
    elfConfiguration[computer].ideEnabled = true;

    wxString tagList[]=
    {
        "filename",
        "dirname",
        "out",
        "in",
        "tracks",
        "heads",
        "sectors",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_OUT,
        TAG_IN,
        TAG_TRACKS,
        TAG_HEADS,
        TAG_SECTORS,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    conf[computer].ide_ = "";
    conf[computer].ideDir_ = conf[computer].mainDir_ ;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_FILENAME:
                conf[computer].ide_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                conf[computer].ideDir_ = dataDir_ + child->GetNodeContent();
                if (conf[computer].ideDir_.Right(1) != pathSeparator_)
                    conf[computer].ideDir_ += pathSeparator_;
            break;

            case TAG_IN:
                if (child->GetAttribute("type") == "status")
                    elfConfiguration[computer].elfPortConf.ideStatus = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "read")
                    elfConfiguration[computer].elfPortConf.ideInput = (int)parseXml_Number(*child);
            break;

           case TAG_OUT:
                if (child->GetAttribute("type") == "select")
                    elfConfiguration[computer].elfPortConf.ideSelectOutput = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "write")
                    elfConfiguration[computer].elfPortConf.ideWriteOutput = (int)parseXml_Number(*child);
            break;
                
            case TAG_TRACKS:
                elfConfiguration[computer].elfPortConf.ideTracks = (int)parseXml_Number(*child);;
            break;

            case TAG_HEADS:
                elfConfiguration[computer].elfPortConf.ideHeads = (int)parseXml_Number(*child);
            break;

            case TAG_SECTORS:
                elfConfiguration[computer].elfPortConf.ideSectors = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_FdcDisk(int computer, wxXmlNode &node)
{
    elfConfiguration[computer].fdcEnabled = true;

    wxString tagList[]=
    {
        "out",
        "in",
        "ef",
        "sides",
        "tracks",
        "sectors",
        "sectorlength",
        "maxfmtcount",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_SIDES,
        TAG_TRACKS,
        TAG_SECTORS,
        TAG_SECTOR_LENGTH,
        TAG_MAX_FMT_COUNT,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_IN:
                if (child->GetAttribute("type") == "read")
                    elfConfiguration[computer].elfPortConf.fdcInput = (int)parseXml_Number(*child);
            break;
                
            case TAG_OUT:
                if (child->GetAttribute("type") == "select")
                    elfConfiguration[computer].elfPortConf.fdcSelectOutput = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "write")
                    elfConfiguration[computer].elfPortConf.fdcWriteOutput = (int)parseXml_Number(*child);
            break;
                
            case TAG_EF:
                    elfConfiguration[computer].elfPortConf.fdcEf = (int)parseXml_Number(*child);
            break;
                
            case TAG_SIDES:
                elfConfiguration[computer].elfPortConf.fdcSides = (int)parseXml_Number(*child);
            break;

            case TAG_TRACKS:
                elfConfiguration[computer].elfPortConf.fdcTracks = (int)parseXml_Number(*child);;
            break;

            case TAG_SECTORS:
                elfConfiguration[computer].elfPortConf.fdcSectors = (int)parseXml_Number(*child);
            break;

            case TAG_SECTOR_LENGTH:
                elfConfiguration[computer].elfPortConf.fdcSectorLength = (int)parseXml_Number(*child);
            break;

            case TAG_MAX_FMT_COUNT:
                elfConfiguration[computer].elfPortConf.fdcMaxFmtCount = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_PixieVideo(int computer, wxXmlNode &node)
{
    elfConfiguration[computer].usePixie = true;
    elfConfiguration[computer].use6845 = false;
    elfConfiguration[computer].use6847 = false;
    elfConfiguration[computer].useTMS9918 = false;
    elfConfiguration[computer].use8275 = false;
    elfConfiguration[computer].useS100 = false;

    wxString tagList[]=
    {
        "out",
        "in",
        "ef",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IN:
                elfConfiguration[computer].elfPortConf.pixieInput = (int)parseXml_Number(*child);
            break;
                
            case TAG_OUT:
                elfConfiguration[computer].elfPortConf.pixieOutput = (int)parseXml_Number(*child);
           break;
                
            case TAG_EF:
                    elfConfiguration[computer].elfPortConf.pixieEf = (int)parseXml_Number(*child);
            break;
                
            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_MC6845Video(int computer, wxXmlNode &node)
{
    long start, end;
    
    elfConfiguration[computer].usePixie = false;
    elfConfiguration[computer].use6845 = true;
    elfConfiguration[computer].use6847 = false;
    elfConfiguration[computer].useTMS9918 = false;
    elfConfiguration[computer].use8275 = false;
    elfConfiguration[computer].useS100 = false;

    wxString tagList[]=
    {
        "ram",
        "font",
        "dirname",
        "out",
        "ef",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_RAM,
        TAG_FONT,
        TAG_DIRNAME,
        TAG_OUT,
        TAG_EF,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    conf[computer].charRom_ = "";
    conf[computer].charRomDir_ = conf[computer].mainDir_ ;
    elfConfiguration[computer].charLine = 64;

    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_RAM:
                if (!parseXml_Range(*child, &start, &end))
                {
                    warningText_ += "No mc6845 RAM range defined";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    elfConfiguration[computer].elfPortConf.mc6845StartRam = (int)start;
                    elfConfiguration[computer].elfPortConf.mc6845EndRam = (int)end;
                }
             break;
                
            case TAG_OUT:
                if (child->GetAttribute("type") == "register")
                    elfConfiguration[computer].elfPortConf.mc6845Address = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "data")
                    elfConfiguration[computer].elfPortConf.mc6845Data = (int)parseXml_Number(*child);
           break;
                
            case TAG_FONT:
                conf[computer].charRom_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                conf[computer].charRomDir_ = dataDir_ + child->GetNodeContent();
                if (conf[computer].charRomDir_.Right(1) != pathSeparator_)
                    conf[computer].charRomDir_ += pathSeparator_;
            break;

            case TAG_EF:
                    elfConfiguration[computer].elfPortConf.mc6845Ef = (int)parseXml_Number(*child);
            break;
                
            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_MC6847Video(int computer, wxXmlNode &node)
{
    long start, end;
    
    elfConfiguration[computer].usePixie = false;
    elfConfiguration[computer].use6845 = false;
    elfConfiguration[computer].use6847 = true;
    elfConfiguration[computer].useTMS9918 = false;
    elfConfiguration[computer].use8275 = false;
    elfConfiguration[computer].useS100 = false;

    wxString tagList[]=
    {
        "ram",
        "out",
        "font",
        "dirname",
        "inv",
        "ext",
        "css",
        "as",
        "ag",
        "gm0",
        "gm1",
        "gm2",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_RAM,
        TAG_OUT,
        TAG_FONT,
        TAG_DIRNAME,
        TAG_INV,
        TAG_EXT,
        TAG_CSS,
        TAG_AS,
        TAG_AG,
        TAG_GM0,
        TAG_GM1,
        TAG_GM2,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    conf[computer].charRom_ = "";
    conf[computer].charRomDir_ = conf[computer].mainDir_ ;

    int tagTypeInt;

    elfConfiguration[computer].elfPortConf.mc6847b7 = 0;
    elfConfiguration[computer].elfPortConf.mc6847b6 = 0;
    elfConfiguration[computer].elfPortConf.mc6847b5 = 0;
    elfConfiguration[computer].elfPortConf.mc6847b4 = 0;
    elfConfiguration[computer].elfPortConf.mc6847b3 = 0;
    elfConfiguration[computer].elfPortConf.mc6847b2 = 0;
    elfConfiguration[computer].elfPortConf.mc6847b1 = 0;
    elfConfiguration[computer].elfPortConf.mc6847b0 = 0;
    elfConfiguration[computer].elfPortConf.mc6847dd7 = 0;
    elfConfiguration[computer].elfPortConf.mc6847dd6 = 0;

    elfConfiguration[computer].elfPortConf.forceHighAg = false;
    elfConfiguration[computer].elfPortConf.forceHighAs = false;
    elfConfiguration[computer].elfPortConf.forceHighExt = false;
    elfConfiguration[computer].elfPortConf.forceHighGm2 = false;
    elfConfiguration[computer].elfPortConf.forceHighGm1 = false;
    elfConfiguration[computer].elfPortConf.forceHighGm0 = false;
    elfConfiguration[computer].elfPortConf.forceHighCss = false;
    elfConfiguration[computer].elfPortConf.forceHighInv = false;

    elfConfiguration[computer].charLine = 32;
    elfConfiguration[computer].screenHeight6847 = 192;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_RAM:
                if (!parseXml_Range(*child, &start, &end))
                {
                    warningText_ += "No mc6847 RAM range defined";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    elfConfiguration[computer].elfPortConf.mc6847StartRam = (int)start;
                    elfConfiguration[computer].elfPortConf.mc6847EndRam = (int)end;
                }
            break;
                
            case TAG_OUT:
                if (child->GetAttribute("type") == "mode")
                {
                    if (!parseXml_Range(*child, &start, &end))
                    {
                        elfConfiguration[computer].elfPortConf.mc6847OutputMode = 0;
                        elfConfiguration[computer].elfPortConf.mc6847Output = start & 07;
                    }
                    else
                    {
                        elfConfiguration[computer].elfPortConf.mc6847OutputMode = 1;
                        elfConfiguration[computer].elfPortConf.mc6847OutputStart = (Word) start;
                        elfConfiguration[computer].elfPortConf.mc6847OutputEnd = (Word) end;
                    }
                }
            break;
 
            case TAG_FONT:
                conf[computer].charRom_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                conf[computer].charRomDir_ = dataDir_ + child->GetNodeContent();
                if (conf[computer].charRomDir_.Right(1) != pathSeparator_)
                    conf[computer].charRomDir_ += pathSeparator_;
            break;

            case TAG_INV:
                if (child->GetAttribute("init") == "high")
                    elfConfiguration[computer].elfPortConf.forceHighInv = true;
                parseXml_Mc6857BitSetup(computer, *child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_EXT:
                if (child->GetAttribute("init") == "high")
                    elfConfiguration[computer].elfPortConf.forceHighExt = true;
                parseXml_Mc6857BitSetup(computer, *child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_CSS:
                if (child->GetAttribute("init") == "high")
                    elfConfiguration[computer].elfPortConf.forceHighCss = true;
                parseXml_Mc6857BitSetup(computer, *child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_AS:
                if (child->GetAttribute("init") == "high")
                    elfConfiguration[computer].elfPortConf.forceHighAs = true;
                parseXml_Mc6857BitSetup(computer, *child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_AG:
                if (child->GetAttribute("init") == "high")
                    elfConfiguration[computer].elfPortConf.forceHighAg = true;
                parseXml_Mc6857BitSetup(computer, *child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_GM0:
                if (child->GetAttribute("init") == "high")
                    elfConfiguration[computer].elfPortConf.forceHighGm0 = true;
                parseXml_Mc6857BitSetup(computer, *child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_GM1:
                if (child->GetAttribute("init") == "high")
                    elfConfiguration[computer].elfPortConf.forceHighGm1 = true;
                parseXml_Mc6857BitSetup(computer, *child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_GM2:
                if (child->GetAttribute("init") == "high")
                    elfConfiguration[computer].elfPortConf.forceHighGm2 = true;
                parseXml_Mc6857BitSetup(computer, *child, tagTypeInt - TAG_INV + 1, childName);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_TMS9918Video(int computer, wxXmlNode &node)
{
    elfConfiguration[computer].usePixie = false;
    elfConfiguration[computer].use6845 = false;
    elfConfiguration[computer].use6847 = false;
    elfConfiguration[computer].useTMS9918 = true;
    elfConfiguration[computer].use8275 = false;
    elfConfiguration[computer].useS100 = false;

    wxString tagList[]=
    {
        "out",
        "ef",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_EF,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                if (child->GetAttribute("type") == "register")
                    elfConfiguration[computer].elfPortConf.tmsModeHighOutput = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "data")
                    elfConfiguration[computer].elfPortConf.tmsModeLowOutput = (int)parseXml_Number(*child);
           break;
                
            case TAG_EF:
                    elfConfiguration[computer].elfPortConf.tmsInterrupt = (int)parseXml_Number(*child);
            break;
                
            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_Intel8275Video(int computer, wxXmlNode &node)
{
    elfConfiguration[computer].usePixie = false;
    elfConfiguration[computer].use6845 = false;
    elfConfiguration[computer].use6847 = false;
    elfConfiguration[computer].useTMS9918 = false;
    elfConfiguration[computer].use8275 = true;
    elfConfiguration[computer].useS100 = false;

    wxString tagList[]=
    {
        "font",
        "dirname",
        "in",
        "out",
        "ef",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_FONT,
        TAG_DIRNAME,
        TAG_IN,
        TAG_OUT,
        TAG_EF,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    conf[computer].charRom_ = "";
    conf[computer].charRomDir_ = conf[computer].mainDir_ ;

    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IN:
                if (child->GetAttribute("type") == "command")
                    elfConfiguration[computer].elfPortConf.i8275WriteCommand = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "parameter")
                    elfConfiguration[computer].elfPortConf.i8275WriteParameter = (int)parseXml_Number(*child);
            break;

            case TAG_OUT:
                if (child->GetAttribute("type") == "status")
                    elfConfiguration[computer].elfPortConf.i8275ReadStatus = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "parameter")
                    elfConfiguration[computer].elfPortConf.i8275ReadParameter = (int)parseXml_Number(*child);
            break;
                
            case TAG_FONT:
                conf[computer].charRom_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                conf[computer].charRomDir_ = dataDir_ + child->GetNodeContent();
                if (conf[computer].charRomDir_.Right(1) != pathSeparator_)
                    conf[computer].charRomDir_ += pathSeparator_;
            break;

            case TAG_EF:
                    elfConfiguration[computer].elfPortConf.i8275VerticalRetrace = (int)parseXml_Number(*child);
            break;
                
            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_SuperVideo(int computer, wxXmlNode &node)
{
    long start, end;
    
    elfConfiguration[computer].usePixie = false;
    elfConfiguration[computer].use6845 = false;
    elfConfiguration[computer].use6847 = false;
    elfConfiguration[computer].useTMS9918 = false;
    elfConfiguration[computer].use8275 = false;
    elfConfiguration[computer].useS100 = true;

    wxString tagList[]=
    {
        "ram",
        "font",
        "dirname",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_RAM,
        TAG_FONT,
        TAG_DIRNAME,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    conf[computer].charRom_ = "";
    conf[computer].charRomDir_ = conf[computer].mainDir_ ;
    elfConfiguration[computer].charLine = 64;

    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_RAM:
                if (!parseXml_Range(*child, &start, &end))
                {
                    warningText_ += "No super video RAM range defined";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    elfConfiguration[computer].elfPortConf.mc6845StartRam = (int)start;
                    elfConfiguration[computer].elfPortConf.mc6845EndRam = (int)end;
                }
            break;
                                
            case TAG_FONT:
                conf[computer].charRom_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                conf[computer].charRomDir_ = dataDir_ + child->GetNodeContent();
                if (conf[computer].charRomDir_.Right(1) != pathSeparator_)
                    conf[computer].charRomDir_ += pathSeparator_;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_Ps2Keyboard (int computer, wxXmlNode &node)
{
    elfConfiguration[computer].useKeyboard = false;
    elfConfiguration[computer].UsePS2 = true;
    elfConfiguration[computer].usePs2gpio = false;

    wxString tagList[]=
    {
        "out",
        "in",
        "ef",
        "caps",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_CAPS,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IN:
                elfConfiguration[computer].elfPortConf.ps2KeyboardInput = (int)parseXml_Number(*child);
            break;
                
            case TAG_OUT:
                elfConfiguration[computer].elfPortConf.ps2KeyboardOutput = (int)parseXml_Number(*child);
           break;
                
            case TAG_EF:
                elfConfiguration[computer].elfPortConf.ps2KeyboardEf = (int)parseXml_Number(*child);
            break;
 
            case TAG_CAPS:
                elfConfiguration[computer].forceUpperCase = true;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_AsciiKeyboard (int computer, wxXmlNode &node)
{
    elfConfiguration[computer].useKeyboard = true;
    elfConfiguration[computer].UsePS2 = false;
    elfConfiguration[computer].ps2Interrupt = false;
    elfConfiguration[computer].usePs2gpio = false;

    wxString tagList[]=
    {
        "in",
        "ef",
        "caps",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IN,
        TAG_EF,
        TAG_CAPS,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_IN:
                elfConfiguration[computer].elfPortConf.keyboardInput = (int)parseXml_Number(*child);
            break;
                
            case TAG_EF:
                elfConfiguration[computer].elfPortConf.keyboardEf = (int)parseXml_Number(*child);
            break;
                
            case TAG_CAPS:
                elfConfiguration[computer].forceUpperCase = true;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_ElfHexKeypad (int computer, wxXmlNode &node)
{
    elfConfiguration[computer].useHexKeyboard = true;
    elfConfiguration[computer].useHexKeyboardEf3 = false;

    wxString tagList[]=
    {
        "out",
        "in",
        "ef",
        "til",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_EF,
        TAG_TIL,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                elfConfiguration[computer].elfPortConf.hexOutput = (int)parseXml_Number(*child);
            break;
                
            case TAG_IN:
                elfConfiguration[computer].elfPortConf.hexInput = (int)parseXml_Number(*child);
            break;
                
            case TAG_EF:
                elfConfiguration[computer].useHexKeyboardEf3 = true;
                elfConfiguration[computer].elfPortConf.hexEf = (int)parseXml_Number(*child);
            break;
                
            case TAG_TIL:
                if (child->GetNodeContent() == "311")
                    elfConfiguration[computer].tilType = TIL311;
                else
                    elfConfiguration[computer].tilType = TIL313;
            break;
                
            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_SerialVt (int computer, wxXmlNode &node)
{
    int baudRateValue_[] =
    {
        38400, 19200, 9600, 4800, 3600, 2400, 2000, 1800, 1200, 600, 300, 200, 150, 134, 110, 75, 50
    };

    wxString tagList[]=
    {
        "font",
        "wav",
        "dirname",
        "out",
        "ef",
        "q",
        "serialport",
        "baud",
        "characters",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_FONT,
        TAG_WAV,
        TAG_DIRNAME,
        TAG_OUT,
        TAG_EF,
        TAG_Q,
        TAG_SERIALPORT,
        TAG_BAUD,
        TAG_CHARACTERS,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    int baud;
    int number;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_FONT:
                if (elfConfiguration[computer].vtType == VT52)
                    elfConfiguration[computer].vt52CharRom_ = child->GetNodeContent();
                if (elfConfiguration[computer].vtType == VT100)
                    elfConfiguration[computer].vt100CharRom_ = child->GetNodeContent();
            break;

            case TAG_WAV:
                elfConfiguration[computer].vtWavFile_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                if (child->GetAttribute("type") == "font")
                {
                    if (elfConfiguration[computer].vtType == VT52)
                    {
                        elfConfiguration[computer].vt52CharRomDir_ = dataDir_ + child->GetNodeContent();
                        if (elfConfiguration[computer].vt52CharRomDir_.Right(1) != pathSeparator_)
                            elfConfiguration[computer].vt52CharRomDir_ += pathSeparator_;
                    }
                    if (elfConfiguration[computer].vtType == VT100)
                    {
                        elfConfiguration[computer].vt100CharRomDir_ = dataDir_ + child->GetNodeContent();
                        if (elfConfiguration[computer].vt100CharRomDir_.Right(1) != pathSeparator_)
                            elfConfiguration[computer].vt100CharRomDir_ += pathSeparator_;
                    }
                }
                if (child->GetAttribute("type") == "wav")
                {
                    elfConfiguration[computer].vtWavFileDir_ = dataDir_ + child->GetNodeContent();
                    if (elfConfiguration[computer].vtWavFileDir_.Right(1) != pathSeparator_)
                        elfConfiguration[computer].vtWavFileDir_ += pathSeparator_;
                }
            break;

            case TAG_OUT:
                elfConfiguration[computer].elfPortConf.vt100Output = (int)parseXml_Number(*child);
            break;
                
            case TAG_EF:
                elfConfiguration[computer].elfPortConf.vt100Ef = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "rev")
                    elfConfiguration[computer].elfPortConf.vt100ReverseEf = 0;
                else
                    elfConfiguration[computer].elfPortConf.vt100ReverseEf = 1;
            break;
                
            case TAG_Q:
                if (child->GetAttribute("type") == "rev")
                    elfConfiguration[computer].elfPortConf.vt100ReverseQ = 1;
                else
                    elfConfiguration[computer].elfPortConf.vt100ReverseQ = 0;
            break;
                
            case TAG_SERIALPORT:
                elfConfiguration[computer].serialPort_ = child->GetNodeContent();
            break;

            case TAG_BAUD:
                baud = (int)parseXml_Number(*child);
                number = 0;
                while (baud < baudRateValue_[number] && baudRateValue_[number] != 50)
                    number++;
                if (child->GetAttribute("type") == "receive")
                elfConfiguration[computer].baudR = number;
                if (child->GetAttribute("type") == "transmit")
                elfConfiguration[computer].baudT = number;
            break;

            case TAG_CHARACTERS:
                if (child->GetNodeContent() == "132")
                {
                    elfConfiguration[computer].vtCharactersPerRow = 132;
                    elfConfiguration[computer].vt100CharWidth = 8;
                }
                else
                {
                    elfConfiguration[computer].vtCharactersPerRow = 80;
                    elfConfiguration[computer].vt100CharWidth = 10;
                }
            break;
                
            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_UartVt (int computer, wxXmlNode &node, bool uart16450)
{
    int baudRateValue_[] =
    {
        38400, 19200, 9600, 4800, 3600, 2400, 2000, 1800, 1200, 600, 300, 200, 150, 134, 110, 75, 50
    };

    wxString tagList[]=
    {
        "font",
        "wav",
        "dirname",
        "in",
        "out",
        "serialport",
        "baud",
        "characters",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_FONT,
        TAG_WAV,
        TAG_DIRNAME,
        TAG_IN,
        TAG_OUT,
        TAG_SERIALPORT,
        TAG_BAUD,
        TAG_CHARACTERS,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    int baud;
    int number;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_FONT:
                if (elfConfiguration[computer].vtType == VT52)
                    elfConfiguration[computer].vt52CharRom_ = child->GetNodeContent();
                if (elfConfiguration[computer].vtType == VT100)
                    elfConfiguration[computer].vt100CharRom_ = child->GetNodeContent();
            break;

            case TAG_WAV:
                elfConfiguration[computer].vtWavFile_ = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                if (child->GetAttribute("type") == "font")
                {
                    if (elfConfiguration[computer].vtType == VT52)
                    {
                        elfConfiguration[computer].vt52CharRomDir_ = dataDir_ + child->GetNodeContent();
                        if (elfConfiguration[computer].vt52CharRomDir_.Right(1) != pathSeparator_)
                            elfConfiguration[computer].vt52CharRomDir_ += pathSeparator_;
                    }
                    if (elfConfiguration[computer].vtType == VT100)
                    {
                        elfConfiguration[computer].vt100CharRomDir_ = dataDir_ + child->GetNodeContent();
                        if (elfConfiguration[computer].vt100CharRomDir_.Right(1) != pathSeparator_)
                            elfConfiguration[computer].vt100CharRomDir_ += pathSeparator_;
                    }
                }
                if (child->GetAttribute("type") == "wav")
                {
                    elfConfiguration[computer].vtWavFileDir_ = dataDir_ + child->GetNodeContent();
                    if (elfConfiguration[computer].vtWavFileDir_.Right(1) != pathSeparator_)
                        elfConfiguration[computer].vtWavFileDir_ += pathSeparator_;
                }
            break;
                
            case TAG_IN:
                if (uart16450)
                {
                    if (child->GetAttribute("type") == "status")
                        elfConfiguration[computer].elfPortConf.uartIn = (int)parseXml_Number(*child);
                    if (child->GetAttribute("type") == "port")
                        elfConfiguration[computer].elfPortConf.uartStatus = (int)parseXml_Number(*child);
                }
                else
                {
                    if (child->GetAttribute("type") == "register")
                        elfConfiguration[computer].elfPortConf.uartIn = (int)parseXml_Number(*child);
                    if (child->GetAttribute("type") == "status")
                        elfConfiguration[computer].elfPortConf.uartStatus = (int)parseXml_Number(*child);
                }
            break;

            case TAG_OUT:
                if (uart16450)
                {
                    if (child->GetAttribute("type") == "select")
                        elfConfiguration[computer].elfPortConf.uartOut = (int)parseXml_Number(*child);
                    if (child->GetAttribute("type") == "port")
                        elfConfiguration[computer].elfPortConf.uartControl = (int)parseXml_Number(*child);
                }
                else
                {
                    if (child->GetAttribute("type") == "register")
                        elfConfiguration[computer].elfPortConf.uartOut = (int)parseXml_Number(*child);
                    if (child->GetAttribute("type") == "control")
                        elfConfiguration[computer].elfPortConf.uartControl = (int)parseXml_Number(*child);
                }
            break;
                
            case TAG_SERIALPORT:
                elfConfiguration[computer].serialPort_ = child->GetNodeContent();
            break;

            case TAG_BAUD:
                baud = (int)parseXml_Number(*child);
                number = 0;
                while (baud < baudRateValue_[number] && baudRateValue_[number] != 50)
                    number++;
                if (child->GetAttribute("type") == "receive")
                elfConfiguration[computer].baudR = number;
                if (child->GetAttribute("type") == "transmit")
                elfConfiguration[computer].baudT = number;
            break;

            case TAG_CHARACTERS:
                if (child->GetNodeContent() == "132")
                {
                    elfConfiguration[computer].vtCharactersPerRow = 132;
                    elfConfiguration[computer].vt100CharWidth = 8;
                }
                else
                {
                    elfConfiguration[computer].vtCharactersPerRow = 80;
                    elfConfiguration[computer].vt100CharWidth = 10;
                }
            break;
                
            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_Printer (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "out",
        "ef",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_EF,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    conf[computer].printerOn_ = true;
    
    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                elfConfiguration[computer].elfPortConf.printerOutput = (int)parseXml_Number(*child);
            break;
                
            case TAG_EF:
                elfConfiguration[computer].elfPortConf.printerEf = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_Cassette (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "ef",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_EF,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    elfConfiguration[computer].useTape = true;
    elfConfiguration[computer].useXmodem = false;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_EF:
                elfConfiguration[computer].elfPortConf.tapeEf = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_QSound (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "hw",
        "sw",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_HW,
        TAG_SW,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    conf[computer].beepFrequency_ = 250;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_HW:
                elfConfiguration[computer].qSound_ = QSOUNDEXT;
                conf[computer].beepFrequency_ = (int)parseXml_Number(*child);
            break;

            case TAG_SW:
                elfConfiguration[computer].qSound_ = QSOUNDSW;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_Panel (int computer, wxXmlNode &node)
{
    wxString tagList[]=
    {
        "in",
        "out",
        "ef",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_IN,
        TAG_OUT,
        TAG_EF,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;
    
    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;
        
        switch (tagTypeInt)
        {
            case TAG_OUT:
                elfConfiguration[computer].elfPortConf.hexOutput = (int)parseXml_Number(*child);
            break;
                
            case TAG_EF:
                elfConfiguration[computer].elfPortConf.printerEf = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}
 
void GuiXml::parseXml_Memory(int computer, wxXmlNode &node)
{
    Word memMask;
    
    wxString tagList[]=
    {
        "rom",
        "mainram",
        "ram",
        "ems",
        "mapper",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_ROM,
        TAG_MAINRAM,
        TAG_RAM,
        TAG_EMS,
        TAG_MAPPER,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_ROM:
                conf[computer].memConfig_.resize(conf[computer].memConfigNumber_+1);
                parseXml_RomRam (computer, *child, (int)(ROM + 256*conf[computer].memConfigNumber_), conf[computer].memConfigNumber_);
                conf[computer].memConfig_[conf[computer].memConfigNumber_].memMask = parseXml_Number(*child, "mask");
                if (conf[computer].memConfig_[conf[computer].memConfigNumber_].memMask == 0)
                    conf[computer].memConfig_[conf[computer].memConfigNumber_].memMask = 0xffff;
                conf[computer].memConfig_[conf[computer].memConfigNumber_].memMask |= 0xff;
                conf[computer].memConfigNumber_++;
            break;

            case TAG_MAINRAM:
                parseXml_RomRam (computer, *child, MAINRAM, 0);
                conf[computer].memConfig_[0].memMask = parseXml_Number(*child, "mask");
                if (conf[computer].memConfig_[0].memMask == 0)
                    conf[computer].memConfig_[0].memMask = 0xffff;
                conf[computer].memConfig_[0].memMask |= 0xff;
            break;

            case TAG_RAM:
                conf[computer].memConfig_.resize(conf[computer].memConfigNumber_+1);
                parseXml_RomRam (computer, *child, (int)(RAM + 256*conf[computer].memConfigNumber_), conf[computer].memConfigNumber_);
                conf[computer].memConfig_[conf[computer].memConfigNumber_].memMask = parseXml_Number(*child, "mask");
                if (conf[computer].memConfig_[conf[computer].memConfigNumber_].memMask == 0)
                    conf[computer].memConfig_[conf[computer].memConfigNumber_].memMask = 0xffff;
                conf[computer].memConfig_[conf[computer].memConfigNumber_].memMask |= 0xff;
                conf[computer].memConfigNumber_++;
            break;

            case TAG_EMS:
                conf[computer].emsConfig_.resize(conf[computer].emsConfigNumber_+1);
                elfConfiguration[computer].elfPortConf.emsOutput.resize(conf[computer].emsConfigNumber_+1);

                if (child->GetAttribute("type") == "ROM")
                    conf[computer].emsConfig_[conf[computer].emsConfigNumber_].emsType = ROM;
                else
                    conf[computer].emsConfig_[conf[computer].emsConfigNumber_].emsType = RAM;

                elfConfiguration[computer].useEms = true;
                parseXml_Ems (computer, *child, (int)(EMSMEMORY + 256*conf[computer].emsConfigNumber_), conf[computer].emsConfigNumber_++);
            break;

            case TAG_MAPPER:
                conf[computer].memConfig_.resize(conf[computer].memConfigNumber_+1);
                elfConfiguration[computer].usePager = true;
                elfConfiguration[computer].usePortExtender = true;
                
                memMask = parseXml_Number(*child, "mask");
                if (memMask == 0)
                    memMask = 0xffff;
                memMask  |= 0xff;

                conf[computer].pagerMaskBits_ = 16;
                conf[computer].pagerMask_ = 0xFFFF;
                while ((memMask & 0x8000) == 0)
                {
                    conf[computer].pagerMaskBits_--;
                    conf[computer].pagerMask_ = conf[computer].pagerMask_ >> 1;
                    memMask = memMask << 1;
                }

                parseXml_portExt (computer, *child, PAGER, conf[computer].memConfigNumber_++);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_RomRam(int computer, wxXmlNode &node, int type, size_t configNumber)
{
    wxString tagList[]=
    {
        "start",
        "end",
        "filename",
        "dirname",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_START,
        TAG_END,
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    conf[computer].memConfig_[configNumber].start = 0;
    conf[computer].memConfig_[configNumber].end = 0;
    conf[computer].memConfig_[configNumber].memMask = 0xFFFF;
    conf[computer].memConfig_[configNumber].filename = "";
    conf[computer].memConfig_[configNumber].dirname = conf[computer].mainDir_ ;
    conf[computer].memConfig_[configNumber].type = type;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_START:
                conf[computer].memConfig_[configNumber].start = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_END:
                conf[computer].memConfig_[configNumber].end = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_FILENAME:
                conf[computer].memConfig_[configNumber].filename = child->GetNodeContent();
                
                conf[computer].memConfig_[configNumber].verifyFileExist = (child->GetAttribute("verify") == "true");
            break;

            case TAG_DIRNAME:
                conf[computer].memConfig_[configNumber].dirname =  dataDir_ + child->GetNodeContent();
                if (conf[computer].memConfig_[configNumber].dirname.Right(1) != pathSeparator_)
                {
                    conf[computer].memConfig_[configNumber].dirname += pathSeparator_;
                }
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_Ems(int computer, wxXmlNode &node, int type, size_t configNumber)
{
    long start, end;
    
    wxString tagList[]=
    {
        "start",
        "end",
        "filename",
        "dirname",
        "out",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_START,
        TAG_END,
        TAG_FILENAME,
        TAG_DIRNAME,
        TAG_OUT,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    int tagTypeInt;

    conf[computer].emsConfig_[configNumber].start = 0;
    conf[computer].emsConfig_[configNumber].end = 0;
    conf[computer].emsConfig_[configNumber].outputMask = 0xFF;
    conf[computer].emsConfig_[configNumber].filename = "";
    conf[computer].emsConfig_[configNumber].dirname = conf[computer].mainDir_ ;
    conf[computer].emsConfig_[configNumber].type = type;
    elfConfiguration[computer].elfPortConf.emsOutput[configNumber] = -1;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_START:
                conf[computer].emsConfig_[configNumber].start = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_END:
                conf[computer].emsConfig_[configNumber].end = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_FILENAME:
                conf[computer].emsConfig_[configNumber].filename = child->GetNodeContent();
            break;

            case TAG_DIRNAME:
                conf[computer].emsConfig_[configNumber].dirname  = dataDir_ + child->GetNodeContent();
                if (conf[computer].emsConfig_[configNumber].dirname.Right(1) != pathSeparator_)
                {
                    conf[computer].emsConfig_[configNumber].dirname += pathSeparator_;
                }
            break;

            case TAG_OUT:
                if (!parseXml_Range(*child, &start, &end))
                    elfConfiguration[computer].elfPortConf.emsOutput[configNumber] = start & 07;
                else
                {
                    conf[computer].emsConfig_[configNumber].outputStart = (Word) start;
                    conf[computer].emsConfig_[configNumber].outputEnd = (Word) end;
                }
                conf[computer].emsConfig_[configNumber].outputMask = parseXml_Number(*child, "mask");
                if (conf[computer].emsConfig_[configNumber].outputMask == 0)
                    conf[computer].emsConfig_[configNumber].outputMask = 0xff;
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

void GuiXml::parseXml_portExt(int computer, wxXmlNode &node, int type, size_t configNumber)
{
    wxString tagList[]=
    {
        "out",
        "in",
        "start",
        "end",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_OUT,
        TAG_IN,
        TAG_START,
        TAG_END,
        TAG_COMMENT,
        TAG_UNDEFINED
    };
    
    conf[computer].memConfig_[configNumber].start = 0;
    conf[computer].memConfig_[configNumber].end = 0;
    conf[computer].memConfig_[configNumber].type = type;

    int tagTypeInt;

    wxXmlNode *child = node.GetChildren();
    while (child)
    {
        wxString childName = child->GetName();

        tagTypeInt = 0;
        while (tagTypeInt != TAG_UNDEFINED && tagList[tagTypeInt] != childName)
            tagTypeInt++;

        switch (tagTypeInt)
        {
            case TAG_START:
                conf[computer].memConfig_[configNumber].start = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_END:
                conf[computer].memConfig_[configNumber].end = parseXml_Number(*child) & 0xffff;
            break;

            case TAG_IN:
                elfConfiguration[computer].elfPortConf.portExtenderInput = (int)parseXml_Number(*child);
            break;

            case TAG_OUT:
                if (child->GetAttribute("type") == "select")
                    elfConfiguration[computer].elfPortConf.portExtenderSelectOutput = (int)parseXml_Number(*child);
                if (child->GetAttribute("type") == "write")
                    elfConfiguration[computer].elfPortConf.portExtenderWriteOutput = (int)parseXml_Number(*child);
            break;

            case TAG_COMMENT:
            break;

            default:
                warningText_ += "Unkown tag: ";
                warningText_ += childName;
                warningText_ += "\n";
            break;
        }
        
        child = child->GetNext();
    }
}

long GuiXml::parseXml_Number(wxXmlNode &node)
{
    return getHexDec(node.GetNodeContent());
}

void GuiXml::parseXml_Mc6857BitSetup(int computer, wxXmlNode &node,  int bitNumber, wxString childName)
{
    wxString bitList[]=
    {
        "",
        "b0",
        "b1",
        "b2",
        "b3",
        "b4",
        "b5",
        "b6",
        "b7",
        "0",
        "1",
        "2",
        "3",
        "4",
        "5",
        "6",
        "7",
        "dd6",
        "dd7",
        "dd8",
        "dd9",
        "dd10",
        "dd11",
        "comment",
        "undefined"
    };
    
    enum
    {
        BIT_NONE,
        BIT_B0,
        BIT_B1,
        BIT_B2,
        BIT_B3,
        BIT_B4,
        BIT_B5,
        BIT_B6,
        BIT_B7,
        BIT_0,
        BIT_1,
        BIT_2,
        BIT_3,
        BIT_4,
        BIT_5,
        BIT_6,
        BIT_7,
        BIT_DD6,
        BIT_DD7,
        BIT_DD8,
        BIT_DD9,
        BIT_DD10,
        BIT_DD11,
        TAG_COMMENT,
        BIT_UNDEFINED
    };

    int bitTypeInt = 0;

    wxString bitType = node.GetNodeContent();
    
    while (bitTypeInt != BIT_UNDEFINED && bitList[bitTypeInt] != bitType)
        bitTypeInt++;
    
    switch (bitTypeInt)
    {
        case BIT_NONE:
        break;

        case BIT_0:
        case BIT_B0:
        case BIT_DD8:
            elfConfiguration[computer].elfPortConf.mc6847b0 =  bitNumber;
        break;

        case BIT_1:
        case BIT_B1:
        case BIT_DD9:
            elfConfiguration[computer].elfPortConf.mc6847b1 =  bitNumber;
        break;

        case BIT_2:
        case BIT_B2:
        case BIT_DD10:
            elfConfiguration[computer].elfPortConf.mc6847b2 =  bitNumber;
        break;

        case BIT_3:
        case BIT_B3:
        case BIT_DD11:
            elfConfiguration[computer].elfPortConf.mc6847b3 =  bitNumber;
        break;

        case BIT_4:
        case BIT_B4:
            elfConfiguration[computer].elfPortConf.mc6847b4 =  bitNumber;
        break;

        case BIT_5:
        case BIT_B5:
            elfConfiguration[computer].elfPortConf.mc6847b5 =  bitNumber;
        break;

        case BIT_6:
        case BIT_B6:
            elfConfiguration[computer].elfPortConf.mc6847b6 =  bitNumber;
        break;

        case BIT_7:
        case BIT_B7:
            elfConfiguration[computer].elfPortConf.mc6847b7 =  bitNumber;
        break;

        case BIT_DD6:
            elfConfiguration[computer].elfPortConf.mc6847dd6 =  bitNumber;
        break;

        case BIT_DD7:
            elfConfiguration[computer].elfPortConf.mc6847dd7 =  bitNumber;
        break;
            
        case TAG_COMMENT:
        break;

        default:
            warningText_ += "Unkown mc6847 bit number";
            warningText_ += childName;
            warningText_ += "\n";
        break;
    }

}

long GuiXml::parseXml_Number(wxXmlNode &node, wxString attribute)
{
    return getHexDec(node.GetAttribute(attribute));
}

bool GuiXml::parseXml_Range(wxXmlNode &node, long *start, long *end)
{
    wxString numberString1, numberString2;

    numberString1 = node.GetNodeContent();
    int dash = numberString1.Find("-");
    int x = numberString1.Find("x");

    if (dash == wxNOT_FOUND && x == wxNOT_FOUND)
    {
        *start = parseXml_Number(node);
        return false;
    }
    
    if (dash == wxNOT_FOUND)
    {
        *start = parseXml_Number(node);
        *end = *start;
        return true;
    }
    numberString2 = numberString1.Right(numberString1.Len()-(dash+1));
    numberString1 = numberString1.Left(dash);
    
    *start = getHexDec(numberString1);
    *end = getHexDec(numberString2);

    return true;
}

long GuiXml::getHexDec(wxString numberString)
{
    int base;
    long number;

    if (numberString.Left(2) == "0x")
    {
        base = 16;
        numberString = numberString.Right(numberString.Len()-2);
    }
    else
        base = 10;
    
    if (!numberString.ToLong(&number, base))
        number = 0;

    return number;
}

wxString GuiXml::getDoubleString(wxString doubleString, wxString tag, double max, wxString maxStr)
{
    double floatValue;

    if (!toDouble(doubleString, (double*)&floatValue))
    {
        warningText_ += tag;
        warningText_ += " not specified in MHz\n";
        return "1.0";
    }
    if (floatValue == 0)
    {
        warningText_ += tag;
        warningText_ += " should not be 0\n";
        return "1.0";
    }
    if (floatValue >= max)
    {
        warningText_ += tag;
        warningText_ += " should be < ";
        warningText_ += maxStr;
        warningText_ += "\n";
        return "1.0";
    }
    return doubleString;
}
