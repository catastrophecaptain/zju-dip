# 浙江大学实验报告

**专业：计算机科学与技术&emsp;姓名：仇国智&emsp;学号：3220102181&emsp;日期：2023/10/8 
课程名称：图像信息处理&emsp;实验名称：图像简单几何变换&emsp; 
指导老师：宋明黎&emsp;成绩：**

<div align=center STYLE="page-break-after: always;">
</div>

# 实验目的和要求

- 对图像进行平移、旋转、缩放、错切、镜像

# 实验内容和原理

## 几何变换

- 几何变化可以用矩阵来进行表示
- 平移：
	$$
  	\left[
  	\begin{matrix} x^{\prime}\\y^{\prime}\\1
  	\end{matrix}
  	\right]
  	=
  	\left[
  	\begin{matrix} 1&0&x_0\\0 &1& y_0 \\0&0&1
  	\end{matrix}
  	\right]
  	\cdot
  	\left[
  	\begin{matrix} x\\y\\1
  	\end{matrix}
  	\right]
	$$
- 旋转：
	$$
  	\left[
  	\begin{matrix} x^{\prime}\\y^{\prime}\\1
  	\end{matrix}
  	\right]
  	=
  	\left[
  	\begin{matrix} \cos \theta&\sin \theta&0\\\sin \theta &\cos \theta& 0 \\0&0&1
  	\end{matrix}
  	\right]
  	\cdot
  	\left[
  	\begin{matrix} x\\y\\1
  	\end{matrix}
  	\right]
	$$
- 缩放
	$$
  \left[
  	\begin{matrix} x^{\prime}\\y^{\prime}\\1
  	\end{matrix}
  	\right]
  	=
  	\left[
  	\begin{matrix} c&0&0\\0 &d& 0 \\0&0&1
  	\end{matrix}
  	\right]
  	\cdot
  	\left[
  	\begin{matrix} x\\y\\1
  	\end{matrix}
  	\right]
	$$
- 错切
	$$
  \left[
  	\begin{matrix} x^{\prime}\\y^{\prime}\\1
  	\end{matrix}
  	\right]
  	=
  	\left[
  	\begin{matrix} 1&d_x&0\\d_y &1& 0 \\0&0&1
  	\end{matrix}
  	\right]
  	\cdot
  	\left[
  	\begin{matrix} x\\y\\1
  	\end{matrix}
  	\right]
	$$
- 镜像
	$$
  \left[
  	\begin{matrix} x^{\prime}\\y^{\prime}\\1
  	\end{matrix}
  	\right]
  	=
  	\left[
  	\begin{matrix} -1&0&0\\0 &-1& 0 \\0&0&1
  	\end{matrix}
  	\right]
  	\cdot
  	\left[
  	\begin{matrix} x\\y\\1
  	\end{matrix}
  	\right]
	$$
- 图像的组合变换是各项简单几何变换的混合操作。变换矩阵是由构成组合变换的各种简单变换的变换矩阵按从左至右的顺序逐次相乘以后得到的结果。
## 插值
- 插值是几何变换最常用的工具，利用已知像素值，采用不同的插值方法，可以模拟出未知像素的像素值。 
- 最近邻插值：为了计算几何变换后新图像中某一点P’处的像素值，可以首先计算该几何变换的逆变换，计算出P’所对应的原图像中的位置P。通常情况下，P的位置不可能正好处在原图像的某一个像素位置上（即P点的坐标通常都不会正好是整数）。寻找与P点最接近的像素Q，把Q点的像素值作为新图像中P’点的像素值。

# 实验步骤与分析
## 线性数学操作
```c
position get_transformation(double point[2], double matrix[3][3])
// get matrix*{point,1}
{
    position ret;
    ret = malloc(2 * sizeof(double));
    ret[0] = matrix[0][0] * point[0] + matrix[0][1] * point[1] + matrix[0][2];
    ret[1] = matrix[1][0] * point[0] + matrix[1][1] * point[1] + matrix[1][2];
    return ret;
}
```
获得输入矩阵与（x,y,1）相乘的结果，并输出矩阵变化后的坐标。
```c
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
```
解得一个3*3矩阵的逆矩阵，即原始变换矩阵的逆矩阵，便于之后得逆变换操作。
## 确定边界
```c
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
```
因为是对一个矩形区域进行线性变换操作，所以变换后得图形也是一个平行四边形，我们可以根据原图像四个顶点的坐标进行变换后的结果来确定变换后图形的大小和偏移值（避免出现负值）。
##　插值
```c
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
```
我们遍历新图像的每一个点,对用逆矩阵进行逆变换得到原图中对应的坐标(可能不为整数),再进行整数截断,就可以唯一对应到原图中的一个点,将该点的rgb赋给新图的点.
## 主函数
```c
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
```
主函数通过设置变换矩阵以实现图像的平移、旋转、缩放、错切、镜像。并将图像存储在`result`文件夹下。

# 实验环境及运行方法

- 实验环境： win11 + gcc 

- 在终端转到该文件夹的code文件夹作为工作目录

  -  输入 
		```
		make
		make run
		```

-  样例：

   - 输入：`..\image\1.bmp`
   - 输出：`The results are stored in ..\reslut、
# 实验结果展示

| 操作 | 图像1 | 图像2 |
| :----:| :-----:| :-----: |
| 原始图像 | ![](..\image\1.bmp) | ![](..\image\2.bmp) |
| 平移 | ![](..\result\translation1.bmp) | ![](..\result\translation2.bmp) |
| 旋转 | ![](..\result\rotation1.bmp) | ![](..\result\rotation2.bmp) |
| 缩放 | ![](..\result\scale1.bmp) | ![](..\result\scale2.bmp) |
| 错切 | ![](..\result\shear1.bmp) | ![](..\result\shear2.bmp) |
| 镜像 | ![](..\result\mirror1.bmp) | ![](..\result\mirror2.bmp) |

# 实验心得体会
许多图形的几何操作既可以用矩阵来进行表示。