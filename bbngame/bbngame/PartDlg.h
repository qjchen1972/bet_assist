#pragma once


// PartDlg 对话框

class PartDlg : public CDialogEx
{
	DECLARE_DYNAMIC(PartDlg)

public:
	PartDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~PartDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PARTDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CListCtrl m_DetailList;
	afx_msg void OnPaint();
	void showDetail();
};
