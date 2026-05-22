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

#include "main.h"
#include "sn76430n_config.h"
#include "wx/tglbtn.h"
#include "wx/xrc/xmlres.h"

#define THIS_PANEL_NAME "PanelVideoSN76430N"

BEGIN_EVENT_TABLE(Sn76430NConfig, Cdp1862Config)
END_EVENT_TABLE()

Sn76430NConfig::Sn76430NConfig(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: Cdp1862Config(title, pos, size, mode, dataDir, iniDir)
{
}

Sn76430NConfig::~Sn76430NConfig()
{
}

void Sn76430NConfig::sn76430NConfigInit()
{
    if (!mode_.gui)
        return;

    XRCCTRL(*this, THIS_PANEL_NAME, wxPanel)->Hide();

    computerConfiguration.sn76430NConfiguration.defined = false;
    computerConfiguration.sn76430NConfiguration.startRam = 0x800;
    computerConfiguration.sn76430NConfiguration.endRam = 0xFFF;

    computerConfiguration.sn76430NConfiguration.defaultX = mainWindowX_+windowInfo.mainwX+windowInfo.xBorder;
    computerConfiguration.sn76430NConfiguration.defaultY = mainWindowY_;

    XRCCTRL(*this, "SN76430NIoGroupText", wxStaticText)->SetLabel("");
    XRCCTRL(*this, "SN76430NVideoRam", wxStaticText)->SetLabel("");
}

void Sn76430NConfig::parseXml_SN76430NVideo(wxXmlNode &node)
{
    videoConfigUsed(THIS_PANEL_NAME);

    if (!computerConfiguration.sn76430NConfiguration.defined)
    {
        computerConfiguration.sn76430NConfiguration.videoNumber = computerConfiguration.numberOfVideoTypes_;
        computerConfiguration.numberOfVideoTypes_++;
    }

    computerConfiguration.sn76430NConfiguration.defined = true;

    wxString tagList[]=
    {
        "ram",
        "zoom",
        "border",
        "pos",
        "color",
        "comment",
        "undefined"
    };

    enum
    {
        TAG_RAM,
        TAG_ZOOM,
        TAG_BORDER,
        TAG_POS,
        TAG_COLOR,
        TAG_COMMENT,
        TAG_UNDEFINED
    };

    computerConfiguration.zoom_[computerConfiguration.sn76430NConfiguration.videoNumber] = "2.00";
    computerConfiguration.videoName_[computerConfiguration.sn76430NConfiguration.videoNumber] = "SN76430N";

    long start, end;
    int tagTypeInt;
    long width, height;
    int red, green, blue, xpos, ypos;
    wxString color, position, label;

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
                    warningText_ += "No SN76430N RAM range defined";
                    warningText_ += childName;
                    warningText_ += "\n";
                    if (mode_.gui)
                        XRCCTRL(*this, "SN76430NVideoRam", wxStaticText)->SetLabel("No SN76430N RAM range defined");
                }
                else
                {
                    computerConfiguration.sn76430NConfiguration.startRam = (Word)start;
                    computerConfiguration.sn76430NConfiguration.endRam = (Word)end;
                }
            break;

            case TAG_ZOOM:
                computerConfiguration.zoom_[computerConfiguration.sn76430NConfiguration.videoNumber] = child->GetNodeContent();
            break;

            case TAG_BORDER:
                if (!parseXml_Size(*child, &width, &height))
                {
                    warningText_ += "Incorrect border size";
                    warningText_ += childName;
                    warningText_ += "\n";
                }
                else
                {
                    screenInfo.borderX[VIDEOXMLSN76430N] = (int)width;
                    screenInfo.borderY[VIDEOXMLSN76430N] = (int)height;
                }
            break;

            case TAG_POS:
                position = child->GetNodeContent();
                xpos = (int)getNextHexDec(&position);
                ypos = (int)getNextHexDec(&position);
                if (xpos != -1)
                    xpos += computerConfiguration.sn76430NConfiguration.defaultX;
                if (ypos != -1)
                    ypos += computerConfiguration.sn76430NConfiguration.defaultY;
                computerConfiguration.sn76430NConfiguration.defaultX = xpos;
                computerConfiguration.sn76430NConfiguration.defaultY = ypos;
            break;

            case TAG_COLOR:
                color = child->GetNodeContent();
                red = (int)getNextHexDec(&color) & 0xff;
                green = (int)getNextHexDec(&color) & 0xff;
                blue = (int)getNextHexDec(&color) & 0xff;
                
                if (child->GetAttribute("type") == "white")
                    screenInfo.defaultColour[COL_SN76430N_WHITE].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "yellow")
                    screenInfo.defaultColour[COL_SN76430N_YELLOW].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "green")
                    screenInfo.defaultColour[COL_SN76430N_GREEN].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "red")
                    screenInfo.defaultColour[COL_SN76430N_RED].Printf("#%02X%02X%02X", red, green, blue);
                if (child->GetAttribute("type") == "black")
                    screenInfo.defaultColour[COL_SN76430N_BLACK].Printf("#%02X%02X%02X", red, green, blue);
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
    label.Printf("Video RAM: %04X-%04X", computerConfiguration.sn76430NConfiguration.startRam, computerConfiguration.sn76430NConfiguration.endRam);
    if (mode_.gui)
        XRCCTRL(*this, "SN76430NVideoRam", wxStaticText)->SetLabel(label);
}

void Sn76430NConfig::updateSn76430NPanel()
{
    if (computerConfiguration.sn76430NConfiguration.defined)
    {
    }
}


