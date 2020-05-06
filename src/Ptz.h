// Ptz.h: interface for the CPtz class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PTZ_H__3CBE7307_BC91_490C_91E9_B08535C6C7F4__INCLUDED_)
#define AFX_PTZ_H__3CBE7307_BC91_490C_91E9_B08535C6C7F4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "dhnetsdk.h"

class CPtz  
{
public:
	CPtz();
	virtual ~CPtz();

public://interface
	
        bool SetPtzParam(LLONG lLoginID, int nChannel);//������̨�����ĵ�½������?���š�
        bool SetLoginID(LLONG lLoginID);
	bool SetChannel(int nChannel);

	bool PtzBasicControl(int nType, int nStep, bool bStop = false);//��̨����,��ͷ����Ȧ�������Ŀ���
	bool PtzMenuControl(int nType);//��̨�˵�����
	bool PtzFastGo(int nX, int nY, int nZoom);//��ά��λ����
	bool PtzExactGo(int param1, int param2, int param3);
	bool PtzReset();
	bool PtzRotate(int nType);//ˮƽ��ת
	bool PtzAuxi(int nType, int nAuxi);//��������
	bool PtzLineScan(int nType);//����ɨ��
	bool PtzLoop(int nType, int nGroup, int nPresetData, bool bBeginLoop = false);//Ѳ��
	bool PtzPreset(int nType, int nPresetData);//Ԥ�Ƶ�
	bool PtzMode(int nType, int nGroup);//��̨Ѳ��
	
protected://data member
        LLONG	                        m_lLoginID;//�豸��½���?
	int				m_nChannel;//����ͨ��

protected://method member
	bool PtzControl(int nType, int nParam1, int nParam2, int nParam3, bool bStop = false);
};

#endif // !defined(AFX_PTZ_H__3CBE7307_BC91_490C_91E9_B08535C6C7F4__INCLUDED_)
