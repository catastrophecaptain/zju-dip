# 浙江大学实验报告

**专业：计算机科学与技术&emsp;姓名：仇国智&emsp;学号：3220102181&emsp;日期：2023/10/8 
课程名称：图像信息处理&emsp;实验名称：图像二值化及图像形态学操作&emsp; 
指导老师：宋明黎&emsp;成绩：**

<div align=center STYLE="page-break-after: always;">
</div>
# 一、实验目的和要求

- 任务一：使用大津算法将rgb颜色空间的bmp图像二值化。
- 任务二：对上述二值化图像进行侵蚀、膨胀、关闭等图像形态学操作。
# 二、实验内容原理
## 图像二值化
选取合适的阈值,将图像像素依据转化后的灰度,分别赋值为黑色和白色
- 基本思想:将二值化得到的二值图像视为两部分，一部分对应前景色（Foreground），另一部分对应背景（Background）。尝试找到一个合适的阈值使得到的前景和背景的内部方差最小，而它们之间的方差则最大。
- 具体操作:
	1.根据计算,得出阈值评估公式:
	$$
	w_f=\frac{N_f}{N},w_b=\frac{N_b}{N},w_f+w_b=1\newline
	\mu=w_f\cdot\mu_f+w_b\cdot\mu_b\newline
	\sigma_{within}^2=w_f\cdot\sigma_f^2+w_b\cdot\sigma_b^2\newline
	\begin{split}
	\sigma_{between}^2&=\sigma^2-\sigma_{within}^2\\
	&=w_f\cdot(\mu_f-\mu)^2+w_b\cdot(\mu_b-\mu)^2\\
	&=w_b\cdot w_f\cdot (\mu_f-\mu_b)^2
	\end{split}
	$$
	2.根据图像灰度图求出 $\sigma_{between}^2$ 最大的灰度作为阈值进行二值化。
	3.因为现实光线等原因，大多情况下需要采用局部阈值的方法进行二值化，以尽可能保存图像信息。
## 图像形态学操作
令A为二值图像，B为二值模板（结构元）。
- 侵蚀：A用B进行侵蚀：
	$$
	A\ominus B=\{(x,y)|(B)_{xy}\subseteq A\}
	$$
- 膨胀：A用B进行膨胀：
	$$
	A \oplus B=\{(z|(B)_z\cap A=\empty\}
	$$
- 打开：A用B进行打开：
	$$
	A\circ B=(A\ominus B)\oplus B
	$$
	
- 关闭：A用B进行关闭：
	$$
	A\bullet B=(A\oplus B)\ominus B
	$$

# 三、实验内容分析
## 二值图像的处理
```c
int binary_get_bmp(Bmp binary, int column, int row)
{
    int width = (binary->biWidth + 31) / 32 * 4;
    uint8_t temp = 0;
    int ret = -1;
    if (column < binary->biHeight && column >= 0 && row < binary->biWidth && row >= 0)
    {
        temp = binary->data[column * width + row / 8];
        temp <<= row % 8;
        temp >>= 7;
        ret = temp;
    }
    return ret;
}
void binary_assign_bmp(Bmp binary, int column, int row, int assign)
{
    int width = (binary->biWidth + 31) / 32 * 4;
    if (assign == 1)
    {
        binary->data[column * width + row / 8] |= ((uint8_t)1) << (7 - row % 8);
    }
    else
    {
        binary->data[column * width + row / 8] &= ~((uint8_t)1) << (7 - row % 8);
    }
}
```
`binary_get_bmp`用于获得二值图像像素点的值，`binary_assign_bmp`用于给二值图像像素点赋值。
## 二值化求解阈值
```c
void thershold_divide_bmp(Bmp binary, Bmp gray, int start_h, int start_w, int height, int width)
{
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
    hash[min_gray][1] = min_gray*hash[min_gray][0];
    double max = -1;
    int thershold = min_gray;
    int flag=(width==binary->biWidth&&height==binary->biHeight)?0:10;
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
}
```
- 先对图像进行哈希预处理，将各个灰度值的像素点数储存到`hash`数组中，然后依据公式$\sigma_{between}^{2}=w_f\cdot w_b\cdot(\mu_f-\mu_b)^2$遍历灰度值计算出$\sigma_{between}^{2}$最大的阈值。
- 但为了避免进行局部二值化时灰度的微小扰动造成噪点影响图像质量，设置`flag`,当灰度属于[`thershold-flag`,`thershold+flag`)区间时采用全局二值化值。当全局二值化时`flag`为0不起影响。

## 局部阈值

```c
for (int k1 = 0; k1 + window_size < binary->biHeight; k1 += window_size/2)
    {
        for (int k2 = 0; k2 + window_size < binary->biWidth; k2 += window_size/2)
        {
            thershold_divide_bmp(binary, gray, k1, k2, window_size, window_size);
        }
    }
```
采用`window_size`*`window_size`的局部窗口进行二值化，以`window_size/2`为单位长度平移，至遍历整个图像。
## 腐蚀与膨胀
```c
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
```
mode为0时为侵蚀，mode为1时为膨胀。设定的二值模板为边长`size`的正方形，对每个像素点应用平移二至模板若有模板内有像素点值为`mode`，则设置新像素点为mode，否则则为原值。
## 主程序
```c
#include "tools.h"
int main()
{
    printf("Please input the location of the image to be processed.\n");
    char source[300];
    gets(source);
    Bmp a = bmp_read(source), b,c;
    c = rgb_to_gray_bmp(a,0);
    a = gray_to_binary_bmp(c,0);
    bmp_write(a,"..\\result\\binary_global.bmp");
    a = gray_to_binary_bmp(c,5);
    bmp_write(a,"..\\result\\binary_enhance.bmp");
    b = erosion_dilation_binary_bmp(a,1,1);
    bmp_write(b,"..\\result\\erosion.bmp");
    b = erosion_dilation_binary_bmp(a,0,1);
    bmp_write(b,"..\\result\\dilation.bmp");
    b = erosion_dilation_binary_bmp(a,1,1);
    b = erosion_dilation_binary_bmp(b,0,1);
    bmp_write(b,"..\\result\\open.bmp");
    b = erosion_dilation_binary_bmp(a,0,1);
    b = erosion_dilation_binary_bmp(b,1,1);
    bmp_write(b,"..\\result\\close.bmp");
    printf("The results are stored in ..\\reslut\n");
}
```
对读入图像进行先灰度化再用两种方法二值化，然后再分别进行侵蚀、膨胀、打开、关闭。输出二值化及形态处理结果图像至result文件夹。
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
# 五、实验结果展示

|操作|`1.bmp`|`2.bmp`|
|:---:|:---:|:---:|
|原始图像|<img src="..\image\1.bmp"  />|![](..\image\2.bmp)|
|全局阈值二值化|![](..\result\binary_global1.bmp)|![](..\result\binary_global2.bmp)|
|局部阈值二值化（降噪前）|![](..\result\noise1.bmp)|![](..\result\noise2.bmp)|
|局部阈值二值化（降噪后）|![](..\result\binary_enhance1.bmp)|![](..\result\binary_enhance2.bmp)|
|侵蚀|![](..\result\erosion1.bmp)|![](..\result\erosion2.bmp)|
|膨胀|![](..\result\dilation1.bmp)|![](..\result\dilation2.bmp)|
|打开|![](..\result\open1.bmp)|![](..\result\open2.bmp)|
|关闭|![](..\result\close1.bmp)|![](..\result\close2.bmp)|
# 六、心得体会
- bmp图像每一行也需要进行四字节的对齐，很容易犯错。
- 局部二值化可以比全局二值化保存更多的细节，但是也会产生噪点的问题，我采用了在阈值一定区间内采用全局二值化像素的方法降噪。
