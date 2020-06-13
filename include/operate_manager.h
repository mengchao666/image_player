#ifndef  __OPERATE_MANAGER_H__ 
#define  __OPERATE_MANAGER_H__ 

#include <formats_manager.h>

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
int PicZoom(PT_PictureData ptPicData,float fSize);

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
int PicRotate(PT_PictureData ptPicData,float fAngle);


#endif /*  __OPERATE_MANAGER_H__  */

