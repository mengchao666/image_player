#include<formats_manager.h>
#include <string.h>

static PT_Picture2RGB g_ptPicParHead;

/**********************************************************************
 * 函数名称： RegisterPicParOpr
 * 功能描述： 注册"图像解析模块", 把所能支持的图像解析模块的操作函数放入链表
 * 输入参数： ptPicParOpr - 一个结构体,内含对应的图像解析算法
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int RegisterPicParOpr(PT_Picture2RGB ptPicParOpr){
	PT_Picture2RGB ptTmp;

	if (!g_ptPicParHead)
	{
		g_ptPicParHead   = ptPicParOpr;
		ptPicParOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptPicParHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptPicParOpr;
		ptPicParOpr->ptNext = NULL;
	}

	return 0;
}

/**********************************************************************
 * 函数名称： PicParseInit
 * 功能描述： 注册图像解析算法
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int PicParseInit(void)
{
	int iError;
	
	iError = BmpParInit()||PngParInit()||JpgParInit();

	return iError;
}

/**********************************************************************
 * 函数名称： GetFmtOpr
 * 功能描述： 根据图像解析模块
 * 输入参数： pcName - 名字
 * 输出参数： 无
 * 返 回 值： NULL   - 失败,没有指定的模块, 
 *            非NULL - 图像解析模块的PT_PictureParsing结构体指针
 ***********************************************************************/
PT_Picture2RGB GetFmtOpr(char *pcName)
{
	PT_Picture2RGB ptTmp = g_ptPicParHead;
	
	while (ptTmp)
	{
		if (strcmp(ptTmp->name, pcName) == 0)
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}

/**********************************************************************
 * 函数名称： RgbaToRgb
 * 功能描述：判断是否为PNG文件
 * 输入参数： ptData - 内含文件信息
 * 输出参数：PT_PictureData->pucRgbData - 内含rgb数据
 * 返 回 值：0 - 成功 其他-失败
 ***********************************************************************/
int RgbaToRgb(PT_PictureData ptData){
	int i ,j;
	int iPos = 0;
	ptData->iRgbSize= ptData->iHeight*ptData->iWidth * 3; 
	ptData->pucRgbData = (unsigned char*)malloc(ptData->iRgbSize);
	if (NULL == ptData->pucRgbData) {
	printf("malloc rgb faile ...\n");
	png_destroy_read_struct(&ptData->ptPngStrPoint, &ptData->ptPngInfoPoint, 0);
	fclose(ptData->ptFp);
	return -1;
	}

	for(i = 0;i<ptData->iHeight;i++)
	{
		for(j = 0;j<ptData->iWidth*4;j+=4){
		ptData->pucRgbData[iPos++] = ptData->pucRawData[i*ptData->iWidth*4+j+3];
		ptData->pucRgbData[iPos++] = ptData->pucRawData[i*ptData->iWidth*4+j+2] ;
		ptData->pucRgbData[iPos++] = ptData->pucRawData[i*ptData->iWidth*4+j+1];
		}
	}
	ptData->iBpp = 24;
	free(ptData->pucRawData);
	return 0;
}

