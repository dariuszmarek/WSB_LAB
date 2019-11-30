#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/tpl.h>

#include "mario.h"
#include "mario_tpl.h"
#include "ballsprites.h"
#include "ballsprites_tpl.h"

#define DEFAULT_FIFO_SIZE	(256*1024)

static void *frameBuffer[2] = { NULL, NULL};
static GXRModeObj *rmode;


GXTexObj texObj;
GXTexObj texObj_mario;

int counter = 30;
int id = 0;


// to do 1: Przeanalizuj kod umożliwiający wyświetlanie tekstury. 
// a)	Wykorzystanie tekstury w funkcji drawSpriteTex
// b)	Zmienna przechowująca koordynaty tekstury a i b.
//Zmodyfikuj kod tak, aby wyświetlił w 4 różnych miejscach różne kolory kuli (różne tekstury wycięte ze wzorca).


float texCoords [] = 
{
	0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.0, 0.5,
};

// -------------------------------------------------------------------


void drawSpriteTex( int x, int y, int width, int height, float* textureCoords )
{
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);			// Rozpoczęcie rysowania kwadratu
		GX_Position2f32(x, y);					// Pozycja lewego górnego narożnika kwadratu na scenie
		GX_TexCoord2f32(textureCoords[0],textureCoords[1]); // Pozycija (skala) lewego górnego narożnika pozycji tektury w obrazie

		GX_Position2f32(x+width-1, y);			// Pozycija prawego górnego narożnika kwadratu na scenie
		GX_TexCoord2f32(textureCoords[2],textureCoords[3]); // Pozycija (skala) prawego górnego narożnika pozycji tektury w obrazie

		GX_Position2f32(x+width-1,y+height-1);	// Prawy dół
		GX_TexCoord2f32(textureCoords[4],textureCoords[5]);

		GX_Position2f32(x,y+height-1);			// Lewy dół
		GX_TexCoord2f32(textureCoords[6],textureCoords[7]);
	GX_End();									// Zakończenie rysowania kwadratu
}

//---------------------------------------------------------------------------------
int main( int argc, char **argv ){
//---------------------------------------------------------------------------------
	u32	fb; 	// początkowy indeks bufora ramki
	u32 first_frame;
	f32 yscale;
	u32 xfbHeight;
	Mtx44 perspective;
	Mtx GXmodelView2D;
	void *gp_fifo = NULL;
	GXColor background = {0, 0, 0, 0xff};
	VIDEO_Init();

	rmode = VIDEO_GetPreferredMode(NULL);

	fb = 0;
	first_frame = 1;
	// przydziel 2 bufory ramki dla podwójnego buforowania
	frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(frameBuffer[fb]);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	fb ^= 1;

	// skonfiguruj fifo, a następnie uruchom flipper
	gp_fifo = memalign(32,DEFAULT_FIFO_SIZE);
	memset(gp_fifo,0,DEFAULT_FIFO_SIZE);

	GX_Init(gp_fifo,DEFAULT_FIFO_SIZE);

	// resetuje bg do koloru
	GX_SetCopyClear(background, 0x00ffffff);

	// pozostałe konfiguracje gx
	GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
	yscale = GX_GetYScaleFactor(rmode->efbHeight,rmode->xfbHeight);
	xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopySrc(0,0,rmode->fbWidth,rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth,xfbHeight);
	GX_SetCopyFilter(rmode->aa,rmode->sample_pattern,GX_TRUE,rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering,((rmode->viHeight==2*rmode->xfbHeight)?GX_ENABLE:GX_DISABLE));

	if (rmode->aa)
		GX_SetPixelFmt(GX_PF_RGB565_Z16, GX_ZC_LINEAR);
	else
		GX_SetPixelFmt(GX_PF_RGB8_Z24, GX_ZC_LINEAR);


	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(frameBuffer[fb],GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	// ustaw deskryptor wierzchołków
	// komunikat dla flippera, aby oczekiwał bezpośrednich danych
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XY, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);


	GX_SetNumChans(1);
	GX_SetNumTexGens(1);
	GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);


	GX_InvalidateTexAll();

	// ladowanie grafik do obiektów
	
	//ladowanie kuli
	TPLFile spriteTPL;
	TPL_OpenTPLFromMemory(&spriteTPL, (void *)ballsprites_tpl,ballsprites_tpl_size);
	TPL_GetTexture(&spriteTPL,ballsprites,&texObj);


	// to do 2: Wyszukaj w Internecie teksturę z animacją i wczytaj ją (np. Mario).




	// ---------------------------------------------------------

	guOrtho(perspective,0,479,0,639,0,300);
	GX_LoadProjectionMtx(perspective, GX_ORTHOGRAPHIC);

	WPAD_Init();


	while(1) {

		WPAD_ScanPads();

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME) exit(0);

		GX_SetViewport(0,0,rmode->fbWidth,rmode->efbHeight,0,1);
		GX_InvVtxCache();
		GX_InvalidateTexAll();

		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
		GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

		guMtxIdentity(GXmodelView2D);
		guMtxTransApply (GXmodelView2D, GXmodelView2D, 0.0F, 0.0F, -5.0F);
		GX_LoadPosMtxImm(GXmodelView2D,GX_PNMTX0);
//--------------------------------------------------------



// to do 3: Zmodyfikuj kod w taki sposób, aby tekstury były wyświetlane na zmianę w jednym punkcie 
// (każda następna zmiana koloru tekstury ballsprites po 30 klatkach).






// ---------------------------------------------------

		GX_LoadTexObj(&texObj, GX_TEXMAP0);
		drawSpriteTex( 400, 400 , 32, 32, texCoords);


// to do 4: Dodaj animację drugiej tekstury.





//---------------------------------------------------------

		GX_DrawDone();

		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
		GX_SetAlphaUpdate(GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(frameBuffer[fb],GX_TRUE);

		VIDEO_SetNextFramebuffer(frameBuffer[fb]);
		if(first_frame) {
			VIDEO_SetBlack(FALSE);
			first_frame = 0;
		}
		VIDEO_Flush();
		VIDEO_WaitVSync();
		fb ^= 1;		// zmiana bufora
	}
	return 0;
}






