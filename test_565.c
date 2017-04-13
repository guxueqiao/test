#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "cv.h"

int bmp565_write(unsigned char *image, int width, int height, const char *filename)
{
    long file_size;
    long data_size;
    int widthAlignBytes;
    FILE *fp;

    // 文件头
    unsigned char header[66] = {
        // BITMAPFILEINFO
        'B', 'M',               // [0-1] bfType:必须是BM字符
        0, 0, 0, 0,             // [2-5] bfSize:总文件大小
        0, 0, 0, 0,             // [6-9] brReserved1,bfReserved2:保留
        sizeof(header), 0, 0, 0,// [10-13] bfOffBits:到图像数据的偏移
        // BITMAPFILEHEADER
        0x28, 0, 0, 0,          // [14-17] biSize:BITMAPINFOHEADER大小40字节
        0, 0, 0, 0,             // [18-21] biWidth:图片宽度
        0, 0, 0, 0,             // [22-25] biHeight:图片高度
        0x01, 0,                // [26-27] biPlanes:必须为1
        0x10, 0,                // [28-29] biBitCount:16位
        0x03, 0, 0, 0,          // [30-33] biCompression:BI_BITFIELDS=3
        0, 0, 0, 0,             // [34-37] biSizeImage:图片大小
        0x12, 0x0B, 0, 0,       // [38-41] biXPelsPerMeter:单位长度内的像素数
        0x12, 0x0B, 0, 0,       // [42-45] biYPelsPerMeter:单位长度内的像素数
        0, 0, 0, 0,             // [46-49] biClrUsed:可用像素数，设为0即可
        0, 0, 0, 0,             // [50-53] biClrImportant:重要颜色数，设为0即可
        // RGBQUAD MASK
        0x0, 0xF8, 0, 0,        // [54-57] 红色掩码
        0xE0, 0x07, 0, 0,       // [58-61] 绿色掩码
        0x1F, 0, 0, 0           // [62-65] 蓝色掩码
    };

    widthAlignBytes = ((width * 16 + 31) & ~31) / 8; // 每行需要的合适字节个数
    data_size = widthAlignBytes * height;      // 图像数据大小
    file_size = data_size + sizeof(header);    // 整个文件的大小

    *((long*)(header + 2)) = file_size;
    *((long*)(header + 18)) = width;
    *((long*)(header + 22)) = height;
    *((long*)(header + 34)) = data_size;

    if (!(fp = fopen(filename, "wb")))
        return 1;

    fwrite(header, sizeof(unsigned char), sizeof(header), fp);

    if (widthAlignBytes == width * 2)
    {
        fwrite(image, sizeof(unsigned char), (size_t)data_size, fp);
    }
    else{
        // 每一行单独写入
    	int i;
        const static int DWZERO = 0;
        for (i = 0; i < height; i++)
        {
            fwrite(image + i * width * 2, sizeof(unsigned char),
                (size_t) width * 2, fp);
            fwrite(&DWZERO, sizeof(unsigned char),
                widthAlignBytes - width * 2, fp);
        }
    }

    fclose(fp);
    return 0;
}

rgb5652rgb888(unsigned char *image,unsigned char *image888)   
{   
	unsigned char R,G,B;   
	B=(*image) & 0x1F;//000BBBBB   
	G=( *(image+1) << 3 ) & 0x38 + ( *image >> 5 ) & 0x07 ;//得到00GGGGGG00   
	R=( *(image+1) >> 3 ) & 0x1F; //得到000RRRRR   
	*(image888+0)=B * 255 / 63; // 把5bits映射到8bits去，自己可以优化一下算法，下同   
	*(image888+1)=G * 255 / 127;   
	*(image888+2)=R * 255 / 63;   
} 

void main(void)
{
	unsigned char *image = (unsigned char *)calloc(20,720*240*2*sizeof(unsigned char));
	unsigned char *image_888 = (unsigned char *)calloc(20,720*240*3*sizeof(unsigned char));
	unsigned char *image_tmp = image_888;

	FILE *fp = fopen("tw6869_ch0.dat", "rb");
	if(fp == NULL){
		return;
	}
	
	int count = 0;
	int num = 0;
	int total = 0;
	do{
		num = fread(image, sizeof(unsigned char), 5*720*240*2, fp);
		total += num;		
		printf("num:%d count:%d\n",num,count++);

	}while(num > 0);

	int i;
	for(i = 0; i < count*720*240; i++){
		rgb5652rgb888(image, image_888);
		image += 2;
		image_888 += 3;
	}

	IplImage *img = cvCreateImage(cvSize(720,240),8,3);


  	for(i = 0; i < count; i++){
		cvZero(img);
		memcpy(img->imageData, image_tmp+720*240*3*i, 720*240*2);
		cvShowImage("test",img);
		cvWaitKey(0);
	}


	free(image);
}
