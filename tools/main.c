#include "tools.h"
int main()
{
     Bmp a = bmp_read("C:\\Users\\35264\\Desktop\\IMG_20231029_125706.bmp"), b;
    // Bmp a = bmp_read("..\\project2\\image\\1.bmp"), b;
    b = rgb_to_gray_bmp(a);
    b = gray_to_binary_bmp(b, 10);
    a = erosion_dilation_binary_bmp(b, 1, 1);
    //a = erosion_dilation_binary_bmp(a, 0, 1);
    // for(int i=0;i<10;i++){
    // a = erosion_dilation_binary_bmp(a, 0, 1);
    // a = erosion_dilation_binary_bmp(a, 1, 1);}
    bmp_write(a, ".\\1.bmp");
    bmp_write(b, ".\\2.bmp");
}