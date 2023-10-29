#ifndef _TOOLS_H_
#define _TOOLS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <limits.h>
struct Bmp
{
    char bfType[2];
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
    uint8_t (*Palette)[4];
    uint8_t *data;
};
typedef struct Bmp *Bmp;
Bmp bmp_read(char * source);
Bmp rgb_to_gray_bmp(Bmp bgr);
Bmp gray_to_binary_bmp(Bmp gray,int window_size);
void thershold_divide_bmp(Bmp binary, Bmp gray, int start_h, int start_w, int height, int width);
Bmp erosion_dilation_binary_bmp(Bmp binary,int mode,int size);
int binary_get_bmp(Bmp binary , int column ,int row);
void binary_assign_bmp(Bmp binary,int column,int row,int assign);
Bmp bmp_write(Bmp bmp,char*purpose);
#endif