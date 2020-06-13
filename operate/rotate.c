#include <stdio.h>
#include <string.h>
#include <math.h>
#include <formats_manager.h>
#include <operate_manager.h>
 #define PI 3.1415926535
//�Ƕȵ�����ת��
#define RADIAN(angle) ((angle)*PI/180.0)





typedef struct ConcernCoor {
	int iLTx;// left top x
	int iLTy;//left top y
	int iLBx;//left bottom x
	int iLBy;//left bottom y
	int iRTx;//right top x
	int iRTy;//right top y
	int iRBx;// right bottom x
	int iRBy;// right bottom y
}T_ConcernCoor, *PT_ConcernCoor;


/**********************************************************************
 * �������ƣ� max
 * �����������Ƚ��������������ؽϴ�ֵ
 * ���������x��y��Ϊint��
 * ��������� ��
 * �� �� ֵ�� x��y�еĽϴ�ֵ
 ***********************************************************************/
static int max(int x,int y){
	return x>y?x:y;
}
/**********************************************************************
 * �������ƣ� PicRotate
 * ���������� ��תͼƬ
 *            ע��ú���������ڴ���������ź��ͼƬ,�����Ҫ��free�����ͷŵ�
 *           	�ο�����"��Ҷ��˼ά"������"ͼ����ת�㷨��ʵ��"
 * ��������� ptPicData - �ں�ͼƬ����������
 *            fAngle    - ��ת�Ƕȣ�0<=angle<=360
 * ��������� ptPicData->pucRotateData,�ں���ת���rgb����
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int PicRotate(PT_PictureData ptPicData,float fAngle)
{
	int i ,j;
    T_ConcernCoor tConCor,tRonCor;
    //ԭͼ��ÿһ��ȥ��ƫ�������ֽ���
    //int iSrcLineSize = bitCount * srcW / 8;
    int iSrcLineSize = ptPicData->iBpp* ptPicData->iZoomWidth / 8;
    int iDesLineSize;
    int iX;//��ת���x����
    int iY;	//��ת���y����

       /* ������ϵA�µ�����ת��Ϊ����ϵB�µ�����,
        * ���ڼ�����ת���ͼ��Ŀ�͸� 
        * tConCor���ڴ������ϵB����תǰ������
        * tRonCor���ڴ������ϵB����ת�������*/
       tConCor.iLTx = -ptPicData->iZoomWidth/2; tConCor.iLTy = ptPicData->iZoomHeight/2;
	tConCor.iRTx = ptPicData->iZoomWidth/2; tConCor.iRTy = ptPicData->iZoomHeight/2;
	tConCor.iLBx = -ptPicData->iZoomWidth/2;tConCor.iLBy = -ptPicData->iZoomHeight/2;
	tConCor.iRBx = ptPicData->iZoomWidth/2;tConCor.iRBy = -ptPicData->iZoomHeight/2;


    /* ��������ϵB����ת������� */
    double sina = sin(RADIAN(fAngle));
    double cosa = cos(RADIAN(fAngle));
    tRonCor.iLTx =tConCor.iLTx * cosa + tConCor.iLTy * sina;
    tRonCor.iLTy = -tConCor.iLTx * sina + tConCor.iLTy * cosa;
    tRonCor.iRTx =tConCor.iRTx * cosa + tConCor.iRTy * sina;
	tRonCor.iRTy = -tConCor.iRTx * sina + tConCor.iRTy * cosa;
    tRonCor.iLBx = tConCor.iLBx * cosa + tConCor.iLBy * sina;
	tRonCor.iLBy = -tConCor.iLBx * sina + tConCor.iLBy * cosa;
    tRonCor.iRBx = tConCor.iRBx * cosa + tConCor.iRBy * sina;
	tRonCor.iRBy = -tConCor.iRBx * sina + tConCor.iRBy * cosa;

	
    /* ������ת��ͼ���͸� */
    ptPicData->iRotateWidth = max(abs(tRonCor.iRBx - tRonCor.iLTx),abs(tRonCor.iRTx - tRonCor.iLBx));
    ptPicData->iRotateHeight = max(abs(tRonCor.iRBy - tRonCor.iLTy),abs(tRonCor.iRTy - tRonCor.iLBy));

	/* ������ϢҪ��֤3�ֽڶ��룬���������п��ܳ���*/
	iDesLineSize = ((ptPicData->iRotateWidth* ptPicData->iBpp+ 23) / 24) * 3 ;
	/* ������ת��Ŀռ䣬ע������Ҫ����ת��Ŀ�͸� */
	ptPicData->pucRotateData = malloc(iDesLineSize * ptPicData->iRotateHeight);
	if(NULL == ptPicData->pucRotateData){
		printf("malloc error\n");
		return -1;
	}

    /* ͨ����ͼ������꣬�����Ӧ��ԭͼ�������*
      * i,j������Ƕ�Ӧ������ϵB�µ�x1,y1*/
    for (i = 0; i < ptPicData->iRotateHeight; i++){        
        for (j = 0; j < ptPicData->iRotateWidth; j++){
            /* ����ϵB�µ�x,y1���꣬����������ת���õ�iX,iY,������ֵ��Ӧx0,y0 */
            iX = (j - ptPicData->iRotateWidth / 2)*cos(RADIAN(360 - fAngle)) + (-i + ptPicData->iRotateHeight / 2)*sin(RADIAN(360 - fAngle));
            iY = -(j - ptPicData->iRotateWidth / 2)*sin(RADIAN(360 - fAngle)) + (-i + ptPicData->iRotateHeight / 2)*cos(RADIAN(360 - fAngle));
            /*���������겻��ԭͼ���ڣ��򲻸�ֵ*/
            if (iX > ptPicData->iZoomWidth / 2 || iX < -ptPicData->iZoomWidth / 2 || iY > ptPicData->iZoomHeight / 2 || iY < -ptPicData->iZoomHeight / 2){
                continue;
            }
            /* �ٽ�����ϵB�µ�x0,y0���꣬ת��Ϊ����ϵA�µ����� */
            int iXN = iX + ptPicData->iZoomWidth / 2; 
	     int iYN = abs(iY - ptPicData->iZoomHeight  / 2);
            /* ֵ����*/
            memcpy(&ptPicData->pucRotateData[i * iDesLineSize + j * 3],&ptPicData->pucZoomData[iYN * iSrcLineSize + iXN * 3],3);  
        }
    }
  return 0;
}

