#include "tools.h"
int main()
{
    printf("Please input the location of the image to be processed.\n");
    char source[300];
    gets(source);
    Bmp graph1 = bmp_read(source), graph2;
    graph2 = mean_filter(graph1, 3, 3);
    bmp_write(graph2, "..\\result\\mean.bmp");
    free(graph2->data);
    free(graph2);
    graph2 = laplacian_filter(graph1);
    bmp_write(graph2, "..\\result\\laplacian.bmp");
    free(graph2->data);
    free(graph2);
    free(graph1->data);
    free(graph1);
    printf("The results are stored in ..\\reslut\n");
}