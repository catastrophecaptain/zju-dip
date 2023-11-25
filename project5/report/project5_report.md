# 浙江大学实验报告

**专业：计算机科学与技术&emsp;姓名：仇国智&emsp;学号：3220102181&emsp;日期：2023/11/25 
课程名称：图像信息处理&emsp;实验名称：图像平均滤波及拉普拉斯图像增强&emsp; 
指导老师：宋明黎&emsp;成绩：**

<div align=center STYLE="page-break-after: always;">
</div>
# 实验目的和要求
- 对图像进行平均滤波
- 对图像进行拉普拉斯算子运算增强

# 实验内容及原理

## 卷积

- 对与连续函数而言卷积定义如下，假设对于两个一维连续函数$f(x)$和$h(x)$，它们的卷积结果为$g(x)$。
	$$
	g(x)=\int_{-\infty}^{\infty}{f(t)h(x-t)\cdot dt}
	$$

- 对于离散序列，其卷积可用与连续函数相类似的方法求得，假设对于两个长度均为$M$的序列$f(x)$和$h(x)$，其卷积结果为$g(x)$。
	$$
	g(x)=\frac{1}{M} \sum_{i=0}^{M-1}{f(i)h(x-i)}
	$$

- 对于图像而言，卷积核是一个大小为M×N的窗口，窗口中的元素与原图像的处于窗口内的像素进行加权求和，结果作为新图像现在窗口中心位置的像素。当窗口滑过原图像并完成上述运算之后，就能够得到卷积后的图像。
![](.\1.jpg)

## 平均滤波

- 用全为1的卷积核进行卷积处理即可，平滑线性空间滤波器的输出是包含在滤波掩模邻域内像素的简单平均值。因此，这些滤波器也称为均值滤波器。均值滤波器的主要应用是去除图像中的不相干细节，即那些与滤波掩模尺寸相比更小的像素区域。将其表示为卷积核如下（三阶且未归一化）。
$$
\left[ \matrix{1&1&1\\1&1&1\\1&1&1} \right]
$$
## 拉普拉斯图像增强

- 基于二阶微分的图像增强——拉普拉斯算子$\nabla^2$。微分算子是实现锐化的工具，其响应程度与图像在该点处的突变程度有关。微分算子增强了边缘和其他突变（如噪声）并削弱了灰度变化缓慢的区域。
- 对于离散序列，拉普拉斯算子可以约化如下：
	$$
	\nabla^2f(x,y)=9\cdot f(x)(y)-\sum_{-1\leq i,j\leq 1}^{}f(x+i,y+j)
	$$
	
- 在拉普拉斯算子处理结果上叠加上原始图像即为增强结果,将其表示为卷积核如下
$$
\left[ \matrix{-1&-1&-1\\-1&9&-1\\-1&-1&-1} \right]
$$
# 实验步骤及分析
##　计算卷积核总权和
采用`double`数组作为卷积核的数据结构，该函数用于计算总权和便于归一化。
```
int sum_core(int width, int height, double core[height][width],
             int width_start, int width_end, int height_start, int height_end)
{
    int sum = 0;
    for (int i = width_start; i <= width_end; i++)
    {
        for (int j = height_start; j <= height_end; j++)
        {
            sum += core[i][j];
        }
    }
    return sum;
}
```
##　边界判断
在进行卷积操作时，边界必然会出现卷积核越界的现象，故需进行边界判断，舍弃部分卷积核。
```
    int width_l = (center_x > core_width / 2) ? core_width / 2 : center_x;
    int width_r = (object->biWidth - 1 - center_x > core_width / 2)
                      ? core_width / 2
                      : object->biWidth - 1 - center_x;
    int height_l = (center_y > core_height / 2) ? core_height / 2 : center_y;
    int height_r = (object->biHeight - 1 - center_y > core_height / 2)
                       ? core_height / 2
                       : object->biHeight - 1 - center_y;
    // make boundary checks to prevent crossing the boundary
    int sum = sum_core(core_width, core_height, core, core_width / 2 - width_l, core_width / 2 + width_r,
                       core_height / 2 - height_l, core_height / 2 + height_r);
    // preparing for normalization
```
## 卷积
对图像进行一次卷积加权求和，归一化，中心像素赋值的操作。
```
    int temp[3] = {0};
    for (int i = center_y - height_l; i <= center_y + height_r; i++)
    {
        for (int j = center_x - width_l; j <= center_x + width_r; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                temp[k] += core[center_y - i + core_height / 2][center_x - j + core_width / 2] * object->data[i * object->biWidth * 4 + j * 4 + k];
            }
        }
    }
    // convolution
    for (int i = 0; i < 3; i++)
    {
        temp[i] = (temp[i] >= 0) ? temp[i] : 0;
        temp[i] /= sum;
        temp[i] = (temp[i] <= 255) ? temp[i] : 255;
        change->data[center_y * object->biWidth * 4 + center_x * 4 + i] = temp[i];
    }
    // assign values to image pixels after normalization
```
## 均值滤波和拉普拉斯增强
依据原理中的卷积核分别遍历图像像素点进行卷积操作。
```
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
    // traverse pixels for convolution
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
    // core:
    // -1  -1  -1
    // -1   9  -1
    // -1  -1  -1
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
    // traverse pixels for convolution
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
|原始图像|平均滤波|拉普拉斯图像增强|
|:---:|:---:|:---:|
|![](..\image\1.bmp)|![](..\result\mean1.bmp)|![](..\result\laplacian1.bmp)|
|![](..\image\2.bmp)|![](..\result\mean2.bmp)|![](..\result\laplacian2.bmp)|
|![](..\image\3.bmp)|![](..\result\mean3.bmp)|![](..\result\laplacian3.bmp)|

# 实验心得
对图像进行卷积操作时必然会在图像边缘点出现越界现象，需要单独处理。