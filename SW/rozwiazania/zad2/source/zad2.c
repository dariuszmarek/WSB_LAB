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

static int *map;

void wpad_initialize(GXRModeObj *rmode)
{
	WPAD_Init();
	WPAD_SetVRes(WPAD_CHAN_ALL, rmode->fbWidth, rmode->xfbHeight);
	WPAD_SetDataFormat(WPAD_CHAN_0, WPAD_FMT_BTNS_ACC_IR);
}

void drawQuad(int x, int y, int width, int height)
{

	GX_Begin(GX_QUADS, GX_VTXFMT0, 4);					  // Draw A Quad
	GX_Position3f32(x, y, 0.0f);						  // Top Left
	GX_Position3f32(x + width - 1, y, 0.0f);			  // Top Right
	GX_Position3f32(x + width - 1, y + height - 1, 0.0f); // Bottom Right
	GX_Position3f32(x, y + height - 1, 0.0f);			  // Bottom Left
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
	u32 fb = 0; // initial framebuffer index
	GXColor background = {0, 0, 0, 0xff};
	VIDEO_Init();
	rmode = VIDEO_GetPreferredMode(NULL);
	frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	VIDEO_Configure(rmode);
	VIDEO_SetNextFramebuffer(frameBuffer[fb]);
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if (rmode->viTVMode & VI_NON_INTERLACE)
		VIDEO_WaitVSync();
	void *gp_fifo = NULL;
	gp_fifo = memalign(32, DEFAULT_FIFO_SIZE);
	memset(gp_fifo, 0, DEFAULT_FIFO_SIZE);
	GX_Init(gp_fifo, DEFAULT_FIFO_SIZE);
	GX_SetCopyClear(background, 0x00ffffff);
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
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GX_SetNumChans(1);
	GX_SetNumTexGens(0);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	guVector cam = {0.0F, 0.0F, 0.0F},
			 up = {0.0F, 1.0F, 0.0F},
			 look = {0.0F, 0.0F, -1.0F};
	guLookAt(view, &cam, &up, &look);
	guOrtho(perspective, 0, rmode->fbWidth, 0, rmode->efbHeight, 0, 300);
	GX_LoadProjectionMtx(perspective, GX_ORTHOGRAPHIC);




	// ---------------------------------------------------

	wpad_initialize(rmode);

	map = (int *)malloc(rmode->fbWidth * rmode->efbHeight * sizeof(int));

	for (int i = 0; i < rmode->fbWidth; i++)
	{
		for (int j = 0; j < rmode->efbHeight; j++)
		{
			*(map + i * rmode->efbHeight + j) = 0;
		}
	}

	// ---------------------------------------------------


	while (1)
	{

	// ---------------------------------------------------

		WPAD_ScanPads();

		// IR Movement
		WPAD_IR(0, &ir);

		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
		{
			exit(0);
		}

	// ---------------------------------------------------


		// do this before drawing
		GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);

		guMtxIdentity(model);

	// ---------------------------------------------------

		// Obliczenie pozycji wskaznika (WPAD) w tablicy jedno wymiarowej z danych
		// tablicy 2 wymiarowej (Ekran) i ustawienie wartości licznika na 120
		*(map + (int)ir.x * rmode->efbHeight + (int)ir.y) = 120;

	// ---------------------------------------------------

		guMtxTransApply(model, model, 0.0f, 0.0f, 0.0f);
		guMtxConcat(view, model, modelview);
		// load the modelview matrix into matrix memory
		GX_LoadPosMtxImm(modelview, GX_PNMTX0);

	// ---------------------------------------------------



		// Przejście po tablicy jednowymiarowej i wyswietlanie kwadratu 3x3 
		// jezeli w tablicy (tablica przechowuje liczniki) znajduje sie wartosc wieksza od 0
		// nastpęnie dekrementacja wartości licznika
		for (int i = 1; i < rmode->fbWidth; i++)
		{
			for (int j = 1; j < rmode->efbHeight; j++)
			{
				if ( *(map + i * rmode->efbHeight + j) > 0 )
				{
					drawQuad(i, j, 3, 3);
					*(map + i * rmode->efbHeight + j) =*(map + i * rmode->efbHeight + j) - 1;
				}
			}
		}

	// ---------------------------------------------------


		// do this stuff after drawing
		GX_DrawDone();

		fb ^= 1; // flip framebuffer
		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(frameBuffer[fb], GX_TRUE);

		VIDEO_SetNextFramebuffer(frameBuffer[fb]);

		VIDEO_Flush();

		VIDEO_WaitVSync();
	}



	return 0;
}
