#pragma once


// CSetupRuleDlg 对话框
class CRuleDlg;
class CMoneyRuleDlg;


class CSetupRuleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetupRuleDlg)

public:
	CSetupRuleDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSetupRuleDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RULESETDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CRuleDlg  *m_BetRuleDlg;
	CMoneyRuleDlg *m_MoneyRuleDlg;
	CTabCtrl m_RuleSetTab;
	virtual BOOL OnInitDialog();
	afx_msg void OnTcnSelchangeRulesettab(NMHDR *pNMHDR, LRESULT *pResult);
};
