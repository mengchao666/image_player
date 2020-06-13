#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>


#include <formats_manager.h>

#pragma pack(push) /* ����ǰpack����ѹջ���� */
#pragma pack(1)    /* �����ڽṹ�嶨��֮ǰʹ��,����Ϊ���ýṹ���и���Ա��1�ֽڶ��� */
typedef struct tagBITMAPFILEHEADER { /* bmfh */
	unsigned short bfType; 
	unsigned long  bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned long  bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER { /* bmih */
	unsigned long  biSize;
	unsigned long  biWidth;
	unsigned long  biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned long  biCompression;
	unsigned long  biSizeImage;
	unsigned long  biXPelsPerMeter;
	unsigned long  biYPelsPerMeter;
	unsigned long  biClrUsed;
	unsigned long  biClrImportant;
} BITMAPINFOHEADER;

#pragma pack(pop) /* �ָ���ǰ��pack���� */


static int DecodeBmp2Rgb(const char *strFileName, PT_PictureData ptData) ;
static void ReleaseBmp2Rgb(PT_PictureData ptPicData);
static T_Picture2RGB g_tBmp2RgbOpr = {
	.name = "bmp2rgb",
	.PictureParsing = DecodeBmp2Rgb,
	.PictureRelease = ReleaseBmp2Rgb,
};


/**********************************************************************
 * �������ƣ� IsBmp
 * ���������� �жϸ��ļ��Ƿ�ΪBMP�ļ�
 * ��������� ptFileMap - �ں��ļ���Ϣ
 * ��������� ��
 * �� �� ֵ�� 0 - ��BMP��ʽ, -1 -����BMP��ʽ
 ***********************************************************************/
int IsBmp(FILE **ppFp, const char *strFileName) 
{
	char strCheckHeader[2]; 
	*ppFp= fopen(strFileName, "rb+");
	if (*ppFp== NULL) {
		return -1;
	}
	if (fread(strCheckHeader, 1, 2, *ppFp) != 2) 
		return -1;
    
	if (strCheckHeader[0] != 0x42 || strCheckHeader[1] != 0x4d)
		return -1;
	else
		return 0;
}



/**********************************************************************
 * �������ƣ� MapFile
 * ���������� ʹ��mmap����ӳ��һ���ļ����ڴ�,�Ժ�Ϳ���ֱ��ͨ���ڴ��������ļ�
 * ��������� PT_PictureData ptData �ں�ͼ������
 * ��������� ptData->iFileSize     : �ļ���С
 *                        ptData->pucFileData : ӳ���ڴ���׵�ַ
 * �� �� ֵ�� 0      - �ɹ�����ֵ - ʧ��
 ***********************************************************************/
int MapFile(PT_PictureData ptData)
{
	int iFd;
	struct stat tStat;
	
	/* ���ļ� */
    	iFd = fileno(ptData->ptFp);
	fstat(iFd, &tStat);
	ptData->iFileSize= tStat.st_size;
	ptData->pucFileData= (unsigned char *)mmap(NULL , tStat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, iFd, 0);
	if (ptData->pucFileData == (unsigned char *)-1)
	{
		printf("mmap error!\n");
		return -1;
	}
	return 0;
}

/**********************************************************************
 * �������ƣ� DecodeBmp2Rgb
 * ������������BMP�ļ�ת��Ϊrgb��ʽ
 * ��������� strFileName - �ļ���
 * 					 ptData - �ں�ͼ����Ϣ
 * �� �� ֵ�� 0      - �ɹ�����ֵ - ʧ��
 *                         -1     - �ļ�����BMP��ʽ
 *                         -2     - ��֧�ֵ�bpp
 *                         -3     - ͼ�񻺴�������ʧ��
 ***********************************************************************/
static int DecodeBmp2Rgb(const char *strFileName, PT_PictureData ptData) {
	int x,y;
	int iPos = 0;
	int iLineWidthAlign;
	BITMAPFILEHEADER *ptBITMAPFILEHEADER;
	BITMAPINFOHEADER *ptBITMAPINFOHEADER;
	unsigned char *aFileHead;
	unsigned char *pucSrc;
	unsigned char *pucDest;
	int iLineBytes;

	/* �жϸ��ļ��Ƿ�ΪBMP��ʽ */
	if (IsBmp(&ptData->ptFp, strFileName)) 
		return -1;

	/* ��BMP�ļ�ӳ�䵽�ڴ�ռ� */	
	MapFile(ptData);


	aFileHead = ptData->pucFileData;

	ptBITMAPFILEHEADER = (BITMAPFILEHEADER *)aFileHead;
	ptBITMAPINFOHEADER = (BITMAPINFOHEADER *)(aFileHead + sizeof(BITMAPFILEHEADER));
	/* ��ȡ��Ҫ��ͼ����Ϣ */
	ptData->iWidth = ptBITMAPINFOHEADER->biWidth;
	ptData->iHeight = ptBITMAPINFOHEADER->biHeight;
	ptData->iBpp = ptBITMAPINFOHEADER->biBitCount;
    	iLineBytes    = ptData->iWidth*ptData->iBpp/8;//һ�����ݵ��ֽ���
	ptData->iBmpDataSize= ptData->iHeight * iLineBytes;//����BMPͼ����ֽ���
	/*��ʱֻ֧��24bpp��ʽ*/
	if (ptData->iBpp != 24)
	{
		printf("iBMPBpp = %d\n", ptData->iBpp);
		printf("sizeof(BITMAPFILEHEADER) = %d\n", sizeof(BITMAPFILEHEADER));
		return -2;
	}

	/* ����ռ� */
	ptData->pucBmpData = malloc(ptData->iBmpDataSize);
	ptData->pucRgbData = malloc(ptData->iBmpDataSize);
	
	if (NULL == ptData->pucBmpData||NULL == ptData->pucRgbData)
		return -2;

	/* ��bmp�ļ��ж�ȡͼ����Ϣ��24bpp��BMPͼ��ΪBGR��ʽ */
	pucDest = ptData->pucBmpData;
	iLineWidthAlign = (iLineBytes + 3) & ~0x3;   /* ��4ȡ�� */
	pucSrc = aFileHead + ptBITMAPFILEHEADER->bfOffBits;

	pucSrc = pucSrc + (ptData->iHeight - 1) * iLineWidthAlign;

	/* ����bmp�ļ��е�Դ���ݣ��������½�Ϊԭ���������ģ���˿�������ʱ��Ҫת������ */
	for (y = 0; y < ptData->iHeight; y++)
	{		
		memcpy(pucDest, pucSrc, ptData->iWidth*3);
		pucSrc  -= iLineWidthAlign;
		pucDest += iLineBytes;
	}

	
	/* ���õ���BGR����ת��ΪRGB���� */
	for (y = 0; y < ptData->iHeight; y++){		
		for(x = 0;x<ptData->iWidth*3;x+=3){
			ptData->pucRgbData[iPos++] = ptData->pucBmpData[y*ptData->iWidth*3+x+2];
			ptData->pucRgbData[iPos++] = ptData->pucBmpData[y*ptData->iWidth*3+x+1];
			ptData->pucRgbData[iPos++] = ptData->pucBmpData[y*ptData->iWidth*3+x+0];
		}
	}
	
	return 0;	

}



/**********************************************************************
 * �������ƣ� ReleaseBmp2Rgb
 * �����������ͷŽ�����ʹ��BMPʱ�Ŀռ�
 * ���������ptData - �ں�ͼ����Ϣ
 * �� �� ֵ�� ��
 ***********************************************************************/
static void ReleaseBmp2Rgb(PT_PictureData ptData){
	free(ptData->pucRotateData);
	free(ptData->pucZoomData);
	free(ptData->pucRgbData);
	free(ptData->pucBmpData);
	if(munmap(ptData->pucFileData,ptData->iFileSize)!=0)
	printf("munmap fault\n");
}

/**********************************************************************
 * �������ƣ� BmpParInit
 * ���������� ע��"BMPͼ������㷨ģ��"
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int BmpParInit(void)
{
	return RegisterPicParOpr(&g_tBmp2RgbOpr);
}

