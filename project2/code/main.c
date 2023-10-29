#include "tools.h"
int main()
{
    printf("Please input the location of the image to be processed.\n");
    char source[300];
    gets(source);
    Bmp a = bmp_read(source), b,c;
    c = rgb_to_gray_bmp(a,0);
    a = gray_to_binary_bmp(c,0);
    bmp_write(a,"..\\result\\binary_global.bmp");
    a = gray_to_binary_bmp(c,5);
    bmp_write(a,"..\\result\\binary_enhance.bmp");
    b = erosion_dilation_binary_bmp(a,1,1);
    bmp_write(b,"..\\result\\erosion.bmp");
    b = erosion_dilation_binary_bmp(a,0,1);
    bmp_write(b,"..\\result\\dilation.bmp");
    b = erosion_dilation_binary_bmp(a,1,1);
    b = erosion_dilation_binary_bmp(b,0,1);
    bmp_write(b,"..\\result\\open.bmp");
    b = erosion_dilation_binary_bmp(a,0,1);
    b = erosion_dilation_binary_bmp(b,1,1);
    bmp_write(b,"..\\result\\close.bmp");
    printf("The results are stored in ..\\reslut\n");
}