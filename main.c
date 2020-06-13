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

#include <display_manager.h>
#include <formats_manager.h>
#include <operate_manager.h>



/* PicPrc <format><file><size 0-10 of zoom><angle 0-360 of rotate> */
int main(int argc, char **argv)
{	
	int iError;
	float fZoomSize;
	float fRotateAngle;
	PT_Picture2RGB ptPicPar;
	PT_DispOpr ptDispOpr;
	T_PictureData tPictureData;
	if (argc != 5){
		printf("Usage:\n");
		printf("%s <format><file><size 0-10 of zoom><angle 0-360 of rotate>\n", argv[0]);
		printf("Example:%s png2rgb test.png 2.2 222.2",argv[0]);
		return 0;
	}


	fZoomSize = atof(argv[3]);
	fRotateAngle = atof( argv[4]);
	if((fZoomSize<0||fZoomSize>10)||(fRotateAngle<0||fRotateAngle>360)){
		printf("Usage:\n");
		printf("%s <format><file><size 0-10 of zoom><angle 0-360 of rotate>\n", argv[0]);
		printf("Example:%s png2rgb test.png 2.2 222.2",argv[0]);
		return 0;
	}
	
	/* 注册显示设备 */
	iError = DisplayInit();
	if(iError != 0){
		printf("DisplayInit fault\n");
		return -1;
	}
	/* 注册图像解析模块 */
	iError = PicParseInit();
	if(iError != 0){
		printf("PicParseInit fault\n");
		return -1;
	}
	/* 可能可支持多个显示设备: 选择和初始化指定的显示设备 */
	SelectAndInitDefaultDispDev("fb");

	ptDispOpr = GetDefaultDispDev();
	if(ptDispOpr==NULL){
		printf("ptDispOpr is null\n");
		return -1;
	}
	ptPicPar = GetFmtOpr(argv[1]);
	if(ptPicPar==NULL){
		printf("convert format is not supported\n");
		return -1;
	}

	ptDispOpr->CleanScreen(0);

	iError = ptPicPar->PictureParsing(argv[2],&tPictureData);

	if(iError != 0){
		printf("Picture Parsing is fault\n");
		return -1;
	}

	iError = PicZoom(&tPictureData,fZoomSize);
	if(iError != 0){
		printf("PicZoom fault\n");
		return -1;
	}
	iError = PicRotate(&tPictureData, fRotateAngle);
	if(iError != 0){
		printf("PicRotate fault\n");
		return -1;
	}
	ptDispOpr->ShowPicture(&tPictureData);

	
	ptPicPar->PictureRelease(&tPictureData);
	if(ptDispOpr->DeviceExit()!=0){
		printf("display device exit fault\n");
		return -1;
	}
	return 0;
}
