# 浙江大学实验报告

**专业：计算机科学与技术&emsp;姓名：仇国智&emsp;学号：3220102181&emsp;日期：2023/10/8 
课程名称：图像信息处理&emsp;实验名称：图像可视性对数增强和图像直方图均匀化&emsp; 
指导老师：宋明黎&emsp;成绩：**

<div align=center STYLE="page-break-after: always;">
</div>
## 一、实验目的和要求

- 任务一：图像可视性对数增强
- 任务二：图像直方图均匀化

## 二、实验内容和原理
### 可视性增强
 为了增强图像的可视信息，可以对图像中的像素进行基于对数的操作
$$
L_d=\frac{\log_2{(L_w+1)}}{\log_2{(L_{max}+1)}}
$$
L_d是显示亮度，L_w是图片原始亮度，L_{max}是图片中最大的亮度{L取值范围均为（0，1）}。这个映射能够确保不管场景的动态范围是怎么样的，其最大值都能映射到1（白），其他的值能够比较平滑地变化。

###　直方图均匀化

- 直方图均匀化的关键在于找到函数$T$，将原图像的非均匀分布的直方图通过变换函数$T$修正为均匀分布的直方图，然后按均衡直方图修正原图像。
	![](.\\1.png)
- 假设：
    	1. 令$r$和$s$分别代表变化前后图像的灰度级，并且 0≤$r$,$s$ ≤1 。
    	2. $P(r)$和$P(s)$ 分别为变化前后各级灰度的概率密度函数（$r$和$s$值已归一化，最大灰度值为1）。
	规定：
    	1. 在$0≤r≤1$中，$T(r)$是单调递增函数，并且$0≤T(r)≤1$。
    	2. 反变换$r = T^{-1}(s)$也为单调递增函数。
		考虑到灰度变换不影响像素的位置分布，也不会增减像素数目。所以有：
	$$
	\int_{0}^{r} P(r)\cdot dx = \int_{0}^{s} P(s)\cdot ds = \int_{0}^{s} 1\cdot ds = s = T(r)
	$$
	综上，转换函数$T$在变量$r$处的函数值$s$，是原直方图中灰度等级为$[0,r]$以内的直方图曲线所覆盖的面积。 
- 将连续情况转化为离散分布，设一幅图像的像素总数为$n$，分$L$个灰度级，$n_k$为第$k$个灰度级出现的像素数，得离散灰度直方图均衡化的转换公式为：
	$$
	s_k=T(r_k)=\frac{1}{n}\sum_{i=0}^{k}n_i
	$$
- 直方图均衡化实质上是减少图像的灰度级以换取对比度的加大。在均衡过程中，原来的直方图上出现概率较小的灰度级被归入很少几个甚至一个灰度级中，故得不到增强。若这些灰度级所构成的图象细节比较重要，则需采用局部区域直方图均衡化处理。 
## 三、实验步骤与分析

### 可视性对数增强

```c
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
```
首先将图片由RGN颜色空间转换到RGB颜色空间，遍历图片数据得到$Y_{max}$`max`,然后根据$L_d=\frac{\log_2{(L_w+1)}}{\log_2{(L_{max}+1)}}$,即$Y_d=255\cdot \frac{\log_2{(\frac{Y_w}{255}+1)}}{\log_2{(\frac{Y_{max}}{255}+1)}}$，重新赋值Y，再重新转换为RGB返回图像。
### 直方图均匀化

```c
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
```
首先遍历图像数据依次进行$sum[k]=n_i,sum[k]=sum[k]+sum[k-1],sum[k]=sum[k]/cnt$,得到$sum[k]=\frac{1}{n}\cdot \sum_{i=0}^{k}n_i$,然后对$Y$重新赋值,输出新的RGB图像。
### 主程序
```c
#include "tools.h"
int main()
{

    printf("Please input the location of the image to be processed.\n");
    char source[300];
    gets(source);
    Bmp a = bmp_read(source), b;
    YUV yuv;
    b = visible_enhance(a);
    a = histogram_equalization(b);
    bmp_write(b, "..\\result\\visible.bmp");
    bmp_write(a, "..\\result\\histogram.bmp");
    printf("The results are stored in ..\\reslut\n");
}
```
依据地址读入图片存入`a`，对`a`进行可视化增强存入`b`，对`b`进行直方图均匀化存入`a`，然后输出`a`，`b` 到图像文件。
## 四、实验环境及运行方法

- 实验环境： win11 + gcc 

- 在终端转到该文件夹的code文件夹作为工作目录

  -  输入 
		```
		make
		make run
		```

-  样例：

   - 输入：`..\image\1.bmp`
   - 输出：`The results are stored in ..\reslut`
## 五、实验结果展示
|原始图像|可视化增强|直方图均匀化（可视化增强后）|
|:---:|:---:|:---:|
|![](..\image\1.bmp)|![](..\result\visible1.bmp)|![](..\result\histogram1.bmp)|
|![](..\image\2.bmp)|![](..\result\visible2.bmp)|![](..\result\histogram2.bmp)|
|![](..\image\3.bmp)|![](..\result\visible3.bmp)|![](..\result\histogram3.bmp)|
## 六、心得体会
在YUV颜色空间和RGB颜色空间相互转换时时需要考虑边界条件。