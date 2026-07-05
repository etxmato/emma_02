/***************************************************************************
 *   Copyright (C) 2004 by Tyler Montbriand                                *
 *   tsm@accesscomm.ca                                                     *
 *   Released under the GNU LGPL                                           *
 ***************************************************************************/

/**
 *  This header file helps decide what audio subsystems should be used based
 *  on the system's apparent operating system, etc.
 */

#ifndef __AUDIO_DETECT_H__
#define __AUDIO_DETECT_H__

#include "SDL_audioin_internal.h"

/**
 *  Linux defaults to using /dev/dsp etc.
 */
#if defined (__linux__) // || (__WXOSX_COCOA__)
# define AUDIOIN_DEVDSP
  extern Sound_InputFunctions driver_DEVDSP;
#endif

/**
 *  Windows defaults to using the windows DIB system
 */
#ifdef __WXMSW__
# define AUDIOIN_WINDIB
  extern Sound_InputFunctions driver_WINDIB;
#endif 

#endif/*__AUDIO_DETECT_H__*/
