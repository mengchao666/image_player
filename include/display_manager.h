#ifndef _DISP_MANAGER_H
#define _DISP_MANAGER_H

#include <formats_manager.h>

typedef struct DispOpr {
	char *name;              /* 显示模块的名字 */
	int (*DeviceInit)(void);     /* 设备初始化函数 */
	void (*ShowPicture)(PT_PictureData ptData);    
	int (*CleanScreen)(unsigned int dwBackColor);                    /* 清屏为某颜色 */
	int (*DeviceExit)(void);
	struct DispOpr *ptNext;      /* 链表 */
}T_DispOpr, *PT_DispOpr;

/**********************************************************************
 * 函数名称： RegisterDispOpr
 * 功能描述： 注册"显示模块", 把所能支持的显示设备的操作函数放入链表
 * 输入参数： ptDispOpr - 一个结构体,内含显示设备的操作函数
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int RegisterDispOpr(PT_DispOpr ptDispOpr);

/**********************************************************************
 * 函数名称： DisplayInit
 * 功能描述： 注册显示设备
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int DisplayInit(void);

/**********************************************************************
 * 函数名称： SelectAndInitDefaultDispDev
 * 功能描述： 根据名字取出指定的"显示模块", 调用它的初始化函数, 并且清屏
 * 输入参数： name - 名字
 * 输出参数： 无
 * 返 回 值： 无
 ***********************************************************************/
void SelectAndInitDefaultDispDev(char *name);


/**********************************************************************
 * 函数名称： GetDefaultDispDev
 * 功能描述： 程序事先用SelectAndInitDefaultDispDev选择了显示模块,
 *            本函数返回该显示模块
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 显示模块的PT_DispOpr结构体指针
 ***********************************************************************/
PT_DispOpr GetDefaultDispDev(void);

/**********************************************************************
 * 函数名称： FBInit
 * 功能描述： 注册"framebuffer显示设备"
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 ***********************************************************************/
int FBInit(void);

#endif /* _DISP_MANAGER_H */


