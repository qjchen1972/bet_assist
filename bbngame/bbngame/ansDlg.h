#pragma once

#include<vector>
// CAnsDlg 对话框
class GameShowDlg;
class CAnsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAnsDlg)

public:
	CAnsDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAnsDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANSDLG };
#endif
	
	int left = 0;
	int top = 20;
	int one_height = 165;
	int one_width = 235;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	//void resetGame(int index);

	DECLARE_MESSAGE_MAP()
public:

	std::vector<GameShowDlg*> gameShowSet;

	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	void draw_road(int index, int init = 1);
	void draw_status(int index);
	void draw_profit(int index);
	void draw_bet(int index);
	void setCheck(int index, int type);

	CButton checkAllBtn;
	CButton inverseBtn;

    afx_msg void checkAll();
	afx_msg void inverse();

};
