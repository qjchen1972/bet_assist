#pragma once


// CMoneyRuleDlg 对话框

class CMoneyRuleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMoneyRuleDlg)

public:
	CMoneyRuleDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMoneyRuleDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MONEYRULEDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_MoneyList;
	virtual BOOL OnInitDialog();
	CEdit m_MoneyEdit;
	CEdit m_WinNumEdit;
	CEdit m_LoseNumEdit;
	CEdit m_MidNumEdit;
	CEdit m_NewBetFileEdit;
	afx_msg void OnBnClickedNewbetrulebtn();
	afx_msg void OnBnClickedAddmoneybtn();
	afx_msg void OnBnClickedUpdatemoneybtn();
	afx_msg void OnBnClickedDelmoneybtn();
	afx_msg void OnBnClickedClosebetrulebtn();
	afx_msg void OnBnClickedSavenewbetrulebtn();


	int m_curRow = -1;
	void GetRuleFromFile(int index);
	void procInit();
	void resetId();

	afx_msg void OnNMClickMoneylist(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_BetRuleCombo;
	CWnd  *m_pParent;
	afx_msg void OnCbnSelchangeBetrulecombo();
	CButton m_AddMoneyBtn;
	CButton m_UpdateMoneyBtn;
	CButton m_DelMoneyBtn;
	afx_msg void OnPaint();

	afx_msg void OnBnClickedDeloldbetrulebtn();
};
