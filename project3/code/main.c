#include "tools.h"
int main()
{

    printf("Please input the location of the image to be processed.\n");
    char source[300];
    gets(source);
    Bmp a = bmp_read(source), b;
    YUV yuv;
    b = visible_enhance(a);
    a = histogram_equalization(b);
    bmp_write(b, "..\\result\\visible.bmp");
    bmp_write(a, "..\\result\\histogram.bmp");
    printf("The results are stored in ..\\reslut\n");
}