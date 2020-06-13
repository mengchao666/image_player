#include <stdlib.h>
#include <string.h>
#include <formats_manager.h>
#include <operate_manager.h>

/**********************************************************************
 * �������ƣ� PicZoom
 * ���������� ����ȡ����ֵ��������ͼƬ
 *            ע��ú���������ڴ���������ź��ͼƬ,�����Ҫ��free�����ͷŵ�
 *            "����ȡ����ֵ"��ԭ����ο�����"lantianyu520"������"ͼ�������㷨"
 * ��������� ptPicData - �ں�����ǰ���ͼ������
 *            fSize    - ���ű���
 * ��������� ptPicData->pucZoomData,�ں����ź������
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int PicZoom(PT_PictureData ptPicData,float fSize)
{
    ptPicData->iZoomWidth = ptPicData->iWidth * fSize;
	ptPicData->iZoomHeight= ptPicData->iHeight* fSize;
    unsigned long* pdwSrcXTable;
	unsigned long x;
	unsigned long y;
	unsigned long dwSrcY;
	unsigned char *pucDest;
	unsigned char *pucSrc;
	unsigned long dwPixelBytes = ptPicData->iBpp/8;
    ptPicData->pucZoomData= malloc(sizeof(unsigned char) * ptPicData->iZoomWidth*ptPicData->iZoomHeight*ptPicData->iBpp/8);
    pdwSrcXTable = malloc(sizeof(unsigned long) * ptPicData->iZoomWidth);
    if (NULL == pdwSrcXTable){
        printf("malloc error!\n");
        return -1;
    }

	/* �⼸��forѭ���ı�����Sx = Dx * Sw/Dw��Sy = Dy * Sh/Dh*/
    for (x = 0; x < ptPicData->iZoomWidth; x++){//���ɱ� pdwSrcXTable
    	/* ��һ��forѭ����Ӧx���������
	 * pdwSrcXTable[x] ��ӦSx,
	 * x ��ӦDx,
	 * ptPicData->iWidth ��ӦSw
	 * ptPicData->iZoomWidth ��Ӧ Dw*/	
        pdwSrcXTable[x]=(x*ptPicData->iWidth/ptPicData->iZoomWidth);
    }

    for (y = 0; y < ptPicData->iZoomHeight; y++){
	/* ��2��ѭ����Ӧy���������
	 * dwSrcY ��ӦSy,
	 * y ��ӦDy,
	 * ptPicData->iHeight ��ӦSh
	 * ptPicData->iZoomHeight ��Ӧ Dh*/	
        dwSrcY = (y * ptPicData->iHeight / ptPicData->iZoomHeight);
	/* ������Щ����ø����ص��RGB���ݴ�ŵĵ�ַ */
		pucDest = ptPicData->pucZoomData + y*ptPicData->iZoomWidth*3;
		pucSrc  = ptPicData->pucRgbData + dwSrcY*ptPicData->iWidth*3;

	/* ��󿽱����� */		
        for (x = 0; x <ptPicData->iZoomWidth; x++){
			 memcpy(pucDest+x*dwPixelBytes, pucSrc+pdwSrcXTable[x]*dwPixelBytes, dwPixelBytes);
        }
    }

    free(pdwSrcXTable);
	return 0;
}