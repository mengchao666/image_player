#include <stdio.h>
#include "jpeglib.h"
#include <setjmp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <string.h>
#include <stdlib.h>
#include <png.h>

#include <formats_manager.h>


static int DecodeJpg2Rgb(const char *strFileName, PT_PictureData ptData) ;
static void ReleaseJpg2Rgb(PT_PictureData ptPicData);
static T_Picture2RGB g_tJpg2RgbOpr = {
	.name = "jpg2rgb",
	.PictureParsing = DecodeJpg2Rgb,
	.PictureRelease = ReleaseJpg2Rgb,
};

/**********************************************************************
 * �������ƣ� IsJpg
 * �����������ж��Ƿ�ΪJpg�ļ�
 * ��������� ptData - �ں�ͼ����Ϣ
 					strFileName - �ļ���
 * �� �� ֵ��0 - ����JPG��ʽ ����-��JPG��ʽ
 ***********************************************************************/
static int IsJpg(PT_PictureData ptData, const char *strFileName) 
{
	int iRet;

	jpeg_stdio_src(&ptData->tInfo, ptData->ptFp);

	/* ��jpeg_read_header���jpeg��Ϣ*/
	iRet = jpeg_read_header(&ptData->tInfo, TRUE);
	
    	return (iRet == JPEG_HEADER_OK);
}

/**********************************************************************
 * �������ƣ� DecodeJpg2Rgb
 * ������������JPG�ļ�����ΪRGB888��ʽ
 * ��������� ptData - �ں��ļ���Ϣ
 *                             strFileName - �ļ���
 * ���������PT_PictureData->pucRgbData - �ں�rgb����
 * �� �� ֵ��0 - �ɹ� ����-ʧ��
 ***********************************************************************/
static int DecodeJpg2Rgb(const char *strFileName, PT_PictureData ptData){
	int iRowSize;
	unsigned char *pucbuffer;
	unsigned char *pucHelp;//������������
	
	/* 1.����ͳ�ʼ��һ��jpeg_compress_struct�ṹ�� */
	ptData->tInfo.err = jpeg_std_error(&ptData->tJerr);
	jpeg_create_decompress(&ptData->tInfo);


	/* 2.ָ��Դ�ļ�*/
	if ((ptData->ptFp= fopen(strFileName, "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", strFileName);
		return -1;
	}

	/* 3.���jpg��Ϣͷ�����ý�ѹ�������ж��Ƿ�ΪJPEG��ʽ�ļ� */
	if (!IsJpg(ptData, strFileName)) {
	printf("file is not jpg ...\n");
	return -1;
	} 

	

	/* Ĭ�ϳߴ�Ϊԭ�ߴ� */
	ptData->tInfo.scale_num = 1;
	ptData->tInfo.scale_denom = 1;
	/* 4. ������ѹ��jpeg_start_decompress */	
	jpeg_start_decompress(&ptData->tInfo);


	/* ��ѹ��ɺ����ͨ��tInfo�еĳ�Ա���ͼ���ĳЩ��Ϣ */
	ptData->iWidth= ptData->tInfo.output_width;
	ptData->iHeight = ptData->tInfo.output_height;
	ptData->iBpp = ptData->tInfo.output_components*8;
	/* ����һ�е����ݳ��� */ 
	iRowSize = ptData->iWidth * ptData->tInfo.output_components;
	pucbuffer = malloc(iRowSize);
	ptData->iRgbSize= iRowSize * ptData->iHeight;
	ptData->pucRgbData = malloc(ptData->iRgbSize);

	/* pucHelpָ��ptData->pucRgbData�׵�ַ */
	pucHelp = ptData->pucRgbData;
	/*  5.ѭ������jpeg_read_scanlines��һ��һ�еػ�ý�ѹ������ */
	while (ptData->tInfo.output_scanline < ptData->tInfo.output_height) 
	{
		/* ����jpeg_read_scanlines�õ���ʱ���浽pucbuffer�� */
		jpeg_read_scanlines(&ptData->tInfo, &pucbuffer, 1);
		/* ������һ���ж����������� */
		memcpy(pucHelp,pucbuffer,iRowSize);
		pucHelp  += iRowSize;
	}
	free(pucbuffer);
	/* 6.��ɶ�ȡ */
	jpeg_finish_decompress(&ptData->tInfo);
	/* 7.�ͷ�jpeg_compress_struct�ṹ�� */
	jpeg_destroy_decompress(&ptData->tInfo);
	return 0;
}


/**********************************************************************
 * �������ƣ� ReleaseJpg2Rgb
 * �����������ͷŽ�����ʹ��JPGʱʹ�õ��ڴ�
 * ��������� ptPicData - �ں��ļ���Ϣ
 * �� �� ֵ����
 ***********************************************************************/
void ReleaseJpg2Rgb(PT_PictureData ptData){
	free(ptData->pucRotateData);
	free(ptData->pucZoomData);
	free(ptData->pucRgbData);
}


/**********************************************************************
 * �������ƣ� JpgParInit
 * ���������� ע��"JPGͼ������㷨ģ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int JpgParInit(void)
{
	return RegisterPicParOpr(&g_tJpg2RgbOpr);
}
