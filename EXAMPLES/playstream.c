#include "ALmixer.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_SOURCES 16

ALboolean g_PlayingAudio[MAX_SOURCES];

void Internal_SoundFinished_CallbackIntercept(ALint which_channel, ALuint al_source, ALmixer_Data* almixer_data, ALboolean finished_naturally, void* user_data)
{
	fprintf(stderr, "Channel %d finished\n", which_channel);
	g_PlayingAudio[which_channel] = AL_FALSE;
}

int main(int argc, char* argv[])
{
	ALint i;
	ALboolean still_playing = AL_TRUE;

	ALmixer_Data* audio_data[MAX_SOURCES];

	if(argc < 1)
	{
		printf("Pass a sound file (or files) as a parameter\n");
	}
	else if(argc-1 > MAX_SOURCES)
	{
		printf("Maximum supported files is %d\n", MAX_SOURCES);
	}
	ALmixer_Init(ALMIXER_DEFAULT_FREQUENCY, ALMIXER_DEFAULT_NUM_SOURCES, ALMIXER_DEFAULT_REFRESH);
	
	for(i=1; i<argc; i++)
	{
		if(!(audio_data[i-1]=ALmixer_LoadStream( argv[i], ALMIXER_DEFAULT_BUFFERSIZE, ALMIXER_DEFAULT_QUEUE_BUFFERS, ALMIXER_DEFAULT_STARTUP_BUFFERS, ALMIXER_DEFAULT_BUFFERS_TO_QUEUE_PER_UPDATE_PASS, AL_FALSE) ))
/*
		if(!(audio_data[i-1]=ALmixer_LoadStream( argv[i], 4096, 2, 1, AL_FALSE) ))
		if(!(audio_data[i-1]=ALmixer_LoadStream( argv[i], 4096, 5 * 4, 2, AL_FALSE) ))
*/
		{
			printf("%s. Quiting program.\n", ALmixer_GetError());
			exit(0);
		}
	}

	ALmixer_SetPlaybackFinishedCallback(Internal_SoundFinished_CallbackIntercept, NULL);

	printf("Is wav available: %d\n", ALmixer_IsDecoderAvailable("wav"));
	printf("Is ogg available: %d\n", ALmixer_IsDecoderAvailable("ogg"));
	printf("Is mp3 available: %d\n", ALmixer_IsDecoderAvailable("mp3"));
	printf("Is mp4 available: %d\n", ALmixer_IsDecoderAvailable("mp4"));

	for(i=1; i<argc; i++)
	{
		g_PlayingAudio[i-1] = AL_TRUE;
		ALmixer_PlayChannel(i-1, audio_data[i-1], 0);
	}

	while(still_playing)
	{
		still_playing = AL_FALSE;
		for(i=1; i<argc; i++)
		{
			still_playing |= g_PlayingAudio[i-1];
		}
		ALmixer_Update();
		ALmixer_Delay(10);
	}
		
	for(i=1; i<argc; i++)
	{
		ALmixer_FreeData(audio_data[i-1]);
	}

	ALmixer_Quit();

	return 0;
}

