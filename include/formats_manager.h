#ifndef  __FORMATS_MANAGER_H__ 
#define  __FORMATS_MANAGER_H__ 
#include <png.h>
#include "jpeglib.h"
#include <stdlib.h>



typedef struct PictureData {
	FILE *ptFp;//�ļ����
	int iBpp;
	int iRgbSize;
	
	int iWidth;
	int iHeight;
	unsigned char *pucRgbData;

	int iZoomWidth;
	int iZoomHeight;
	unsigned char *pucZoomData;

	int iRotateWidth;
	int iRotateHeight;
	unsigned char *pucRotateData;
	/* use for BMP */
	unsigned char *pucFileData;//����bmp�ļ�������	
	unsigned char *pucBmpData;//bmpͼ�������
	int iFileSize;//BMP�ļ���С
	int iBmpDataSize;//BMPԭʼ����rgba��С
	
	/* use for PNG */
	png_structp ptPngStrPoint;//png�ṹ��ָ��
	png_infop ptPngInfoPoint;//png��Ϣ�ṹ��ָ��
	unsigned char *pucRawData;//ԭʼrgba����
	int iRawSize;//ԭʼrgba���ݴ�С
	int iChannels;//ԭʼ����ͨ����

		/* use for JPG */
	struct jpeg_decompress_struct tInfo;
	struct jpeg_error_mgr tJerr;
}T_PictureData, *PT_PictureData;



typedef struct Picture2RGB {
	char *name;
	int (*PictureParsing)(const char *strFileName, PT_PictureData ptData);
	void (*PictureRelease)(PT_PictureData ptData);
	struct Picture2RGB *ptNext;        /* ���� */
}T_Picture2RGB, *PT_Picture2RGB;

/**********************************************************************
 * �������ƣ� RegisterPicParOpr
 * ���������� ע��"ͼ�����ģ��", ������֧�ֵ�ͼ�����ģ��Ĳ���������������
 * ��������� ptPicParOpr - һ���ṹ��,�ں���Ӧ��ͼ������㷨
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int RegisterPicParOpr(PT_Picture2RGB ptPicParOpr);

/**********************************************************************
 * �������ƣ� PicParseInit
 * ���������� ע��ͼ������㷨
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int PicParseInit(void);



/**********************************************************************
 * �������ƣ� GetFmtOpr
 * ���������� ����ͼ�����ģ��
 * ��������� pcName - ����
 * ��������� ��
 * �� �� ֵ�� NULL   - ʧ��,û��ָ����ģ��, 
 *            ��NULL - ͼ�����ģ���PT_PictureParsing�ṹ��ָ��
 ***********************************************************************/
PT_Picture2RGB GetFmtOpr(char *pcName);


/**********************************************************************
 * �������ƣ� BmpParInit
 * ���������� ע��"BMPͼ������㷨ģ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int BmpParInit(void);

/**********************************************************************
 * �������ƣ� PngParInit
 * ���������� ע��"PNGͼ������㷨ģ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int PngParInit(void);

/**********************************************************************
 * �������ƣ� JpgParInit
 * ���������� ע��"JPGͼ������㷨ģ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int JpgParInit(void);

/**********************************************************************
 * �������ƣ� RgbaToRgb
 * �����������ж��Ƿ�ΪPNG�ļ�
 * ��������� ptData - �ں��ļ���Ϣ
 * ���������PT_PictureData->pucRgbData - �ں�rgb����
 * �� �� ֵ��0 - �ɹ� ����-ʧ��
 ***********************************************************************/
int RgbaToRgb(PT_PictureData ptData);


#endif /* __FORMATS_MANAGER_H__ */

