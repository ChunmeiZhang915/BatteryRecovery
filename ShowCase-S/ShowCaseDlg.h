
// ShowCaseDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "Network.h"



// CShowCaseDlg �Ի���
class CShowCaseDlg : public CDialogEx
{
// ����
public:
	CShowCaseDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SHOWCASE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	void ReSize(int nID, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnEnChangeTimeEdit();
	afx_msg void OnStnClickedPicStatic();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
	long m_currentNodeId;
	double m_currentNodeEnergy;
	LONGLONG m_curerntNodeWorkingTime;
	CString m_currentWorkType;
	int m_currentNodeRelaxedTime;
	int m_currentNodeTotalWorkTime;
	CString m_currentNodeStatus;
	CSpinButtonCtrl m_timeSpinCtrl;
	LONGLONG m_currentTime;	
	CStatic m_pic;
	CRect m_rect;//��ʾ��ǰ�Ի���Ĵ�С
	bool m_flag;
	bool m_po;
	Network* nt;
	
	CString m_allNeighbors;
	afx_msg void OnStnDblclickPicStatic();
	afx_msg void OnBnClickedRunbtn();
};
