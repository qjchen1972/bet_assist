// CAnsDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "bbngame.h"
#include "ansDlg.h"
#include "afxdialogex.h"
#include"GameShowDlg.h"
#include"websocket.h"

// CAnsDlg 对话框

IMPLEMENT_DYNAMIC(CAnsDlg, CDialogEx)

CAnsDlg::CAnsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ANSDLG, pParent)
{	
}

CAnsDlg::~CAnsDlg()
{
	for (int i = 0; i < gameShowSet.size(); i++)
		delete gameShowSet[i];
}

void CAnsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAnsDlg, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDS_GAME1, checkAll)
	ON_BN_CLICKED(IDS_GAME2, inverse)
END_MESSAGE_MAP()


void CAnsDlg::checkAll() {
	for (int i = 0; i < gGamer.size(); i++) {
		int type = gameShowSet[i]->m_NameCheck.GetCheck();
		if(type  == 0)	setCheck(i, 1);
	}
}

void CAnsDlg::inverse() {

	for (int i = 0; i < gGamer.size(); i++) {
		int type = gameShowSet[i]->m_NameCheck.GetCheck();
		setCheck(i, 1 - type);
	}
}


// CAnsDlg 消息处理程序
BOOL CAnsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	checkAllBtn.Create(CString(_T("全选")), WS_CHILD | WS_VISIBLE,
		CRect(left, 0, 80, top), this, IDS_GAME1);
	inverseBtn.Create(CString(_T("反选")), WS_CHILD | WS_VISIBLE ,
		CRect(100, 0, 180, top), this, IDS_GAME2);

	checkAllBtn.SetCheck(BST_CHECKED);

	// TODO:  在此添加额外的初始化
	for (int i = 0; i < gGamer.size(); i++) {
		if (gGamer[i]->noShow) break;
		GameShowDlg *dlg = new GameShowDlg(i);
		
		int x = left + (i % 6) * (one_width + 5);
		int y = top + (i / 6) * one_height;

		dlg->Create(IDD_GAMESHOWDLG, this);
		dlg->SetWindowPos(
			NULL,
			x,
			y,
			one_width,
			one_height,
			SWP_SHOWWINDOW);
		gameShowSet.push_back(dlg);
	}
	return TRUE;
}

void CAnsDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()

	CRect rect;
	GetClientRect(&rect);
	dc.FillSolidRect(rect, RGB(255, 255, 255));
	CDialogEx::OnPaint();
}


void CAnsDlg::setCheck(int index, int type) {
	if (gGamer[index]->noShow) return;
	if (this == nullptr || this->m_hWnd == nullptr) return;
	int temp = gameShowSet[index]->m_NameCheck.GetCheck();
	if (temp == type) return;
	gameShowSet[index]->setGameCtrl(type);	
	gameShowSet[index]->m_NameCheck.SetCheck(type);
}

void CAnsDlg::draw_road(int index, int init) {
	if (gGamer[index]->noShow) return;
	if (this == nullptr || this->m_hWnd == nullptr ) return;
	gameShowSet[index]->setRoad(init);
}
void CAnsDlg::draw_status(int index) {
	if (gGamer[index]->noShow) return;
	if (this == nullptr || this->m_hWnd == nullptr ) return;
	gameShowSet[index]->setStatus();
}

void CAnsDlg::draw_bet(int index) {
	if (gGamer[index]->noShow) return;
	if (this == nullptr || this->m_hWnd == nullptr ) return;
	gameShowSet[index]->setBet();
}
void CAnsDlg::draw_profit(int index) {
	if (gGamer[index]->noShow) return;
	if (this == nullptr || this->m_hWnd == nullptr ) return;
	gameShowSet[index]->setProfit();
}

