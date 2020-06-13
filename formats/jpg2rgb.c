#include <stdio.h>
#include "jpeglib.h"
#include <setjmp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <string.h>
#include <stdlib.h>
#include <png.h>

#include <formats_manager.h>


static int DecodeJpg2Rgb(const char *strFileName, PT_PictureData ptData) ;
static void ReleaseJpg2Rgb(PT_PictureData ptPicData);
static T_Picture2RGB g_tJpg2RgbOpr = {
	.name = "jpg2rgb",
	.PictureParsing = DecodeJpg2Rgb,
	.PictureRelease = ReleaseJpg2Rgb,
};

/**********************************************************************
 * 函数名称： IsJpg
 * 功能描述：判断是否为Jpg文件
 * 输入参数： ptData - 内含图像信息
 					strFileName - 文件名
 * 返 回 值：0 - 不是JPG格式 其他-是JPG格式
 ***********************************************************************/
static int IsJpg(PT_PictureData ptData, const char *strFileName) 
{
	int iRet;

	jpeg_stdio_src(&ptData->tInfo, ptData->ptFp);

	/* 用jpeg_read_header获得jpeg信息*/
	iRet = jpeg_read_header(&ptData->tInfo, TRUE);
	
    	return (iRet == JPEG_HEADER_OK);
}

/**********************************************************************
 * 函数名称： DecodeJpg2Rgb
 * 功能描述：把JPG文件解析为RGB888格式
 * 输入参数： ptData - 内含文件信息
 *                             strFileName - 文件名
 * 输出参数：PT_PictureData->pucRgbData - 内含rgb数据
 * 返 回 值：0 - 成功 其他-失败
 ***********************************************************************/
static int DecodeJpg2Rgb(const char *strFileName, PT_PictureData ptData){
	int iRowSize;
	unsigned char *pucbuffer;
	unsigned char *pucHelp;//辅助拷贝变量
	
	/* 1.分配和初始化一个jpeg_compress_struct结构体 */
	ptData->tInfo.err = jpeg_std_error(&ptData->tJerr);
	jpeg_create_decompress(&ptData->tInfo);


	/* 2.指定源文件*/
	if ((ptData->ptFp= fopen(strFileName, "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", strFileName);
		return -1;
	}

	/* 3.获得jpg信息头并设置解压参数并判断是否为JPEG格式文件 */
	if (!IsJpg(ptData, strFileName)) {
	printf("file is not jpg ...\n");
	return -1;
	} 

	

	/* 默认尺寸为原尺寸 */
	ptData->tInfo.scale_num = 1;
	ptData->tInfo.scale_denom = 1;
	/* 4. 启动解压：jpeg_start_decompress */	
	jpeg_start_decompress(&ptData->tInfo);


	/* 解压完成后可以通过tInfo中的成员获得图像的某些信息 */
	ptData->iWidth= ptData->tInfo.output_width;
	ptData->iHeight = ptData->tInfo.output_height;
	ptData->iBpp = ptData->tInfo.output_components*8;
	/* 计算一行的数据长度 */ 
	iRowSize = ptData->iWidth * ptData->tInfo.output_components;
	pucbuffer = malloc(iRowSize);
	ptData->iRgbSize= iRowSize * ptData->iHeight;
	ptData->pucRgbData = malloc(ptData->iRgbSize);

	/* pucHelp指向ptData->pucRgbData首地址 */
	pucHelp = ptData->pucRgbData;
	/*  5.循环调用jpeg_read_scanlines来一行一行地获得解压的数据 */
	while (ptData->tInfo.output_scanline < ptData->tInfo.output_height) 
	{
		/* 调用jpeg_read_scanlines得到的时候会存到pucbuffer中 */
		jpeg_read_scanlines(&ptData->tInfo, &pucbuffer, 1);
		/* 将数据一行行读到缓冲区中 */
		memcpy(pucHelp,pucbuffer,iRowSize);
		pucHelp  += iRowSize;
	}
	free(pucbuffer);
	/* 6.完成读取 */
	jpeg_finish_decompress(&ptData->tInfo);
	/* 7.释放jpeg_compress_struct结构体 */
	jpeg_destroy_decompress(&ptData->tInfo);
	return 0;
}


/**********************************************************************
 * 函数名称： ReleaseJpg2Rgb
 * 功能描述：释放解析及使用JPG时使用的内存
 * 输入参数： ptPicData - 内含文件信息
 * 返 回 值：无
 ***********************************************************************/
void ReleaseJpg2Rgb(PT_PictureData ptData){
	free(ptData->pucRotateData);
	free(ptData->pucZoomData);
	free(ptData->pucRgbData);
}


/**********************************************************************
 * 函数名称： JpgParInit
 * 功能描述： 注册"JPG图像解析算法模块"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int JpgParInit(void)
{
	return RegisterPicParOpr(&g_tJpg2RgbOpr);
}
