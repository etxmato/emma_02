/***************************************************************************
 *   Copyright (C) 2004 by Tyler Montbriand                                *
 *   tsm@accesscomm.ca                                                     *
 *   Released under the GNU LGPL                                           *
 ***************************************************************************/

#include "audio_detect.h"

#ifdef AUDIOIN_DEVDSP

#ifdef __WXOSX_COCOA__
#include <liboss/soundcard.h>
#else
#include <sys/soundcard.h>
#endif
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "SDL_audioin.h"
#include "SDL_audioin_internal.h"

#define DEFAULT_DEV "/dev/dsp"
#define MAX_RETRY   3


static volatile int fd=-1;
// Converts SDL audio types to UNIX ones
extern int mask_in[],mask_out[],samplesize[];

static SDL_AudioSpec localspec;
static SDL_Thread *ainthread=NULL;

int ainthread_func(void *);

static int          DSP_GetFrameSize(int fd);
static int          DSP_SetFrameSize(int fd, int framesize);
static int          DSP_SetChannels(int fd, int channels);
static int          DSP_SetRate(int fd, int rate);
static unsigned int DSP_GetMask(int fd);
static int          DSP_SetFormat(int fd, unsigned int format);

static void DEVDSP_CloseAudioIn();
static int DEVDSP_PauseAudioIn(int pause_on);
static int DEVDSP_OpenAudioIn(SDL_AudioSpec *requested, SDL_AudioSpec *restult);

Sound_InputFunctions driver_DEVDSP=
{
  "DSP",
  "Input driver using raw /dev/dsp and ioctls",
  "tsm@accesscomm.ca",
  null_init,
  null_quit,
  DEVDSP_OpenAudioIn,
  DEVDSP_CloseAudioIn,
  DEVDSP_PauseAudioIn
};

int DEVDSP_OpenAudioIn(SDL_AudioSpec *requested, SDL_AudioSpec *result)
{
  const char *dev=DEFAULT_DEV;
  int mask=0,n,tries=0;

  if(requested==NULL)
  {
    SDL_SetError("Requested format unspecified");
    return(-1);
  }

  if(getenv("SDL_DSP_DEV")!=NULL)
    dev=getenv("SDL_DSP_DEV");

  localspec=(*requested);
  requested=&localspec;

  fd=open(dev,O_RDONLY|O_NONBLOCK);

  while((fd<0)&&((tries++)<3))
  {
    SDL_Delay(500);
//    fprintf(stderr,"Failed to open device, retry %d\n",tries);
    /**
     * Open in nonblocking mode so if the device is busy, it will
     * fail instead of hanging
     */
    fd=open(dev,O_RDONLY|O_NONBLOCK);
  }

  if(fd<0)
  {
 //   SDL_SetError("Hmm....Couldn't open audio device %s : %s",dev,strerror(errno));
 //   fprintf(stderr,
 //     "Could not open audio device for reading.  It may be already in use.\n\n"
 //     "Note that anything using full-duplex mode can block all other\n"
 //     "programs from using the device even when nothing else has opened it\n"
 //     "for write.\n");
    return(-1);
  }

  /* Re-enable blocking, we need it */
  if(fcntl(fd,F_SETFL,O_RDONLY)<0)
  {
    SDL_SetError("Could not enable blocking: %s",strerror(errno));
    close(fd);
    fd=-1;
    return(-1);
  }

  mask=DSP_GetMask(fd);
  if(mask==0)
  {
    SDL_SetError("No formats supported?  Not good.");
    close(fd);
    fd=-1;
    return(-1);
  }

  for(n=0; mask_in[n]>=0; n++)
    if((mask_in[n]==requested->format)&&(mask&mask_out[n]))
    {
      requested->size=requested->samples*requested->channels*samplesize[n];
      if((DSP_SetFormat(fd,mask_out[n])<0)||
         (DSP_SetChannels(fd,requested->channels)<0)||
         (DSP_SetFrameSize(fd,requested->size)<0)||
         (DSP_SetRate(fd,requested->freq)<0))
      {
        SDL_SetError("Couldn't initialize device");
        close(fd);
        fd=-1;
        return(-1);
      }
      audioin_status=SDL_AUDIO_PAUSED;
      ainthread=SDL_CreateThread(ainthread_func, "", NULL);
      return(0);
    }

  SDL_SetError("Format not supported");
  close(fd);
  fd=-1;
  return(-1);
}

int DEVDSP_PauseAudioIn(int pause_on)
{
  if(audioin_status!=SDL_AUDIO_STOPPED)
  {
    if(pause_on)
      audioin_status=SDL_AUDIO_PAUSED;
    else
      audioin_status=SDL_AUDIO_PLAYING;

    return(0);
  }

  SDL_SetError("Cannot pause, audio not open");
  return(-1);
}

void DEVDSP_CloseAudioIn()
{
  int status;
  if(audioin_status!=SDL_AUDIO_STOPPED)
  {
    audioin_status=SDL_AUDIO_STOPPED;
    SDL_WaitThread(ainthread,&status);

    if(status!=0)
      fprintf(stderr,"Thread exited with status %d\n",status);

    ainthread=NULL;
    close(fd);
  }
}

int ainthread_func(void *dat)
{
  Uint8 *buf=NULL;

  if(audioin_status==SDL_AUDIO_STOPPED)
    return(-1);

  buf=(Uint8 *)malloc(localspec.size);

  while(audioin_status!=SDL_AUDIO_STOPPED)
  {
    ssize_t bytes=0;
    bytes=read(fd,buf,localspec.size);

    if(bytes<0)
    {
      audioin_status=SDL_AUDIO_STOPPED;
      free(buf);
      fprintf(stderr,"audio input thread cannot read from audio device\n");
      SDL_SetError("Could not read from audio device");
      return(-1);
    }

    if(audioin_status==SDL_AUDIO_PLAYING)
    {
      if(localspec.callback!=NULL)
        localspec.callback(localspec.userdata,buf,localspec.size);
    }
  }

  free(buf);
  return(0);
}

unsigned int DSP_GetMask(int fd)
{
  unsigned int mask=0;
  ioctl(fd,SNDCTL_DSP_GETFMTS,&mask);
  return(mask);
}

int DSP_SetChannels(int fd, int channels)
{
  return(ioctl(fd,SNDCTL_DSP_CHANNELS,&channels));
}

int DSP_SetRate(int fd, int rate)
{
  return(ioctl(fd,SNDCTL_DSP_SPEED,&rate));
}

int DSP_GetFrameSize(int fd)
{
  int framesize=0;
  ioctl(fd,SNDCTL_DSP_GETBLKSIZE,&framesize);
  return(framesize);
}

int DSP_SetFrameSize(int fd, int framesize)
{
  return(ioctl(fd,SNDCTL_DSP_SETFRAGMENT,&framesize));
}

int DSP_SetFormat(int fd, unsigned int format)
{
  return(ioctl(fd,SNDCTL_DSP_SETFMT,&format));
}

int mask_in[]=
{
  AUDIO_U8
, AUDIO_S8
, AUDIO_U16LSB
, AUDIO_S16LSB
, AUDIO_U16MSB
, AUDIO_S16MSB
, -1
};
int mask_out[]=
{
   AFMT_U8
,  AFMT_S8
,  AFMT_U16_LE
,  AFMT_S16_LE
,  AFMT_U16_BE
,  AFMT_S16_BE
, -1
};
int samplesize[]=
{ 1,1,2,2,2,2,-1 };

#endif/*AUDIOIN_DEVDSP*/
