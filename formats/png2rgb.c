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
 * �������ƣ� IsnotPng
 * �����������ж��Ƿ�ΪPNG�ļ�
 * ��������� ppFp - �ļ����ָ��
 					strFileName - �ļ���
 * �� �� ֵ��0 - ��PNG��ʽ ����-����PNG��ʽ
 ***********************************************************************/
int IsnotPng(FILE **ppFp, const char *strFileName) 
{
	char strCheckHeader[8]; 
	*ppFp= fopen(strFileName, "rb");
	if (*ppFp== NULL) {
		return -1;
	}
	/* ��ȡPNG�ļ�ǰ8���ֽڣ�ʹ�ÿ⺯��png_sig_cmp�����ж��Ƿ�ΪPNG��ʽ */
	if (fread(strCheckHeader, 1, 8, *ppFp) != 8) 
		return -1;
	return png_sig_cmp(strCheckHeader, 0, 8); 

}

/**********************************************************************
 * �������ƣ� DecodePng2Rgb
 * ������������PNG�ļ�����ΪRGB888��ʽ
 * ��������� ptData - �ں��ļ���Ϣ
 *                             strFileName - �ļ���
 * ���������PT_PictureData->pucRgbData - �ں�rgb����
 * �� �� ֵ��0 - �ɹ� ����-ʧ��
 ***********************************************************************/
static int DecodePng2Rgb(const char *strFileName, PT_PictureData ptData) 
{	 
	int i, j;
	int iPos = 0;
	png_bytepp pucPngData; 
	/* 0.�жϸ��ļ��Ƿ�ΪPNG��ʽ */
	if (IsnotPng(&ptData->ptFp, strFileName)) {
		printf("file is not png ...\n");
		return -1;
	} 

	/* 1.����ͳ�ʼ��������libpng��صĽṹ��png_ptr��info_ptr */
	ptData->ptPngStrPoint  = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL); 
	ptData->ptPngInfoPoint= png_create_info_struct(ptData->ptPngStrPoint);

 	/* 2.���ô���ķ��ص� */
	setjmp(png_jmpbuf(ptData->ptPngStrPoint));
	rewind(ptData->ptFp); //�ȼ�fseek(fp, 0, SEEK_SET);

 	/* 3.ָ��Դ�ļ� */
	png_init_io(ptData->ptPngStrPoint, ptData->ptFp);

	/* 4.��ȡPNGͼ��������Ϣ��ͨ��������ȣ��߶ȵ� 
	  * ʹ��PNG_TRANSFORM_EXPAND���������������Ǹ���ͨ�����Ĳ�ͬ��
	  * ��PNGͼ��ת��ΪBGR888��ABGR8888��ʽ*/
	png_read_png(ptData->ptPngStrPoint, ptData->ptPngInfoPoint, PNG_TRANSFORM_EXPAND, 0); 
	ptData->iChannels 	= png_get_channels(ptData->ptPngStrPoint, ptData->ptPngInfoPoint); 
	ptData->iWidth 	 = png_get_image_width(ptData->ptPngStrPoint, ptData->ptPngInfoPoint);
	ptData->iHeight  = png_get_image_height(ptData->ptPngStrPoint, ptData->ptPngInfoPoint);


	/* 5.��info_ptr�е�ͼ�����ݶ�ȡ���� */
	pucPngData = png_get_rows(ptData->ptPngStrPoint, ptData->ptPngInfoPoint); //Ҳ���Էֱ�ÿһ�л�ȡpng_get_rowbytes();
	if (ptData->iChannels == 4) { //�ж���24λ����32λ
		ptData->iRawSize= ptData->iWidth * ptData->iHeight*4; //�����ڴ��ȼ���ռ�	
		ptData->pucRawData= (unsigned char*)malloc(ptData->iRawSize);
		if (NULL == ptData->pucRawData) {
			printf("malloc rgba faile ...\n");
			png_destroy_read_struct(&ptData->ptPngStrPoint, &ptData->ptPngInfoPoint, 0);
			fclose(ptData->ptFp);
			return -1;
		}
		/* ��pucPngData�����ʵ�ʵ�RGBA���ݳ��� 
		 * Դ����ΪABGR��ʽ*/
		for (i = 0; i < ptData->iHeight; i++) 
			for (j = 0; j < ptData->iWidth * 4; j += 4) {
					ptData->pucRawData[iPos++] = pucPngData[i][j + 3];
					ptData->pucRawData[iPos++] = pucPngData[i][j + 2];
					ptData->pucRawData[iPos++] = pucPngData[i][j + 1];
					ptData->pucRawData[iPos++] = pucPngData[i][j + 0];
				}

		/* ���õ���RGBAת��ΪRGB888��ʽ */
		if(RgbaToRgb(ptData)!=0)
			return -1;

	}
	else if (ptData->iChannels == 3 ) { //�ж���ɫ�����24λ����32λ
		ptData->iRgbSize= ptData->iWidth * ptData->iHeight*3; //�����ڴ��ȼ���ռ�	
		ptData->pucRgbData = (unsigned char*)malloc(ptData->iRgbSize);
		if (NULL == ptData->pucRgbData) {
			printf("malloc rgba faile ...\n");
			png_destroy_read_struct(&ptData->ptPngStrPoint, &ptData->ptPngInfoPoint, 0);
			fclose(ptData->ptFp);
			return -1;
		}
		/* ��pucPngData�����ʵ�ʵ�RGB����
		  * Դ����ΪBGR��ʽ*/
		for (i = 0; i < ptData->iHeight; i ++) {
			for (j = 0; j < ptData->iWidth*3; j += 3) {
				ptData->pucRgbData[iPos++] = pucPngData[i][j+2];
				ptData->pucRgbData[iPos++] = pucPngData[i][j+1];
				ptData->pucRgbData[iPos++] = pucPngData[i][j+0];
			}
		}
		ptData->iBpp = 24;//ת��֮��ĸ�ʽΪRGB888��ʽ
	} 
	else return -1; 

	
	/* 6:�����ڴ� */
	png_destroy_read_struct(&ptData->ptPngStrPoint, &ptData->ptPngInfoPoint, 0);
	fclose(ptData->ptFp);


	return 0;
} 



/**********************************************************************
 * �������ƣ� ReleasePng2Rgb
 * �����������ͷŽ�����ʹ��PNGʱʹ�õ��ڴ�
 * ��������� ptPicData - �ں��ļ���Ϣ
 * �� �� ֵ����
 ***********************************************************************/
static void ReleasePng2Rgb(PT_PictureData ptPicData){
	free(ptPicData->pucRotateData);
	free(ptPicData->pucZoomData);
	free(ptPicData->pucRgbData);
}

/**********************************************************************
 * �������ƣ� PngParInit
 * ���������� ע��"PNGͼ������㷨ģ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int PngParInit(void)
{
	return RegisterPicParOpr(&g_tPng2RgbOpr);
}