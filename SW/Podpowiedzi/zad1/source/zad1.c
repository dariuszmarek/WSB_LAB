#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

#define DEFAULT_FIFO_SIZE (256 * 1024)

static void *frameBuffer[2] = {NULL, NULL};
GXRModeObj *rmode;


	// ---------------------------------------------------


ir_t ir;

//to do 1 --------------------------------------------------------------

// Stwórz zmienną potrzebną do mapowania wyświetlanych punktów



// -------------------------------------------------------------------

void wpad_initialize(GXRModeObj *rmode)
{
	WPAD_Init();
	WPAD_SetVRes(WPAD_CHAN_ALL, rmode->fbWidth, rmode->xfbHeight);
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
}

void drawQuad(int x, int y, int width, int height)
{
	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);					  // Rozpoczęcie rysowania kwadratu
	GX_Position3f32(x, y, 0.0f);						  // Pozycja lewego górnego narożnika kwadratu na scenie
	GX_Position3f32(x + width - 1, y, 0.0f);			  // Pozycja prawego górnego narożnika kwadratu na scenie
	GX_Position3f32(x + width - 1, y + height - 1, 0.0f); // Prawy dół
	GX_Position3f32(x, y + height - 1, 0.0f);			  // Lewy dół
	GX_End();											  // Zakończenie rysowania kwadratu
}

	// ---------------------------------------------------


int main(int argc, char **argv)
{
	f32 yscale;
	u32 xfbHeight;
	Mtx view;
	Mtx44 perspective;
	Mtx model, modelview;
	u32 fb = 0; // początkowy indeks bufora ramki
void *gp_fifo = NULL;
	// Kolor tła
	GXColor background = {0, 0, 0, 0xff};

	// Inicjalizacja wyświetlacza
	VIDEO_Init();
	
	
	// stworzenie domyślnego modu wideo
	rmode = VIDEO_GetPreferredMode(NULL);

	// stworzenie 2 buforów sceny
	frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	
	// ustwienie konfiguracji 
	VIDEO_Configure(rmode);
	
	// ustawienie wyswietlanego bufora sceny na ekranie
	VIDEO_SetNextFramebuffer(frameBuffer[fb]);

	// Wyłączenie czarnego obrazu sceny
	VIDEO_SetBlack(FALSE);

	// Wysiwtlenie bufora sceny na ekranie
	VIDEO_Flush();

	// Oczekiwanie na synchronizację
	VIDEO_WaitVSync();	
	if (rmode->viTVMode & VI_NON_INTERLACE)
	{
		VIDEO_WaitVSync();
	}

	
	gp_fifo = memalign(32, DEFAULT_FIFO_SIZE);
	memset(gp_fifo, 0, DEFAULT_FIFO_SIZE);
	GX_Init(gp_fifo, DEFAULT_FIFO_SIZE);
	
	// resetuje bg do koloru
	GX_SetCopyClear(background, 0x00ffffff);
	
	//GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
	yscale = GX_GetYScaleFactor(rmode->efbHeight, rmode->xfbHeight);
	 xfbHeight = GX_SetDispCopyYScale(yscale);
	GX_SetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
	GX_SetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
	GX_SetDispCopyDst(rmode->fbWidth, xfbHeight);
	GX_SetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
	GX_SetFieldMode(rmode->field_rendering, ((rmode->viHeight == 2 * rmode->xfbHeight) ? GX_ENABLE : GX_DISABLE));
	
	
	GX_SetCullMode(GX_CULL_NONE);
	GX_CopyDisp(frameBuffer[fb], GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	
		// setup the vertex descriptor
	// tells the flipper to expect direct data
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	
	GX_SetNumChans(1);
	GX_SetNumTexGens(0);

	
	GX_SetTevOp(GX_TEVSTAGE0, GX_REPLACE);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	
	GX_InvalidateTexAll();
	
	guVector cam = {0.0F, 0.0F, 0.0F},
			 up = {0.0F, 1.0F, 0.0F},
			 look = {0.0F, 0.0F, -1.0F};
	guLookAt(view, &cam, &up, &look);
	guOrtho(perspective, 0, rmode->fbWidth -1, 0, rmode->efbHeight - 1, 0, 300);
	GX_LoadProjectionMtx(perspective, GX_ORTHOGRAPHIC);




	// ---------------------------------------------------

	wpad_initialize(rmode);

	// to do 2 ---------------------------------------------------- 
	// Przydziel pamięć dla całego (mapowanego) ekranu gry. 
	// Podczas inicjalizacji oznacz wszystkie wartości jako false.
	// Podpowiedz: W jaki sposób pobierać przykładowo szerokość okna?       rmode->fbWidth
	
	
	
	// -------------------------------------------------------------


	while (1)
	{
		GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
		GX_InvVtxCache();
		GX_ClearVtxDesc();
		GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
		guMtxIdentity(model);
		guMtxTransApply(model, model, 0.0f, 0.0f, -5.0f);
		guMtxConcat(view, model, modelview);
		GX_LoadPosMtxImm(modelview, GX_PNMTX0);


	// ---------------------------------------------------
	// Rozpoczęcie skanowania położenia kontrolerów  
		WPAD_ScanPads();

		// IR Movement
		WPAD_IR(0, &ir);

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
		{
			exit(0);
		}





	// to do 3 -------------------------------------------
	// stwórz wskaźnik na odczytywane koordynaty x i y kontrolera i oznacz je jako wartość true 
	
	



	
		
		

	// to do 4 ---------------------------------------------
	// przechodząc po wszystkich elementach wyświetlanego ekranu wywołaj funkcję:
	// drawQuad(i, j, 3, 3);
	// dla wszystkich wartości true
	







	// ---------------------------------------------------


		// Wyświetlanie obrazu na scenie
		GX_DrawDone();
		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
		GX_SetAlphaUpdate(GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(frameBuffer[fb], GX_TRUE);
		VIDEO_SetNextFramebuffer(frameBuffer[fb]);
		VIDEO_Flush();
		VIDEO_WaitVSync();
		
		// zmiana bufora
		fb ^= 1; 
	}



	return 0;
}
