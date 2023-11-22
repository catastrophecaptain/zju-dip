#include "tools.h"
int main()
{
    // double matrix[3][3]={1,1,0,0,1,0,0,0,1};
    printf("Please input the location of the image to be processed.\n");
    char source[300];
    gets(source);
    Bmp graph1 = bmp_read(source), graph2;
    // double matrix[3][3]={cos(PI/6),-sin(PI/6),0,sin(PI/6),cos(PI/6),0,0,0,1};
    double translation[3][3] = {1, 0, 100, 0, 1, 100, 0, 0, 1};
    graph2 = simple_matrix_transformation(graph1, translation);
    // need to manually delete offset
    bmp_write(graph2, "..\\result\\translation.bmp");
    int *a = malloc(100000000);
    free(a);
    free(graph2->data);
    free(graph2);
    double rotation[3][3] = {cos(PI / 6), -sin(PI / 6), 0, sin(PI / 6), cos(PI / 6), 0, 0, 0, 1};
    graph2 = simple_matrix_transformation(graph1, rotation);
    bmp_write(graph2, "..\\result\\rotation.bmp");
    free(graph2->data);
    free(graph2);
    double scale[3][3] = {2, 0, 0, 0, 0.5, 0, 0, 0, 1};
    graph2 = simple_matrix_transformation(graph1, scale);
    bmp_write(graph2, "..\\result\\scale.bmp");
    free(graph2->data);
    free(graph2);
    double shear[3][3] = {1, 1, 0, 0, 1, 0, 0, 0, 1};
    graph2 = simple_matrix_transformation(graph1, shear);
    bmp_write(graph2, "..\\result\\shear.bmp");
    free(graph2->data);
    free(graph2);
    double mirror[3][3] = {-1, 0, 0, 0, 1, 0, 0, 0, 1};
    graph2 = simple_matrix_transformation(graph1, mirror);
    bmp_write(graph2, "..\\result\\mirror.bmp");
    free(graph2->data);
    free(graph2);
    printf("The results are stored in ..\\reslut\n");
}