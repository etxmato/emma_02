#include "audio_detect.h"

#ifdef AUDIOIN_WINDIB

#include <SDL.h>
#include <SDL_thread.h>
#include <string.h>
#include <malloc.h>
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>

#include "SDL_audioin_internal.h"

#define NUM_BUFFERS 2
static WAVEHDR *wHeader[NUM_BUFFERS]={NULL,NULL};
static HWAVEIN __hWaveIn=NULL;

static int WINDIB_OpenAudioIn(SDL_AudioSpec *, SDL_AudioSpec *);
static void WINDIB_CloseAudioIn(void);
static int WINDIB_PauseAudioIn(int);

typedef struct queuebuf
{
  HWAVEIN hwi;
  WAVEHDR *hdr;
} queuebuf;

int enqueue_thread(void *dat);
static void Enqueue(HWAVEIN hwi, WAVEHDR *hdr);
static queuebuf Dequeue();

static SDL_Thread *thread=NULL;
static SDL_mutex *mutex=NULL;
static SDL_sem   *readsem=NULL,*writesem=NULL;
static queuebuf  queue[NUM_BUFFERS]={{NULL,NULL},{NULL,NULL}};
static int readpos=0,writepos=0;

Sound_InputFunctions driver_WINDIB=
{
  "WINDIB",
  "Input driver using Windows DIB audio",
  "tsm@accesscomm.ca",
  null_init,
  null_quit,
  WINDIB_OpenAudioIn,
  WINDIB_CloseAudioIn,
  WINDIB_PauseAudioIn
};


WAVEHDR *CreateWaveHeader(Uint32 buflen)
{
	WAVEHDR *hdr=(WAVEHDR *)malloc(sizeof(WAVEHDR));

	if(hdr==NULL)	return NULL;
	memset(hdr,0,sizeof(WAVEHDR));

	hdr->dwBufferLength=buflen;
	hdr->dwFlags=0;
	hdr->lpData=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,buflen);
	
	return(hdr);
}

void CALLBACK waveInProc(HWAVEIN hwi,UINT uMsg, DWORD_PTR dwInstance,
	DWORD_PTR dwParam1, DWORD_PTR dwParam2)
{
	SDL_AudioSpec *spec=(SDL_AudioSpec *)dwInstance;
	void (*CallBack)(void *,Uint8 *, int);
	WAVEHDR *header;

	switch(uMsg)
	{
	default:
//		fprintf(stderr,"Unknown message %d\n",uMsg);
		break;

	case WIM_CLOSE:
		break;

	case WIM_OPEN:
		break;

	case WIM_DATA:
		CallBack=spec->callback;
		header=(WAVEHDR *)dwParam1;

		if(header->dwBytesRecorded)
			CallBack(spec->userdata,header->lpData,header->dwBytesRecorded);

		header->dwBytesRecorded = 0;
	    header->dwFlags &= ~WHDR_DONE;

		Enqueue(hwi,header);

		break;
	}
}

int WINDIB_OpenAudioIn(SDL_AudioSpec *requested, SDL_AudioSpec *result)
{	
	Uint32 n=0;
	SDL_AudioSpec *nSpec;
	WAVEFORMATEX f_ex;

	if(__hWaveIn!=NULL)
	{
//	  fprintf(stderr,"Already Initialized\n");
	  SDL_SetError("Already initialized");
	  return(-1);
	}

	if(requested==NULL)	return -1;

	if(waveInGetNumDevs()==0)
	{
//		fprintf(stderr,"No audio devices, open failed.\n");
		return(-1);
	}
	
	memset(&f_ex,0,sizeof(f_ex));		

	switch(requested->format)
	{
	case AUDIO_U16:
	case AUDIO_S16:
		f_ex.wBitsPerSample=16;
		break;

	case AUDIO_U8:
	case AUDIO_S8:
		f_ex.wBitsPerSample=8;
		break;
	}

	f_ex.nChannels=requested->channels;
	f_ex.wFormatTag=WAVE_FORMAT_PCM;
	f_ex.nSamplesPerSec=requested->freq;
	f_ex.nBlockAlign=(f_ex.wBitsPerSample*f_ex.nChannels)/8;
	f_ex.nAvgBytesPerSec=f_ex.nSamplesPerSec*f_ex.nBlockAlign;

	nSpec=(SDL_AudioSpec *)malloc(sizeof(SDL_AudioSpec));
	memcpy(nSpec,requested,sizeof(SDL_AudioSpec));

	mutex=SDL_CreateMutex();
	readsem=SDL_CreateSemaphore(0);
	writesem=SDL_CreateSemaphore(NUM_BUFFERS);

	thread=SDL_CreateThread(enqueue_thread,"",NULL);
	if(thread==NULL)
	{  
	  SDL_DestroyMutex(mutex); mutex=NULL;
	  SDL_DestroySemaphore(readsem); readsem=NULL;
	  SDL_DestroySemaphore(writesem); writesem=NULL;
	  SDL_SetError("Couldn't create queue thread!\n");
	  return(-1);
	}

	if(waveInOpen(&__hWaveIn,WAVE_MAPPER,&f_ex,(DWORD_PTR)waveInProc,(DWORD_PTR)nSpec,CALLBACK_FUNCTION)!=MMSYSERR_NOERROR)
	{
//		fprintf(stderr,"Couldn't open.\n\n");
		free(nSpec);
		SDL_DestroyMutex(mutex); mutex=NULL;
		SDL_DestroySemaphore(readsem); readsem=NULL;
		SDL_DestroySemaphore(writesem); writesem=NULL;
		return(-1);
	}
	free(nSpec);

	if(result)
	{
		memcpy(result,requested,sizeof(SDL_AudioSpec));

		result->size=requested->samples*f_ex.nBlockAlign;
		result->silence=0;

		switch(requested->format)
		{
		case AUDIO_U16:
		case AUDIO_S16:
			result->format=AUDIO_S16;
			break;

		case AUDIO_U8:
		case AUDIO_S8:
			result->format=AUDIO_U8;
			break;
		}
	}

	for(n=0; n<NUM_BUFFERS; n++)
	{
		wHeader[n]=CreateWaveHeader(requested->samples*f_ex.nBlockAlign);
//        fprintf(stderr,"Header %p\n",wHeader[n]);

		waveInPrepareHeader(__hWaveIn,wHeader[n],sizeof(WAVEHDR));
		waveInAddBuffer(__hWaveIn,wHeader[n],sizeof(WAVEHDR));
	}

	audioin_status=SDL_AUDIO_PAUSED;

	return(0);
}

int WINDIB_PauseAudioIn(int pause)
{
	if(__hWaveIn==NULL)
	{
//	  fprintf(stderr,"Device not open!\n");
	  SDL_SetError("Device not open");
	  return(-1);
	}

	if(pause)
	{
		audioin_status=SDL_AUDIO_PAUSED;
		waveInStop(__hWaveIn);
	}
	else
	{
		audioin_status=SDL_AUDIO_PLAYING;
		waveInStart(__hWaveIn);
	}

	return(0);
}

void WINDIB_CloseAudioIn()
{
	int n;
	audioin_status=SDL_AUDIO_STOPPED;

	if(__hWaveIn!=NULL)
	{
		waveInReset(__hWaveIn);

		for(n=0; n<NUM_BUFFERS; n++)
		{
			waveInUnprepareHeader(__hWaveIn,wHeader[n],sizeof(WAVEHDR));
			HeapFree(GetProcessHeap(),0,wHeader[n]->lpData);
			free(wHeader[n]);
			wHeader[n]=NULL;
		}

		waveInClose(__hWaveIn);
		__hWaveIn=NULL;
	}

	Enqueue(NULL,NULL);
	SDL_WaitThread(thread,NULL);
	thread=NULL;

	SDL_DestroyMutex(mutex); mutex=NULL;
	SDL_DestroySemaphore(readsem); readsem=NULL;
	SDL_DestroySemaphore(writesem); writesem=NULL;	

	audioin_status=SDL_AUDIO_STOPPED;
}

int enqueue_thread(void *dat)
{
  queuebuf buf;

  buf=Dequeue();
  while(buf.hdr!=NULL)
  {
  if(audioin_status!=SDL_AUDIO_STOPPED)
	waveInAddBuffer(buf.hwi,buf.hdr,sizeof(WAVEHDR));
    buf=Dequeue();
  }

  return(0);
}

void Enqueue(HWAVEIN hwi, WAVEHDR *hdr)
{
  SDL_SemWait(writesem);
  SDL_mutexP(mutex);
    queue[writepos%NUM_BUFFERS].hdr=hdr;
	queue[writepos%NUM_BUFFERS].hwi=hwi;
	writepos++;
  SDL_mutexV(mutex);
  SDL_SemPost(readsem);
}

queuebuf Dequeue()
{
  queuebuf buf;
  SDL_SemWait(readsem);
  SDL_mutexP(mutex);
    buf=queue[readpos%NUM_BUFFERS];
	readpos++;
  SDL_mutexV(mutex);
  SDL_SemPost(writesem);
  return(buf);
}


#endif/*AUDIOIN_WINDIB*/
