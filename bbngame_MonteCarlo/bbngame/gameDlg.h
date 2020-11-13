#pragma once

#include<string>
#include<thread>
#include <mutex>
#include<vector>
#include"userdata.h"

class KLineImage {

public:

	void setRect(CRect rect) {
		imageRect.left = rect.left + 2;
		imageRect.right = rect.right - 2;
		imageRect.top = rect.top + 2;
		imageRect.bottom = rect.bottom - 2;
		imageSize = imageRect.Width() / div;
		ctrlRect = rect;
	}

	void getKlineInfo(std::vector<Kline_t> &data, int start, int end, float &max, float &min) {

		max = data[start].high;
		min = data[start].low;
		for (int i = start + 1; i <= end; i++) {
			if (max < data[i].high) max = data[i].high;
			if (min > data[i].low) min = data[i].low;
		}
	}

	void setImageInfo(float max, float min) {
		if (max < baseMax) maxProfit = baseMax;
		else maxProfit = max;
		if (min > baseMin) minProfit = baseMin;
		else minProfit = min;
	}


	void drawImage(std::vector<Kline_t> &data, int start, int end, CDC* dc) {

		CDC MemDC;
		CBitmap MemBitmap;
		MemDC.CreateCompatibleDC(NULL);
		MemBitmap.CreateCompatibleBitmap(dc, ctrlRect.right, ctrlRect.bottom);
		CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);
		MemDC.FillSolidRect(0, 0, ctrlRect.right, ctrlRect.bottom, RGB(255, 255, 255));
		if (data.size() > 0) {
			if (gConfig.profitStatNum == 1)
				drawSingle(data, start, end, &MemDC);
			else
				drawKline(data, start, end, &MemDC);
			drawAvg(data, start, end, &MemDC, gConfig.avgNum, RGB(0, 0, 255));
			drawAvg(data, start, end, &MemDC, 2 * gConfig.avgNum, RGB(255, 0, 0));
		}
		dc->BitBlt(0, 0, ctrlRect.right, ctrlRect.bottom, &MemDC, 0, 0, SRCCOPY);
		MemBitmap.DeleteObject();
		MemDC.DeleteDC();	
	}

	int imageSize;

private:

	float baseMax = 100;
	float baseMin = -100;
	float maxProfit;
	float minProfit;

	int div = 10;
	int length = 7;
	int mid = 3;

	CRect imageRect;
	CRect ctrlRect;

	void dataToAxis(int index, float profit, int &x, int &y) {

		x = imageRect.left + index * div;
		y = int(imageRect.bottom - (profit - minProfit) * imageRect.Height() / (maxProfit - minProfit));
	}

	void drawSingle(std::vector<Kline_t> &data, int start, int end, CDC* dc) {

		CPen pen(PS_SOLID, 2, RGB(0, 0, 0));
		CPen *oldpen = dc->SelectObject(&pen);
		int prev_x, prev_y, now_x, now_y;

		for (int i = start; i <= end; i++) {
			if (i == start)	dataToAxis(i - start, data[i].end, prev_x, prev_y);
			else {
				dataToAxis(i - start, data[i].end, now_x, now_y);
				dc->MoveTo(prev_x + mid, prev_y);
				dc->LineTo(now_x + mid, now_y);
				prev_x = now_x;
				prev_y = now_y;
			}
		}
		dc->SelectObject(oldpen);
	}

	void drawKline(std::vector<Kline_t> &data, int start, int end, CDC* dc) {

		int start_x, start_y, end_x, end_y;
		CRect rect;
		int high_y, low_y;

		for (int i = start; i <= end; i++) {
			dataToAxis(i - start, data[i].start, start_x, start_y);
			dataToAxis(i - start, data[i].end, end_x, end_y);

			if (abs(start_y - end_y) <= 1) {
				dc->MoveTo(start_x, start_y);
				dc->LineTo(start_x + length, start_y);
				high_y = low_y = start_y;
			}
			else if (start_y >= end_y) {
				rect.left = end_x;
				rect.top = end_y;
				rect.right = rect.left + length;
				rect.bottom = start_y;
				dc->FillSolidRect(&rect, RGB(255, 0, 0));
				high_y = end_y;
				low_y = start_y;
			}
			else {
				rect.left = start_x;
				rect.top = start_y;
				rect.right = rect.left + length;
				rect.bottom = end_y;
				dc->FillSolidRect(&rect, RGB(0, 255, 0));
				high_y = start_y;
				low_y = end_y;
			}
			int x, y;
			dataToAxis(i - start, data[i].high, x, y);
			if (y < high_y) {
				dc->MoveTo(x + mid, high_y);
				dc->LineTo(x + mid, y);
			}
			dataToAxis(i - start, data[i].low, x, y);
			if (y > low_y) {
				dc->MoveTo(x + mid, y);
				dc->LineTo(x + mid, low_y);
			}
		}
	}

	float getAvgLine(std::vector<Kline_t> &data, int index, int avgnum) {
		int pos = index + 1 - avgnum;
		if (pos < 0) pos = 0;
		float total = 0;
		int num = 0;
		for (int i = pos; i <= index; i++) {
			total += data[i].end;
			num++;
		}
		return total / num;
	}

	void drawAvg(std::vector<Kline_t> &data, int start, int end, CDC* dc, int avgNum, COLORREF penColor) {

		//CPen pen(PS_SOLID, 1, RGB(0, 0, 255));
		CPen pen(PS_SOLID, 1, penColor);
		CPen *oldpen = dc->SelectObject(&pen);

		int prev_x, prev_y, now_x, now_y;
		for (int i = start; i <= end; i++) {
			if (i == start) dataToAxis(i - start, getAvgLine(data, i, avgNum), prev_x, prev_y);
			else {
				dataToAxis(i - start, getAvgLine(data, i, avgNum), now_x, now_y);
				dc->MoveTo(prev_x + mid, prev_y);
				dc->LineTo(now_x + mid, now_y);
				prev_x = now_x;
				prev_y = now_y;
			}
		}
		dc->SelectObject(oldpen);
	}
};

struct pos_t {
	int row = 0;
	int col = 0;
	bool isEnd = false;
};

// CBBNGameDlg 对话框
constexpr auto WM_UPDATE_MESSAGE = WM_USER + 200;
constexpr auto TIMER_ID = 0;

class CAnsDlg;

class CGameDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGameDlg)

public:
	CGameDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CGameDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GAME_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	bool isNotify = false;

	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnUpdateMessage(WPARAM wParam, LPARAM lParam);
	void UpdateUserInfo();
	virtual BOOL OnInitDialog();

	KLineImage *klineImage = nullptr;

	CAnsDlg  *m_ans = nullptr;
	CTabCtrl m_AnsTab;
	afx_msg void OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedSidloginbtn();


	CEdit m_UserCreditEdit;
	CEdit m_TotalBetEdit;
	CEdit m_TotalProfitEdit;
	CEdit m_BetProfitEdit;

	CComboBox m_SetInRuleCombox;
	CComboBox m_SetBetRuleCombox;

	CButton m_UseNewGameCb;
	CButton m_OmitMidCb;

	CStatic m_KlineCtrl;

	CEdit m_CurMaxProfitEdit;
	CEdit m_CurMinProfitEdit;

	CEdit m_TargetProfitEdit;
	CEdit m_TargetLoseEdit;

	CEdit m_BetRateEdit;
	CEdit m_BackRateEdit;
	CComboBox m_GameModeCombox;


	CButton m_StartGameBtn;
	CButton m_StopGameBtn;
	CButton m_SidLoginBtn;

	CButton m_SetupRuleBtn;

	afx_msg void OnBnClickedSetuprulebtn();
	afx_msg void OnBnClickedStartgamebtn();
	afx_msg void OnBnClickedStopgamebtn();
	afx_msg void OnBnClickedUsenewgamecb();
	afx_msg void OnCbnDropdownSetinrulecombox();
	afx_msg void OnCbnDropdownSetbetrulecombox();
	
	CEdit m_WinStopOneValueEdit;
	CButton m_WinStopOneCb;

	CButton m_LoseStopOneCb;
	CEdit m_LoseStopOneValueEdit;

	CButton m_AvgUseFakeCb;

	afx_msg void OnBnClickedResetbtn();

	CButton m_AllTimeCheck;
	afx_msg void OnBnClickedAlltimecheck();
	CButton m_ResetBtn;
	//afx_msg void OnCbnSelchangeGamemodecombox();
	void enableCtrl();
	void disableCtrl(); 

	CScrollBar m_ScrollBar;
	bool isLive = true;

	int scrolSize = 0;
	int scrolPos = 0;
	std::vector<Kline_t> allData;

	float allMaxProfit = 0;
	float allMinProfit = 0;

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	
	CButton m_LoseStopAllCb;
	CEdit m_LoseStartTimeEdit;
	CEdit m_LoseStopAllValueEdit;

	CButton m_WinStopAllCb;
	CEdit m_WinStopAllValueEdit;
	CEdit m_WinStartTimeEdit;
	void needStartProc(CDC *dc);
	void klineProc();

	afx_msg void OnBnClickedDetailprofitbtn();

	bool isOver(std::vector<pos_t> &allPos);
	void tendProc();
	CButton m_RuleTendBtn;
	afx_msg void OnBnClickedRuletendbtn();
	int queryTime = 0;
	CButton m_AvgRateCb;
};
