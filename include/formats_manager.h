#ifndef  __FORMATS_MANAGER_H__ 
#define  __FORMATS_MANAGER_H__ 
#include <png.h>
#include "jpeglib.h"
#include <stdlib.h>



typedef struct PictureData {
	FILE *ptFp;//文件句柄
	int iBpp;
	int iRgbSize;
	
	int iWidth;
	int iHeight;
	unsigned char *pucRgbData;

	int iZoomWidth;
	int iZoomHeight;
	unsigned char *pucZoomData;

	int iRotateWidth;
	int iRotateHeight;
	unsigned char *pucRotateData;
	/* use for BMP */
	unsigned char *pucFileData;//整个bmp文件的数据	
	unsigned char *pucBmpData;//bmp图像的数据
	int iFileSize;//BMP文件大小
	int iBmpDataSize;//BMP原始数据rgba大小
	
	/* use for PNG */
	png_structp ptPngStrPoint;//png结构体指针
	png_infop ptPngInfoPoint;//png信息结构体指针
	unsigned char *pucRawData;//原始rgba数据
	int iRawSize;//原始rgba数据大小
	int iChannels;//原始数据通道数

		/* use for JPG */
	struct jpeg_decompress_struct tInfo;
	struct jpeg_error_mgr tJerr;
}T_PictureData, *PT_PictureData;



typedef struct Picture2RGB {
	char *name;
	int (*PictureParsing)(const char *strFileName, PT_PictureData ptData);
	void (*PictureRelease)(PT_PictureData ptData);
	struct Picture2RGB *ptNext;        /* 链表 */
}T_Picture2RGB, *PT_Picture2RGB;

/**********************************************************************
 * 函数名称： RegisterPicParOpr
 * 功能描述： 注册"图像解析模块", 把所能支持的图像解析模块的操作函数放入链表
 * 输入参数： ptPicParOpr - 一个结构体,内含对应的图像解析算法
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int RegisterPicParOpr(PT_Picture2RGB ptPicParOpr);

/**********************************************************************
 * 函数名称： PicParseInit
 * 功能描述： 注册图像解析算法
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int PicParseInit(void);



/**********************************************************************
 * 函数名称： GetFmtOpr
 * 功能描述： 根据图像解析模块
 * 输入参数： pcName - 名字
 * 输出参数： 无
 * 返 回 值： NULL   - 失败,没有指定的模块, 
 *            非NULL - 图像解析模块的PT_PictureParsing结构体指针
 ***********************************************************************/
PT_Picture2RGB GetFmtOpr(char *pcName);


/**********************************************************************
 * 函数名称： BmpParInit
 * 功能描述： 注册"BMP图像解析算法模块"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int BmpParInit(void);

/**********************************************************************
 * 函数名称： PngParInit
 * 功能描述： 注册"PNG图像解析算法模块"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int PngParInit(void);

/**********************************************************************
 * 函数名称： JpgParInit
 * 功能描述： 注册"JPG图像解析算法模块"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int JpgParInit(void);

/**********************************************************************
 * 函数名称： RgbaToRgb
 * 功能描述：判断是否为PNG文件
 * 输入参数： ptData - 内含文件信息
 * 输出参数：PT_PictureData->pucRgbData - 内含rgb数据
 * 返 回 值：0 - 成功 其他-失败
 ***********************************************************************/
int RgbaToRgb(PT_PictureData ptData);


#endif /* __FORMATS_MANAGER_H__ */

