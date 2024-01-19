#include "tools.h"
int main()
{
    printf("Please input the location of the image to be processed.\n");
    char source[300];
    gets(source);
    Bmp graph1 = bmp_read(source), graph2;
    graph2 = bilateral_filter(graph1, 51, 51, 12.5, 30); 
    bmp_write(graph2, "..\\result\\bilateral.bmp"); 
    free(graph2->data);
    free(graph2);
    free(graph1->data);
    free(graph1);
    printf("The results are stored in ..\\reslut\n");
}