#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <string.h>
#include <display_manager.h>
#include <formats_manager.h>
#include <operate_manager.h>
#define FB_DEVICE_NAME "/dev/fb0"
static int g_iFbFd;

struct fb_var_screeninfo g_tFBVar;
struct fb_fix_screeninfo g_tFBFix;			
unsigned char *g_pucFBMem;
unsigned int g_dwScreenSize;

static unsigned int g_dwLineWidth;
static unsigned int g_dwPixelWidth;
static int FBDeviceInit(void);
static void FbShowPicture(PT_PictureData ptData);
static int FBCleanScreen(unsigned int dwBackColor);
static int FBDeviceExit(void);
static T_DispOpr g_tFBOpr = {
	.name = "fb",
	.DeviceInit = FBDeviceInit,     
	.ShowPicture = FbShowPicture,    
	.CleanScreen = FBCleanScreen,
	.DeviceExit = FBDeviceExit,
};





/**********************************************************************
 * �������ƣ� FBDeviceInit
 * ���������� "framebuffer��ʾ�豸"�ĳ�ʼ������
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
static int FBDeviceInit(void)
{
	int ret;
	
	g_iFbFd = open(FB_DEVICE_NAME, O_RDWR);
	if (0 > g_iFbFd)
	{
		printf("can't open %s\n", FB_DEVICE_NAME);
	}

	ret = ioctl(g_iFbFd, FBIOGET_VSCREENINFO, &g_tFBVar);
	if (ret < 0)
	{
		printf("can't get fb's var\n");
		return -1;
	}

	ret = ioctl(g_iFbFd, FBIOGET_FSCREENINFO, &g_tFBFix);
	if (ret < 0)
	{
		printf("can't get fb's fix\n");
		return -1;
	}
	
	g_dwScreenSize = g_tFBVar.xres * g_tFBVar.yres * g_tFBVar.bits_per_pixel / 8;
	g_pucFBMem = (unsigned char *)mmap(NULL , g_dwScreenSize, PROT_READ | PROT_WRITE, MAP_SHARED, g_iFbFd, 0);
	if (0 > g_pucFBMem)	
	{
		printf("can't mmap\n");
		return -1;
	}

	g_dwLineWidth  = g_tFBVar.xres * g_tFBVar.bits_per_pixel / 8;
	g_dwPixelWidth = g_tFBVar.bits_per_pixel / 8;
	
	return 0;
}


/**********************************************************************
 * �������ƣ� FBShowPixel
 * ������������LCD�ϵ�ĳ�����ص���Ϊrgbֵ
 * ��������� iX - x�������������
 * 				     iY - y�������������
 *                		     dwColor - ��ɫֵ����ʽΪrgb888 
 * �� �� ֵ�� 0 - �ɹ�
 *                         -1 - ʧ�� 
 ***********************************************************************/
static int FBShowPixel(int iX, int iY, unsigned int dwColor)
{
	unsigned char *pucFB;
	unsigned short *pwFB16bpp;
	unsigned int *pdwFB32bpp;
	unsigned short wColor16bpp; /* 565 */
	int iRed;
	int iGreen;
	int iBlue;

	if ((iX >= g_tFBVar.xres) || (iY >= g_tFBVar.yres))
	{
		printf("out of region\n");
		return -1;
	}

	pucFB      = g_pucFBMem + g_dwLineWidth * iY + g_dwPixelWidth * iX;
	pwFB16bpp  = (unsigned short *)pucFB;
	pdwFB32bpp = (unsigned int *)pucFB;
	
	switch (g_tFBVar.bits_per_pixel)
	{
		case 8:
		{
			*pucFB = (unsigned char)dwColor;
			break;
		}
		case 16:
		{
			iRed   = (dwColor >> (16+3)) & 0x1f;
			iGreen = (dwColor >> (8+2)) & 0x3f;
			iBlue  = (dwColor >> 3) & 0x1f;
			wColor16bpp = (iRed << 11) | (iGreen << 5) | iBlue;
			*pwFB16bpp	= wColor16bpp;
			break;
		}
		case 32:
		{
			*pdwFB32bpp = dwColor;
			break;
		}
		default :
		{
			printf("can't support %d bpp\n", g_tFBVar.bits_per_pixel);
			return -1;
		}
	}

	return 0;
}


/**********************************************************************
 * �������ƣ� FbShowPicture
 * ������������ת�����rgb������ʾ��LCD��
 * ��������� ptData - �ں�ͼ����Ϣ
 * �� �� ֵ�� ��
 ***********************************************************************/
static void FbShowPicture(PT_PictureData ptData){
	int i ,j;
	int iColor;
	for(i = 0;i<ptData->iRotateHeight;i++){
		for(j = 0;j<ptData->iRotateWidth*3;j+=3){
			if(j/3<g_tFBVar.xres&&i<g_tFBVar.yres){
				iColor = (ptData->pucRotateData[i*ptData->iRotateWidth*3+j+0]<<16) + (ptData->pucRotateData[i*ptData->iRotateWidth*3+j+1]<<8) + (ptData->pucRotateData[i*ptData->iRotateWidth*3+j+2]<<0);
				if(FBShowPixel(j/3, i, iColor)!=0){
					printf("FBShowPixel error,postion:x = %d,y = %d\n",i,j/3);
				}	
			}
		}
	}
}

/**********************************************************************
 * �������ƣ� FBCleanScreen
 * ������������LCD��Ϊĳ����ɫ
 * ��������� dwBackColor - ��ɫֵ
 * �� �� ֵ�� 0 - �ɹ�
 *                        -1 -ʧ��
 ***********************************************************************/
static int FBCleanScreen(unsigned int dwBackColor)
{
	unsigned char *pucFB;
	unsigned short *pwFB16bpp;
	unsigned int *pdwFB32bpp;
	unsigned short wColor16bpp; /* 565 */
	int iRed;
	int iGreen;
	int iBlue;
	int i = 0;

	pucFB      = g_pucFBMem;
	pwFB16bpp  = (unsigned short *)pucFB;
	pdwFB32bpp = (unsigned int *)pucFB;

	switch (g_tFBVar.bits_per_pixel)
	{
		case 8:
		{
			memset(g_pucFBMem, dwBackColor, g_dwScreenSize);
			break;
		}
		case 16:
		{
			iRed   = (dwBackColor >> (16+3)) & 0x1f;
			iGreen = (dwBackColor >> (8+2)) & 0x3f;
			iBlue  = (dwBackColor >> 3) & 0x1f;
			wColor16bpp = (iRed << 11) | (iGreen << 5) | iBlue;
			while (i < g_dwScreenSize)
			{
				*pwFB16bpp	= wColor16bpp;
				pwFB16bpp++;
				i += 2;
			}
			break;
		}
		case 32:
		{
			while (i < g_dwScreenSize)
			{
				*pdwFB32bpp	= dwBackColor;
				pdwFB32bpp++;
				i += 4;
			}
			break;
		}
		default :
		{
			printf("can't support %d bpp\n", g_tFBVar.bits_per_pixel);
			return -1;
		}
	}

	return 0;
}


static int FBDeviceExit(){
	return munmap(g_pucFBMem,g_dwScreenSize);
}

/**********************************************************************
 * �������ƣ� FBInit
 * ���������� ע��"framebuffer��ʾ�豸"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int FBInit(void)
{
	return RegisterDispOpr(&g_tFBOpr);
}


