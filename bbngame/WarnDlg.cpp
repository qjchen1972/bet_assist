// WarnDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "bbngame.h"
#include "WarnDlg.h"
#include "afxdialogex.h"
#include"userdata.h"

// WarnDlg 对话框

IMPLEMENT_DYNAMIC(WarnDlg, CDialogEx)

WarnDlg::WarnDlg(int type, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WARNDIALOG, pParent)
{
	nType = type;
}

WarnDlg::~WarnDlg()
{
}

void WarnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TIMEEDIT, m_TimeEdit);
}


BEGIN_MESSAGE_MAP(WarnDlg, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// WarnDlg 消息处理程序

void WarnDlg::drawObj(CDC *dc, int type) {

	CFont myFont; myFont.CreateFont(100, 0, 0, 0, 100, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_SWISS, _T("Arial"));//设置字体
	CFont *poldFont = dc->SelectObject(&myFont);
	//设置字体
	CRect  rect;
	rect.SetRect(20, 80, 1500, 1500);
   //dc.SetBkMode(TRANSPARENT);
	dc->SetTextColor(RGB(0, 0, 0));
	if (type == 0) {
		dc->SetBkColor(RGB(0, 255, 255));
		dc->DrawText(_T("今天已经达成目标，请注意合理游戏！"), -1, rect, DT_SINGLELINE);
	}
	else {
		dc->SetBkColor(RGB(255, 0, 0));
		dc->DrawText(_T("今天止损已经到了，请注意合理游戏！"), -1, rect, DT_SINGLELINE);
	}	
	
	dc->SelectObject(poldFont);
	CString str;
	str.Format(_T("本次耗费时间: %d 分钟"), (time(NULL) - gUserData->procer.realStat.startTime) / 60 );
	m_TimeEdit.SetWindowText(str);
}

void WarnDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()

	CRect rect;
	GetClientRect(&rect);
	dc.FillSolidRect(rect, RGB(255, 255, 255));

	drawObj(&dc, nType);
}


BOOL WarnDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CFont m_oFont;
	m_oFont.CreatePointFont(180, _T("隶书"));
	m_TimeEdit.SetFont(&m_oFont);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
