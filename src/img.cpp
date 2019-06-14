#include "img.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


static stbtt_fontinfo fontInfo;
static Font font = {};
#define FONT_TEST_IMAGE_WIDTH        512
#define FONT_TEST_IMAGE_HEIGHT       256
static unsigned char image[FONT_TEST_IMAGE_HEIGHT][FONT_TEST_IMAGE_WIDTH];


// TODO(michalc): move to loadAsset.cpp
void
LoadFont(char* path)
{
  unsigned char* fontData;
  fontData = LoadEntireFile(path);
  // NOTE(mc): there might be more than one font in a file so stbtt_GetFontOffsetForIndex
  // allows you to choose which one you want
  stbtt_InitFont(&fontInfo,
                 (const unsigned char*)fontData,
                 stbtt_GetFontOffsetForIndex((const unsigned char*)fontData, 0)
                 );



  // for the whole font
  font.size = 48;
  font.scaleFactor = stbtt_ScaleForPixelHeight(&fontInfo, font.size);
  stbtt_GetFontVMetrics(&fontInfo, &font.ascent, &font.descent, &font.lineGap);
  int bBoxx0, bBoxx1, bBoxy0, bBoxy1;
  stbtt_GetFontBoundingBox(&fontInfo, &bBoxx0, &bBoxy0, &bBoxx1, &bBoxy1);
}

void
GetTextImage(char* text)
{
  int ch = 0;
  int advance, lsb, x0, y0, x1, y1;
  real32 xpos = 20.0f;  // initial horizontal position
  int baseline = (int)(font.ascent * font.scaleFactor);

  while (text[ch]) {
    real32 x_shift = xpos - (real32)floor(xpos);
    stbtt_GetCodepointHMetrics(&fontInfo, text[ch], &advance, &lsb);
    stbtt_GetCodepointBitmapBoxSubpixel(&fontInfo, text[ch],
                                        font.scaleFactor, font.scaleFactor,
                                        x_shift, 0, &x0, &y0, &x1, &y1);
    stbtt_MakeCodepointBitmapSubpixel(&fontInfo,
                                      &image[baseline + y0][(int)xpos+x0],
                                      x1 - x0, y1 - y0,
                                      FONT_TEST_IMAGE_WIDTH,
                                      font.scaleFactor, font.scaleFactor,
                                      x_shift, 0, text[ch]);

    xpos += (advance * font.scaleFactor);
    if (text[ch+1])
    {
      xpos += font.scaleFactor * stbtt_GetCodepointKernAdvance(&fontInfo,
                                                               text[ch],
                                                               text[ch+1]);
    }
    ++ch;

  }  // <-- EO while(text[ch])

  stbi_write_png("image.png", FONT_TEST_IMAGE_WIDTH, FONT_TEST_IMAGE_HEIGHT, 1, image, 0);
}
