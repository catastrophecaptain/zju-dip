#include "tools.h"
Bmp bmp_read(char *source)
{
    FILE *fpt = fopen(source, "rb");
    Bmp ret = malloc(sizeof(struct Bmp));
    fread(&(ret->bfType), 1, 2, fpt);
    fread(&(ret->bfSize), 4, 1, fpt);
    fread(&(ret->bfReserved1), 2, 1, fpt);
    fread(&(ret->bfReserved2), 2, 1, fpt);
    fread(&(ret->bfOffBits), 4, 1, fpt);
    fread(&(ret->biSize), 4, 1, fpt);
    fread(&(ret->biWidth), 4, 1, fpt);
    fread(&(ret->biHeight), 4, 1, fpt);
    fread(&(ret->biPlanes), 2, 1, fpt);
    fread(&(ret->biBitCount), 2, 1, fpt);
    fread(&(ret->biCompression), 4, 1, fpt);
    fread(&(ret->biSizeImage), 4, 1, fpt);
    fread(&(ret->biXPelsPerMeter), 4, 1, fpt);
    fread(&(ret->biYPelsPerMeter), 4, 1, fpt);
    fread(&(ret->biClrUsed), 4, 1, fpt);
    fread(&(ret->biClrImportant), 4, 1, fpt);
    ret->Palette = malloc(ret->bfOffBits - 54);
    fread((uint8_t *)ret->Palette, 1, ret->bfOffBits - 54, fpt);
    ret->data = malloc(ret->bfSize - ret->bfOffBits);
    fread(ret->data, 1, ret->bfSize - ret->bfOffBits, fpt);
    fclose(fpt);
    return ret;
}
Bmp rgb_to_gray_bmp(Bmp rgb,int isextend)
{
    int *yuv_graph = (int *)malloc(sizeof(int) * (rgb->bfSize - rgb->bfOffBits));
    Bmp rgb_gray = malloc(sizeof(struct Bmp));
    *rgb_gray = *rgb;
    rgb_gray->Palette = malloc(256 * 4);
    for (int i = 0; i < 256; i++)
    {
        rgb_gray->Palette[i][0] = rgb_gray->Palette[i][1] = rgb_gray->Palette[i][2] = i;
        rgb_gray->Palette[i][3] = 0;
        rgb_gray->bfOffBits += 4;
    }
    rgb_gray->biBitCount = 8;
    uint32_t width = (rgb_gray->biWidth * 8 + 31) / 32 * 4; // width must be 4*n
    rgb_gray->bfSize = rgb_gray->bfOffBits + width * rgb_gray->biHeight;
    rgb_gray->biSizeImage = rgb_gray->bfSize - rgb_gray->bfOffBits;
    rgb_gray->data = malloc(rgb_gray->bfSize - rgb_gray->bfOffBits);
    int max_gray = INT_MIN, min_gray = INT_MAX;
    for (int i = 0; i < rgb->bfSize - rgb->bfOffBits; i += 4)
    {
        yuv_graph[i] = 0.114 * rgb->data[i] + 0.587 * rgb->data[i + 1] + 0.299 * rgb->data[i + 2];
        yuv_graph[i + 1] = 0.435 * rgb->data[i] - 0.289 * rgb->data[i + 1] - 0.147 * rgb->data[i + 2];
        yuv_graph[i + 2] = -0.100 * rgb->data[i] - 0.515 * rgb->data[i + 1] + 0.615 * rgb->data[i + 2];
        // yuv_graph[i] = Y, yuv_graph[i + 1] = U, yuv_graph[i + 2] = V
        // rgb_gray->data[i] = b, rgb_gray->data[i + 1] = g, rgb_gray->data[i + 2] = r
        if (yuv_graph[i] > max_gray)
        {
            max_gray = yuv_graph[i];
        }
        if (yuv_graph[i] < min_gray)
        {
            min_gray = yuv_graph[i];
        }
    }
    // translate rgb_gray to yuv and find gray range
    for (int i = 0; i < rgb_gray->biHeight; i++)
    {
        for (int j = 0; j < rgb_gray->biWidth; j++)
        {
            if(isextend==1)
            {
            rgb_gray->data[width * i + j] = (yuv_graph[4 * (rgb_gray->biWidth * i + j)] - min_gray) / (double)(max_gray - min_gray) * 255;
            }else
            {
                rgb_gray->data[width * i + j] = yuv_graph[4 * (rgb_gray->biWidth * i + j)];
            }
        }
    }
    return rgb_gray;
}
void thershold_divide_bmp(Bmp binary, Bmp gray, int start_h, int start_w, int height, int width)
{
    // divide gray[start_h~start_h+height][start_w~start_w+width] into binary
    uint32_t width_gray = (binary->biWidth * 8 + 31) / 32 * 4;
    long long int hash[256][2] = {0}, sum = 0, max_gray = -1, min_gray = 266;
    double average = 0;
    for (int i = start_h; i < start_h + height; i++)
    {
        for (int j = start_w; j < start_w + width; j++)
        {
            hash[gray->data[i * width_gray + j]][0]++;
            sum++;
            average += gray->data[i * width_gray + j];
            max_gray = (gray->data[i * width_gray + j] > max_gray) ? gray->data[i * width_gray + j] : max_gray;
            min_gray = (gray->data[i * width_gray + j] < min_gray) ? gray->data[i * width_gray + j] : min_gray;
        }
    }
    // use hash to store the gray information
    average /= sum;
    hash[min_gray][1] = min_gray*hash[min_gray][0];
    double max = -1;
    int thershold = min_gray;
    int flag=(width==binary->biWidth&&height==binary->biHeight)?0:5;
    // to avoid noise
    for (int i = min_gray + 1; i < max_gray + 1; i++)
    {
        hash[i][1] = i * hash[i][0] + hash[i - 1][1];
        hash[i][0] += hash[i - 1][0];
        double rate = (double)hash[i - 1][0] / sum, average_min = (double)hash[i - 1][1] / hash[i - 1][0], average_max = (double)((long long)sum * average - hash[i - 1][1]) / (sum - hash[i - 1][0]);
        double temp = rate * (1 - rate) * pow(average_min - average_max, 2);
        if (max < temp)
        {
            max = temp;
            thershold = i;
        }
    }
    // find the thershold with the max gap
    for (int i = start_h; i < start_h+height; i++)
    {
        for (int j = start_w; j < start_w+width; j++)
        {
            if (gray->data[i * width_gray + j] >= thershold+flag)
            {
                binary_assign_bmp(binary, i, j, 1);
            }
            else if(gray->data[i * width_gray + j] < thershold-flag)
            {
                binary_assign_bmp(binary, i, j, 0);
            }
        }
    }
    // binary
}
Bmp gray_to_binary_bmp(Bmp gray, int window_size)
{
    // window_size: use window_size*window_size local window to calculate the
    // local thershold, if windows_size is zero, use global thershold
    Bmp binary = malloc(sizeof(struct Bmp));
    *binary = *gray;
    binary->Palette = malloc(8);
    binary->Palette[1][3] = binary->Palette[0][0] = binary->Palette[0][1] =
        binary->Palette[0][2] = binary->Palette[0][3] = 0;
    binary->Palette[1][0] = binary->Palette[1][1] = binary->Palette[1][2] = 255;
    binary->bfOffBits = 54 + 8, binary->biBitCount = 1;
    uint32_t width = (binary->biWidth + 31) / 32 * 4;
    // four byte alignment
    binary->bfSize = binary->bfOffBits + width * binary->biHeight;
    binary->data = malloc(binary->bfSize - binary->bfOffBits);
    // iniltialize the
    thershold_divide_bmp(binary, gray, 0, 0, binary->biHeight, binary->biWidth);
    if(window_size==0)return binary;
    // if window_size=0 , just global binary.
    for (int k1 = 0; k1 + window_size < binary->biHeight; k1 += window_size/2)
    {
        for (int k2 = 0; k2 + window_size < binary->biWidth; k2 += window_size/2)
        {
            thershold_divide_bmp(binary, gray, k1, k2, window_size, window_size);
        }
    }
    return binary;
}
int binary_get_bmp(Bmp binary, int row, int column)
{
    // get binary[row,column]
    int width = (binary->biWidth + 31) / 32 * 4;
    uint8_t temp = 0;
    int ret = -1;
    if (row < binary->biHeight && row >= 0 && column < binary->biWidth && column >= 0)
    {
        temp = binary->data[row * width + column / 8];
        temp <<= column % 8;
        temp >>= 7;
        ret = temp;
    }
    return ret;
}
void binary_assign_bmp(Bmp binary, int row, int column, int assign)
{
    //set binary[row,column]
    int width = (binary->biWidth + 31) / 32 * 4;
    if (assign == 1)
    {
        binary->data[row * width + column / 8] |= ((uint8_t)1) << (7 - column % 8);
    }
    else
    {
        binary->data[row * width + column / 8] &= ~((uint8_t)1) << (7 - column % 8);
    }
}
Bmp erosion_dilation_binary_bmp(Bmp binary, int mode, int size)
{
    // mode 0 erosion , mode 1 dialtion
    // size: use size*size square as the basic square
    Bmp ret = malloc(sizeof(struct Bmp));
    *ret = *binary;
    ret->Palette = malloc(8);
    ret->data = malloc(ret->bfSize - ret->bfOffBits);
    memccpy(ret->Palette, binary->Palette, 1, 8);
    for (int i = 0; i < binary->biHeight; i++)
    {
        for (int j = 0; j < binary->biWidth; j++)
        {
            int flag = 1;
            for (int k1 = -size; k1 <= size; k1++)
            {
                for (int k2 = -size; k2 <= size; k2++)
                {
                    if (binary_get_bmp(binary, i + k1, j + k2) == mode)
                    {
                        flag = 0;
                        break;
                        break;
                    }
                }
            }
            if (flag == 1)
            {
                binary_assign_bmp(ret, i, j, binary_get_bmp(binary, i, j));
            }
            else
            {
                binary_assign_bmp(ret, i, j, mode);
            }
        }
    }
    return ret;
}
Bmp bmp_write(Bmp bmp, char *purpose)
{
    FILE *fpt = fopen(purpose, "wb");
    fwrite(&(bmp->bfType), 1, 2, fpt);
    fwrite(&(bmp->bfSize), 1, 52, fpt);
    fwrite(bmp->Palette, 1, bmp->bfOffBits - 54, fpt);
    fwrite(bmp->data, 1, bmp->bfSize - bmp->bfOffBits, fpt);
    fclose(fpt);
}
