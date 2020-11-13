#pragma once


// WarnDlg 对话框

class WarnDlg : public CDialogEx
{
	DECLARE_DYNAMIC(WarnDlg)

public:
	WarnDlg(int type, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~WarnDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WARNDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	void drawObj(CDC *dc, int type);
	int nType = 0;
	CEdit m_TimeEdit;
	virtual BOOL OnInitDialog();
};
