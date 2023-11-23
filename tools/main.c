#include "tools.h"
int main()
{
    Bmp graph1 = bmp_read(".\\c.bmp"),graph2;
    graph2=mean_filter(graph1,3,3);
    bmp_write(graph2,".\\1.bmp");
    free(graph2->data);
    free(graph2);
    graph2=laplacian_filter(graph1);
    bmp_write(graph2,".\\2.bmp");
    //free(graph2->data);
    //free(graph2);
    graph2=mean_filter(graph2,3,3);
    bmp_write(graph2,".\\3.bmp");
    free(graph2->data);
    free(graph2);
    free(graph1->data);
    free(graph1);
}