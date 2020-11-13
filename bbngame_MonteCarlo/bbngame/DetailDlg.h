#pragma once
#include<string>

// DetailDlg 对话框

class DetailDlg : public CDialogEx
{
	DECLARE_DYNAMIC(DetailDlg)

public:
	DetailDlg(int index, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DetailDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DETAILDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int nIndex;	
	void showDetail();
	CListCtrl m_DetailList;
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
};
