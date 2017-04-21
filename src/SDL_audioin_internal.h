/***************************************************************************
 *   Copyright (C) 2004 by Tyler Montbriand                                *
 *   tsm@accesscomm.ca                                                     *
 *                                                                         *
 *   Released under the GNU LGPL                                           *
 ***************************************************************************/

#ifndef __SDL_AUDIOIN_INTERNAL_H__
#define __SDL_AUDIOIN_INTERNAL_H__

#include <SDL_audio.h>

typedef struct __SDOUND_INPUTFUNCTIONS__
{
  const char     *name;        /* Short name like "DSP", "WINDIB", etc */
  const char     *description; /* Input driver description */
  const char     *url;         /* Author email or webpage */  

  /**
   * Attempts to initialize the driver.  Expected to return -1 failure, 
   * 0 on success.
   */
  int  (*init)(void);
  /* Attempts to de-initialize driver. */
  void (*quit)(void);
  /**
   * Attempts to open audio input device according to specifications.
   * If these specifications cannot be met, different ones may be used.
   * Returns 0 on success, -1 on error.
   */
  int (*open)(SDL_AudioSpec *given, SDL_AudioSpec *actual);
  /* Attempts to close an opened audio input device. */
  void (*close)(void);
  /* Attempts to pause or unpause an opened audio input device. */
  int (*pause)(int);
} Sound_InputFunctions;

extern Sound_InputFunctions driver_RAND;

typedef union UniPtr
{
  Uint8       *u8ptr;
  Sint8       *s8ptr;
  Uint16      *u16ptr;
  Sint16      *s16ptr;
  Uint32      *u32ptr;
  Sint32      *s32ptr;
  float       *fptr;
  double      *dptr;
  long double *ldptr;
  void        *vptr;  
} UniPtr;

/**
 * Returns 1 if audiospec is valid, 0 if not.
 * Outputs more verbose errorcodes to SDL_GetError.
 * ignores samples and silence.
 */
DECLSPEC int  SDLCALL AudioSpecValid(const SDL_AudioSpec *spec);
/*! \brief Calculates the byte size of the buffer. */
DECLSPEC int SDLCALL AudioSpecSize(const SDL_AudioSpec *spec);
/*! \brief Returns the # of milliseconds of sound in one buffer. */
DECLSPEC int  SDLCALL AudioSpecTicks(const SDL_AudioSpec *spec);

DECLSPEC int SDLCALL null_init(void);
DECLSPEC void SDLCALL null_quit(void);

extern volatile SDL_audiostatus audioin_status;

#endif/*__SDL_AUDIOIN_INTERNAL_H__*/
