#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <limits.h>
// color encode : bgr
unsigned int total_length, head_length;
// head_length is the space of data not including image data
unsigned char *graph;
// store the graph to be input and output
int *yuv_graph;
// store the yuv color data of input graph
int main()
{
    char location[501];
    // store the location of input graph
    int max_gray = INT_MIN, min_gray = INT_MAX;
    // store the range of gray in order to rearrange them
    FILE *fpt_ori, *fpt_gray, *fpt_mod;
    // store pointers point to input image , gray image and Y-modified image
    // results is stored in ./result

    printf("Please input the location of input image:\n\n");
    fgets(location, 500, stdin);
    // get the location of input image
    location[strlen(location) - 1] = 0;
    // throw the enter away
    fpt_ori = fopen(location, "rb");
    fseek(fpt_ori, 2L, SEEK_SET);
    fread(&total_length, 4, 1, fpt_ori);
    // read bfSize
    fseek(fpt_ori, 10L, SEEK_SET);
    fread(&head_length, 4, 1, fpt_ori);
    // read bfOffBits
    graph = (unsigned char *)malloc(total_length);
    fseek(fpt_ori, 0L, SEEK_SET);
    fread(graph, 1, total_length, fpt_ori);
    fclose(fpt_ori);
    // read the data of input image


    yuv_graph = (int *)malloc(sizeof(int) * total_length);
    for (int i = head_length; i < total_length; i += 4)
    {
        yuv_graph[i] = 0.114 * graph[i] + 0.587 * graph[i + 1] + 0.299 * graph[i + 2];
        yuv_graph[i + 1] = 0.435 * graph[i] - 0.289 * graph[i + 1] - 0.147 * graph[i + 2];
        yuv_graph[i + 2] = -0.100 * graph[i] - 0.515 * graph[i + 1] + 0.615 * graph[i + 2];
        // yuv_graph[i] = Y, yuv_graph[i + 1] = U, yuv_graph[i + 2] = V
        // graph[i] = b, graph[i + 1] = g, graph[i + 2] = r
        if (yuv_graph[i] > max_gray)
        {
            max_gray = yuv_graph[i];
        }
        if (yuv_graph[i] < min_gray)
        {
            min_gray = yuv_graph[i];
        }
    }
    // translate rgb to yuv and find gray range


    for (int i = head_length; i < total_length; i += 4)
    {
        graph[i] = graph[i + 1] = graph[i + 2] = (yuv_graph[i] - min_gray) / (double)(max_gray - min_gray) * 255;
    }
    // rearrange gray data
    fpt_gray = fopen(".\\result\\gray.bmp", "wb");
    fwrite(graph, 1, total_length, fpt_gray);
    fclose(fpt_gray);

    for (int i = head_length; i < total_length; i += 4)
    {
        int temp[3];
        yuv_graph[i] += 50;
        // modify Y
        yuv_graph[i] = (yuv_graph[i] > 0) ? yuv_graph[i] : 0;
        // avoid Y < 0
        temp[0] = yuv_graph[i] + 2.036 * yuv_graph[i + 1];
        temp[1] = yuv_graph[i] - 0.3954 * yuv_graph[i + 1] - 0.5805 * yuv_graph[i + 2];
        temp[2] = yuv_graph[i] + 1.1398 * yuv_graph[i + 2];
        // yuv_graph[i] = Y, yuv_graph[i + 1] = U, yuv_graph[i + 2] = V
        // temp[0] = b, temp[1] = g, temp[2] = r
        double rate = 1;
        // rate means  maximum of rgb / maximum limit if there is overflow
        if (temp[0] > 255 | temp[1] > 255 | temp[2] > 255)
        {
            int max = -1;

            for (int j = 0; j < 3; j++)
            {
                if (max < temp[j])
                {
                    max = temp[j];
                    rate = max / (float)255;
                }
            }
        }
        for (int j = 0; j < 3; j++)
        {
            graph[i + j] = (temp[j] / rate > 0) ? (temp[j] / rate) : 0;
        }
    }
    fpt_mod = fopen(".\\result\\mod.bmp", "wb");
    fwrite(graph, 1, total_length, fpt_mod);
    fclose(fpt_mod);
    printf("\nThe results are stored in .\\reslut\\gray.bmp and .\\reslut\\mod.bmp .");
}