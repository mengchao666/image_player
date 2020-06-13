#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>


#include <formats_manager.h>

#pragma pack(push) /* 将当前pack设置压栈保存 */
#pragma pack(1)    /* 必须在结构体定义之前使用,这是为了让结构体中各成员按1字节对齐 */
typedef struct tagBITMAPFILEHEADER { /* bmfh */
	unsigned short bfType; 
	unsigned long  bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned long  bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER { /* bmih */
	unsigned long  biSize;
	unsigned long  biWidth;
	unsigned long  biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned long  biCompression;
	unsigned long  biSizeImage;
	unsigned long  biXPelsPerMeter;
	unsigned long  biYPelsPerMeter;
	unsigned long  biClrUsed;
	unsigned long  biClrImportant;
} BITMAPINFOHEADER;

#pragma pack(pop) /* 恢复先前的pack设置 */


static int DecodeBmp2Rgb(const char *strFileName, PT_PictureData ptData) ;
static void ReleaseBmp2Rgb(PT_PictureData ptPicData);
static T_Picture2RGB g_tBmp2RgbOpr = {
	.name = "bmp2rgb",
	.PictureParsing = DecodeBmp2Rgb,
	.PictureRelease = ReleaseBmp2Rgb,
};


/**********************************************************************
 * 函数名称： IsBmp
 * 功能描述： 判断该文件是否为BMP文件
 * 输入参数： ptFileMap - 内含文件信息
 * 输出参数： 无
 * 返 回 值： 0 - 是BMP格式, -1 -不是BMP格式
 ***********************************************************************/
int IsBmp(FILE **ppFp, const char *strFileName) 
{
	char strCheckHeader[2]; 
	*ppFp= fopen(strFileName, "rb+");
	if (*ppFp== NULL) {
		return -1;
	}
	if (fread(strCheckHeader, 1, 2, *ppFp) != 2) 
		return -1;
    
	if (strCheckHeader[0] != 0x42 || strCheckHeader[1] != 0x4d)
		return -1;
	else
		return 0;
}



/**********************************************************************
 * 函数名称： MapFile
 * 功能描述： 使用mmap函数映射一个文件到内存,以后就可以直接通过内存来访问文件
 * 输入参数： PT_PictureData ptData 内含图像数据
 * 输出参数： ptData->iFileSize     : 文件大小
 *                        ptData->pucFileData : 映射内存的首地址
 * 返 回 值： 0      - 成功其他值 - 失败
 ***********************************************************************/
int MapFile(PT_PictureData ptData)
{
	int iFd;
	struct stat tStat;
	
	/* 打开文件 */
    	iFd = fileno(ptData->ptFp);
	fstat(iFd, &tStat);
	ptData->iFileSize= tStat.st_size;
	ptData->pucFileData= (unsigned char *)mmap(NULL , tStat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, iFd, 0);
	if (ptData->pucFileData == (unsigned char *)-1)
	{
		printf("mmap error!\n");
		return -1;
	}
	return 0;
}

/**********************************************************************
 * 函数名称： DecodeBmp2Rgb
 * 功能描述：把BMP文件转化为rgb格式
 * 输入参数： strFileName - 文件名
 * 					 ptData - 内含图像信息
 * 返 回 值： 0      - 成功其他值 - 失败
 *                         -1     - 文件不是BMP格式
 *                         -2     - 不支持的bpp
 *                         -3     - 图像缓存区分配失败
 ***********************************************************************/
static int DecodeBmp2Rgb(const char *strFileName, PT_PictureData ptData) {
	int x,y;
	int iPos = 0;
	int iLineWidthAlign;
	BITMAPFILEHEADER *ptBITMAPFILEHEADER;
	BITMAPINFOHEADER *ptBITMAPINFOHEADER;
	unsigned char *aFileHead;
	unsigned char *pucSrc;
	unsigned char *pucDest;
	int iLineBytes;

	/* 判断该文件是否为BMP格式 */
	if (IsBmp(&ptData->ptFp, strFileName)) 
		return -1;

	/* 将BMP文件映射到内存空间 */	
	MapFile(ptData);


	aFileHead = ptData->pucFileData;

	ptBITMAPFILEHEADER = (BITMAPFILEHEADER *)aFileHead;
	ptBITMAPINFOHEADER = (BITMAPINFOHEADER *)(aFileHead + sizeof(BITMAPFILEHEADER));
	/* 获取必要的图像信息 */
	ptData->iWidth = ptBITMAPINFOHEADER->biWidth;
	ptData->iHeight = ptBITMAPINFOHEADER->biHeight;
	ptData->iBpp = ptBITMAPINFOHEADER->biBitCount;
    	iLineBytes    = ptData->iWidth*ptData->iBpp/8;//一行数据的字节数
	ptData->iBmpDataSize= ptData->iHeight * iLineBytes;//整个BMP图像的字节数
	/*暂时只支持24bpp格式*/
	if (ptData->iBpp != 24)
	{
		printf("iBMPBpp = %d\n", ptData->iBpp);
		printf("sizeof(BITMAPFILEHEADER) = %d\n", sizeof(BITMAPFILEHEADER));
		return -2;
	}

	/* 分配空间 */
	ptData->pucBmpData = malloc(ptData->iBmpDataSize);
	ptData->pucRgbData = malloc(ptData->iBmpDataSize);
	
	if (NULL == ptData->pucBmpData||NULL == ptData->pucRgbData)
		return -2;

	/* 从bmp文件中读取图像信息，24bpp的BMP图像为BGR格式 */
	pucDest = ptData->pucBmpData;
	iLineWidthAlign = (iLineBytes + 3) & ~0x3;   /* 向4取整 */
	pucSrc = aFileHead + ptBITMAPFILEHEADER->bfOffBits;

	pucSrc = pucSrc + (ptData->iHeight - 1) * iLineWidthAlign;

	/* 对于bmp文件中的源数据，是以左下角为原点计算坐标的，因此拷贝数据时需要转换坐标 */
	for (y = 0; y < ptData->iHeight; y++)
	{		
		memcpy(pucDest, pucSrc, ptData->iWidth*3);
		pucSrc  -= iLineWidthAlign;
		pucDest += iLineBytes;
	}

	
	/* 将得到的BGR数据转化为RGB数据 */
	for (y = 0; y < ptData->iHeight; y++){		
		for(x = 0;x<ptData->iWidth*3;x+=3){
			ptData->pucRgbData[iPos++] = ptData->pucBmpData[y*ptData->iWidth*3+x+2];
			ptData->pucRgbData[iPos++] = ptData->pucBmpData[y*ptData->iWidth*3+x+1];
			ptData->pucRgbData[iPos++] = ptData->pucBmpData[y*ptData->iWidth*3+x+0];
		}
	}
	
	return 0;	

}



/**********************************************************************
 * 函数名称： ReleaseBmp2Rgb
 * 功能描述：释放解析及使用BMP时的空间
 * 输入参数：ptData - 内含图像信息
 * 返 回 值： 无
 ***********************************************************************/
static void ReleaseBmp2Rgb(PT_PictureData ptData){
	free(ptData->pucRotateData);
	free(ptData->pucZoomData);
	free(ptData->pucRgbData);
	free(ptData->pucBmpData);
	if(munmap(ptData->pucFileData,ptData->iFileSize)!=0)
	printf("munmap fault\n");
}

/**********************************************************************
 * 函数名称： BmpParInit
 * 功能描述： 注册"BMP图像解析算法模块"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int BmpParInit(void)
{
	return RegisterPicParOpr(&g_tBmp2RgbOpr);
}

