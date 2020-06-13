#include <stdio.h>
#include <string.h>
#include <math.h>
#include <formats_manager.h>
#include <operate_manager.h>
 #define PI 3.1415926535
//角度到弧度转化
#define RADIAN(angle) ((angle)*PI/180.0)





typedef struct ConcernCoor {
	int iLTx;// left top x
	int iLTy;//left top y
	int iLBx;//left bottom x
	int iLBy;//left bottom y
	int iRTx;//right top x
	int iRTy;//right top y
	int iRBx;// right bottom x
	int iRBy;// right bottom y
}T_ConcernCoor, *PT_ConcernCoor;


/**********************************************************************
 * 函数名称： max
 * 功能描述：比较两个参数，返回较大值
 * 输入参数：x，y均为int型
 * 输出参数： 无
 * 返 回 值： x，y中的较大值
 ***********************************************************************/
static int max(int x,int y){
	return x>y?x:y;
}
/**********************************************************************
 * 函数名称： PicRotate
 * 功能描述： 旋转图片
 *            注意该函数会分配内存来存放缩放后的图片,用完后要用free函数释放掉
 *           	参考网友"落叶的思维"所著的"图像旋转算法与实现"
 * 输入参数： ptPicData - 内含图片的象素数据
 *            fAngle    - 旋转角度，0<=angle<=360
 * 输出参数： ptPicData->pucRotateData,内含旋转后的rgb数据
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int PicRotate(PT_PictureData ptPicData,float fAngle)
{
	int i ,j;
    T_ConcernCoor tConCor,tRonCor;
    //原图像每一行去除偏移量的字节数
    //int iSrcLineSize = bitCount * srcW / 8;
    int iSrcLineSize = ptPicData->iBpp* ptPicData->iZoomWidth / 8;
    int iDesLineSize;
    int iX;//旋转后的x坐标
    int iY;	//旋转后的y坐标

       /* 将坐标系A下的坐标转换为坐标系B下的坐标,
        * 用于计算旋转后的图像的宽和高 
        * tConCor用于存放坐标系B下旋转前的坐标
        * tRonCor用于存放坐标系B下旋转后的坐标*/
       tConCor.iLTx = -ptPicData->iZoomWidth/2; tConCor.iLTy = ptPicData->iZoomHeight/2;
	tConCor.iRTx = ptPicData->iZoomWidth/2; tConCor.iRTy = ptPicData->iZoomHeight/2;
	tConCor.iLBx = -ptPicData->iZoomWidth/2;tConCor.iLBy = -ptPicData->iZoomHeight/2;
	tConCor.iRBx = ptPicData->iZoomWidth/2;tConCor.iRBy = -ptPicData->iZoomHeight/2;


    /* 计算坐标系B下旋转后的坐标 */
    double sina = sin(RADIAN(fAngle));
    double cosa = cos(RADIAN(fAngle));
    tRonCor.iLTx =tConCor.iLTx * cosa + tConCor.iLTy * sina;
    tRonCor.iLTy = -tConCor.iLTx * sina + tConCor.iLTy * cosa;
    tRonCor.iRTx =tConCor.iRTx * cosa + tConCor.iRTy * sina;
	tRonCor.iRTy = -tConCor.iRTx * sina + tConCor.iRTy * cosa;
    tRonCor.iLBx = tConCor.iLBx * cosa + tConCor.iLBy * sina;
	tRonCor.iLBy = -tConCor.iLBx * sina + tConCor.iLBy * cosa;
    tRonCor.iRBx = tConCor.iRBx * cosa + tConCor.iRBy * sina;
	tRonCor.iRBy = -tConCor.iRBx * sina + tConCor.iRBy * cosa;

	
    /* 计算旋转后图像宽和高 */
    ptPicData->iRotateWidth = max(abs(tRonCor.iRBx - tRonCor.iLTx),abs(tRonCor.iRTx - tRonCor.iLBx));
    ptPicData->iRotateHeight = max(abs(tRonCor.iRBy - tRonCor.iLTy),abs(tRonCor.iRTy - tRonCor.iLBy));

	/* 像素信息要保证3字节对齐，否则数据有可能出错*/
	iDesLineSize = ((ptPicData->iRotateWidth* ptPicData->iBpp+ 23) / 24) * 3 ;
	/* 分配旋转后的空间，注意这里要用旋转后的宽和高 */
	ptPicData->pucRotateData = malloc(iDesLineSize * ptPicData->iRotateHeight);
	if(NULL == ptPicData->pucRotateData){
		printf("malloc error\n");
		return -1;
	}

    /* 通过新图像的坐标，计算对应的原图像的坐标*
      * i,j坐标就是对应的坐标系B下的x1,y1*/
    for (i = 0; i < ptPicData->iRotateHeight; i++){        
        for (j = 0; j < ptPicData->iRotateWidth; j++){
            /* 坐标系B下的x,y1坐标，经过逆运算转换得到iX,iY,这两个值对应x0,y0 */
            iX = (j - ptPicData->iRotateWidth / 2)*cos(RADIAN(360 - fAngle)) + (-i + ptPicData->iRotateHeight / 2)*sin(RADIAN(360 - fAngle));
            iY = -(j - ptPicData->iRotateWidth / 2)*sin(RADIAN(360 - fAngle)) + (-i + ptPicData->iRotateHeight / 2)*cos(RADIAN(360 - fAngle));
            /*如果这个坐标不在原图像内，则不赋值*/
            if (iX > ptPicData->iZoomWidth / 2 || iX < -ptPicData->iZoomWidth / 2 || iY > ptPicData->iZoomHeight / 2 || iY < -ptPicData->iZoomHeight / 2){
                continue;
            }
            /* 再将坐标系B下的x0,y0坐标，转换为坐标系A下的坐标 */
            int iXN = iX + ptPicData->iZoomWidth / 2; 
	     int iYN = abs(iY - ptPicData->iZoomHeight  / 2);
            /* 值拷贝*/
            memcpy(&ptPicData->pucRotateData[i * iDesLineSize + j * 3],&ptPicData->pucZoomData[iYN * iSrcLineSize + iXN * 3],3);  
        }
    }
  return 0;
}

