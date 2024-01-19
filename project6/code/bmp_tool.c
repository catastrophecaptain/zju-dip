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
Bmp rgb_to_gray_bmp(Bmp rgb, int isextend)
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
            if (isextend == 1)
            {
                rgb_gray->data[width * i + j] = (yuv_graph[4 * (rgb_gray->biWidth * i + j)] - min_gray) / (double)(max_gray - min_gray) * 255;
            }
            else
            {
                rgb_gray->data[width * i + j] = yuv_graph[4 * (rgb_gray->biWidth * i + j)];
            }
        }
    }
    return rgb_gray;
}
void thershold_divide_bmp(Bmp binary, Bmp gray, int start_h, int start_w, int height, int width, int tol)
{
    // tol to avoid small noise in partial picture.
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
    average /= sum;
    hash[min_gray][1] = min_gray * hash[min_gray][0];
    double max = -1;
    int thershold = min_gray;
    int flag = (width == binary->biWidth && height == binary->biHeight) ? 0 : tol;
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
    for (int i = start_h; i < start_h + height; i++)
    {
        for (int j = start_w; j < start_w + width; j++)
        {
            if (gray->data[i * width_gray + j] >= thershold + flag)
            {
                binary_assign_bmp(binary, i, j, 1);
            }
            else if (gray->data[i * width_gray + j] < thershold - flag)
            {
                binary_assign_bmp(binary, i, j, 0);
            }
        }
    }
}
Bmp gray_to_binary_bmp(Bmp gray, int window_size)
{
    // if window_size=0 , just global binary.
    Bmp binary = malloc(sizeof(struct Bmp));
    *binary = *gray;
    binary->Palette = malloc(8);
    binary->Palette[1][3] = binary->Palette[0][0] = binary->Palette[0][1] =
        binary->Palette[0][2] = binary->Palette[0][3] = 0;
    binary->Palette[1][0] = binary->Palette[1][1] = binary->Palette[1][2] = 255;
    binary->bfOffBits = 54 + 8, binary->biBitCount = 1;
    uint32_t width = (binary->biWidth + 31) / 32 * 4;
    binary->bfSize = binary->bfOffBits + width * binary->biHeight;
    binary->data = malloc(binary->bfSize - binary->bfOffBits);
    thershold_divide_bmp(binary, gray, 0, 0, binary->biHeight, binary->biWidth, 5);
    if (window_size == 0)
        return binary;
    // if window_size=0 , just global binary.
    for (int k1 = 0; k1 + window_size < binary->biHeight; k1 += window_size / 2)
    {
        for (int k2 = 0; k2 + window_size < binary->biWidth; k2 += window_size / 2)
        {
            thershold_divide_bmp(binary, gray, k1, k2, window_size, window_size, 5);
        }
    }
    return binary;
}
int binary_get_bmp(Bmp binary, int row, int column)
{
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
                    /*if(k1==k2&&k1!=0)
                    {
                        continue;
                    }*/
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
Bmp yuv_to_rbg(Bmp origin, YUV yuv)
{
    Bmp ret = (Bmp)malloc(sizeof(struct Bmp));
    *ret = *origin;
    ret->data = malloc(4 * origin->biHeight * origin->biWidth);
    for (int i = 0; i < origin->biWidth * origin->biHeight; i++)
    {
        int temp[3];
        temp[0] = yuv[i][0] + 2.036 * yuv[i][1];
        temp[1] = yuv[i][0] - 0.3954 * yuv[i][1] - 0.5805 * yuv[i][2];
        temp[2] = yuv[i][0] + 1.1398 * yuv[i][2];
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
            ret->data[i * 4 + j] = (temp[j] / rate > 0) ? (temp[j] / rate) : 0;
        }
    }
    return ret;
}
YUV rgb_to_yuv(Bmp rgb)
{
    YUV yuv = (YUV)malloc(rgb->biWidth * rgb->biHeight * 3 * sizeof(int));
    for (int i = 0; i < rgb->biHeight * rgb->biWidth; i++)
    {
        yuv[i][0] = 0.114 * rgb->data[i * 4] + 0.587 * rgb->data[i * 4 + 1] + 0.299 * rgb->data[i * 4 + 2];
        yuv[i][1] = 0.435 * rgb->data[i * 4] - 0.289 * rgb->data[i * 4 + 1] - 0.147 * rgb->data[i * 4 + 2];
        yuv[i][2] = -0.100 * rgb->data[i * 4] - 0.515 * rgb->data[i * 4 + 1] + 0.615 * rgb->data[i * 4 + 2];
    }
    return yuv;
}
Bmp visible_enhance(Bmp source)
{
    YUV yuv = rgb_to_yuv(source);
    int max = -10000000;
    for (int i = 0; i < source->biHeight * source->biWidth; i++)
    {
        max = (yuv[i][0] > max) ? yuv[i][0] : max;
    }

    for (int i = 0; i < source->biHeight * source->biWidth; i++)
    {
        yuv[i][0] = 255 * log(yuv[i][0] / (double)255 + 1) / log(max / (double)255 + 1);
    }
    return yuv_to_rbg(source, yuv);
}
Bmp histogram_equalization(Bmp source)
{
    YUV yuv = rgb_to_yuv(source);
    long long int sum[256] = {0}, cnt = 0;
    for (int i = 0; i < source->biHeight * source->biWidth; i++)
    {
        sum[yuv[i][0]]++;
        cnt++;
    }
    for (int i = 1; i < 256; i++)
    {
        sum[i] += sum[i - 1];
    }
    for (int i = 0; i < 256; i++)
    {
        sum[i] = sum[i] / (double)cnt * 255;
    }
    for (int i = 0; i < source->biHeight * source->biWidth; i++)
    {
        yuv[i][0] = sum[yuv[i][0]];
    }
    return yuv_to_rbg(source, yuv);
}
position get_transformation(double point[2], double matrix[3][3])
// get matrix*{point,1}
{
    position ret;
    ret = malloc(2 * sizeof(double));
    ret[0] = matrix[0][0] * point[0] + matrix[0][1] * point[1] + matrix[0][2];
    ret[1] = matrix[1][0] * point[0] + matrix[1][1] * point[1] + matrix[1][2];
    return ret;
}
Bmp simple_matrix_transformation(Bmp source, double matrix[3][3])
{
    double determinant = matrix[0][0] * matrix[1][1] * matrix[2][2] +
                         matrix[0][1] * matrix[1][2] * matrix[2][0] +
                         matrix[0][2] * matrix[1][0] * matrix[2][1] -
                         matrix[2][0] * matrix[1][1] * matrix[0][2] -
                         matrix[2][1] * matrix[1][2] * matrix[0][0] -
                         matrix[2][2] * matrix[1][0] * matrix[0][1];
    double inverse[3][3];
    inverse[0][0] = matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1];
    inverse[1][0] = matrix[2][0] * matrix[1][2] - matrix[1][0] * matrix[2][2];
    inverse[2][0] = matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0];
    inverse[0][1] = matrix[0][2] * matrix[2][1] - matrix[0][1] * matrix[2][2];
    inverse[1][1] = matrix[0][0] * matrix[2][2] - matrix[0][2] * matrix[2][0];
    inverse[2][1] = matrix[0][1] * matrix[2][0] - matrix[0][0] * matrix[2][1];
    inverse[0][2] = matrix[0][1] * matrix[1][2] - matrix[0][2] * matrix[1][1];
    inverse[1][2] = matrix[0][2] * matrix[1][0] - matrix[0][0] * matrix[1][2];
    inverse[2][2] = matrix[0][0] * matrix[1][1] - matrix[1][0] * matrix[0][1];
    // get inverse matrix
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            inverse[i][j] /= determinant;
        }
    }
    position point_temp[4];
    for (int i = 0; i < 4; i++)
    {
        point_temp[i] = malloc(2 * sizeof(double));
    }
    point_temp[0][0] = 0, point_temp[0][1] = 0,
    point_temp[1][0] = source->biWidth - 1, point_temp[1][1] = source->biHeight - 1,
    point_temp[2][0] = 0, point_temp[2][1] = source->biHeight - 1,
    point_temp[3][0] = source->biWidth - 1, point_temp[3][1] = 0;
    double wid_min = INT_MAX, wid_max = INT_MIN, hei_min = INT_MAX, hei_max = INT_MIN;
    for (int i = 0; i < 4; i++)
    {
        position temp = get_transformation(point_temp[i], matrix);
        wid_min = (wid_min > temp[0]) ? temp[0] : wid_min;
        wid_max = (wid_max < temp[0]) ? temp[0] : wid_max;
        hei_min = (hei_min > temp[1]) ? temp[1] : hei_min;
        hei_max = (hei_max < temp[1]) ? temp[1] : hei_max;
        free(temp);
    }
    // calculate four extreme point position after transformation to determine
    // the size of image and offset
    for (int i = 0; i < 4; i++)
    {
        free(point_temp[i]);
    }
    Bmp desti = malloc(sizeof(struct Bmp));
    *desti = *source;
    desti->biWidth = wid_max - wid_min + 3;
    desti->biHeight = hei_max - hei_min + 3;
    desti->bfSize = desti->biWidth * desti->biHeight * 4 + desti->bfOffBits;
    desti->data = malloc(desti->bfSize - desti->bfOffBits);
    for (int j = 0; j < desti->biHeight; j++)
    {
        for (int i = 0; i < desti->biWidth; i++)
        {
            position temp;
            temp = malloc(sizeof(double) * 2);
            temp[0] = i + wid_min;
            temp[1] = j + hei_min;
            temp = get_transformation(temp, inverse);
            if (temp[0] >= 0 & temp[1] >= 0 & temp[0] < source->biWidth & temp[1] < source->biHeight)
            {
                memcpy(desti->data + (j * desti->biWidth + i) * 4, source->data + (((int)temp[1]) * source->biWidth + ((int)temp[0])) * 4, 4);
            }
            else
            {
                desti->data[(j * desti->biWidth + i) * 4] = 255;
                desti->data[(j * desti->biWidth + i) * 4 + 1] = 255;
                desti->data[(j * desti->biWidth + i) * 4 + 2] = 255;
                desti->data[(j * desti->biWidth + i) * 4 + 3] = 255;
            }
            free(temp);
        }
    }
    // Select the pixel assignment of the original image based on integer
    // truncation of the original coordinates of the inverse transformation
    return desti;
}
double sum_core(int width, int height, double core[height][width],
             int width_start, int width_end, int height_start, int height_end)
{
    double sum = 0;
    for (int i = height_start; i <= height_end; i++)
    {
        for (int j = width_start; j <= width_end; j++)
        {
            sum += core[i][j];
        }
    }
    return sum;
}
void convolution_4byte(Bmp change, Bmp object, int center_x, int center_y, int core_width,
                       int core_height, double core[core_height][core_width])
{
    int width_l = (center_x > core_width / 2) ? core_width / 2 : center_x;
    int width_r = (object->biWidth - 1 - center_x > core_width / 2)
                      ? core_width / 2
                      : object->biWidth - 1 - center_x;
    int height_l = (center_y > core_height / 2) ? core_height / 2 : center_y;
    int height_r = (object->biHeight - 1 - center_y > core_height / 2)
                       ? core_height / 2
                       : object->biHeight - 1 - center_y;
    double sum = sum_core(core_width, core_height, core, core_width / 2 - width_l, core_width / 2 + width_r,
                       core_height / 2 - height_l, core_height / 2 + height_r);
    double temp[3] = {0};
    for (int i = center_y - height_l; i <= center_y + height_r; i++)
    {
        for (int j = center_x - width_l; j <= center_x + width_r; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                temp[k] += core[i-center_y  + core_height / 2][j-center_x  + core_width / 2] * object->data[i * object->biWidth * 4 + j * 4 + k];
            }
        }
    }
    for (int i = 0; i < 3; i++)
    {
        temp[i] = (temp[i] >= 0) ? temp[i] : 0;
        temp[i] /= sum;
        temp[i] = (temp[i] <= 255) ? temp[i] : 255;
        change->data[center_y * object->biWidth * 4 + center_x * 4 + i] = temp[i];
    }
}
Bmp mean_filter(Bmp source, int core_width, int core_height)
{
    double core[core_height][core_width];
    for (int i = 0; i < core_height; i++)
    {
        for (int j = 0; j < core_width; j++)
        {
            core[i][j] = 1;
        }
    }
    Bmp ret = malloc(sizeof(struct Bmp));
    *ret = *source;
    ret->data = malloc(ret->biWidth * ret->biHeight * 4);
    for (int i = 0; i < ret->biHeight; i++)
    {
        for (int j = 0; j < ret->biWidth; j++)
        {
            convolution_4byte(ret, source, j, i, core_width, core_height, core);
        }
    }
    return ret;
}
Bmp laplacian_filter(Bmp source)
{
    double core[3][3];
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            core[i][j] = -1;
        }
    }
    core[1][1] = 9;
    Bmp ret = malloc(sizeof(struct Bmp));
    *ret = *source;
    ret->data = malloc(ret->biWidth * ret->biHeight * 4);
    for (int i = 0; i < ret->biHeight; i++)
    {
        for (int j = 0; j < ret->biWidth; j++)
        {
            convolution_4byte(ret, source, j, i, 3, 3, core);
        }
    }
    return ret;
}
Bmp bilateral_filter(Bmp source, int core_width, int core_height, double sigma_p, double sigma_c)
// core_width and core_height must be odd
{
    double core[core_height][core_width];
    for (int i = 0; i < core_height; i++)
    {
        for (int j = 0; j < core_width; j++)
        {
            core[i][j] = 0;
        }
    }
    Bmp ret = malloc(sizeof(struct Bmp));
    *ret = *source;
    ret->data = malloc(ret->biWidth * ret->biHeight * 4);
    for (int y = 0; y < source->biHeight; y++)
    {
        for (int x = 0; x < source->biWidth; x++)
        {
            for (int i = (y - core_height / 2 >= 0) ? y - core_height / 2 : 0;
                 i <= ((y + core_height / 2 < source->biHeight) ? core_height / 2 + y
                                                               : source->biHeight - 1);
                 i++)
            {
                for (int j = (x - core_width / 2 >= 0) ? x - core_width / 2 : 0;
                     j <= ((x + core_width / 2 < source->biWidth) ? x + core_width / 2
                                                                 : (source->biWidth - 1));
                     j++)
                {
                    core[i - y + core_height / 2][j - x + core_width / 2] =
                        exp(-(pow(i - y, 2) + pow(j - x, 2)) / (2 * pow(sigma_p, 2)) -
                            (pow(source->data[(y * source->biWidth + x) * 4] - source->data[(i * source->biWidth + j) * 4], 2) +
                             pow(source->data[(y * source->biWidth + x) * 4 + 1] - source->data[(i * source->biWidth + j) * 4 + 1], 2) +
                            pow(source->data[(y * source->biWidth + x) * 4 + 2] - source->data[(i * source->biWidth + j) * 4 + 2], 2)) /
                                (2 * pow(sigma_c, 2)));
                }
            }
            convolution_4byte(ret, source, x, y, core_width, core_height, core);
        }
    }
    return ret;
}