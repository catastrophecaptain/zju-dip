# 浙江大学实验报告
**专业：计算机科学与技术&emsp;姓名：仇国智&emsp;学号：3220102181&emsp;日期：2023/10/8 
课程名称：图像信息处理&emsp;实验名称：bmp文件读写及rgb和yuv色彩空间转化&emsp; 
指导老师：宋明黎&emsp;成绩：**
<div align=center STYLE="page-break-after: always;">
</div>
## 一、实验目的和要求

- 任务 1 ：根据读入 bmp 图像的 RGB 图像数据生成对应的 YUV 图像数据。
- 任务 2 ：根据 YUV 图像数据的 Y 值，生成灰度图数据，并在区间 [0 , 255] 内重新分配满灰度图数据，最后导出为 bmp 图像。
- 任务3 ： 修改 YUV 图像数据的 Y 值，重新生成 RGB 图像数据，并导出为 bmp 图像。

## 二、实验内容和原理
- 当 RGB 图像所有点的 RGB 均相等时，即为灰度图
- bmp 图数据组成如下：
<img src=".\bmp_structure.png"  />
![](.\filehead.png)
可以从 Image file header 读出 `bfSize` 和 `bfOffBits` ，以确定图像数据范围以进行处理。
- RGB 和 YUV 有如下转换公式：
	$$
	\begin{pmatrix}Y \\ U \\ V \end{pmatrix}=
	\begin{pmatrix} 0.299 & 0.587 & 0.114 \\ -0.147 & -0.289 & 0.435 \\ 	0.615 & -0.515 & -0.100 \end{pmatrix} \cdot
	\begin{pmatrix} R\\G\\B\end{pmatrix}
	$$
	$$
	\begin{pmatrix}R \\ G \\ B \end{pmatrix}=
	\begin{pmatrix} 1.000 & 0.000 & 1.140 \\ 1.000 & -0.395 & -0.581 \\ 	1.000 & 2.036 & 0.000 \end{pmatrix} \cdot
	\begin{pmatrix} Y\\U\\V\end{pmatrix}
	$$
- bmp 采用 4bytes 来存储一个像素点，按地址由小到大数据分别为 B ， G ， R  ，补充。

## 三、实验步骤与分析

- ```c
    printf("Please input the location of input image:\n");
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
	```
	根据输入的文件绝对地址，读入 `bfOffBits` 和 `bfSize` 确定图像信息范围，并读入所有 输入 bmp 信息。
- ```c
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
	```
	依据公式将 RBG 转换为 YUV ，并确定 Y 值对应的最大灰度值。
- ```c
    for (int i = head_length; i < total_length; i += 4)
    {
        graph[i] = graph[i + 1] = graph[i + 2] = (yuv_graph[i] - min_gray) / (double)(max_gray - min_gray) * 255;
    }
    // rearrange gray data
    fpt_gray = fopen(".\\result\\gray.bmp", "wb");
    fwrite(graph, 1, total_length, fpt_gray);
    fclose(fpt_gray);
	```
	重新在区间 [0 , 255] 分配灰度值，并存入 R 、 B 、 G 。输出灰度图至 ` .\result\gray.bmp` 。
- ```c
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
    ```
	修改每个点的 Y 值，并将 YUV 转化为 RGB 。对于上界溢出即大于 255 ，根据最大值和 255 之比，重新分配该点 RGB ，对于下界溢出即小于 0，将该值直接设置为 0 。这样就避免了溢出导致的错误。最后输出该 RBG 的 bmp 图像。
## 四、实验环境及运行方法
- 实验环境： windos11 + gcc 。
- 运行方法：
	1. 在终端转到该文件夹作为工作目录
	2. 输入 `gcc main.c -o main.exe` `./main.exe`
	3. 样例：
		- 输入：.\image\1.bmp
		- 输出：The results are stored in .\reslut\gray.bmp and .\reslut\mod.bmp .

##　五、实验结果展示

|输入图像|输出灰度图|输出 Y 修改图|
|:---:|:---:|:---:|
|![](..\image\1.bmp)|![](..\result\gray1.bmp)|![](..\result\mod1.bmp)|
|![](..\image\2.bmp)|![](..\result\gray2.bmp)|![](..\result\mod2.bmp)|
|![](..\image\3.bmp)|![](..\result\gray3.bmp)|![](..\result\mod3.bmp)|
|![](..\image\4.bmp)|![](..\result\gray4.bmp)|![](..\result\mod4.bmp)|
|![](..\image\5.bmp)|![](..\result\gray5.bmp)|![](..\result\mod5.bmp)|
## 六、心得体会
可以使用二进制编辑器来查看图片文件，对于理解图片数据内容构成有很大帮助。