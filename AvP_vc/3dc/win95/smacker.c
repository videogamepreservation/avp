/* KJL 15:25:20 8/16/97
 *
 * smacker.c - functions to handle FMV playback
 *
 */
#include "3dc.h"
#include "module.h"
#include "inline.h"
#include "stratdef.h"
#include "gamedef.h"
#include "smacker.h"
#include "avp_menus.h"
#include "avp_userprofile.h"
#include "d3_func.h"

#define UseLocalAssert 1
#include "ourasert.h"

int VolumeOfNearestVideoScreen;
int PanningOfNearestVideoScreen;

extern char *ScreenBuffer;
extern LPDIRECTSOUND DSObject; 
extern int GotAnyKey;
extern void DirectReadKeyboard(void);
extern IMAGEHEADER ImageHeaderArray[];
#if MaxImageGroups>1
extern int NumImagesArray[];
#else
extern int NumImages;
#endif

void PlayFMV(char *filenamePtr);
static int GetSmackerPixelFormat(DDPIXELFORMAT *pixelFormatPtr);

void FindLightingValueFromFMV(unsigned short *bufferPtr);
void FindLightingValuesFromTriggeredFMV(unsigned char *bufferPtr, FMVTEXTURE *ftPtr);

int SmackerSoundVolume=ONE_FIXED/512;
int MoviesAreActive;
int IntroOutroMoviesAreActive=1;

int FmvColourRed;
int FmvColourGreen;
int FmvColourBlue;

void PlayFMV(char *filenamePtr)
{
}






void InitFMV(void)
{
}

int NextFMVFrame(void*bufferPtr, int x, int y, int w, int h, int fmvNumber)
{
	return 1;

}

void UpdateFMVPalette(PALETTEENTRY *FMVPalette, int fmvNumber)
{
}


void CloseFMV(void)
{
}

static int GetSmackerPixelFormat(DDPIXELFORMAT *pixelFormatPtr)
{

	if( (pixelFormatPtr->dwFlags & DDPF_RGB) && !(pixelFormatPtr->dwFlags & DDPF_PALETTEINDEXED8) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
void StartMenuMusic(void)
{
}

void PlayMenuMusic(void)
{
}
void EndMenuMusic(void)
{
}


/* KJL 12:45:23 10/08/98 - FMVTEXTURE stuff */
#define MAX_NO_FMVTEXTURES 10
FMVTEXTURE FMVTexture[MAX_NO_FMVTEXTURES];
int NumberOfFMVTextures;

void ScanImagesForFMVs(void)
{
	
	extern void SetupFMVTexture(FMVTEXTURE *ftPtr);
	int i;
	IMAGEHEADER *ihPtr;
	NumberOfFMVTextures=0;

	#if MaxImageGroups>1
	for (j=0; j<MaxImageGroups; j++)
	{
		if (NumImagesArray[j])
		{
			ihPtr = &ImageHeaderArray[j*MaxImages];
			for (i = 0; i<NumImagesArray[j]; i++, ihPtr++)
			{
	#else
	{
		if(NumImages)
		{
			ihPtr = &ImageHeaderArray[0];
			for (i = 0; i<NumImages; i++, ihPtr++)
			{
	#endif
				char *strPtr;
				if(strPtr = strstr(ihPtr->ImageName,"FMVs"))
				{
					Smack *smackHandle;
					char filename[30];
					{
						char *filenamePtr = filename;
						do
						{
							*filenamePtr++ = *strPtr;
						}
						while(*strPtr++!='.');

						*filenamePtr++='s';
						*filenamePtr++='m';
						*filenamePtr++='k';
						*filenamePtr=0;
					}
					
					smackHandle = 0;
					if (smackHandle)
					{
						FMVTexture[NumberOfFMVTextures].IsTriggeredPlotFMV = 0;
					}
					else
					{
						FMVTexture[NumberOfFMVTextures].IsTriggeredPlotFMV = 1;
					}

					{
						FMVTexture[NumberOfFMVTextures].SmackHandle = smackHandle;
						FMVTexture[NumberOfFMVTextures].ImagePtr = ihPtr;
						FMVTexture[NumberOfFMVTextures].StaticImageDrawn=0;
						SetupFMVTexture(&FMVTexture[NumberOfFMVTextures]);
						NumberOfFMVTextures++;
					}
				}
			}		
		}
	}


}

void UpdateAllFMVTextures(void)
{	
	extern void UpdateFMVTexture(FMVTEXTURE *ftPtr);
	int i = NumberOfFMVTextures;

	while(i--)
	{
		UpdateFMVTexture(&FMVTexture[i]);
	}

}

void ReleaseAllFMVTextures(void)
{	
	extern void UpdateFMVTexture(FMVTEXTURE *ftPtr);
	int i = NumberOfFMVTextures;

	while(i--)
	{
		FMVTexture[i].MessageNumber = 0;

		if (FMVTexture[i].SrcTexture)
		{
			ReleaseD3DTexture(FMVTexture[i].SrcTexture);
			FMVTexture[i].SrcTexture=0;
		}
		if (FMVTexture[i].SrcSurface)
		{
			ReleaseDDSurface(FMVTexture[i].SrcSurface);
			FMVTexture[i].SrcSurface=0;
		}
		if (FMVTexture[i].DestTexture)
		{	
			ReleaseD3DTexture(FMVTexture[i].DestTexture);
			FMVTexture[i].DestTexture = 0;
		}
	}

}


int NextFMVTextureFrame(FMVTEXTURE *ftPtr, void *bufferPtr)
{
	int smackerFormat;
	int w = 128;
	
	{
		extern D3DINFO d3d;
		smackerFormat = GetSmackerPixelFormat(&(d3d.TextureFormat[d3d.CurrentTextureFormat].ddsd.ddpfPixelFormat));
	}
	if (smackerFormat) w*=2;

	if (!ftPtr->StaticImageDrawn || smackerFormat)
	{
		int i = w*96/4;
		unsigned int seed = FastRandom();
		int *ptr = (int*)bufferPtr;
		do
		{
			seed = ((seed*1664525)+1013904223);
			*ptr++ = seed;
		}
		while(--i);
		ftPtr->StaticImageDrawn=1;
	}
	FindLightingValuesFromTriggeredFMV((unsigned char*)bufferPtr,ftPtr);
	return 1;

}

void UpdateFMVTexturePalette(FMVTEXTURE *ftPtr)
{
	unsigned char *c;
	int i;

	if (MoviesAreActive && ftPtr->SmackHandle)
	{
	}
	else
	{
	  	{
			unsigned int seed = FastRandom();
			for(i=0;i<256;i++)
			{   
				int l = (seed&(seed>>24)&(seed>>16));
				seed = ((seed*1664525)+1013904223);
				ftPtr->SrcPalette[i].peRed=l;
			    ftPtr->SrcPalette[i].peGreen=l;
		   		ftPtr->SrcPalette[i].peBlue=l;
		 	}	
		}
	}
}

extern void StartTriggerPlotFMV(int number)
{
	int i = NumberOfFMVTextures;
	char buffer[25];

	if (CheatMode_Active != CHEATMODE_NONACTIVE) return;

	sprintf(buffer,"FMVs//message%d.smk",number);
	{
		FILE* file=fopen(buffer,"rb");
		if(!file)
		{
			return;
		}
		fclose(file);
	}
	while(i--)
	{
		if (FMVTexture[i].IsTriggeredPlotFMV)
		{
			FMVTexture[i].MessageNumber = number;
		}
	}
}
extern void StartFMVAtFrame(int number, int frame)
{
}
extern void GetFMVInformation(int *messageNumberPtr, int *frameNumberPtr)
{
	int i = NumberOfFMVTextures;

	while(i--)
	{
		if (FMVTexture[i].IsTriggeredPlotFMV)
		{
			if(FMVTexture[i].SmackHandle)
			{
				*messageNumberPtr = FMVTexture[i].MessageNumber;
				*frameNumberPtr = 0;
				return;
			}
		}
	}

	*messageNumberPtr = 0;
	*frameNumberPtr = 0;
}


extern void InitialiseTriggeredFMVs(void)
{
	int i = NumberOfFMVTextures;
	while(i--)
	{
		if (FMVTexture[i].IsTriggeredPlotFMV)
		{
			if(FMVTexture[i].SmackHandle)
			{
				FMVTexture[i].MessageNumber = 0;
			}

			FMVTexture[i].SmackHandle = 0;
		}
	}
}

void FindLightingValuesFromTriggeredFMV(unsigned char *bufferPtr, FMVTEXTURE *ftPtr)
{
	unsigned int totalRed=0;
	unsigned int totalBlue=0;
	unsigned int totalGreen=0;
	#if 0
	int pixels = 128*96;//64*48;//256*192;
	do
	{
		unsigned char source = (*bufferPtr++);
		totalBlue += ftPtr->SrcPalette[source].peBlue;
		totalGreen += ftPtr->SrcPalette[source].peGreen;
		totalRed += ftPtr->SrcPalette[source].peRed; 
	}
	while(--pixels);
	#else
	int pixels = 128*96/4;//64*48;//256*192;
	unsigned int *source = (unsigned int*) (bufferPtr);
	do
	{
		int s = *source++;
		{
			int t = s&255;
			totalBlue += ftPtr->SrcPalette[t].peBlue;
			totalGreen += ftPtr->SrcPalette[t].peGreen;
			totalRed += ftPtr->SrcPalette[t].peRed; 
		}
		#if 0
		s >>=8;
		{
			int t = s&255;
			totalBlue += ftPtr->SrcPalette[t].peBlue;
			totalGreen += ftPtr->SrcPalette[t].peGreen;
			totalRed += ftPtr->SrcPalette[t].peRed; 
		}
		s >>=8;
		{
			int t = s&255;
			totalBlue += ftPtr->SrcPalette[t].peBlue;
			totalGreen += ftPtr->SrcPalette[t].peGreen;
			totalRed += ftPtr->SrcPalette[t].peRed; 
		}
		s >>=8;
		{
			int t = s;
			totalBlue += ftPtr->SrcPalette[t].peBlue;
			totalGreen += ftPtr->SrcPalette[t].peGreen;
			totalRed += ftPtr->SrcPalette[t].peRed; 
		}
		#endif
	}
	while(--pixels);
	#endif
	FmvColourRed = totalRed/48*16;
	FmvColourGreen = totalGreen/48*16;
	FmvColourBlue = totalBlue/48*16;

}

extern int NumActiveBlocks;
extern DISPLAYBLOCK *ActiveBlockList[];
#include "showcmds.h"
int GetVolumeOfNearestVideoScreen(void)
{											  
	extern VIEWDESCRIPTORBLOCK *Global_VDB_Ptr;
	int numberOfObjects = NumActiveBlocks;
	int leastDistanceRecorded = 0x7fffffff;
	VolumeOfNearestVideoScreen = 0;

	{
		extern char LevelName[];
		if (!_stricmp(LevelName,"invasion_a"))
		{
			VolumeOfNearestVideoScreen = ONE_FIXED;
			PanningOfNearestVideoScreen = ONE_FIXED/2;
		}
	}

	while (numberOfObjects)
	{
		DISPLAYBLOCK* objectPtr = ActiveBlockList[--numberOfObjects];
		STRATEGYBLOCK* sbPtr = objectPtr->ObStrategyBlock;

		if (sbPtr)
		{
			if (sbPtr->I_SBtype == I_BehaviourVideoScreen)
			{
				int dist;
				VECTORCH disp;

				disp.vx = objectPtr->ObWorld.vx - Global_VDB_Ptr->VDB_World.vx;
				disp.vy = objectPtr->ObWorld.vy - Global_VDB_Ptr->VDB_World.vy;
				disp.vz = objectPtr->ObWorld.vz - Global_VDB_Ptr->VDB_World.vz;
				
				dist = Approximate3dMagnitude(&disp);
				if (dist<leastDistanceRecorded && dist<ONE_FIXED)
				{
					leastDistanceRecorded = dist;
					VolumeOfNearestVideoScreen = ONE_FIXED + 1024 - dist/2;
					if (VolumeOfNearestVideoScreen>ONE_FIXED) VolumeOfNearestVideoScreen = ONE_FIXED;

					{
						VECTORCH rightEarDirection;
						#if 0
						rightEarDirection.vx = Global_VDB_Ptr->VDB_Mat.mat11;
						rightEarDirection.vy = Global_VDB_Ptr->VDB_Mat.mat12;
						rightEarDirection.vz = Global_VDB_Ptr->VDB_Mat.mat13;
						Normalise(&disp);
						#else
						rightEarDirection.vx = Global_VDB_Ptr->VDB_Mat.mat11;
						rightEarDirection.vy = 0;
						rightEarDirection.vz = Global_VDB_Ptr->VDB_Mat.mat31;
						disp.vy=0;
						Normalise(&disp);
						Normalise(&rightEarDirection);
						#endif
						PanningOfNearestVideoScreen = 32768 + DotProduct(&disp,&rightEarDirection)/2;
					}
				}
			}
		}
	}
	PrintDebuggingText("Volume: %d, Pan %d\n",VolumeOfNearestVideoScreen,PanningOfNearestVideoScreen);
	return VolumeOfNearestVideoScreen;
}