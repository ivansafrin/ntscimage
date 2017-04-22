
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "snes_ntsc.h"

uint16_t drawRGB24toRGB565(uint8_t r, uint8_t g, uint8_t b)
{
	return ((r / 8) << 11) | ((g / 4) << 5) | (b / 8);
}

int main(int argc, char **argv) {
	
	if(argc < 3) {
		printf("Usage: ntscimage [-stcab] inimage outimage.png\n");
		printf("inimage can be most common image formats, outimage is always 24-bit PNG.\n");
		printf("\n\n");
		return 1;
	}
	printf("Reading %s\n", argv[1]);
	int w,h,comp;
	unsigned char* image = stbi_load(argv[1], &w, &h, &comp, STBI_rgb);
	if(image == NULL) {
		printf("Unable to open file!\n");
		return 1;
	}
	
	snes_ntsc_t* ntsc = (snes_ntsc_t*) malloc( sizeof (snes_ntsc_t) );

	snes_ntsc_setup_t setup = snes_ntsc_composite;
	memset((void*)&setup, 0, sizeof(snes_ntsc_composite));

	setup.sharpness = 0.3;
//	setup.saturation = 0.1;
//	setup.contrast = 0.2;
	setup.artifacts = -0.5;
	setup.bleed = -0.9;

	snes_ntsc_init(ntsc, &setup );
	int burst_phase = 0;
	burst_phase = 1;
	
	uint16_t *reduced = malloc(w * h * 2);
	for(int i=0; i < w * h; i++) {
		reduced[i] = drawRGB24toRGB565(image[(i*3)+2], image[(i*3)+1], image[(i*3)+0]);
	}	

	int outWidth = SNES_NTSC_OUT_WIDTH(w);
	int outHeight = h;

	unsigned char *out = malloc(outWidth * outHeight * 4);	
	snes_ntsc_blit(ntsc, reduced, w, burst_phase, w, h, out, outWidth * 4);

	unsigned char *outImage = malloc(outWidth * outHeight * 2 * 3);

	int outPitch = outWidth * 3;

	for(int y = h-1; y >=0; --y) {
		unsigned char const* in = out + (y * (outWidth * 4));
		unsigned char* pout = outImage + (y * 2 * outPitch);
		for(int n = outWidth; n >= 0; --n) {
			uint32_t prev = *(uint32_t*) in;
			uint32_t next = *(uint32_t*) (in + (outWidth * 4));
			uint32_t mixed = prev + next + ((prev ^ next) & 0x0821);
			uint32_t mx = (mixed >> 1)- (mixed >> 2 & 0x18);
			unsigned char *m = (unsigned char *)&mx;
			unsigned char *p = (unsigned char *)&prev;
			pout[0] = p[0];
			pout[1] = p[1];
			pout[2] = p[2];
			
			pout[0+outPitch] = m[0];
			pout[1+outPitch] = m[1];
			pout[2+outPitch] = m[2];

			in += 4;
			pout += 3;
		}
	}

	if(!stbi_write_png(argv[2], outWidth, outHeight * 2, 3, outImage, 0)) {
		printf("Unable to save image\n");
		return 1;
	}
}
