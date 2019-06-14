#ifndef IMG_H
#define IMG_H

struct Font {
  int32 ascent;
  int32 descent;
  int32 lineGap;
  real32 scaleFactor;
  uint32 size;
};

void LoadFont(char* path);
void GetTextImage(char* text); 

#endif
