#pragma once


// CRuleDlg 对话框
class Rule;
class CRuleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRuleDlg)

public:
	CRuleDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CRuleDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RULEDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	int m_curRow = -1;
	void GetRuleFromFile(int index);
	void procInit();
	void resetId();

	CWnd  *m_pParent;
	CListCtrl m_DoorList;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedAddrulebtn();
	afx_msg void OnBnClickedUpdaterulebtn();
	afx_msg void OnBnClickedDelrulebtn();
	afx_msg void OnNMClickDoorlist(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_RuleFileComb;
	Rule *rule;
	afx_msg void OnBnClickedSavenewrulebtn();
	afx_msg void OnBnClickedCloserulebtn();
	CEdit m_SaveRuleNameEdit;
	
	afx_msg void OnCbnSelchangeRulefilecombo();
	
	afx_msg void OnBnClickedNewrulebtn();
	CButton m_DelRuleBtn;
	CButton m_UpdateRuleBtn;
	CEdit m_RuleEdit;
	CEdit m_BetEdit;
	CButton m_AddRuleBtn;
	afx_msg void OnPaint();
	CButton m_DelOldRuleBtn;
	afx_msg void OnBnClickedDeloldrulebtn();
	CEdit m_TipEdit;
};
