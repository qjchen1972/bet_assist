#pragma once


// SidLoginDlg 对话框

class SidLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SidLoginDlg)

public:
	SidLoginDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~SidLoginDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIDLOGINDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSidlogin();
	CEdit m_SidEdit;
	virtual BOOL OnInitDialog();
};
