#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <time.h>
#define DEFAULT_FIFO_SIZE (256 * 1024)

static void *frameBuffer[2] = {NULL, NULL};
GXRModeObj *rmode;

// ---------------------------------------------------

// ---------------------------------------------------
//podpowiedz - skopiuj podaną w instrukcji tablicę kolorów oraz stwórz strukturę umożliwiającą przechowywanie countera oraz koloru, nazwij ją: point



// ---------------------------------------------------

ir_t ir;

//static point *map;

void wpad_initialize(GXRModeObj *rmode)
{
	WPAD_Init();
	WPAD_SetVRes(WPAD_CHAN_ALL, rmode->fbWidth, rmode->xfbHeight);
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
}

void drawQuad(int x, int y, int width, int height, f32 color[3])
{

	GX_Begin(GX_QUADS, GX_VTXFMT0, 4); // Rozpoczęcie rysowania kwadratu

	GX_Position3f32(x, y, 0.0f); // Pozycja lewego górnego narożnika kwadratu na scenie
	GX_Color3f32(color[0], color[1], color[2]);

	GX_Position3f32(x + width - 1, y, 0.0f); // Pozycja prawego górnego narożnika kwadratu na scenie
	GX_Color3f32(color[0], color[1], color[2]);

	GX_Position3f32(x + width - 1, y + height - 1, 0.0f); // Prawy dół
	GX_Color3f32(color[0], color[1], color[2]);

	GX_Position3f32(x, y + height - 1, 0.0f); // Lewy dół
	GX_Color3f32(color[0], color[1], color[2]);

	GX_End(); // Zakończenie rysowania kwadratu
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
	// Kolor tła
	GXColor background = {0, 0, 0, 0xff};

	// Inicjalizacja wyświetlacza
	VIDEO_Init();
	//WPAD_Init();

	rmode = VIDEO_GetPreferredMode(NULL);

	// przydziel 2 bufory ramki dla podwójnego buforowania
	frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));

	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(frameBuffer[fb]);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if (rmode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync();

	// skonfiguruj fifo, a następnie uruchom flipper
	void *gp_fifo = NULL;
	gp_fifo = memalign(32, DEFAULT_FIFO_SIZE);
	memset(gp_fifo, 0, DEFAULT_FIFO_SIZE);

	GX_Init(gp_fifo, DEFAULT_FIFO_SIZE);

	// resetuje bg do koloru
	GX_SetCopyClear(background, 0x00ffffff);

	// pozostałe konfiguracje gx
	GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
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

	// ustaw deskryptor wierzchołków
	// oczekuj bezpośrednich danych
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);

	// skonfiguruj tabelę atrybutów wierzchołków
	// opisz dane	
	// args: lokalizacja 0-7, rodzaj danych, format danych, rozmiar, skala
	// na przykład w pierwszym połączeniu przesyłamy dane pozycji
	// 3 wartości X, Y, Z o rozmiarze F32. Skala określa liczbę ułamkową
	// bity dla danych innych niż zmiennoprzecinkowe.
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGB8, 0);

	GX_SetNumChans(1);
	GX_SetNumTexGens(0);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

	// konfiguracja kamery
	// looking down the -z axis with y up
	guVector cam = {0.0F, 0.0F, 0.0F},
			 up = {0.0F, 1.0F, 0.0F},
			 look = {0.0F, 0.0F, -1.0F};
	guLookAt(view, &cam, &up, &look);

	guOrtho(perspective, 0, rmode->fbWidth, 0, rmode->efbHeight, 0, 300);
	GX_LoadProjectionMtx(perspective, GX_ORTHOGRAPHIC);

	// ---------------------------------------------------

	wpad_initialize(rmode);
	
	
	
	
	
	
	
	

	// ---------------------------------------------------

	while (1)
	{

		// ---------------------------------------------------

		WPAD_ScanPads();

		// IR MovementW
		WPAD_IR(0, &ir);

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
		{
			exit(0);
		}

		// ---------------------------------------------------

		// pozostałe konfiguracje gx
		GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);

		guMtxIdentity(model);

		// ---------------------------------------------------



		
		// ---------------------------------------------------

		guMtxTransApply(model, model, 0.0f, 0.0f, 0.0f);
		guMtxConcat(view, model, modelview);
		// załaduj modelview do pamięci macierzy
		GX_LoadPosMtxImm(modelview, GX_PNMTX0);

	
	
	
	
	
	
	

		// ---------------------------------------------------

		// Wyświetlanie obrazu na scenie
		GX_DrawDone();

		fb ^= 1; // zmiana bufora
		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(frameBuffer[fb], GX_TRUE);

		VIDEO_SetNextFramebuffer(frameBuffer[fb]);

		VIDEO_Flush();

		VIDEO_WaitVSync();
	}

	return 0;
}
