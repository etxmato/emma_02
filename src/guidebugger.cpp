/*
 *******************************************************************
 *** This software is copyright 2008 by Marcel van Tongeren      ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/*
 *******************************************************************
 *** This software is copyright 2006 by Michael H Riley          ***
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
#include "wx/tglbtn.h"

#include "main.h"
#include "guidebugger.h"
#include "computerconfig.h"

wxString registerTypeText[] =
{
    "D", "%02X",
    "P", "%X",
    "X", "%X",
    "B", "%02X",
    "T", "%02X",
    "DF", "%X",
    "Q", "%X",
    "CH", "%02X",
    "CNTR", "%02X",
    "R0", "%04X",
    "R1", "%04X",
    "R2", "%04X",
    "R3", "%04X",
    "R4", "%04X",
    "R5", "%04X",
    "R6", "%04X",
    "R7", "%04X",
    "R8", "%04X",
    "R9", "%04X",
    "RA", "%04X",
    "RB", "%04X",
    "RC", "%04X",
    "RD", "%04X",
    "RE", "%04X",
    "RF", "%04X",
};

Word registerSize[] =
{
    0xff,       // D
    0xf,        // P
    0xf,        // X
    0xff,       // B
    0xff,       // T
    1,          // DF
    1,          // Q
    0xff,       // CH
    0xff,       // CNTR
    0xffff,     // R0
    0xffff,
    0xffff,
    0xffff,
    0xffff,
    0xffff,
    0xffff,
    0xffff,
    0xffff,
    0xffff,
    0xffff,
    0xffff,
    0xffff,
    0xffff,
    0xffff,
    0xffff,    // RF
};

wxString registerBits[] =
{
    "4",        // P
    "4",        // X
    "8",        // D
    "8",        // B
    "8",        // T
    "8",        // CH
    "8",        // CNTR
    "1",        // DF
    "1",        // Q
    "16",       // R0
    "16",
    "16",
    "16",
    "16",
    "16",
    "16",
    "16",
    "16",
    "16",
    "16",
    "16",
    "16",
    "16",
    "16",
    "16",       // RF
};

int numberOfBytes[] =
{
    1, 2, 1, 2, 1, 2,
    2, 2, 2, 2, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 2,
    2, 2, // C
    3, 2, 2, 3, 4, 1, 1, 2, 3, 2, 2, 3, 2, // D
    2,
    2, 1, 1,
    1, 1, 2, 1, // I
    3, 3, 3, 3, 3, 3, 3 , 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 , 1,
    1,
    1, 1, 1,
    1, 2, 2, // O
    1, 1, 1, // P
    1, 1, 4, 2, 2, 1, 1, 2,// R
    1, 4, 2, 2, 1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, // S
    2, 2, 1, 2
};

int opCode[] =
{
    0x74, 0x7c, 0xf4, 0xfc, 0xf2, 0xfa, // A
    0x34, 0x35, 0x36, 0x37, 0x68, 0x33, 0x33, 0x3b, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 0x3b, 0x39, 0x3a, 0x33, 0x31, 0x30, 0x68, 0x32, // B
    0x68, 0x68, // C
    0x68, 0x68, 0x68, 0x68, 0x68, 0x20, 0x71, 0x68, 0x68, 0x68, 0x68, 0x68, 0x68, // D
    0x68, // E
    0x68, 0x90, 0x80, // G
    0x00, 0x10, 0x69, 0x60, // I
    0xc3, 0xcb, 0xc9, 0xca, 0xc1, 0xc0, 0xc2, 0x40, 0x68, 0xf8, 0x00, 0xf0, 0x72, 0xcf, 0xcc, 0xc8, 0xc7, 0xc5, 0xc6, 0xcd, 0xce, // L
    0x79, // M
    0x38, 0xc8, 0xc4, // N
    0xf1, 0xf9, 0x61, // O
    0xb0, 0xa0, 0xc0, // P
    0x7a, 0x70, 0x68, 0x68, 0x68, 0x7e, 0x76, 0x68, // R
    0x78, 0x68, 0x68, 0x68, 0xf5, 0x75, 0x7d, 0xfd, 0xd0, 0x7b, 0xe0, 0xfe, 0x7e, 0xf6, 0x76, 0x38, 0xf7, 0x77, 0x7f, 0xff, 0x68, 0x68, 0x68, 0x68, 0x68, 0x50, 0x73, // S
    0x68, 0x68, 0xf3, 0xfb // X
};

int opCode2[] =
{
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, // A
    0x30, 0x30, 0x30, 0x30, 0x3e, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3f, 0x30, // B
    0x0d, 0x0c, // C
    0x7c, 0x74, 0xf4, 0xfc, 0x20, 0x30, 0x30, 0x76, 0x7f, 0xf7, 0x77, 0xff, 0x01, // D
    0x09,
    0x08, 0x30, 0x30, // G
    0x30, 0x30, 0x30, 0x30, // I
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x06, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, // L
    0x30, // M
    0x30, 0x30, 0x30, // N
    0x30, 0x30, 0x30, // O
    0x30, 0x30, 0xc0, // P
    0x30, 0x30, 0xc0, 0x60, 0xb0, 0x30, 0x30, 0xa0, // R
    0x30, 0x80, 0x05, 0x03, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x04, 0x02, 0x90, 0x07, 0x00, 0x30, 0x30, // S
    0x0b, 0x0a, 0x30, 0x30 // X
};

int macro[] =
{
    0, 0, 0, 0, 0, 0, // A
    0, 0, 0, 0, 0, 0, MEM_TYPE_OPCODE_BGE, MEM_TYPE_OPCODE_BL, MEM_TYPE_OPCODE_BM, 0, 0, 0, 0, 0, 0, 0, MEM_TYPE_OPCODE_BPZ, 0, 0, 0, 0, // B
    0, 0, // C
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // D
    0,
    0, 0, 0, // G
    0, 0, 0, 0, // I
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MEM_TYPE_OPCODE_LSKP, 0, 0, 0, 0, 0, // L
    0, // M
    0, 0, 0, // N
    0, 0, 0, // O
    0, 0, 0, // P
    0, 0, 0, 0, 0, MEM_TYPE_OPCODE_RSHL, MEM_TYPE_OPCODE_RSHR, 0, // R
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MEM_TYPE_OPCODE_SKP, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // S
    0, 0, 0, 0 // X
};

int minCpuType[] =
{
    CPU1802, CPU1802, SYSTEM00, CPU1801, SYSTEM00, CPU1801, // A
    SYSTEM00, SYSTEM00, SYSTEM00, SYSTEM00, CPU1804, SYSTEM00, SYSTEM00, CPU1801, CPU1801, CPU1801, CPU1801, CPU1801, CPU1801, CPU1801, CPU1802, SYSTEM00, SYSTEM00, CPU1802, SYSTEM00, CPU1804, SYSTEM00, // B
    CPU1804, CPU1804, // C
    CPU1805, CPU1805, CPU1805, CPU1805, CPU1805, SYSTEM00, CPU1801, CPU1805, CPU1805, CPU1805, CPU1805, CPU1805, CPU1804, // D
    CPU1804,
    CPU1804, SYSTEM00, SYSTEM00, // G
    SYSTEM00, SYSTEM00, SYSTEM00, SYSTEM00, // I
    CPU1802, CPU1802, CPU1802, CPU1802, CPU1802, CPU1802, CPU1802, SYSTEM00, CPU1804, CPU1801, SYSTEM00, SYSTEM00, CPU1802, CPU1802, CPU1802, CPU1802, CPU1802, CPU1802, CPU1802, CPU1802, CPU1802, // L
    CPU1802, // M
    CPU1801, CPU1802, CPU1802, // N
    SYSTEM00, CPU1801, SYSTEM00, // O
    SYSTEM00, SYSTEM00, SYSTEM00, // P
    CPU1802, SYSTEM00, CPU1804, CPU1804, CPU1804, CPU1802, CPU1802, CPU1804, // R
    SYSTEM00, CPU1804, CPU1804, CPU1804, SYSTEM00, CPU1802, CPU1802, CPU1801, SYSTEM00, CPU1802, SYSTEM00, CPU1802, CPU1802, SYSTEM00, CPU1802, CPU1801, SYSTEM00, CPU1802, CPU1802, CPU1801, CPU1804, CPU1804, CPU1804, CPU1804, CPU1804, SYSTEM00, CPU1802, // S
    CPU1804, CPU1804, SYSTEM00, CPU1801 // X
};

bool useRegister[] =
{
    false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, // C
    false, false, false, false, true, true, false, false, false, false, false, false, false, // D
    false,
    false, true, true,
    false, true, false, false,
    false, false, false, false, false, false, false, true, false, false, true, false, false, false, false, false, false, false, false, false, false,
    false,
    false, false, false,
    false, false, false,
    true, true, true,
    false, false, true, true, false, false, true, // R
    false, true, true, false, false, false, false, false, false, true, false, true, false, false, false, false, false, false, false, false, false, false, false, true, false, false, true, false,
    false, false, false, false,
};

BEGIN_EVENT_TABLE(GuiDebugger, GuiComx)

    EVT_BUTTON(XRCID("BreakPointSet"), GuiDebugger::onBreakPointSet)
    EVT_TEXT_ENTER(XRCID("BreakPointAddress"), GuiDebugger::onBreakPointSet)
    EVT_BUTTON(XRCID("TregSet"), GuiDebugger::onRegisterTrapSet)
    EVT_TEXT_ENTER(XRCID("TregValue"), GuiDebugger::onRegisterTrapSet)
    EVT_BUTTON(XRCID("TrapSet"), GuiDebugger::onInstructionTrapSet)
    EVT_TEXT_ENTER(XRCID("TrapValue"), GuiDebugger::onInstructionTrapSet)
    EVT_CHOICE(XRCID("TrapCommand"), GuiDebugger::onInstructionTrapCommand)
    EVT_BUTTON(XRCID("MemSet"), GuiDebugger::onMemoryTrapSet)

    EVT_LIST_DELETE_ITEM(XRCID("BreakPointAndTrapWindow"), GuiDebugger::deleteBreakPointAndTrap)
    EVT_LIST_END_LABEL_EDIT(XRCID("BreakPointAndTrapWindow"), GuiDebugger::editBreakPointAndTrap)
    EVT_LIST_ITEM_SELECTED(XRCID("BreakPointAndTrapWindow"), GuiDebugger::selectBreakPointAndTrap)
    EVT_LIST_ITEM_DESELECTED(XRCID("BreakPointAndTrapWindow"), GuiDebugger::deselectBreakPointAndTrap)
    EVT_LIST_KEY_DOWN(XRCID("BreakPointAndTrapWindow"), GuiDebugger::keyBreakPointAndTrap)

END_EVENT_TABLE()

GuiDebugger::GuiDebugger(const wxString& title, const wxPoint& pos, const wxSize& size, Mode mode, wxString dataDir, wxString iniDir)
: GuiComx(title, pos, size, mode, dataDir, iniDir)
{
    numberOfBreakPointsAndTraps_ = 0;
    breakPointAndTraps.clear();

#if defined(__WXMSW__)
    uncheckBitmap_ = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/unchecked.png", wxBITMAP_TYPE_PNG);
    checkedBitmap_ = wxBitmap(applicationDirectory_ + IMAGES_FOLDER + "/checked.png", wxBITMAP_TYPE_PNG);
    imageList_ = new wxImageList(13, 13, true, 2);
    checkedButton_ = imageList_->Add(checkedBitmap_, wxColour (255, 255, 255));
    uncheckButton_ = imageList_->Add(uncheckBitmap_, wxColour (255, 255, 255));
#endif
}

GuiDebugger::~GuiDebugger()
{
#if defined(__WXMSW__)
    delete imageList_;
#endif
}

void GuiDebugger::readGuiDebuggerConfig()
{
    if (!mode_.gui)
        return;

    XRCCTRL(*this, "TraceTrapButton", wxToggleButton)->SetValue(breakTrap_);

#if defined (__WXMSW__)
    breakPointAndTrapWindowPointer->SetImageList(imageList_, wxIMAGE_LIST_SMALL);
#endif
}

void GuiDebugger::cycleDebugger()
{
    wxString printBuffer;
    Byte programCounter = p_Computer->getProgramCounter();
    Word programCounterAddress = p_Computer->getScratchpadRegister(programCounter);

    if (p_Computer->getSteps() != 0 && numberOfBreakPointsAndTraps_ > 0)
    {
        for (int i=0; i<numberOfBreakPointsAndTraps_; i++)
        {
            if (breakPointAndTraps[i].addressStart == programCounterAddress && breakPointAndTraps[i].selected && breakPointAndTraps[i].breakPointAndTrapType == BPAT_TYPE_BREAK_POINT)
            {
                printBuffer.Printf("      Breakpoint at %04X", programCounterAddress);
                
//                p_Main->showInstructionTrace(programCounterAddress);
                p_Main->debugTrace(printBuffer);
                if (breakTrap_)
                {
                    p_Computer->setSteps(0);
                    p_Main->eventPauseState();
                }
                else
                {
                    showInstructionTrap_ = true;
                    showInstructionTrapAddress_ = programCounterAddress;
                }
                i = numberOfBreakPointsAndTraps_;
            }
        }
    }

    bool trapFound = false;
    Byte mask;
    Byte instruction;

    if (p_Computer->getSteps() != 0 && numberOfBreakPointsAndTraps_ > 0)
    {
        for (int i=0; i<numberOfBreakPointsAndTraps_; i++)
        {
            if (breakPointAndTraps[i].selected && breakPointAndTraps[i].breakPointAndTrapType == BPAT_TYPE_INSTRUCTION_TRAP)
            {
                mask = breakPointAndTraps[i].b7;
                instruction = p_Computer->readMemDebug(programCounterAddress);
                if (instruction == 0x60)
                    mask = 0xff;
                if (instruction == 0x68)
                {
                    if ((breakPointAndTraps[i].count == 1 && breakPointAndTraps[i].b1 == instruction) ||
                        (breakPointAndTraps[i].count == 2 && breakPointAndTraps[i].b1 == instruction &&
                                             (breakPointAndTraps[i].b2&mask) == (p_Computer->readMemDebug(programCounterAddress+1)&mask)) ||
                        (breakPointAndTraps[i].count == 3 && breakPointAndTraps[i].b1 == instruction &&
                                             (breakPointAndTraps[i].b2&mask) == (p_Computer->readMemDebug(programCounterAddress+1)&mask) &&
                                             breakPointAndTraps[i].b3 == p_Computer->readMemDebug(programCounterAddress+2)) ||
                        (breakPointAndTraps[i].count == 4 && breakPointAndTraps[i].b1 == instruction &&
                                             (breakPointAndTraps[i].b2&mask) == (p_Computer->readMemDebug(programCounterAddress+1)&mask) &&
                                             breakPointAndTraps[i].b3 == p_Computer->readMemDebug(programCounterAddress+2) &&
                                             breakPointAndTraps[i].b4 == p_Computer->readMemDebug(programCounterAddress+3)))
                    {
                        trapFound = true;
                    }
                }
                else
                {
                    if ((breakPointAndTraps[i].count == 1 && (breakPointAndTraps[i].b1&mask) == (instruction&mask)) ||
                        (breakPointAndTraps[i].count == 2 && breakPointAndTraps[i].b1 == instruction &&
                                             breakPointAndTraps[i].b2 == p_Computer->readMemDebug(programCounterAddress+1)) ||
                        (breakPointAndTraps[i].count == 3 && breakPointAndTraps[i].b1 == instruction &&
                                             breakPointAndTraps[i].b2 == p_Computer->readMemDebug(programCounterAddress+1) &&
                                             breakPointAndTraps[i].b3 == p_Computer->readMemDebug(programCounterAddress+2)))
                    {
                        trapFound = true;
                    }
                }
            }
            if (trapFound)
            {
                if (!breakTrap_)
                {
                    showInstructionTrap_ = true;
                    showInstructionTrapAddress_ = programCounterAddress;
                }
                else
                {
                    p_Computer->setSteps(0);
                    printBuffer.Printf("Instruction Trap at %04X", programCounterAddress);
                    p_Main->debugTrace(printBuffer);
                    p_Main->eventPauseState();
                    i = numberOfBreakPointsAndTraps_;
                }
            }
        }
    }

    if (p_Computer->getSteps() > 0)
    {
        if (!p_Computer->getSkipTraceMode())
            p_Computer->setSteps(p_Computer->getSteps()-1);

        if (p_Computer->getSteps() == 0)
            p_Main->eventPauseState();
    }

    if (performStep_)
    {
        if (!breakTrap_)
        {
            if (!trace_)
            {
                for (int i=0; i<numberOfBreakPointsAndTraps_; i++)
                {
                    if (breakPointAndTraps[i].selected && breakPointAndTraps[i].breakPointAndTrapType == BPAT_TYPE_INSTRUCTION_TRAP)
                    {
                        mask = breakPointAndTraps[i].b7;
                        instruction = p_Computer->readMemDebug(programCounterAddress);
                        if (instruction == 0x60)
                            mask = 0xff;
                        if (instruction == 0x68)
                        {
                            if ((breakPointAndTraps[i].count == 1 && breakPointAndTraps[i].b1 == instruction && breakPointAndTraps[i].selected) ||
                                (breakPointAndTraps[i].count == 2 && breakPointAndTraps[i].b1 == instruction &&
                                                     (breakPointAndTraps[i].b2&mask) == (p_Computer->readMemDebug(programCounterAddress+1)&mask) && breakPointAndTraps[i].selected) ||
                                (breakPointAndTraps[i].count == 3 && breakPointAndTraps[i].b1 == instruction &&
                                                     (breakPointAndTraps[i].b2&mask) == (p_Computer->readMemDebug(programCounterAddress+1)&mask) &&
                                                     breakPointAndTraps[i].b3 == p_Computer->readMemDebug(programCounterAddress+2) && breakPointAndTraps[i].selected) ||
                                (breakPointAndTraps[i].count == 4 && breakPointAndTraps[i].b1 == instruction &&
                                                     (breakPointAndTraps[i].b2&mask) == (p_Computer->readMemDebug(programCounterAddress+1)&mask) &&
                                                     breakPointAndTraps[i].b3 == p_Computer->readMemDebug(programCounterAddress+2) &&
                                                     breakPointAndTraps[i].b4 == p_Computer->readMemDebug(programCounterAddress+3) && breakPointAndTraps[i].selected))
                            {
                                trapFound = true;
                            }
                        }
                        else
                        {
                            if ((breakPointAndTraps[i].count == 1 && (breakPointAndTraps[i].b1&mask) == (instruction&mask) && breakPointAndTraps[i].selected) ||
                                (breakPointAndTraps[i].count == 2 && breakPointAndTraps[i].b1 == instruction &&
                                                     breakPointAndTraps[i].b2 == p_Computer->readMemDebug(programCounterAddress+1) && breakPointAndTraps[i].selected) ||
                                (breakPointAndTraps[i].count == 3 && breakPointAndTraps[i].b1 == instruction &&
                                                     breakPointAndTraps[i].b2 == p_Computer->readMemDebug(programCounterAddress+1) &&
                                                     breakPointAndTraps[i].b3 == p_Computer->readMemDebug(programCounterAddress+2) && breakPointAndTraps[i].selected))
                            {
                                trapFound = true;
                            }
                        }
                    }
                    if (trapFound)
                    {
                        showInstructionTrap_ = true;
                        showInstructionTrapAddress_ = programCounterAddress;
                    }
                }
            }
        }
        p_Computer->setSteps(steps_);
        performStep_ = false;
        p_Computer->forceLedUpdate();
        p_Main->eventPauseState();
    }
}

void GuiDebugger::checkRegisterTrap(Byte registerType, Word execAddress, Word value, bool noTrace)
{
    if (noTrace)
        return;
    
    for (int i=0; i<numberOfBreakPointsAndTraps_; i++)
    {
        if (breakPointAndTraps[i].selected && breakPointAndTraps[i].breakPointAndTrapType == BPAT_TYPE_REGISTER_TRAP)
        {
            if (breakPointAndTraps[i].type == registerType && breakPointAndTraps[i].value == value)
            {
                wxString valueString;
                valueString.Printf(registerTypeText[registerType*2+1], value);
                p_Main->showInstructionTrace(execAddress);
                p_Main->debugTrace("      Register write: " + registerTypeText[registerType*2] + "=" + valueString);
                if (breakTrap_)
                {
                    p_Computer->setSteps(0);
                    p_Main->eventPauseState();
                }
            }
        }
    }
}

void GuiDebugger::checkMemoryTrap(Word execAddress, Word address, Byte value, int type, bool dmaReadWrite)
{
    if (!traceDmaMt_ && dmaReadWrite)
        return;
    
    for (int i=0; i<numberOfBreakPointsAndTraps_; i++)
    {
        if (breakPointAndTraps[i].selected && breakPointAndTraps[i].breakPointAndTrapType == BPAT_TYPE_MEMORY_TRAP)
        {
            if (type == breakPointAndTraps[i].type || breakPointAndTraps[i].type == MEM_TRAP_BOTH)
            {
                if (address >= breakPointAndTraps[i].addressStart && address <= breakPointAndTraps[i].addressEnd)
                {
                    if (value == breakPointAndTraps[i].value || breakPointAndTraps[i].value == -1)
                    {
                        wxString addressString;
                        addressString.Printf("%04X", address);
                        wxString valueString;
                        valueString.Printf("%02X", value);
                        if (dmaReadWrite)
                        {
                            wxString traceText;
                            if (type == MEM_TRAP_READ)
                                traceText.Printf("----  DMA out   R0=%04X", p_Computer->getScratchpadRegister(0));
                            else
                                traceText.Printf("----  DMA in    R0=%04X", p_Computer->getScratchpadRegister(0));
                            p_Main->debugTrace(traceText);
                        }
                        else
                            p_Main->showInstructionTrace(execAddress);
                        if (type == MEM_TRAP_READ)
                            p_Main->debugTrace("      Memory read: D=M(" + addressString + ")=" + valueString);
                        else
                            p_Main->debugTrace("      Memory write: M(" + addressString + ")=" + valueString);
                        if (breakTrap_)
                        {
                            p_Computer->setSteps(0);
                            p_Main->eventPauseState();
                        }
                    }
                }
            }
        }
    }
}

void GuiDebugger::deleteBreakPointAndTrap(wxListEvent&event)
{
    long selectedItem = event.GetIndex();

    if (selectedItem > -1)
    {
        breakPointAndTraps.erase(breakPointAndTraps.begin() + selectedItem);
        numberOfBreakPointsAndTraps_--;
        p_Main->SetDebugMode();
        breakPointAndTrapWindowPointer->Refresh();
    }
}

void GuiDebugger::editBreakPointAndTrap(wxListEvent&event)
{
    wxString fullStringValue = event.GetText();
    fullStringValue = fullStringValue.MakeUpper();
    long selectedItem = event.GetIndex();

    if (fullStringValue == "")
    {
        breakPointAndTrapWindowPointer->DeleteItem(selectedItem);
        event.Veto();
        return;
    }

    wxString type = p_Main->extractWord(&fullStringValue);
    fullStringValue.Trim(false);
    
    if (type == "BP")
        if (!editBreakPoint(selectedItem, fullStringValue))
            event.Veto();

    if (type == "RT")
        if (!editRegisterTrap(selectedItem, fullStringValue))
            event.Veto();

    if (type == "IT")
        if (!editInstructionTrap(selectedItem, fullStringValue))
            event.Veto();

    if (type == "MT")
        if (!editMemoryTrap(selectedItem, fullStringValue))
            event.Veto();

    breakPointAndTrapWindowPointer->Refresh();
}

void GuiDebugger::switchBreakPointAndTrap(int item)
{
    breakPointAndTraps[item].selected = !breakPointAndTraps[item].selected ;
    if (breakPointAndTraps[item].selected)
        breakPointAndTrapWindowPointer->SetItemImage(item, checkedButton_, -1);
    else
        breakPointAndTrapWindowPointer->SetItemImage(item, uncheckButton_, -1);
}

void GuiDebugger::selectBreakPointAndTrap(wxListEvent&event)
{
    selectedBreakPointAndTrap_ = (int)event.GetIndex();
}

void GuiDebugger::deselectBreakPointAndTrap(wxListEvent&WXUNUSED(event))
{
    selectedBreakPointAndTrap_ = -1;
}

void GuiDebugger::keyBreakPointAndTrap(wxListEvent&event)
{
    const int keyCode = event.GetKeyCode();
    if (keyCode == WXK_DELETE && selectedBreakPointAndTrap_ != -1)
        breakPointAndTrapWindowPointer->DeleteItem(selectedBreakPointAndTrap_);
}

void GuiDebugger::onBreakPointSet(wxCommandEvent&WXUNUSED(event))
{
    BreakPointAndTrap breakPoint;
    
    long breakPointAddress = get16BitValue("BreakPointAddress");
    if (breakPointAddress == -1)
    {
        (void)wxMessageBox( "No Break Point value specified\n",
                                    "Emma 02", wxICON_ERROR | wxOK );
        return;
    }

    breakPoint.addressStart = breakPointAddress;
    breakPoint.selected = true;
    breakPoint.breakPointAndTrapType = BPAT_TYPE_BREAK_POINT;
    
    breakPointAndTraps.push_back(breakPoint);
    
    addBreakPoint();
    p_Main->SetDebugMode();
}

void GuiDebugger::addBreakPoint()
{
    wxString printBuffer;

    printBuffer.Printf("BP %04X", breakPointAndTraps[numberOfBreakPointsAndTraps_].addressStart);
    breakPointAndTrapWindowPointer->InsertItem(numberOfBreakPointsAndTraps_, printBuffer);
    numberOfBreakPointsAndTraps_++;
}

bool GuiDebugger::editBreakPoint(long selectedItem, wxString fullStringValue)
{
    wxString strValue = p_Main->extractWord(&fullStringValue);
    long value = get16BitFromHexString(strValue, "value");
    if (value == -1)
        return false;

    breakPointAndTraps[selectedItem].addressStart = value;
    breakPointAndTraps[selectedItem].breakPointAndTrapType = BPAT_TYPE_BREAK_POINT;
    return true;
}

void GuiDebugger::onRegisterTrapSet(wxCommandEvent&WXUNUSED(event))
{
    BreakPointAndTrap registerTrap;

    Byte registerType = XRCCTRL(*this,"TregRegister",wxChoice)->GetCurrentSelection();

    long registerValue = get16BitValue("TregValue");
    if (registerValue == -1)
    {
        (void)wxMessageBox( "No Register Value value specified\n",
                                    "Emma 02", wxICON_ERROR | wxOK );
        return;
    }

    if (registerValue > registerSize[registerType])
    {
        (void)wxMessageBox( "Please specify value of "+ registerBits[registerType] + " bit max\n",
                                    "Emma 02", wxICON_ERROR | wxOK );
        return;
    }

    registerTrap.type = registerType;
    registerTrap.value = (int)registerValue;
    registerTrap.selected = true;
    registerTrap.breakPointAndTrapType = BPAT_TYPE_REGISTER_TRAP;

    breakPointAndTraps.push_back(registerTrap);

    addRegisterTrap();
    p_Main->SetDebugMode();
}

void GuiDebugger::addRegisterTrap()
{
    wxString printBuffer;

    printBuffer.Printf("RT " + registerTypeText[breakPointAndTraps[numberOfBreakPointsAndTraps_].type*2] + " " + registerTypeText[breakPointAndTraps[numberOfBreakPointsAndTraps_].type*2+1], breakPointAndTraps[numberOfBreakPointsAndTraps_].value);
    breakPointAndTrapWindowPointer->InsertItem(numberOfBreakPointsAndTraps_, printBuffer);
    numberOfBreakPointsAndTraps_++;
}

bool GuiDebugger::editRegisterTrap(long selectedItem, wxString fullStringValue)
{
    wxString registerStr = p_Main->extractWord(&fullStringValue);
    Byte registerType = getRegister(registerStr);

    if (registerType != TREG_FAULT)
    {
        wxString strValue = p_Main->extractWord(&fullStringValue);
        long registerValue = get16BitFromHexString(strValue, "address");
        if (registerValue == -1)
            return false;

        if (registerValue > registerSize[registerType])
        {
            (void)wxMessageBox( "Please specify value of "+ registerBits[registerType] + " bit max\n",
                                        "Emma 02", wxICON_ERROR | wxOK );
            return false;
        }

        breakPointAndTraps[selectedItem].type = registerType;
        breakPointAndTraps[selectedItem].value = (int)registerValue;
        breakPointAndTraps[selectedItem].breakPointAndTrapType = BPAT_TYPE_REGISTER_TRAP;
        return true;
    }
    else
    {
        (void)wxMessageBox( "Please specify R0 to RF, D, P, X, T, B, CH, CN, CNTR, DF or Q\n",
                                    "Emma 02", wxICON_ERROR | wxOK );
        return false;
    }
}

int GuiDebugger::getRegister(wxString buffer)
{
    if (buffer == "D")  return TREG_D;
    if (buffer == "DF")  return TREG_DF;
    if (buffer == "P")  return TREG_P;
    if (buffer == "X")  return TREG_X;
    if (buffer == "T")  return TREG_T;
    if (buffer == "B")  return TREG_B;
    if (buffer == "CH")  return TREG_CH;
    if (buffer == "CN")  return TREG_CNTR;
    if (buffer == "CNTR")  return TREG_CNTR;
    if (buffer == "Q")  return TREG_Q;
    if (buffer == "R0")  return TREG_R0;
    if (buffer == "R1")  return TREG_R1;
    if (buffer == "R2")  return TREG_R2;
    if (buffer == "R3")  return TREG_R3;
    if (buffer == "R4")  return TREG_R4;
    if (buffer == "R5")  return TREG_R5;
    if (buffer == "R6")  return TREG_R6;
    if (buffer == "R7")  return TREG_R7;
    if (buffer == "R8")  return TREG_R8;
    if (buffer == "R9")  return TREG_R9;
    if (buffer == "RA")  return TREG_RA;
    if (buffer == "RB")  return TREG_RB;
    if (buffer == "RC")  return TREG_RC;
    if (buffer == "RD")  return TREG_RD;
    if (buffer == "RE")  return TREG_RE;
    if (buffer == "RF")  return TREG_RF;
    return TREG_FAULT;
}

void GuiDebugger::onInstructionTrapSet(wxCommandEvent&WXUNUSED(event))
{
    BreakPointAndTrap instructionTrap;

    int command = XRCCTRL(*this,"TrapCommand",wxChoice)->GetCurrentSelection();
    wxString strValue = XRCCTRL(*this,"TrapValue",wxTextCtrl)->GetValue(), errorCode;
    long trapValue;

    instructionTrap.b7 = 0xff;
    instructionTrap.count = numberOfBytes[command];
    instructionTrap.b1 = opCode[command];
    instructionTrap.typeOpcode = macro[command];

    if (opCode[command] == 0x68)
    {
        if (cpuType_ >= minCpuType[command])
        {
            instructionTrap.b2 = opCode2[command];
            if (useRegister[command])
            {
                int registerValue = XRCCTRL(*this,"TrapRegister",wxChoice)->GetCurrentSelection()-1;
                if ((registerValue >= 0) && (registerValue <= 0xf))
                {
                    instructionTrap.b2 |= registerValue;
                }
                else
                {
                    instructionTrap.b7 = 0xf0;
                }
            }
            if (instructionTrap.count == 3)
            {
                strValue = strValue.MakeUpper();
                if (strValue == "X")
                {
                    instructionTrap.count = 2;
                }
                else
                {
                    trapValue = get8BitFromHexString(strValue, "value");
                    if (trapValue == -1)
                        return;
                    instructionTrap.b3 = trapValue;
                }
            }
            if (instructionTrap.count == 4)
            {
                strValue = strValue.MakeUpper();
                if (strValue == "X")
                {
                    instructionTrap.count = 2;
                }
                else
                {
                    trapValue = get16BitFromHexString(strValue, "value");
                    if (trapValue == -1)
                        return;
                    instructionTrap.b3 = (trapValue >> 8) & 0xff;
                    instructionTrap.b4 = trapValue & 0xff;
                }
            }
        }
        else
        {
            if (cpuType_ <= CPU1801)
                errorCode = "Not supported on CDP1801\n";
            if (cpuType_ == CPU1802)
                errorCode = "Not supported on CDP1802\n";
            if (cpuType_ == CPU1804)
                errorCode = "Not supported on CDP1804\n";
            (void)wxMessageBox( errorCode,
                                        "Emma 02", wxICON_ERROR | wxOK );
            return;
        }
    }
    else
    {
        if (cpuType_ < minCpuType[command])
        {
            errorCode = "Not supported on CDP1801\n";
            (void)wxMessageBox( errorCode,
                               "Emma 02", wxICON_ERROR | wxOK );
            return;
        }
        
        if (useRegister[command])
        {
            int registerValue = XRCCTRL(*this,"TrapRegister",wxChoice)->GetCurrentSelection()-1;
            if ((registerValue >= 0) && (registerValue <= 0xf))
            {
                instructionTrap.b1 |= registerValue;
            }
            else
            {
                instructionTrap.b7 = 0xf0;
            }
        }
        if (opCode[command] == 0x61 || opCode[command] == 0x69)
        {
            strValue = strValue.MakeUpper();
            instructionTrap.count = 1;
            if (strValue == "X")
            {
                instructionTrap.b7 = 0xf8;
            }
            else
            {
                if (!strValue.ToLong(&trapValue))
                {
                    (void)wxMessageBox( "Please specify value 0 to 7\n",
                                                "Emma 02", wxICON_ERROR | wxOK );
                    return;
                }
                if ((trapValue >= 0) && (trapValue <= 7))
                {
                    instructionTrap.b1 = instructionTrap.b1 + trapValue - 1;
                }
                else
                {
                    (void)wxMessageBox( "Please specify value 0 to 7\n",
                                                "Emma 02", wxICON_ERROR | wxOK );
                    return;
                }
            }
        }
        if (instructionTrap.count == 2)
        {
            strValue = strValue.MakeUpper();
            if (strValue == "X")
            {
                instructionTrap.count = 1;
            }
            else
            {
                trapValue = get8BitFromHexString(strValue, "value");
                if (trapValue == -1)
                    return;
                instructionTrap.b2 = trapValue;
            }
        }
        if (instructionTrap.count == 3)
        {
            strValue = strValue.MakeUpper();
            if (strValue == "X")
            {
                instructionTrap.count = 1;
            }
            else
            {
                trapValue = get16BitFromHexString(strValue, "value");
                if (trapValue == -1)
                    return;
                instructionTrap.b2 = (trapValue >> 8) & 0xff;
                instructionTrap.b3 = trapValue & 0xff;
            }
        }
    }
    
    instructionTrap.selected = true;
    instructionTrap.breakPointAndTrapType = BPAT_TYPE_INSTRUCTION_TRAP;

    breakPointAndTraps.push_back(instructionTrap);

    p_Main->addInstructionTrap();
    p_Main->SetDebugMode();
}

void GuiDebugger::onInstructionTrapCommand(wxCommandEvent&event)
{
    int command = event.GetSelection();
    wxString value = XRCCTRL(*this, "TrapValue", wxTextCtrl)->GetValue();

    if (opCode2[command] == 0x30)
    {
        XRCCTRL(*this, "TrapValue", wxTextCtrl)->Enable(numberOfBytes[command] > 1);
        if (numberOfBytes[command] == 2)
        {
            XRCCTRL(*this, "TrapValue", wxTextCtrl)->SetMaxLength(2);
            if (value != "")
                XRCCTRL(*this, "TrapValue", wxTextCtrl)->SetValue(value.Right(2));
        }
        if (numberOfBytes[command] == 3)
            XRCCTRL(*this, "TrapValue", wxTextCtrl)->SetMaxLength(4);
    }
    else
    {
        XRCCTRL(*this, "TrapValue", wxTextCtrl)->Enable(numberOfBytes[command] > 2);
        if (numberOfBytes[command] == 3)
        {
            XRCCTRL(*this, "TrapValue", wxTextCtrl)->SetMaxLength(2);
            if (value != "")
                XRCCTRL(*this, "TrapValue", wxTextCtrl)->SetValue(value.Right(2));
        }
        if (numberOfBytes[command] == 4)
            XRCCTRL(*this, "TrapValue", wxTextCtrl)->SetMaxLength(4);
    }

    XRCCTRL(*this, "TrapRegister", wxChoice)->Enable(useRegister[command]);
}

void GuiDebugger::addInstructionTrap()
{
    wxString printBuffer, printBuffer2;

    printBuffer = "";
    printBuffer2 = "";

    int n = breakPointAndTraps[numberOfBreakPointsAndTraps_].b1 & 0xf, n1805;
    int inst = breakPointAndTraps[numberOfBreakPointsAndTraps_].b1 >> 4, i1805;
    switch(inst)
    {
        case 0x0:
            switch(n)
            {
                case 0x0:
                    printBuffer.operator += ("IDL");
                break;

                default:
                    if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                        printBuffer.Printf("LDN  Rx");
                    else
                        printBuffer.Printf("LDN  R%X",n);
                break;
            }
        break;

        case 0x1:
            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                printBuffer.Printf("INC  Rx");
            else
                printBuffer.Printf("INC  R%X",n);
        break;

        case 0x2:
            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                printBuffer.Printf("DEC  Rx");
            else
                printBuffer.Printf("DEC  R%X",n);
        break;

        case 0x3:
            printBuffer2.Printf("%02X", breakPointAndTraps[numberOfBreakPointsAndTraps_].b2);
            switch(n)
            {
                case 0x0:
                    printBuffer.Printf("BR   ");
                break;
                case 0x1:
                    if (cpuType_ == SYSTEM00)
                        printBuffer.Printf("BNZ  ");
                    else
                        printBuffer.Printf("BQ   ");
                break;
                case 0x2:
                    printBuffer.Printf("BZ   ");
                break;
                case 0x3:
                    if (breakPointAndTraps[numberOfBreakPointsAndTraps_].typeOpcode == MEM_TYPE_OPCODE_BGE)
                        printBuffer.Printf("BGE  ");
                    else
                    {
                        if (breakPointAndTraps[numberOfBreakPointsAndTraps_].typeOpcode == MEM_TYPE_OPCODE_BPZ)
                            printBuffer.Printf("BPZ  ");
                        else
                            printBuffer.Printf("BDF  ");
                    }
                break;
                case 0x4:
                    printBuffer.Printf("B1   ");
                break;
                case 0x5:
                    printBuffer.Printf("B2   ");
                break;
                case 0x6:
                    printBuffer.Printf("B3   ");
                break;
                case 0x7:
                    printBuffer.Printf("B4   ");
                break;
                case 0x8:
                    if (breakPointAndTraps[numberOfBreakPointsAndTraps_].typeOpcode == MEM_TYPE_OPCODE_SKP)
                        printBuffer.operator += ("SKP");
                    else
                        printBuffer.operator += ("NBR");
                break;
                case 0x9:
                    printBuffer.Printf("BNQ  ");
                break;
                case 0xa:
                    printBuffer.Printf("BNZ  ");
                break;
                case 0xb:
                    if (breakPointAndTraps[numberOfBreakPointsAndTraps_].typeOpcode == MEM_TYPE_OPCODE_BL)
                        printBuffer.Printf("BL   ");
                    else
                    {
                        if (breakPointAndTraps[numberOfBreakPointsAndTraps_].typeOpcode == MEM_TYPE_OPCODE_BM)
                            printBuffer.Printf("BM   ");
                        else
                            printBuffer.Printf("BNF  ");
                    }
                break;
                case 0xc:
                    printBuffer.Printf("BN1  ");
                break;
                case 0xd:
                    printBuffer.Printf("BN2  ");
                break;
                case 0xe:
                    printBuffer.Printf("BN3  ");
                break;
                case 0xf:
                    printBuffer.Printf("BN4  ");
                break;
            }
            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].count == 2)
                printBuffer.operator += (printBuffer2);
            else
            {
                if (printBuffer.Len() == 5)
                    printBuffer.operator += ("x");
            }
        break;

        case 0x4:
            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                printBuffer.Printf("LDA  Rx");
            else
                printBuffer.Printf("LDA  R%X",n);
        break;

        case 0x5:
            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                printBuffer.Printf("STR  Rx");
            else
                printBuffer.Printf("STR  R%X",n);
        break;

        case 0x6:
            switch(n)
            {
                case 0x0:
                    printBuffer.operator += ("IRX");
                break;
                case 0x1:
                case 0x2:
                case 0x3:
                case 0x4:
                case 0x5:
                case 0x6:
                case 0x7:
                    if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf8)
                        printBuffer.Printf("OUT  x");
                    else
                        printBuffer.Printf("OUT  %X",n);
                break;
                case 0x8:
                    if (cpuType_ <= CPU1801)
                        printBuffer.Printf("INP  %X",n-8);
                    else
                    {
                    n1805 = breakPointAndTraps[numberOfBreakPointsAndTraps_].b2 & 0xf;
                    i1805 = breakPointAndTraps[numberOfBreakPointsAndTraps_].b2 >> 4;
                    switch(i1805)
                    {
                        case 0:
                            switch(n1805)
                            {
                                case 0:
                                    printBuffer.Printf("STPC");
                                break;
                                case 1:
                                    printBuffer.Printf("DTC");
                                break;
                                case 2:
                                    printBuffer.Printf("SPM2");
                                break;
                                case 3:
                                    printBuffer.Printf("SCM2");
                                break;
                                case 4:
                                    printBuffer.Printf("SPM1");
                                break;
                                case 5:
                                    printBuffer.Printf("SCM1");
                                break;
                                case 6:
                                    printBuffer.Printf("LDC");
                                break;
                                case 7:
                                    printBuffer.Printf("STM");
                                break;
                                case 8:
                                    printBuffer.Printf("GEC");
                                break;
                                case 9:
                                    printBuffer.Printf("ETQ");
                                break;
                                case 0xa:
                                    printBuffer.Printf("XIE");
                                break;
                                case 0xb:
                                    printBuffer.Printf("XID");
                                break;
                                case 0xc:
                                    printBuffer.Printf("CIE");
                                break;
                                case 0xd:
                                    printBuffer.Printf("CID");
                                break;
                            }
                        break;
                        case 2:
                            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].count == 2)
                            {
                                if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                                    printBuffer.Printf("DBNZ Rx,x");
                                else
                                    printBuffer.Printf("DBNZ R%X,x", n1805);
                            }
                            else
                            {
                                if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                                    printBuffer.Printf("DBNZ Rx,%04X", breakPointAndTraps[numberOfBreakPointsAndTraps_].b3<<8|breakPointAndTraps[numberOfBreakPointsAndTraps_].b4);
                                else
                                    printBuffer.Printf("DBNZ R%X,%04X", n1805, breakPointAndTraps[numberOfBreakPointsAndTraps_].b3<<8|breakPointAndTraps[numberOfBreakPointsAndTraps_].b4);
                            }
                        break;
                        case 3:
                            printBuffer2.Printf("%02X", breakPointAndTraps[numberOfBreakPointsAndTraps_].b3);
                            switch(n1805)
                            {
                                case 0xe:
                                    printBuffer.Printf("BCI  ");
                                break;
                                case 0xf:
                                    printBuffer.Printf("BXI  ");
                                break;
                            }
                            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].count == 3)
                                printBuffer.operator += (printBuffer2);
                            else
                            {
                                printBuffer.operator += ("x");
                            }
                        break;
                        case 6:
                            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                                printBuffer.Printf("RLXA Rx");
                            else
                                printBuffer.Printf("RLXA R%X", n1805);
                        break;
                        case 7:
                            printBuffer2.Printf("%02X", breakPointAndTraps[numberOfBreakPointsAndTraps_].b3);
                            switch(n1805)
                            {
                                case 0x4:
                                    printBuffer.Printf("DADC");
                                break;
                                case 0x6:
                                    printBuffer.Printf("DSAV");
                                break;
                                case 0x7:
                                    printBuffer.Printf("DSMB");
                                break;
                                case 0xc:
                                    printBuffer.Printf("DACI ");
                                break;
                                case 0xf:
                                    printBuffer.Printf("DSBI ");
                                break;
                            }
                            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].count == 3)
                                printBuffer.operator += (printBuffer2);
                            else
                            {
                                if (printBuffer.Len() == 5)
                                    printBuffer.operator += ("x");
                            }
                        break;
                        case 8:
                            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].count == 2)
                            {
                                if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                                    printBuffer.Printf("SCAL Rx,x");
                                else
                                    printBuffer.Printf("SCAL R%X,x", n1805);
                            }
                            else
                            {
                                if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                                    printBuffer.Printf("SCAL Rx,%04X", breakPointAndTraps[numberOfBreakPointsAndTraps_].b3<<8|breakPointAndTraps[numberOfBreakPointsAndTraps_].b4);
                                else
                                    printBuffer.Printf("SCAL R%X,%04X", n1805, breakPointAndTraps[numberOfBreakPointsAndTraps_].b3<<8|breakPointAndTraps[numberOfBreakPointsAndTraps_].b4);
                            }
                        break;
                        case 9:
                            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                                printBuffer.Printf("SRET Rx");
                            else
                                printBuffer.Printf("SRET R%X", n1805);
                        break;
                        case 0xa:
                            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                                printBuffer.Printf("RSXD Rx");
                            else
                                printBuffer.Printf("RSXD R%X", n1805);
                        break;
                        case 0xb:
                            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                                printBuffer.Printf("RNX  Rx");
                            else
                                printBuffer.Printf("RNX  R%X", n1805);
                        break;
                        case 0xc:
                            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].count == 2)
                            {
                                if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                                    printBuffer.Printf("RLDI Rx,x");
                                else
                                    printBuffer.Printf("RLDI R%X,x", n1805);
                            }
                            else
                            {
                                if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                                    printBuffer.Printf("RLDI Rx,%04X", breakPointAndTraps[numberOfBreakPointsAndTraps_].b3<<8|breakPointAndTraps[numberOfBreakPointsAndTraps_].b4);
                                else
                                    printBuffer.Printf("RLDI R%X,%04X", n1805, breakPointAndTraps[numberOfBreakPointsAndTraps_].b3<<8|breakPointAndTraps[numberOfBreakPointsAndTraps_].b4);
                            }
                        break;
                        case 0xf:
                            printBuffer2.Printf("%02X", breakPointAndTraps[numberOfBreakPointsAndTraps_].b3);
                            switch(n1805)
                            {
                                case 0x4:
                                    printBuffer.Printf("DADD");
                                break;
                                case 0x7:
                                    printBuffer.Printf("DSM");
                                break;
                                case 0xc:
                                    printBuffer.Printf("DADI ");
                                break;
                                case 0xf:
                                    printBuffer.Printf("DSMI ");
                                break;
                            }
                            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].count == 3)
                                printBuffer.operator += (printBuffer2);
                            else
                            {
                                if (printBuffer.Len() == 5)
                                    printBuffer.operator += ("x");
                            }
                        break;

                    }
                    }
                break;
                case 0x9:
                case 0xa:
                case 0xb:
                case 0xc:
                case 0xd:
                case 0xe:
                case 0xf:
                    if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf8)
                        printBuffer.Printf("INP  x");
                    else
                        printBuffer.Printf("INP  %X",n-8);
                break;
            }
        break;
        case 0x7:
            printBuffer2.Printf("%02X", breakPointAndTraps[numberOfBreakPointsAndTraps_].b2);
            switch(n)
            {
                case 0x0:
                    printBuffer.operator += ("RET");
                break;
                case 0x1:
                    printBuffer.operator += ("DIS");
                break;
                case 0x2:
                    printBuffer.operator += ("LDXA");
                break;
                case 0x3:
                    printBuffer.operator += ("STXD");
                break;
                case 0x4:
                    printBuffer.operator += ("ADC");
                break;
                case 0x5:
                    printBuffer.operator += ("SDB");
                break;
                case 0x6:
                    if (breakPointAndTraps[numberOfBreakPointsAndTraps_].typeOpcode == MEM_TYPE_OPCODE_RSHR)
                        printBuffer.operator += ("RSHR");
                    else
                        printBuffer.operator += ("SHRC");
                break;
                case 0x7:
                    printBuffer.operator += ("SMB");
                break;
                case 0x8:
                    printBuffer.operator += ("SAV");
                break;
                case 0x9:
                    printBuffer.operator += ("MARK");
                break;
                case 0xa:
                    printBuffer.operator += ("REQ");
                break;
                case 0xb:
                    printBuffer.operator += ("SEQ");
                break;
                case 0xe:
                    if (breakPointAndTraps[numberOfBreakPointsAndTraps_].typeOpcode == MEM_TYPE_OPCODE_RSHL)
                        printBuffer.operator += ("RSHL");
                    else
                        printBuffer.operator += ("SHLC");
                break;
                case 0xc:
                    printBuffer.Printf("ADCI ");
                break;
                case 0xd:
                    printBuffer.Printf("SDBI ");
                break;
                case 0xf:
                    printBuffer.Printf("SMBI ");
                break;
            }
            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].count == 2)
                printBuffer.operator += (printBuffer2);
            else
            {
                if (printBuffer.Len() == 5)
                    printBuffer.operator += ("x");
            }
        break;
        case 0x8:
            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                printBuffer.Printf("GLO  Rx");
            else
                printBuffer.Printf("GLO  R%X",n);
        break;
        case 0x9:
            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                printBuffer.Printf("GHI  Rx");
            else
                printBuffer.Printf("GHI  R%X",n);
        break;
        case 0xa:
            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                printBuffer.Printf("PLO  Rx");
            else
                printBuffer.Printf("PLO  R%X",n);
        break;
        case 0xb:
            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                printBuffer.Printf("PHI  Rx");
            else
                printBuffer.Printf("PHI  R%X",n);
        break;
        case 0xc:
            if (cpuType_ == SYSTEM00)
            {
                if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                    printBuffer.Printf("PNI  Rx");
                else
                    printBuffer.Printf("PNI  R%X",n);
                break;
            }
            printBuffer2.Printf("%04X", breakPointAndTraps[numberOfBreakPointsAndTraps_].b2<<8|breakPointAndTraps[numberOfBreakPointsAndTraps_].b3);
            switch(n)
            {
                case 0x0:
                    printBuffer.Printf("LBR  ");
                break;
                case 0x1:
                    printBuffer.Printf("LBQ  ");
                break;
                case 0x2:
                    printBuffer.Printf("LBZ  ");
                break;
                case 0x3:
                    printBuffer.Printf("LBDF ");
                break;
                case 0x4:
                    printBuffer.operator += ("NOP");
                break;
                case 0x5:
                    printBuffer.operator += ("LSNQ");
                break;
                case 0x6:
                    printBuffer.operator += ("LSNZ");
                break;
                case 0x7:
                    printBuffer.operator += ("LSNF");
                break;
                case 0x8:
                    if (breakPointAndTraps[numberOfBreakPointsAndTraps_].typeOpcode == MEM_TYPE_OPCODE_LSKP)
                        printBuffer.operator += ("LSKP");
                    else
                        printBuffer.operator += ("NLBR");
                break;
                case 0x9:
                    printBuffer.Printf("LBNQ ");
                break;
                case 0xa:
                    printBuffer.Printf("LBNZ ");
                break;
                case 0xb:
                    printBuffer.Printf("LBNF ");
                break;
                case 0xc:
                    printBuffer.operator += ("LSIE");
                break;
                case 0xd:
                    printBuffer.operator += ("LSQ");
                break;
                case 0xe:
                    printBuffer.operator += ("LSZ");
                break;
                case 0xf:
                    printBuffer.operator += ("LSDF");
                break;
            }
            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].count == 3)
                printBuffer.operator += (printBuffer2);
            else
            {
                if (printBuffer.Len() == 5)
                    printBuffer.operator += ("x");
            }
        break;
        case 0xd:
            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                printBuffer.Printf("SEP  Rx");
            else
                printBuffer.Printf("SEP  R%X",n);
        break;
        case 0xe:
            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].b7 == 0xf0)
                printBuffer.Printf("SEX  Rx");
            else
                printBuffer.Printf("SEX  R%X",n);
        break;
        case 0xf:
            printBuffer2.Printf("%02X", breakPointAndTraps[numberOfBreakPointsAndTraps_].b2);
            switch(n)
            {
                case 0x0:
                    printBuffer.operator += ("LDX");
                break;
                case 0x1:
                    printBuffer.operator += ("OR");
                break;
                case 0x2:
                    printBuffer.operator += ("AND");
                break;
                case 0x3:
                    printBuffer.operator += ("XOR");
                break;
                case 0x4:
                    printBuffer.operator += ("ADD");
                break;
                case 0x5:
                    printBuffer.operator += ("SD");
                break;
                case 0x6:
                    printBuffer.operator += ("SHR");
                break;
                case 0x7:
                    printBuffer.operator += ("SM");
                break;
                case 0x8:
                    printBuffer.Printf("LDI  ");
                break;
                case 0x9:
                    printBuffer.Printf("ORI  ");
                break;
                case 0xa:
                    printBuffer.Printf("ANI  ");
                break;
                case 0xb:
                    printBuffer.Printf("XRI  ");
                break;
                case 0xc:
                    printBuffer.Printf("ADI  ");
                break;
                case 0xd:
                    printBuffer.Printf("SDI  ");
                break;
                case 0xe:
                    printBuffer.operator += ("SHL");
                break;
                case 0xf:
                    printBuffer.Printf("SMI  ");
                break;
            }
            if (breakPointAndTraps[numberOfBreakPointsAndTraps_].count == 2)
                printBuffer.operator += (printBuffer2);
            else
            {
                if (printBuffer.Len() == 5)
                    printBuffer.operator += ("x");
            }
        break;
    }
    breakPointAndTrapWindowPointer->InsertItem(numberOfBreakPointsAndTraps_, printBuffer);
    numberOfBreakPointsAndTraps_++;
}

bool GuiDebugger::editInstructionTrap(long selectedItem, wxString fullStringValue)
{
    Word count;
    Byte b1, b2, b3, b4, b5, b6, b7;
    int typeOpcode=0;

    count = p_Main->assemble(&fullStringValue, &b1, &b2, &b3, &b4, &b5, &b6, &b7, true);

    if (count >10 && count <15)
        count = ASS_ERROR_INST;

    if (count == 21)
        count = ASS_ERROR_INST;

    if (count >= MEM_TYPE_OPCODE_RSHR)
    {
        switch (count)
        {
            case MEM_TYPE_OPCODE_RSHR:
                typeOpcode = MEM_TYPE_OPCODE_RSHR;
                count = 1;
            break;
            case MEM_TYPE_OPCODE_RSHL:
                typeOpcode = MEM_TYPE_OPCODE_RSHL;
                count = 1;
            break;
            case MEM_TYPE_OPCODE_BPZ:
                typeOpcode = MEM_TYPE_OPCODE_BPZ;
                count = 2;
            break;
            case MEM_TYPE_OPCODE_BGE:
                typeOpcode = MEM_TYPE_OPCODE_BGE;
                count = 2;
            break;
            case MEM_TYPE_OPCODE_BM:
                typeOpcode = MEM_TYPE_OPCODE_BM;
                count = 2;
            break;
            case MEM_TYPE_OPCODE_BL:
                typeOpcode = MEM_TYPE_OPCODE_BL;
                count = 2;
            break;
            case MEM_TYPE_OPCODE_LSKP:
                typeOpcode = MEM_TYPE_OPCODE_LSKP;
                count = 1;
            break;
            case MEM_TYPE_OPCODE_SKP:
                typeOpcode = MEM_TYPE_OPCODE_SKP;
                count = 1;
            break;
            case MEM_TYPE_OPCODE_RLDL:
            case MEM_TYPE_OPCODE_LDV:
            case MEM_TYPE_OPCODE_LDL:
            case MEM_TYPE_OPCODE_LDL_SLOT:
            case MEM_TYPE_OPCODE_LDRL:
            case MEM_TYPE_OPCODE_LDRL_SLOT:
            case MEM_TYPE_OPCODE_LBR_SLOT:
            case MEM_TYPE_OPCODE_JUMP_SLOT:
                count = ASS_ERROR_INST;
            break;
        }
    }
    if (count > 0 && count < 7)
    {
        breakPointAndTraps[selectedItem].count = count;
        breakPointAndTraps[selectedItem].b1 = b1;
        breakPointAndTraps[selectedItem].b2 = b2;
        breakPointAndTraps[selectedItem].b3 = b3;
        breakPointAndTraps[selectedItem].b4 = b4;
        breakPointAndTraps[selectedItem].b5 = b5;
        breakPointAndTraps[selectedItem].b6 = b6;
        breakPointAndTraps[selectedItem].b7 = b7;
        breakPointAndTraps[selectedItem].typeOpcode = typeOpcode;
        breakPointAndTraps[selectedItem].breakPointAndTrapType = BPAT_TYPE_INSTRUCTION_TRAP;
        return true;
    }
    else
    {
        (void)wxMessageBox( disAssErrorCodes[count-ASS_ERROR_START-1]+"\n",
                                    "Emma 02", wxICON_ERROR | wxOK );
        return false;
    }
}

void GuiDebugger::onMemoryTrapSet(wxCommandEvent&WXUNUSED(event))
{
    BreakPointAndTrap memoryTrap;

    Byte memoryType = XRCCTRL(*this,"TmemType",wxChoice)->GetCurrentSelection();
    
    wxString hexNumberString = XRCCTRL(*this,"TmemAddressStart",wxTextCtrl)->GetValue();
    if (hexNumberString == "")
    {
        (void)wxMessageBox( "Please specify a Memory Trap start address\n",
                                    "Emma 02", wxICON_ERROR | wxOK );
        return;
    }

    long hexNumberLong;
    hexNumberString.ToLong(&hexNumberLong, 16);
    Word startAddress = (Word) hexNumberLong;
    
    Word endAddress;
    hexNumberString = XRCCTRL(*this,"TmemAddressEnd",wxTextCtrl)->GetValue();
    if (hexNumberString == "")
        endAddress = startAddress;
    else
    {
        hexNumberString.ToLong(&hexNumberLong, 16);
        endAddress = (Word) hexNumberLong;
        if (endAddress < startAddress)
        {
            (void)wxMessageBox( "Please specify end address >= start address\n",
                                        "Emma 02", wxICON_ERROR | wxOK );
            return;
        }
    }
    
    int value;
    hexNumberString = XRCCTRL(*this,"TmemValue",wxTextCtrl)->GetValue();
    if (hexNumberString == "")
        value = -1;
    else
    {
        hexNumberString.ToLong(&hexNumberLong, 16);
        value = (Byte) hexNumberLong;
    }
        
    memoryTrap.type = memoryType;
    memoryTrap.addressStart = startAddress;
    memoryTrap.addressEnd = endAddress;
    memoryTrap.value = (int) value;
    memoryTrap.selected = true;
    memoryTrap.breakPointAndTrapType = BPAT_TYPE_MEMORY_TRAP;

    breakPointAndTraps.push_back(memoryTrap);

    addMemoryTrap();
    p_Main->SetDebugMode();
}

void GuiDebugger::addMemoryTrap()
{
    wxString typeSting, printBuffer;
    
    switch (breakPointAndTraps[numberOfBreakPointsAndTraps_].type)
    {
        case MEM_TRAP_READ:
            typeSting = "MT R ";
        break;
        case MEM_TRAP_WRITE:
            typeSting = "MT W ";
        break;
        default:
            typeSting = "MT B ";
        break;
    }
    if (breakPointAndTraps[numberOfBreakPointsAndTraps_].addressEnd == breakPointAndTraps[numberOfBreakPointsAndTraps_].addressStart)
        printBuffer.Printf(typeSting + "%04X", breakPointAndTraps[numberOfBreakPointsAndTraps_].addressStart);
    else
        printBuffer.Printf(typeSting + "%04X-%04X", breakPointAndTraps[numberOfBreakPointsAndTraps_].addressStart, breakPointAndTraps[numberOfBreakPointsAndTraps_].addressEnd);
    
    if (breakPointAndTraps[numberOfBreakPointsAndTraps_].value != -1)
    {
        wxString valueStr;
        valueStr.Printf(":%02X", breakPointAndTraps[numberOfBreakPointsAndTraps_].value);
        printBuffer += valueStr;
    }
    breakPointAndTrapWindowPointer->InsertItem(numberOfBreakPointsAndTraps_, printBuffer);
    numberOfBreakPointsAndTraps_++;
}

bool GuiDebugger::editMemoryTrap(long selectedItem, wxString fullStringValue)
{
    fullStringValue.Replace(":", "-");
    Byte memoryType = MEM_TRAP_BOTH;
    if (fullStringValue.Left(2) == "B ")
        fullStringValue = fullStringValue.Right(fullStringValue.Len()-2);
    if (fullStringValue.Left(2) == "R ")
    {
        memoryType = MEM_TRAP_READ;
        fullStringValue = fullStringValue.Right(fullStringValue.Len()-2);
    }
    if (fullStringValue.Left(2) == "W ")
    {
        memoryType = MEM_TRAP_WRITE;
        fullStringValue = fullStringValue.Right(fullStringValue.Len()-2);
    }
    
    wxString separator;
    wxString strValue = p_Main->extractWordMemTrap (&fullStringValue);
    long value = get16BitFromHexString(strValue, "address");
    if (value == -1)
        return false;
    Word startAddress = value;

    if (fullStringValue.Left(1) == "-")
    {
        fullStringValue = fullStringValue.Right(fullStringValue.Len()-1);
        strValue = p_Main->extractNextWord(&fullStringValue, &separator);
        value = get16BitFromHexString(strValue, "address");
        if (value == -1)
            return false;
    }
    Word endAddress = value;
    
    if (separator == "-")
    {
        strValue = p_Main->extractNextWord (&fullStringValue, &separator);
        value = get8BitFromHexString(strValue, "value");
        if (value == -1)
            return false;
    }
    else
        value = -1;

    breakPointAndTraps[selectedItem].type = memoryType;
    breakPointAndTraps[selectedItem].addressStart = startAddress;
    breakPointAndTraps[selectedItem].addressEnd = endAddress;
    breakPointAndTraps[selectedItem].value = (int)value;
    breakPointAndTraps[selectedItem].breakPointAndTrapType = BPAT_TYPE_MEMORY_TRAP;
    
    return true;
}

long GuiDebugger::get16BitFromHexString(wxString strValue, wxString type)
{
    if (strValue == "")
        return -1;

    long value;
    if (!strValue.ToLong(&value, 16))
    {
        (void)wxMessageBox( "Please specify " + type + " in hexadecimal\n",
                                    "Emma 02", wxICON_ERROR | wxOK );
        return -1;
    }

    if (value > 0xffff)
    {
        (void)wxMessageBox( "Please specify " + type + " of 16 bit max\n",
                                    "Emma 02", wxICON_ERROR | wxOK );
        return -1;
    }

    return value;
}

long GuiDebugger::get8BitFromHexString(wxString strValue, wxString type)
{
    if (strValue == "")
        return -1;

    long value;
    if (!strValue.ToLong(&value, 16))
    {
        (void)wxMessageBox( "Please specify " + type + " in hexadecimal\n",
                                    "Emma 02", wxICON_ERROR | wxOK );
        return -1;
    }

    if (value > 0xff)
    {
        (void)wxMessageBox( "Please specify " + type + " of 8 bit max\n",
                                    "Emma 02", wxICON_ERROR | wxOK );
        return -1;
    }

    return value;
}

