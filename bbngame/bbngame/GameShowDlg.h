#pragma once
#include<vector>
#include"userdata.h"


#define BANKERCOLOR  RGB(255, 51, 51)
#define PLAYERCOLOR  RGB(0, 102, 255)
#define MIDCOLOR     RGB(0, 204, 0)

class RoadImage {

public:
	void setRect(CRect rect) {
		road_Rect = rect;
		dis = rect.Height() / height;
		width = rect.Width() / dis;
		road_Rect.left = (rect.Height() - dis * height) / 2;
		road_Rect.top = (rect.Width() - dis * width) / 2;
		size = height * width;
	}

	void draw_table(CDC* dc) {

		CPen pen(PS_SOLID, 2, RGB(117, 116, 112));
		CPen *oldpen = dc->SelectObject(&pen);

		for (int i = 0; i <= height; i++) {
			dc->MoveTo(road_Rect.left, road_Rect.top + i * dis);
			dc->LineTo(road_Rect.left + width * dis, road_Rect.top + i * dis);
		}
		for (int i = 0; i <= width; i++) {
			dc->MoveTo(road_Rect.left + i * dis, road_Rect.top);
			dc->LineTo(road_Rect.left + i * dis, road_Rect.top + height * dis);
		}
		dc->SelectObject(oldpen);
	}

	void draw_ans(CDC* dc, Round_t ans, int index) {

		int x = index / height;
		int y = index % height;

		COLORREF color;
		if (ans.cardAns == BANKER)
			color = BANKERCOLOR;
		else if (ans.cardAns == PLAYER)
			color = PLAYERCOLOR;
		else
			color = MIDCOLOR;

		CBrush brush;
		brush.CreateSolidBrush(color);
		CBrush *oldbrush = dc->SelectObject(&brush);
		CRect rect(
			road_Rect.left + x * dis + 1,
			road_Rect.top + y * dis + 1,
			road_Rect.left + (x + 1) * dis - 1,
			road_Rect.top + (y + 1) * dis - 1);

		dc->Ellipse(rect);
		dc->SelectObject(oldbrush);


		CPen pen(PS_SOLID, 2, RGB(255, 255, 255));
		CPen *oldpen = dc->SelectObject(&pen);
		COLORREF oldbc = dc->GetBkColor();
		COLORREF oldtc = dc->GetTextColor();
		dc->SetTextColor(RGB(234, 242, 255));
		dc->SetBkColor(color);
		CString str;
		str.Format(_T("%d"), ans.cardNum);
		dc->DrawText(str, rect, DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		dc->SelectObject(oldpen);
		dc->SetTextColor(oldtc);
		dc->SetBkColor(oldbc);
	}

	/*void draw_Road(CDC* dc, std::vector<Round_t> &road_map) {

		draw_table(dc);
		int pos;		
		if (road_map.size() < size)
			pos = road_map.size() - 1;
		else
			pos = size - 1;
		for (int i = pos; i >= 0; i--)
			draw_ans(dc, road_map[i], pos - i);
	}*/

	void drawRoad(CDC* dc, std::vector<Round_t> &road_map) {

		CDC MemDC;
		CBitmap MemBitmap;
		MemDC.CreateCompatibleDC(NULL);
		MemBitmap.CreateCompatibleBitmap(dc, road_Rect.right, road_Rect.bottom);
		CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);
		MemDC.FillSolidRect(0, 0, road_Rect.right, road_Rect.bottom, RGB(255, 255, 255));
		draw_table(&MemDC);
		int pos;
		if (road_map.size() < size )
			pos = road_map.size() - 1;
		else
			pos = size - 1;
		for (int i = pos; i >= 0; i--)
			draw_ans(&MemDC, road_map[i], pos - i);
		dc->BitBlt(0, 0, road_Rect.right, road_Rect.bottom, &MemDC, 0, 0, SRCCOPY);
		MemBitmap.DeleteObject();
		MemDC.DeleteDC();
	}

public:
	int size;
private:
	CRect road_Rect;
	int dis = 17;
	int height = 6;
	int width = 12;

};


// GameShowDlg 对话框
class DetailDlg;
class GameShowDlg : public CDialogEx
{
	DECLARE_DYNAMIC(GameShowDlg)

public:
	GameShowDlg(int index, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~GameShowDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GAMESHOWDLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int nIndex = 0;

	RoadImage *map = nullptr;
	DetailDlg *detailDlg = nullptr;

	CButton m_NameCheck;
	CStatic m_RoadCtrl;
	CButton m_DetailBtn;
	CEdit m_ProfitEdit;
	CEdit m_BetEdit;

	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedNamecheck();
	afx_msg void OnBnClickedDetailbtn();
	void setGameCtrl(int type);

	void setRoad(int init = 1);
	void setStatus();
	void setProfit();
	void setBet();

	CStatic m_BetColorCtrl;
	CStatic m_StatusStatic;
	CStatic m_BankerCtrl;
	CStatic m_PlayerCtrl;
	CStatic m_MidCtrl;

	int bankerNum = 0;
	int playerNum = 0;
	int midNum = 0;

private:

	void drawRoad(bool ClientMode = true, int init = 1);
	void drawBet(bool ClientMode);
	void updateNum(CDC* dc, CRect rect, int num, COLORREF color);
};
