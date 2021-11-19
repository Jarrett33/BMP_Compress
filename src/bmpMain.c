/**
 * E21301229郑俊杰数字图像处理作业1
 * 查找相关图像格式资料
 * 设计算法编制读取二值bmp图像
 * 实现按位压缩以及解压缩
 * 2021_11
 */
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"

void compress(FILE *);   //压缩函数
void decompress(FILE *); //解压缩函数

int main()
{
    FILE *fpOrigin;
    if ((fpOrigin = fopen("4b.bmp", "rb")) == NULL)
    {
        printf("打开图片失败");
        exit(0);
    }
    // 压缩
    compress(fpOrigin);
    FILE *fpCompressed;
    if ((fpCompressed = fopen("4bcompressed.bmp", "rb")) == NULL)
    {
        printf("打开图片失败");
        exit(0);
    }
    // 解压缩
    decompress(fpCompressed);

    fclose(fpOrigin);
    fclose(fpCompressed);
    printf("success！\n");
    return 0;
}

/**
 * 压缩算法
 * 由于二值图像一个像素只占一个字节，且仅有字节的最低位才有效
 * 所以将连续的8个字节缩位成一个字节以达到压缩目的
 */
void compress(FILE *fpOrigin)
{
    unsigned char ImgData[3000], CompressedData[3000]; //像素数据按行提取，数组存储一行像素信息
    int i, j, m;

    BITMAPFILEHEADER *fileHeader;
    BITMAPINFOHEADER *infoHeader;
    RGBQUAD *ipRGB;
    //给定义的结构体变量申请这个结构体大小的内存空间
    fileHeader = (BITMAPFILEHEADER *)malloc(sizeof(BITMAPFILEHEADER));
    infoHeader = (BITMAPINFOHEADER *)malloc(sizeof(BITMAPINFOHEADER));
    ipRGB = (RGBQUAD *)malloc(2 * sizeof(RGBQUAD));
    //从bmp文件中读取数据块给文件信息头和图片信息头
    fread(fileHeader, sizeof(BITMAPFILEHEADER), 1, fpOrigin);
    fread(infoHeader, sizeof(BITMAPINFOHEADER), 1, fpOrigin);
    fread(ipRGB, 2 * sizeof(RGBQUAD), 1, fpOrigin);

    //创建压缩文件
    FILE *fp_compressed;
    if ((fp_compressed = fopen("4bcompressed.bmp", "wb")) == NULL)
    {
        printf("创建图片失败");
        exit(0);
    }
    //压缩时，图像的头信息不改变
    //读取原图的信息头、文件头、调色板到新建的位面图片
    fwrite(fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_compressed);
    fwrite(infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_compressed);
    fwrite(ipRGB, 2 * sizeof(RGBQUAD), 1, fp_compressed);

    for (i = 0; i < infoHeader->biHeight; i++) //一行一行的扫描
    {
        //初始化
        m = 0;
        CompressedData[0] = 0;
        //二值bmp图一个像素即为一个字节，则每一行占的实际字节数即为biWidth，故biHeight行每行扫描biWidth次
        for (j = 0; j < infoHeader->biWidth; ) //读每行数据
        {
            //将每八个字节压缩为一个字节
            for (int k = 0; k < 8; k++)
            {
                fread(&ImgData[j], 1, 1, fpOrigin); //每次读取一个字节，存入ImgData数组
                int a = (int)ImgData[j]; 
                CompressedData[m] += (a << k); //将读取的字节存入CompressData的第k位
                if((++j) == infoHeader->biWidth){
                    break; //扫描到行边界时结束当前行
                }
            }
            CompressedData[++m] = 0;
        }
        //将得到的数据写入压缩文件
        fwrite(CompressedData, m, 1, fp_compressed);
    }
    //释放资源
    fclose(fp_compressed);
    //释放资源
    free(fileHeader);
    free(infoHeader);
    free(ipRGB);
}

/**
 * 解压缩算法
 * 与压缩算法相反，将压缩文件的每个字节的八位分别提取出来
 * 以达到解压缩的目的
 */
void 
decompress(FILE *fpCompressed)
{
    unsigned char ImgData[3000], DecomData[3000]; //位图信息按行提取，存储位面的一行信息
    int i, j;
    unsigned char m[2];
    
    BITMAPFILEHEADER *fileHeader;
    BITMAPINFOHEADER *infoHeader;
    RGBQUAD *ipRGB;
    //给定义的结构体变量申请这个结构体大小的内存空间
    fileHeader = (BITMAPFILEHEADER *)malloc(sizeof(BITMAPFILEHEADER));
    infoHeader = (BITMAPINFOHEADER *)malloc(sizeof(BITMAPINFOHEADER));
    ipRGB = (RGBQUAD *)malloc(2 * sizeof(RGBQUAD));
    //从bmp文件中读取数据块给文件信息头和图片信息头
    fread(fileHeader, sizeof(BITMAPFILEHEADER), 1, fpCompressed);
    fread(infoHeader, sizeof(BITMAPINFOHEADER), 1, fpCompressed);
    fread(ipRGB, 2 * sizeof(RGBQUAD), 1, fpCompressed);

    //创建解压缩文件
    FILE *fp_decompressed;
    if ((fp_decompressed = fopen("4bdecompressed.bmp", "wb")) == NULL)
    {
        printf("创建图片失败");
        exit(0);
    }
    //读取原图的信息头、文件头、调色板到新建的位面图片
    fwrite(fileHeader, sizeof(BITMAPFILEHEADER), 1, fp_decompressed);
    fwrite(infoHeader, sizeof(BITMAPINFOHEADER), 1, fp_decompressed);
    fwrite(ipRGB, 2 * sizeof(RGBQUAD), 1, fp_decompressed);

    for (i = 0; i < infoHeader->biHeight; i++) //一行一行的扫描
    {
        //二值bmp图一个像素即为一个字节，则每一行占的实际字节数即为biWidth，故biHeight行每行扫描biWidth次
        for (j = 0; j < infoHeader->biWidth;)
        {
            fread(&m[0], 1, 1, fpCompressed); //每次从压缩文件中读取一个字节，存入ImgData数组
            int a = (int)m[0];
            for (int k = 0; k < 8; k++)
            {
                //将压缩文件的每个字节按位提取出来，存入DeComData数组
                DecomData[j] = ((a >> k) & 1);
                if((++j) == infoHeader->biWidth){
                    break;
                }
            }
        }
        //将解压缩得到的数据写入解压缩文件
        fwrite(DecomData, j, 1, fp_decompressed);
    }
    //释放资源
    fclose(fp_decompressed);
    //释放资源
    free(fileHeader);
    free(infoHeader);
    free(ipRGB);
}
