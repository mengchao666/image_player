#include<formats_manager.h>
#include <string.h>

static PT_Picture2RGB g_ptPicParHead;

/**********************************************************************
 * �������ƣ� RegisterPicParOpr
 * ���������� ע��"ͼ�����ģ��", ������֧�ֵ�ͼ�����ģ��Ĳ���������������
 * ��������� ptPicParOpr - һ���ṹ��,�ں���Ӧ��ͼ������㷨
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int RegisterPicParOpr(PT_Picture2RGB ptPicParOpr){
	PT_Picture2RGB ptTmp;

	if (!g_ptPicParHead)
	{
		g_ptPicParHead   = ptPicParOpr;
		ptPicParOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptPicParHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptPicParOpr;
		ptPicParOpr->ptNext = NULL;
	}

	return 0;
}

/**********************************************************************
 * �������ƣ� PicParseInit
 * ���������� ע��ͼ������㷨
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int PicParseInit(void)
{
	int iError;
	
	iError = BmpParInit()||PngParInit()||JpgParInit();

	return iError;
}

/**********************************************************************
 * �������ƣ� GetFmtOpr
 * ���������� ����ͼ�����ģ��
 * ��������� pcName - ����
 * ��������� ��
 * �� �� ֵ�� NULL   - ʧ��,û��ָ����ģ��, 
 *            ��NULL - ͼ�����ģ���PT_PictureParsing�ṹ��ָ��
 ***********************************************************************/
PT_Picture2RGB GetFmtOpr(char *pcName)
{
	PT_Picture2RGB ptTmp = g_ptPicParHead;
	
	while (ptTmp)
	{
		if (strcmp(ptTmp->name, pcName) == 0)
		{
			return ptTmp;
		}
		ptTmp = ptTmp->ptNext;
	}
	return NULL;
}

/**********************************************************************
 * �������ƣ� RgbaToRgb
 * �����������ж��Ƿ�ΪPNG�ļ�
 * ��������� ptData - �ں��ļ���Ϣ
 * ���������PT_PictureData->pucRgbData - �ں�rgb����
 * �� �� ֵ��0 - �ɹ� ����-ʧ��
 ***********************************************************************/
int RgbaToRgb(PT_PictureData ptData){
	int i ,j;
	int iPos = 0;
	ptData->iRgbSize= ptData->iHeight*ptData->iWidth * 3; 
	ptData->pucRgbData = (unsigned char*)malloc(ptData->iRgbSize);
	if (NULL == ptData->pucRgbData) {
	printf("malloc rgb faile ...\n");
	png_destroy_read_struct(&ptData->ptPngStrPoint, &ptData->ptPngInfoPoint, 0);
	fclose(ptData->ptFp);
	return -1;
	}

	for(i = 0;i<ptData->iHeight;i++)
	{
		for(j = 0;j<ptData->iWidth*4;j+=4){
		ptData->pucRgbData[iPos++] = ptData->pucRawData[i*ptData->iWidth*4+j+3];
		ptData->pucRgbData[iPos++] = ptData->pucRawData[i*ptData->iWidth*4+j+2] ;
		ptData->pucRgbData[iPos++] = ptData->pucRawData[i*ptData->iWidth*4+j+1];
		}
	}
	ptData->iBpp = 24;
	free(ptData->pucRawData);
	return 0;
}

