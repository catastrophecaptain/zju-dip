#include "tools.h"
int main()
{
    position a;
    a=malloc(2*sizeof(double));
    a[0]=1,a[1]=1;
    //double matrix[3][3]={1,1,0,0,1,0,0,0,1};
    double matrix[3][3]={cos(PI/6),-sin(PI/6),0,sin(PI/6),cos(PI/6),0,0,0,1};
    Bmp graph1 = bmp_read(".\\a.bmp"),graph2;
    graph2=simple_matrix_transformation(graph1,matrix);
    bmp_write(graph2,".\\1.bmp");
}