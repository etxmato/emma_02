/***************************************************************************
 *   Copyright (C) 2004 by Tyler Montbriand                                *
 *   tsm@accesscomm.ca                                                     *
 *   Released under the GNU LGPL                                           *
 ***************************************************************************/
/**
 *  SDL_audioinput:
 *
 *  One often-voiced complaint about SDL is that while it has nice facilities
 *  for audio OUTPUT, it has nothing for audio INPUT, and anyone who asks for
 *  them is told to go fu^G^Gwrite write it themself.  So I wrote it.
 *
 *  It is nearly identical in functionality to the SDL audio output subsystem,
 *  except it's for input and it's not a subsystem.  This means that in
 *  addition to the standard open/close/pause/status calls, there's also init
 *  and quit functions.  Otherwise it's interface is as identical as I've been
 *  able to make it.
 *
 *  It's modular, so if it doesn't support your OS of choice, write an input
 *  driver and email it to me at tsm@accesscomm.ca and I'll put it in the next
 *  release.  :)  Currently supports Linux and Windows.
 */

#ifndef __SDL_AUDIOIN_H__
#define __SDL_AUDIOIN_H__

#include <SDL_audio.h>
#include <begin_code.h>

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

/**
 *  Attempts to initialize audio input system.  If it can find a working driver
 *  it returns 0, if not, returns -1.
 */
DECLSPEC int SDLCALL SDL_InitAudioIn(void);
/**
 *  Attempts to open audio device with specified parameters.  The values obtained
 *  may be slightly different than the ones asked for.  Returns 0 on success,
 *  -1 on error.
 */
DECLSPEC int SDLCALL SDL_OpenAudioIn(SDL_AudioSpec *desired, SDL_AudioSpec *obtained);
/**
 *  Pauses or unpauses an open audio device.  Returns 0 on success, -1
 *  on failure.
 */
DECLSPEC int SDLCALL SDL_PauseAudioIn(int pause_on);
/**
 *  Closes an open audio input device.  Is safe to call even if there is not
 *  a device open.
 */
DECLSPEC void SDLCALL SDL_CloseAudioIn();
/**
 *  Quits the audio input system.  Also closes any open audio input devices.
 */
DECLSPEC void SDLCALL SDL_QuitAudioIn(void);
/**
 *  Returns the current status of the audio input device.  If it returns
 *  SDL_AUDIO_STOPPED, the device hasn't been opened yet.
 */
#if defined(__WINXP__) || defined(__linux__)
DECLSPEC SDL_audiostatus  SDLCALL SDL_GetAudioInStatus(void);
#else
DECLSPEC SDL_AudioStatus  SDLCALL SDL_GetAudioInStatus(void);
#endif

#ifdef __cplusplus
}
#endif/*__cplusplus*/

#include <close_code.h>

#endif/*__SDL_AUDIOIN_H__*/
