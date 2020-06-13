#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>

#include <formats_manager.h>

static int DecodePng2Rgb(const char *strFileName, PT_PictureData ptData) ;
static void ReleasePng2Rgb(PT_PictureData ptPicData);
static T_Picture2RGB g_tPng2RgbOpr = {
	.name = "png2rgb",
	.PictureParsing = DecodePng2Rgb,
	.PictureRelease = ReleasePng2Rgb,
};



/**********************************************************************
 * 函数名称： IsnotPng
 * 功能描述：判断是否为PNG文件
 * 输入参数： ppFp - 文件句柄指针
 					strFileName - 文件名
 * 返 回 值：0 - 是PNG格式 其他-不是PNG格式
 ***********************************************************************/
int IsnotPng(FILE **ppFp, const char *strFileName) 
{
	char strCheckHeader[8]; 
	*ppFp= fopen(strFileName, "rb");
	if (*ppFp== NULL) {
		return -1;
	}
	/* 读取PNG文件前8个字节，使用库函数png_sig_cmp即可判断是否为PNG格式 */
	if (fread(strCheckHeader, 1, 8, *ppFp) != 8) 
		return -1;
	return png_sig_cmp(strCheckHeader, 0, 8); 

}

/**********************************************************************
 * 函数名称： DecodePng2Rgb
 * 功能描述：把PNG文件解析为RGB888格式
 * 输入参数： ptData - 内含文件信息
 *                             strFileName - 文件名
 * 输出参数：PT_PictureData->pucRgbData - 内含rgb数据
 * 返 回 值：0 - 成功 其他-失败
 ***********************************************************************/
static int DecodePng2Rgb(const char *strFileName, PT_PictureData ptData) 
{	 
	int i, j;
	int iPos = 0;
	png_bytepp pucPngData; 
	/* 0.判断该文件是否为PNG格式 */
	if (IsnotPng(&ptData->ptFp, strFileName)) {
		printf("file is not png ...\n");
		return -1;
	} 

	/* 1.分配和初始化两个与libpng相关的结构体png_ptr，info_ptr */
	ptData->ptPngStrPoint  = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); 
	ptData->ptPngInfoPoint= png_create_info_struct(ptData->ptPngStrPoint);

 	/* 2.设置错误的返回点 */
	setjmp(png_jmpbuf(ptData->ptPngStrPoint));
	rewind(ptData->ptFp); //等价fseek(fp, 0, SEEK_SET);

 	/* 3.指定源文件 */
	png_init_io(ptData->ptPngStrPoint, ptData->ptFp);

	/* 4.获取PNG图像数据信息和通道数，宽度，高度等 
	  * 使用PNG_TRANSFORM_EXPAND宏做参数的作用是根据通道数的不同，
	  * 将PNG图像转换为BGR888或ABGR8888格式*/
	png_read_png(ptData->ptPngStrPoint, ptData->ptPngInfoPoint, PNG_TRANSFORM_EXPAND, 0); 
	ptData->iChannels 	= png_get_channels(ptData->ptPngStrPoint, ptData->ptPngInfoPoint); 
	ptData->iWidth 	 = png_get_image_width(ptData->ptPngStrPoint, ptData->ptPngInfoPoint);
	ptData->iHeight  = png_get_image_height(ptData->ptPngStrPoint, ptData->ptPngInfoPoint);


	/* 5.将info_ptr中的图像数据读取出来 */
	pucPngData = png_get_rows(ptData->ptPngStrPoint, ptData->ptPngInfoPoint); //也可以分别每一行获取png_get_rowbytes();
	if (ptData->iChannels == 4) { //判断是24位还是32位
		ptData->iRawSize= ptData->iWidth * ptData->iHeight*4; //申请内存先计算空间	
		ptData->pucRawData= (unsigned char*)malloc(ptData->iRawSize);
		if (NULL == ptData->pucRawData) {
			printf("malloc rgba faile ...\n");
			png_destroy_read_struct(&ptData->ptPngStrPoint, &ptData->ptPngInfoPoint, 0);
			fclose(ptData->ptFp);
			return -1;
		}
		/* 从pucPngData里读出实际的RGBA数据出来 
		 * 源数据为ABGR格式*/
		for (i = 0; i < ptData->iHeight; i++) 
			for (j = 0; j < ptData->iWidth * 4; j += 4) {
					ptData->pucRawData[iPos++] = pucPngData[i][j + 3];
					ptData->pucRawData[iPos++] = pucPngData[i][j + 2];
					ptData->pucRawData[iPos++] = pucPngData[i][j + 1];
					ptData->pucRawData[iPos++] = pucPngData[i][j + 0];
				}

		/* 将得到的RGBA转换为RGB888格式 */
		if(RgbaToRgb(ptData)!=0)
			return -1;

	}
	else if (ptData->iChannels == 3 ) { //判断颜色深度是24位还是32位
		ptData->iRgbSize= ptData->iWidth * ptData->iHeight*3; //申请内存先计算空间	
		ptData->pucRgbData = (unsigned char*)malloc(ptData->iRgbSize);
		if (NULL == ptData->pucRgbData) {
			printf("malloc rgba faile ...\n");
			png_destroy_read_struct(&ptData->ptPngStrPoint, &ptData->ptPngInfoPoint, 0);
			fclose(ptData->ptFp);
			return -1;
		}
		/* 从pucPngData里读出实际的RGB数据
		  * 源数据为BGR格式*/
		for (i = 0; i < ptData->iHeight; i ++) {
			for (j = 0; j < ptData->iWidth*3; j += 3) {
				ptData->pucRgbData[iPos++] = pucPngData[i][j+2];
				ptData->pucRgbData[iPos++] = pucPngData[i][j+1];
				ptData->pucRgbData[iPos++] = pucPngData[i][j+0];
			}
		}
		ptData->iBpp = 24;//转化之后的格式为RGB888格式
	} 
	else return -1; 

	
	/* 6:销毁内存 */
	png_destroy_read_struct(&ptData->ptPngStrPoint, &ptData->ptPngInfoPoint, 0);
	fclose(ptData->ptFp);


	return 0;
} 



/**********************************************************************
 * 函数名称： ReleasePng2Rgb
 * 功能描述：释放解析及使用PNG时使用的内存
 * 输入参数： ptPicData - 内含文件信息
 * 返 回 值：无
 ***********************************************************************/
static void ReleasePng2Rgb(PT_PictureData ptPicData){
	free(ptPicData->pucRotateData);
	free(ptPicData->pucZoomData);
	free(ptPicData->pucRgbData);
}

/**********************************************************************
 * 函数名称： PngParInit
 * 功能描述： 注册"PNG图像解析算法模块"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int PngParInit(void)
{
	return RegisterPicParOpr(&g_tPng2RgbOpr);
}