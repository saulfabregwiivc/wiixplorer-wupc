/****************************************************************************
 * libwiigui Template
 * Tantric 2009
 *
 * video.h
 * Video routines
 ***************************************************************************/

#ifndef _VIDEO_H_
#define _VIDEO_H_

#include <ogcsys.h>

void InitVideo ();
void StopGX();
void ResetVideo_Menu();
void Menu_Render();
u8 * Video_GetFrame(int * width, int * height);
void Menu_DrawImg(u8 data[], u16 width, u16 height, u8 format, f32 xpos, f32 ypos, f32 zpos, f32 degrees, f32 scaleX, f32 scaleY, u8 alpha);
void Menu_DrawImgCut(u8 data[], u16 width, u16 height, u8 format, f32 xpos, f32 ypos, f32 zpos, f32 degrees, f32 scaleX, f32 scaleY, u8 alpha, f32 minwidth, f32 maxwidth, f32 minheight, f32 maxheight);
void Menu_DrawRectangle(f32 x, f32 y, f32 z, f32 width, f32 height, GXColor * color, bool multicolor, bool filled);

#endif
