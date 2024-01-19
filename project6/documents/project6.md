# 浙江大学实验报告

**专业：计算机科学与技术&emsp;姓名：仇国智&emsp;学号：3220102181&emsp;日期：2023/11/25 
课程名称：图像信息处理&emsp;实验名称：图像的双边滤波&emsp; 
指导老师：宋明黎&emsp;成绩：**

<div align=center STYLE="page-break-after: always;">
</div>
# 实验目的和要求
- 对图像进行双边滤波
# 实验内容及原理
## 平均滤波

- 图像有两个主要特征：空间域S，它是图像中可能位置的集合。这与分辨率有关，即图像中的行数和列数。颜色强度域R，它是一组可能的像素值。用于表示像素值的位数可以变化。常见的像素表示是无符号字节（0到255）和浮点。双边滤波同时考虑到了颜色和空间因素，在高斯滤波的基础上加入了像素值权重项，也就是说既要考虑距离因素，也要考虑像素值差异的影响，距离越大颜色相差越大则该像素点对于目标像素点影响越小。其计算公式如下：
$$
I_p=\frac{1}{W_p}\cdot\sum_{q\in S} G_{\sigma_p}(\Vert p-q\Vert)G_{\sigma_c}(\lvert I_p-I_q\rvert)I_q\\ 
W_p=\sum_{q\in S} G_{\sigma_p}(\Vert p-q\Vert)G_{\sigma_c}(\lvert I_p-I_q\rvert)
$$
可以将上述公式转换为卷积，卷积核的值随移动变化即$$G_{\sigma_p}(\Vert p-q\Vert)G_{\sigma_c}(\lvert I_p-I_q\rvert)$$

# 实验步骤及分析

##　初始化
新创建图片，初始化卷积核
```
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
```
##　边界判断
在进行卷积操作时，边界必然会出现卷积核越界的现象，故需进行边界判断，舍弃部分卷积核。
```
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
                   ...
                }
            }
```
## 计算卷积核
依据公式计算卷积核
```
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
```
## 主函数
通过I/O操作与用户交互
```
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
```
## 完整代码
```
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
```
```
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
```
# 实验环境及运行方法
- 实验环境： win11 + gcc 

- 下载文件解压后在终端转到该文件夹的code文件夹作为工作目录

  -  输入 
		```
		make
		make run
		```

-  样例：

   - 输入：`..\image\1.bmp`
   - 输出：`The results are stored in ..\reslut`
# 实验结果展示
|原始图像|平均滤波|
|:---:|:---:|
|![](..\image\1.bmp)|![](..\result\bilateral1.bmp)|
|![](..\image\2.bmp)|![](..\result\bilateral2.bmp)|
|![](..\image\3.bmp)|![](..\result\bilateral3.bmp)|

# 实验心得

- 对图像进行卷积操作时必然会在图像边缘点出现越界现象，需要单独处理。
- 需要注意在类型转换时发生的误差。
