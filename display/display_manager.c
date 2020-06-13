
#include <display_manager.h>
#include <string.h>

static PT_DispOpr g_ptDispOprHead;
static PT_DispOpr g_ptDefaultDispOpr;

/**********************************************************************
 * �������ƣ� RegisterDispOpr
 * ���������� ע��"��ʾģ��", ������֧�ֵ���ʾ�豸�Ĳ���������������
 * ��������� ptDispOpr - һ���ṹ��,�ں���ʾ�豸�Ĳ�������
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int RegisterDispOpr(PT_DispOpr ptDispOpr)
{
	PT_DispOpr ptTmp;

	if (!g_ptDispOprHead)
	{
		g_ptDispOprHead   = ptDispOpr;
		ptDispOpr->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptDispOprHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = ptDispOpr;
		ptDispOpr->ptNext = NULL;
	}

	return 0;
}

/**********************************************************************
 * �������ƣ� GetDispOpr
 * ���������� ��������ȡ��ָ����"��ʾģ��"
 * ��������� pcName - ����
 * ��������� ��
 * �� �� ֵ�� NULL   - ʧ��,û��ָ����ģ��, 
 *            ��NULL - ��ʾģ���PT_DispOpr�ṹ��ָ��
 ***********************************************************************/
PT_DispOpr GetDispOpr(char *pcName)
{
	PT_DispOpr ptTmp = g_ptDispOprHead;
	
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
 * �������ƣ� SelectAndInitDefaultDispDev
 * ���������� ��������ȡ��ָ����"��ʾģ��", �������ĳ�ʼ������, ��������
 * ��������� name - ����
 * ��������� ��
 * �� �� ֵ�� ��
 ***********************************************************************/
void SelectAndInitDefaultDispDev(char *name)
{
	g_ptDefaultDispOpr = GetDispOpr(name);
	if (g_ptDefaultDispOpr)
	{
		g_ptDefaultDispOpr->DeviceInit();
		g_ptDefaultDispOpr->CleanScreen(0);
	}
}

/**********************************************************************
 * �������ƣ� GetDefaultDispDev
 * ���������� ����������SelectAndInitDefaultDispDevѡ������ʾģ��,
 *            ���������ظ���ʾģ��
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��ʾģ���PT_DispOpr�ṹ��ָ��
 ***********************************************************************/
PT_DispOpr GetDefaultDispDev(void)
{
	return g_ptDefaultDispOpr;
}

/**********************************************************************
 * �������ƣ� DisplayInit
 * ���������� ע����ʾ�豸
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0 - �ɹ�, ����ֵ - ʧ��
 ***********************************************************************/
int DisplayInit(void)
{
	int iError;
	
	iError = FBInit();

	return iError;
}





