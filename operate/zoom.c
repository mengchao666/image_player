#include <stdlib.h>
#include <string.h>
#include <formats_manager.h>
#include <operate_manager.h>

/**********************************************************************
 * 函数名称： PicZoom
 * 功能描述： 近邻取样插值方法缩放图片
 *            注意该函数会分配内存来存放缩放后的图片,用完后要用free函数释放掉
 *            "近邻取样插值"的原理请参考网友"lantianyu520"所著的"图像缩放算法"
 * 输入参数： ptPicData - 内含缩放前后的图像数据
 *            fSize    - 缩放倍数
 * 输出参数： ptPicData->pucZoomData,内含缩放后的数据
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int PicZoom(PT_PictureData ptPicData,float fSize)
{
    ptPicData->iZoomWidth = ptPicData->iWidth * fSize;
	ptPicData->iZoomHeight= ptPicData->iHeight* fSize;
    unsigned long* pdwSrcXTable;
	unsigned long x;
	unsigned long y;
	unsigned long dwSrcY;
	unsigned char *pucDest;
	unsigned char *pucSrc;
	unsigned long dwPixelBytes = ptPicData->iBpp/8;
    ptPicData->pucZoomData= malloc(sizeof(unsigned char) * ptPicData->iZoomWidth*ptPicData->iZoomHeight*ptPicData->iBpp/8);
    pdwSrcXTable = malloc(sizeof(unsigned long) * ptPicData->iZoomWidth);
    if (NULL == pdwSrcXTable){
        printf("malloc error!\n");
        return -1;
    }

	/* 这几个for循环的本质是Sx = Dx * Sw/Dw，Sy = Dy * Sh/Dh*/
    for (x = 0; x < ptPicData->iZoomWidth; x++){//生成表 pdwSrcXTable
    	/* 第一个for循环对应x方向的坐标
	 * pdwSrcXTable[x] 对应Sx,
	 * x 对应Dx,
	 * ptPicData->iWidth 对应Sw
	 * ptPicData->iZoomWidth 对应 Dw*/	
        pdwSrcXTable[x]=(x*ptPicData->iWidth/ptPicData->iZoomWidth);
    }

    for (y = 0; y < ptPicData->iZoomHeight; y++){
	/* 第2个循环对应y方向的坐标
	 * dwSrcY 对应Sy,
	 * y 对应Dy,
	 * ptPicData->iHeight 对应Sh
	 * ptPicData->iZoomHeight 对应 Dh*/	
        dwSrcY = (y * ptPicData->iHeight / ptPicData->iZoomHeight);
	/* 根据这些可算得各像素点的RGB数据存放的地址 */
		pucDest = ptPicData->pucZoomData + y*ptPicData->iZoomWidth*3;
		pucSrc  = ptPicData->pucRgbData + dwSrcY*ptPicData->iWidth*3;

	/* 最后拷贝数据 */		
        for (x = 0; x <ptPicData->iZoomWidth; x++){
			 memcpy(pucDest+x*dwPixelBytes, pucSrc+pdwSrcXTable[x]*dwPixelBytes, dwPixelBytes);
        }
    }

    free(pdwSrcXTable);
	return 0;
}