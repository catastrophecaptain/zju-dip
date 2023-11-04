#include "tools.h"
int main()
{
     Bmp a = bmp_read("D:\\courses\\23_1\\dip\\project1\\image\\2.bmp"), b;
    YUV yuv;
    b=visible_enhance(a);
    a=histogram_equalization(b);
    bmp_write(a, ".\\1.bmp");
    bmp_write(b, ".\\2.bmp");
}