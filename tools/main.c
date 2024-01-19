#include "tools.h"
int main()
{
    Bmp graph1 = bmp_read("D:\\courses\\23_1\\dip\\tools\\3.bmp"),graph2;
    graph2=rgb_to_gray_bmp(graph1,1);
    graph2=gray_to_binary_bmp(graph2,10);
    //graph2=bilateral_filter(graph1,51,51,12.5,30);
    /*for(int i=0;i<10;i++)
    {
        graph2=bilateral_filter(graph2,3,3,3,3);
    }*/
    bmp_write(graph2,".\\1.bmp");
    free(graph2->data);
    free(graph2);
    //graph2=laplacian_filter(graph1);
    //bmp_write(graph2,".\\2.bmp");
    //free(graph2->data);
    //free(graph2);
    //graph2=mean_filter(graph2,3,3);
    //bmp_write(graph2,".\\3.bmp");
    //free(graph2->data);
    //free(graph2);
    free(graph1->data);
    free(graph1);
}