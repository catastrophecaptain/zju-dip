#ifndef _TOOLS_H_
#define _TOOLS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <limits.h>
#define PI 3.14159265
struct Bmp
{
    char bfType[2];
    uint32_t bfSize;//
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;//
    uint32_t biSize;//
    uint32_t biWidth;//
    uint32_t biHeight;//
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
typedef int (*YUV)[3];
typedef struct Bmp *Bmp;
typedef double *position;
Bmp bmp_read(char *source);
Bmp rgb_to_gray_bmp(Bmp bgr, int isextend);
Bmp gray_to_binary_bmp(Bmp gray, int window_size);
void thershold_divide_bmp(Bmp binary, Bmp gray, int start_h, int start_w, int height, int width, int tol);
Bmp erosion_dilation_binary_bmp(Bmp binary, int mode, int size);
int binary_get_bmp(Bmp binary, int column, int row);
void binary_assign_bmp(Bmp binary, int column, int row, int assign);
Bmp bmp_write(Bmp bmp, char *purpose);
Bmp yuv_to_rbg(Bmp origin, YUV yuv);
YUV rgb_to_yuv(Bmp rgb);
Bmp visible_enhance(Bmp source);
Bmp histogram_equalization(Bmp source);
position get_transformation(position point, double matrix[3][3]);
Bmp simple_matrix_transformation(Bmp source, double matrix[3][3]);
#endif