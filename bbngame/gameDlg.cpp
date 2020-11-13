// CBBNGameDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "bbngame.h"
#include"ansDlg.h"
#include"CSetupRuleDlg.h"
#include "gameDlg.h"
#include "afxdialogex.h"
#include"websocket.h"
#include"setup.h"
#include"rule.h"
#include"ansDlg.h"
#include "WarnDlg.h"
#include "SidLoginDlg.h"
#include"PartDlg.h"
#include"httpsocket.h"


// CBBNGameDlg 对话框

IMPLEMENT_DYNAMIC(CGameDlg, CDialogEx)

CGameDlg::CGameDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GAME_DIALOG, pParent)
{
}

CGameDlg::~CGameDlg()
{
	if (m_ans) {
		delete	m_ans;
		m_ans = nullptr;
	}
	if (klineImage) delete klineImage;

	for (int i = 0; i < gGamer.size(); i++)
		if (gGamer[i]) delete gGamer[i];
	if (gUserData) delete gUserData;
}

void CGameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_AnsTab);
	DDX_Control(pDX, ID_SIDLOGINBTN, m_SidLoginBtn);
	DDX_Control(pDX, IDC_KLINECTRL, m_KlineCtrl);
	DDX_Control(pDX, IDC_STARTGAMEBTN, m_StartGameBtn);
	DDX_Control(pDX, IDC_STOPGAMEBTN, m_StopGameBtn);
	DDX_Control(pDX, IDC_USERCREDITEDIT, m_UserCreditEdit);
	DDX_Control(pDX, IDC_TOTALBETEDIT, m_TotalBetEdit);
	DDX_Control(pDX, IDC_TOTALPROFITEDIT, m_TotalProfitEdit);
	DDX_Control(pDX, IDC_SETINRULECOMBOX, m_SetInRuleCombox);
	DDX_Control(pDX, IDC_SETBETRULECOMBOX, m_SetBetRuleCombox);
	DDX_Control(pDX, IDC_USENEWGAMECB, m_UseNewGameCb);
	DDX_Control(pDX, IDC_CURMAXPROFITEDIT, m_CurMaxProfitEdit);
	DDX_Control(pDX, IDC_CURMINPROFITEDIT, m_CurMinProfitEdit);
	DDX_Control(pDX, IDC_SETUPRULEBTN, m_SetupRuleBtn);
	DDX_Control(pDX, IDC_BETPROFITEDIT, m_BetProfitEdit);
	DDX_Control(pDX, IDC_OMITMIDCB, m_OmitMidCb);
	DDX_Control(pDX, IDC_TARGETPROFITEDIT, m_TargetProfitEdit);
	DDX_Control(pDX, IDC_BETRATEEDIT, m_BetRateEdit);
	DDX_Control(pDX, IDC_BACKRATEEDIT, m_BackRateEdit);
	DDX_Control(pDX, IDC_GAMEMODECOMBOX, m_GameModeCombox);
	DDX_Control(pDX, IDC_LOSESTOPALLCB, m_LoseStopAllCb);
	DDX_Control(pDX, IDC_WINSTOPONEVALUEEDIT, m_WinStopOneValueEdit);
	DDX_Control(pDX, IDC_WINSTOPONECB, m_WinStopOneCb);
	DDX_Control(pDX, IDC_LOSESTOPONEVALUEEDIT, m_LoseStopOneValueEdit);
	DDX_Control(pDX, IDC_AVGUSEFAKECB, m_AvgUseFakeCb);
	DDX_Control(pDX, IDC_LOSESTOPONECB, m_LoseStopOneCb);
	DDX_Control(pDX, IDC_ALLTIMECHECK, m_AllTimeCheck);
	DDX_Control(pDX, IDC_RESETBTN, m_ResetBtn);
	DDX_Control(pDX, IDC_SCROLLBAR, m_ScrollBar);
	DDX_Control(pDX, IDC_TARGETLOSEEDIT, m_TargetLoseEdit);
	DDX_Control(pDX, IDC_LOSESTARTTIMEEDIT, m_LoseStartTimeEdit);
	DDX_Control(pDX, IDC_WINSTOPALLCB, m_WinStopAllCb);
	DDX_Control(pDX, IDC_LOSESTOPALLVALUEEDIT, m_LoseStopAllValueEdit);
	DDX_Control(pDX, IDC_WINSTOPALLVALUEEDIT, m_WinStopAllValueEdit);
	DDX_Control(pDX, IDC_WINSTARTTIMEEDIT, m_WinStartTimeEdit);
	DDX_Control(pDX, IDC_RULETENDBTN, m_RuleTendBtn);
	DDX_Control(pDX, IDC_AVGRATECB, m_AvgRateCb);
}


BEGIN_MESSAGE_MAP(CGameDlg, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &CGameDlg::OnBnClickedCancel)
	ON_BN_CLICKED(ID_SIDLOGINBTN, &CGameDlg::OnBnClickedSidloginbtn)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CGameDlg::OnSelchangeTab)
	ON_WM_TIMER()
	ON_MESSAGE(WM_UPDATE_MESSAGE, OnUpdateMessage)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_STARTGAMEBTN, &CGameDlg::OnBnClickedStartgamebtn)
	ON_BN_CLICKED(IDC_STOPGAMEBTN, &CGameDlg::OnBnClickedStopgamebtn)
	ON_BN_CLICKED(IDC_SETUPRULEBTN, &CGameDlg::OnBnClickedSetuprulebtn)
	ON_BN_CLICKED(IDC_USENEWGAMECB, &CGameDlg::OnBnClickedUsenewgamecb)
	ON_CBN_DROPDOWN(IDC_SETINRULECOMBOX, &CGameDlg::OnCbnDropdownSetinrulecombox)
	ON_CBN_DROPDOWN(IDC_SETBETRULECOMBOX, &CGameDlg::OnCbnDropdownSetbetrulecombox)
	ON_BN_CLICKED(IDC_RESETBTN, &CGameDlg::OnBnClickedResetbtn)
	ON_BN_CLICKED(IDC_ALLTIMECHECK, &CGameDlg::OnBnClickedAlltimecheck)
	//ON_CBN_SELCHANGE(IDC_GAMEMODECOMBOX, &CGameDlg::OnCbnSelchangeGamemodecombox)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_DETAILPROFITBTN, &CGameDlg::OnBnClickedDetailprofitbtn)
	ON_BN_CLICKED(IDC_RULETENDBTN, &CGameDlg::OnBnClickedRuletendbtn)
END_MESSAGE_MAP()


// CBBNGameDlg 消息处理程序


void CGameDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CGameDlg::OnBnClickedSidloginbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	SidLoginDlg dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK){
		m_SidLoginBtn.EnableWindow(FALSE);
		m_StopGameBtn.EnableWindow(false);
		m_StartGameBtn.EnableWindow(true);
		m_ResetBtn.EnableWindow(true);
		isNotify = false;
	}
}


void CGameDlg::disableCtrl() {

	m_SetInRuleCombox.EnableWindow(false);
	m_SetBetRuleCombox.EnableWindow(false);
	m_OmitMidCb.EnableWindow(false);
	m_UseNewGameCb.EnableWindow(false);
	m_BetRateEdit.EnableWindow(false);

	m_TargetProfitEdit.EnableWindow(false);
	m_TargetLoseEdit.EnableWindow(false);
	m_BackRateEdit.EnableWindow(false);
	m_GameModeCombox.EnableWindow(false);
		
	m_LoseStopAllCb.EnableWindow(false);
	m_LoseStopAllValueEdit.EnableWindow(false);
	m_LoseStartTimeEdit.EnableWindow(false);

	m_WinStopAllCb.EnableWindow(false);
	m_WinStopAllValueEdit.EnableWindow(false);
	m_WinStartTimeEdit.EnableWindow(false);

	m_WinStopOneCb.EnableWindow(false);
	m_WinStopOneValueEdit.EnableWindow(false);
	m_LoseStopOneCb.EnableWindow(false);
	m_LoseStopOneValueEdit.EnableWindow(false);	
	m_AvgUseFakeCb.EnableWindow(false);
	if (gConfig.userType) {
		m_AvgRateCb.EnableWindow(false);
	}
}


void CGameDlg::enableCtrl() {
	m_SetInRuleCombox.EnableWindow(true);
	m_SetBetRuleCombox.EnableWindow(true);
	m_OmitMidCb.EnableWindow(true);
	m_UseNewGameCb.EnableWindow(true);
	m_BetRateEdit.EnableWindow(true);


	m_TargetProfitEdit.EnableWindow(true);
	m_TargetLoseEdit.EnableWindow(true);
	m_BackRateEdit.EnableWindow(true);
	m_GameModeCombox.EnableWindow(true);

	m_LoseStopAllCb.EnableWindow(true);
	m_LoseStopAllValueEdit.EnableWindow(true);
	m_LoseStartTimeEdit.EnableWindow(true);

	m_WinStopAllCb.EnableWindow(true);
	m_WinStopAllValueEdit.EnableWindow(true);
	m_WinStartTimeEdit.EnableWindow(true);

	m_WinStopOneCb.EnableWindow(true);
	m_WinStopOneValueEdit.EnableWindow(true);
	m_LoseStopOneCb.EnableWindow(true);
	m_LoseStopOneValueEdit.EnableWindow(true);
	m_AvgUseFakeCb.EnableWindow(true);
    m_AvgUseFakeCb.EnableWindow(true);
	if (gConfig.userType) {
		m_AvgRateCb.EnableWindow(true);
	}
}
	
BOOL CGameDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	
	Tool tool;
	std::string title = "版本号" + gConfig.ver + ",到期时间" + tool.getDate(gConfig.validDate);
	if (gConfig.userType)
		title += "(VIP用户)";
	else
		title += " (普通用户)";
	SetWindowText(CString(title.c_str()));

	gUserData = new UserStat();

	//  执行此操作
	if (!gConfig.get_gamesite()) {
		MessageBox(_T("读入游戏配置文件出错"), _T("出错!"));
		m_SidLoginBtn.EnableWindow(TRUE);
		OnCancel();
		return TRUE;
	}
	gConfig.writeConfig();
	
	if(gConfig.isOmitMid) m_OmitMidCb.SetCheck(1);
	else m_OmitMidCb.SetCheck(0);
	if (gConfig.useNew) m_UseNewGameCb.SetCheck(1);
	else m_UseNewGameCb.SetCheck(0);
	
	CString str;
	str.Format(_T("%.2f"), gConfig.targetProfit);
	m_TargetProfitEdit.SetWindowText(str);
	str.Format(_T("%.2f"), gConfig.targetLose);
	m_TargetLoseEdit.SetWindowText(str);
	str.Format(_T("%.2f"), gConfig.moneyRate);
	m_BetRateEdit.SetWindowText(str);
	str.Format(_T("%.2f"), gConfig.backRate);
	m_BackRateEdit.SetWindowText(str);

	m_GameModeCombox.InsertString(0, _T("模拟游戏"));
	m_GameModeCombox.InsertString(1, _T("真实游戏"));
	if(gConfig.realMoney) m_GameModeCombox.SetCurSel(1);
	else m_GameModeCombox.SetCurSel(0);
	

	if (gConfig.loseRestart) m_LoseStopAllCb.SetCheck(1);
	else m_LoseStopAllCb.SetCheck(0);
	str.Format(_T("%.2f"), gConfig.loseValue);
	m_LoseStopAllValueEdit.SetWindowText(str);
	str.Format(_T("%d"), gConfig.loseTime);
	m_LoseStartTimeEdit.SetWindowText(str);

	if (gConfig.winRestart) m_WinStopAllCb.SetCheck(1);
	else m_WinStopAllCb.SetCheck(0);
	str.Format(_T("%.2f"), gConfig.winValue);
	m_WinStopAllValueEdit.SetWindowText(str);
	str.Format(_T("%d"), gConfig.winTime);
	m_WinStartTimeEdit.SetWindowText(str);


	if (gConfig.oneWinStop) m_WinStopOneCb.SetCheck(1);
	else m_WinStopOneCb.SetCheck(0);
	str.Format(_T("%.2f"), gConfig.oneTargetWin);
	m_WinStopOneValueEdit.SetWindowText(str);

	if (gConfig.oneLoseStop) m_LoseStopOneCb.SetCheck(1);
	else m_LoseStopOneCb.SetCheck(0);
	str.Format(_T("%.2f"), gConfig.oneTargetLose);
	m_LoseStopOneValueEdit.SetWindowText(str);

	if (gConfig.avgLowIsFake) m_AvgUseFakeCb.SetCheck(1);
	else m_AvgUseFakeCb.SetCheck(0);
	
	if (!gConfig.userType) {
		m_RuleTendBtn.ShowWindow(FALSE);
		m_AvgRateCb.ShowWindow(FALSE);
	}

	for (int i = 0; i < gConfig.gameSite.size(); i++) {
		if (gConfig.gameSite[i].roomid) break;
		WebSocket *web = new WebSocket(
			gConfig.gameSite[i].dns,
			gConfig.gameSite[i].port,
			gConfig.gameSite[i].path,
			gConfig.gameSite[i].gameType,
			gConfig.gameSite[i].gameCode,
			gGamer.size(),
			gConfig.gameSite[i].roomid);
		web->set_gameDlg(this);
		gGamer.push_back(web);		
		//if( i == 3) break;
	}

	for (int i = 0; i < gRuleConfig.inRuleFileList.size(); i++) {
		CString  str(gRuleConfig.inRuleFileList[i].c_str());
		m_SetInRuleCombox.InsertString(i, str);
	}
	if (gRuleConfig.inRuleFileList.size() > gRuleConfig.inRuleIndex)
		m_SetInRuleCombox.SetCurSel(gRuleConfig.inRuleIndex);
	

	for (int i = 0; i < gRuleConfig.betRuleFileList.size(); i++) {
		CString  str(gRuleConfig.betRuleFileList[i].c_str());
		m_SetBetRuleCombox.InsertString(i, str);
	}	

	if (gRuleConfig.betRuleFileList.size() > gRuleConfig.betRuleIndex )
		m_SetBetRuleCombox.SetCurSel(gRuleConfig.betRuleIndex);

	m_StopGameBtn.EnableWindow(false);
	m_SidLoginBtn.EnableWindow(true);
	m_StartGameBtn.EnableWindow(false);
	m_ResetBtn.EnableWindow(false);
	
	// TODO:  在此添加额外的初始化
	CFont m_oFont;
	m_oFont.CreatePointFont(150, _T("隶书"));
	m_UserCreditEdit.SetFont(&m_oFont);
	m_TotalBetEdit.SetFont(&m_oFont);
	m_TotalProfitEdit.SetFont(&m_oFont);
	m_BetProfitEdit.SetFont(&m_oFont);
	m_CurMaxProfitEdit.SetFont(&m_oFont);
	m_CurMinProfitEdit.SetFont(&m_oFont);

	CRect tabRect;    // 标签控件客户区的Rect   
   // 获取标签控件客户区Rect，并对其调整，以适合放置标签页   
	m_AnsTab.GetClientRect(&tabRect);
	tabRect.left += 1;
	tabRect.right -= 1;
	tabRect.top += 40;
	tabRect.bottom -= 1;

	CSize size, size1;
	size.cx = 150;
	size.cy = 40;
	size1 = m_AnsTab.SetItemSize(size);

	CFont m_tabFont;
	m_tabFont.CreatePointFont(200, _T("宋书"));
	m_AnsTab.SetFont(&m_tabFont);
	m_AnsTab.InsertItem(0, _T("BB富贵厅"));
	m_ans = new CAnsDlg();
	m_ans->Create(IDD_ANSDLG, &m_AnsTab);
	m_ans->SetWindowPos(
		NULL,
		tabRect.left,
		tabRect.top,
		tabRect.Width(),
		tabRect.Height(),
		SWP_SHOWWINDOW);
	
	CRect rect;
	m_KlineCtrl.GetClientRect(&rect);
	if (!klineImage) klineImage = new KLineImage();
	klineImage->setRect(rect);

	SetTimer(TIMER_ID, 1000, NULL);
	if (isLive)	m_AllTimeCheck.SetCheck(1);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}



void CGameDlg::OnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	/*switch (m_AnsTab.GetCurSel()){
		case 0:
			break;
		case 1:
			break;
		default:
			break;
	}*/
}
void CGameDlg::UpdateUserInfo() {

	CString str;
	str.Format(_T("%.2f"), gUserData->getCredit());
	m_UserCreditEdit.SetWindowText(str);
	str.Format(_T("%.2f"), gUserData->procer.realStat.bet);
	m_TotalBetEdit.SetWindowText(str);
	str.Format(_T("%.2f"), gUserData->procer.realStat.profit);
	m_TotalProfitEdit.SetWindowText(str);
	str.Format(_T("%.2f"), gUserData->procer.realStat.bet * gConfig.backRate / 100);
	m_BetProfitEdit.SetWindowText(str);
}


LRESULT CGameDlg::OnUpdateMessage(WPARAM wParam, LPARAM lParam) {

	int val = (int)wParam;
	if (val == 0) {
		if (isNotify == true) return 0;
		isNotify = true;
		for (int i = 0; i < gGamer.size(); i++) {
			gGamer[i]->stop();
		}
		m_SidLoginBtn.EnableWindow(true);
		MessageBox(_T("SID登录错误"), _T("错误"));
		return 1;
	}
	else if (val <= 2) {
		gConfig.isStop = true;
		for (int i = 0; i < gGamer.size(); i++) {
			m_ans->setCheck(i, 0);
		}

		int type; 
		if (val == 1) type = 0;
		else if (val == 2) type = 1;
		WarnDlg *dlg = new WarnDlg(type);
		dlg->DoModal();
		delete dlg;
		m_StopGameBtn.EnableWindow(false);
		m_StartGameBtn.EnableWindow(false);
		m_ResetBtn.EnableWindow(true);
		return 1;
	}		
	else if (val <= 4) {
		
		gConfig.needStart = val;
		gUserData->addTemp();

		gConfig.stopTime = time(NULL);
		gConfig.isStop = true;
		for (int i = 0; i < gGamer.size(); i++) {
			//gGamer[i]->pGameProcer->gameData.initTemp();
			m_ans->setCheck(i, 0);
			//m_ans->draw_profit(gGamer[i]->pGameProcer->gameData.roomId);
		}
		//gUserData->initTemp();
		m_StopGameBtn.EnableWindow(false);
		m_StartGameBtn.EnableWindow(true);
		m_ResetBtn.EnableWindow(true);
		return 1;
	}	
	else if (val == 5) {
		int index = *(int*)lParam;
		m_ans->draw_bet(index);
		m_ans->setCheck(index, 0);
	}
	else;
}

void CGameDlg::needStartProc(CDC *dc) {

	if (!gConfig.needStart) return;
	uint64_t now = time(NULL);
	uint64_t startTime;
	if (gConfig.needStart == 3) startTime = gConfig.stopTime + gConfig.winTime * 60;
	else startTime = gConfig.stopTime + gConfig.loseTime * 60;
	CRect  rect;
	rect.SetRect(450, 0, 900, 40);

	if (now >= startTime) {
		gUserData->tempProcer.setStartTime(now);
		gConfig.isStop = false;

		gConfig.needStart = 0;
		for (int i = 0; i < gGamer.size(); i++) {
			m_ans->setCheck(i, 1);
			gGamer[i]->pGameProcer->gameData.initTemp();
			//m_ans->draw_profit(gGamer[i]->pGameProcer->gameData.roomId);
		}
		gUserData->initTemp();
		//CRect  rect;
		//rect.SetRect(450, 0, 900, 40);
		Invalidate();
		//InvalidateRect(rect);
		m_StopGameBtn.EnableWindow(true);
		m_StartGameBtn.EnableWindow(false);
		m_ResetBtn.EnableWindow(false);
	}
	else {
		CString str;
		str.Format(_T("%d 秒后自动重新开始"), startTime - now);
		CFont myFont; myFont.CreateFont(40, 0, 0, 0, 100, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_SWISS, _T("Arial"));//设置字体
		CClientDC dc(this);
		CFont *poldFont = dc.SelectObject(&myFont);
		//dc.SetBkMode(TRANSPARENT);
		dc.FillSolidRect(rect, RGB(0, 255, 255));
		dc.SetTextColor(RGB(0, 0, 0));
		dc.SetBkColor(RGB(0, 255, 255));

		dc.DrawText(str, -1, rect, DT_SINGLELINE);
		dc.SelectObject(poldFont);
	}
}
	
void  CGameDlg::klineProc() {
	scrolSize = allData.size() - klineImage->imageSize;
	if (scrolSize < 0) scrolSize = 0;
	m_ScrollBar.SetScrollRange(0, scrolSize);
	scrolPos = scrolSize;
	m_ScrollBar.SetScrollPos(scrolPos);
	int end = scrolPos;
	float curMaxProfit = 0;
	float curMinProfit = 0;

	if (allData.size() > 0) {
		klineImage->getKlineInfo(allData, 0, allData.size() - 1, allMaxProfit, allMinProfit);
		end = scrolPos + klineImage->imageSize - 1;
		if (end > allData.size() - 1) end = allData.size() - 1;
		klineImage->getKlineInfo(allData, scrolPos, end, curMaxProfit, curMinProfit);
	}
	klineImage->setImageInfo(allMaxProfit, allMinProfit);

	CDC* pDC = m_KlineCtrl.GetDC();
	klineImage->drawImage(allData, scrolPos, end, pDC);
	m_KlineCtrl.ReleaseDC(pDC);

	CString str;
	str.Format(_T("%.2f"), curMaxProfit);
	m_CurMaxProfitEdit.SetWindowText(str);
	str.Format(_T("%.2f"), curMinProfit);
	m_CurMinProfitEdit.SetWindowText(str);
}

void CGameDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CClientDC dc(this);
	needStartProc(&dc);
	if (isLive) {
		gUserData->tempProcer.getAllKline(allData);
		klineProc();
	}

	queryTime++;
	if (queryTime >= 60) {
		HttpSocket *http;
		http = new HttpSocket(gConfig.dns, gConfig.port);
		int ret = http->query(gConfig.userName, gConfig.loginId);
		delete http;
		queryTime = 0;
		if (ret == 2) {
			MessageBox(_T("账号在其他地方登录了"), _T("出错!"));
			OnCancel();
		}
		else if (ret == 3) {
			MessageBox(_T("账号已经到期"), _T("出错!"));
			OnCancel();
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}



void CGameDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
	
	needStartProc(&dc);
	int end = scrolPos;
	float curMaxProfit = 0;
	float curMinProfit = 0;
	if (allData.size() > 0) {
		end = scrolPos + klineImage->imageSize - 1;
		if (end > allData.size() - 1) end = allData.size() - 1;
		klineImage->getKlineInfo(allData, scrolPos, end, curMaxProfit, curMinProfit);
	}

	PAINTSTRUCT ps;
	CDC* pDC = m_KlineCtrl.BeginPaint(&ps);
	klineImage->drawImage(allData, scrolPos, end, pDC);
	m_KlineCtrl.EndPaint(&ps);

	CString str;
	str.Format(_T("%.2f"), curMaxProfit);
	m_CurMaxProfitEdit.SetWindowText(str);
	str.Format(_T("%.2f"), curMinProfit);
	m_CurMinProfitEdit.SetWindowText(str);
}



void CGameDlg::OnBnClickedStartgamebtn()
{

	// TODO: 在此添加控件通知处理程序代码
	if (m_SetInRuleCombox.GetCurSel() < 0 || m_SetBetRuleCombox.GetCurSel() < 0 || 
		m_GameModeCombox.GetCurSel() < 0) {
		MessageBox(_T("空的规则"), _T("错误"));
		return;
	}

	gRuleConfig.SetInRule(m_SetInRuleCombox.GetCurSel());
	gRuleConfig.SetBetRule(m_SetBetRuleCombox.GetCurSel());

	if (m_OmitMidCb.GetCheck()) gConfig.isOmitMid = true;
	else gConfig.isOmitMid = false;
	
	if (m_UseNewGameCb.GetCheck()) gConfig.useNew = true;
	else gConfig.useNew = false;
	
	CString str;
	m_TargetProfitEdit.GetWindowText(str);
	if (str.GetLength() == 0) {
		MessageBox(_T("目标赢利需要设置"), _T("错误"));
		return;
	}
	gConfig.targetProfit = atof(CT2A(str.GetBuffer()));
	m_TargetLoseEdit.GetWindowText(str);
	if (str.GetLength() == 0) {
		MessageBox(_T("目标止损需要设置"), _T("错误"));
		return;
	}
	gConfig.targetLose = atof(CT2A(str.GetBuffer()));
	m_BetRateEdit.GetWindowText(str);
	gConfig.moneyRate = atof(CT2A(str.GetBuffer()));
	m_BackRateEdit.GetWindowText(str);
	gConfig.backRate = atof(CT2A(str.GetBuffer()));	

	int index = m_GameModeCombox.GetCurSel();
	if (index == 0) gConfig.realMoney = false;
	else gConfig.realMoney = true;	

	if (m_LoseStopAllCb.GetCheck()) gConfig.loseRestart = true;
	else gConfig.loseRestart = false;
	m_LoseStopAllValueEdit.GetWindowText(str);
	gConfig.loseValue = atof(CT2A(str.GetBuffer()));
	m_LoseStartTimeEdit.GetWindowText(str);
	gConfig.loseTime = atoi(CT2A(str.GetBuffer()));

	if (m_WinStopAllCb.GetCheck()) gConfig.winRestart = true;
	else gConfig.winRestart = false;
	m_WinStopAllValueEdit.GetWindowText(str);
	gConfig.winValue = atof(CT2A(str.GetBuffer()));
	m_WinStartTimeEdit.GetWindowText(str);
	gConfig.winTime = atoi(CT2A(str.GetBuffer()));


	if (m_WinStopOneCb.GetCheck()) gConfig.oneWinStop = true;
	else gConfig.oneWinStop = false;
	m_WinStopOneValueEdit.GetWindowText(str);
	gConfig.oneTargetWin = atof(CT2A(str.GetBuffer()));


	if (m_LoseStopOneCb.GetCheck()) gConfig.oneLoseStop = true;
	else gConfig.oneLoseStop = false;
	m_LoseStopOneValueEdit.GetWindowText(str);
	gConfig.oneTargetLose = atof(CT2A(str.GetBuffer()));

	if (m_AvgUseFakeCb.GetCheck()) gConfig.avgLowIsFake = true;
	else gConfig.avgLowIsFake = false;	

	if (gConfig.userType) {
		if (m_AvgRateCb.GetCheck()) gConfig.avgRate = true;
		else gConfig.avgRate = false;
	}

	int type = m_AllTimeCheck.GetCheck();
	if (type == 0) {
		m_AllTimeCheck.SetCheck(1);
		isLive = true;
	}

	gConfig.writeConfig();

	gConfig.isStop = false;
	if (gConfig.needStart) {
		gConfig.needStart = 0;
		for (int i = 0; i < gGamer.size(); i++) {
			m_ans->setCheck(i, 1);
			gGamer[i]->pGameProcer->gameData.initTemp();			
			//m_ans->draw_profit(gGamer[i]->pGameProcer->gameData.roomId);
		}
		gUserData->initTemp();
		//CRect  rect;
		//rect.SetRect(450, 0, 900, 40);
		Invalidate();
	}

	uint64_t now = time(NULL);

	gUserData->procer.setStartTime(now);
	gUserData->tempProcer.setStartTime(now);

	m_StopGameBtn.EnableWindow(true);
	m_StartGameBtn.EnableWindow(false);
	m_ResetBtn.EnableWindow(false);
	disableCtrl();
}


void CGameDlg::OnBnClickedStopgamebtn()
{
	// TODO: 在此添加控件通知处理程序代码

	gConfig.isStop = true;
	m_StopGameBtn.EnableWindow(false);
	m_ResetBtn.EnableWindow(true);
	m_StartGameBtn.EnableWindow(true);
	enableCtrl();
}


void CGameDlg::OnBnClickedSetuprulebtn()
{
	// TODO: 在此添加控件通知处理程序代码
	CSetupRuleDlg  dlg;
	dlg.DoModal();
}


void CGameDlg::OnBnClickedUsenewgamecb()
{
	// TODO: 在此添加控件通知处理程序代码
	gConfig.useNew = 1 - gConfig.useNew;
	m_ans->Invalidate();
}




void CGameDlg::OnCbnDropdownSetinrulecombox()
{
	// TODO: 在此添加控件通知处理程序代码
	m_SetInRuleCombox.ResetContent();
	for (int i = 0; i < gRuleConfig.inRuleFileList.size(); i++) {
		CString  str(gRuleConfig.inRuleFileList[i].c_str());
		m_SetInRuleCombox.InsertString(i, str);
	}
}


void CGameDlg::OnCbnDropdownSetbetrulecombox()
{
	// TODO: 在此添加控件通知处理程序代码
	m_SetBetRuleCombox.ResetContent();
	for (int i = 0; i < gRuleConfig.betRuleFileList.size(); i++) {
		CString  str(gRuleConfig.betRuleFileList[i].c_str());
		m_SetBetRuleCombox.InsertString(i, str);
	}
}


void CGameDlg::OnBnClickedResetbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	//gConfig.needStart = 0;
	gUserData->init();
	UpdateUserInfo();
	for (int i = 0; i < gGamer.size(); i++) {
		m_ans->setCheck(i, 1);
		gGamer[i]->pGameProcer->gameData.init();
		//m_ans->setCheck(i, 1);
		m_ans->draw_profit(gGamer[i]->pGameProcer->gameData.roomId);
	}
	gConfig.needStart = 0;
	Invalidate();

	m_StopGameBtn.EnableWindow(false);
	m_StartGameBtn.EnableWindow(true);
	m_ResetBtn.EnableWindow(true);
	enableCtrl();
}


void CGameDlg::OnBnClickedAlltimecheck()
{
	// TODO: 在此添加控件通知处理程序代码
	//isLive = 1 - isLive;
	int type = m_AllTimeCheck.GetCheck();
	if (type == 0) {
		gUserData->procer.getAllKline(allData);
		klineProc();
		isLive = false;
	}
	else 
		isLive = true;
}


/*void CGameDlg::OnCbnSelchangeGamemodecombox()
{
	// TODO: 在此添加控件通知处理程序代码
	int index = m_GameModeCombox.GetCurSel();
	if (index == 1) {
		gConfig.realMoney = true;
	}
	else if (index == 0) {
		gConfig.realMoney = false;
	}
	else;
}*/


void CGameDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int TempPos = pScrollBar->GetScrollPos();
	switch (nSBCode){
	case SB_THUMBPOSITION://拖动滑块
		pScrollBar->SetScrollPos(nPos);
		TempPos = nPos;
		break;
	case SB_LINELEFT://点击左边的箭头
		if (TempPos >= 1){
			TempPos--;
		}
		pScrollBar->SetScrollPos(TempPos);
		break;

	case SB_LINERIGHT://点击右边的箭头

		if (TempPos < scrolSize){
			TempPos++;
		}
		pScrollBar->SetScrollPos(TempPos);
		break;
	case SB_PAGELEFT:
		TempPos -= 10;
		if (TempPos < 0) TempPos = 0;
		if (TempPos > scrolSize) TempPos = scrolSize;
		pScrollBar->SetScrollPos(TempPos);
		break;
	case SB_PAGERIGHT:
		TempPos += 10;
		if (TempPos < 0) TempPos = 0;
		if (TempPos > scrolSize) TempPos = scrolSize;
		pScrollBar->SetScrollPos(TempPos);
		break;
	case SB_LEFT:
		TempPos = 0;
		pScrollBar->SetScrollPos(TempPos);
		break;
	case SB_RIGHT:
		TempPos = scrolSize;
		pScrollBar->SetScrollPos(TempPos);
		break;	
	}
	scrolPos = TempPos;

	int end = scrolPos;
	float curMaxProfit = 0;
	float curMinProfit = 0;
	if (allData.size() > 0) {
		end = scrolPos + klineImage->imageSize - 1;
		if (end > allData.size() - 1) end = allData.size() - 1;
		klineImage->getKlineInfo(allData, scrolPos, end, curMaxProfit, curMinProfit);
	}

	CDC* pDC = m_KlineCtrl.GetDC();
	klineImage->drawImage(allData, scrolPos, end, pDC);
	m_KlineCtrl.ReleaseDC(pDC);

	CString str;
	str.Format(_T("%.2f"), curMaxProfit);
	m_CurMaxProfitEdit.SetWindowText(str);
	str.Format(_T("%.2f"), curMinProfit);
	m_CurMinProfitEdit.SetWindowText(str);

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CGameDlg::OnBnClickedDetailprofitbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	PartDlg dlg;
	dlg.DoModal();
}



bool CGameDlg::isOver(std::vector<pos_t> &allPos) {

	for (int i = 0; i < gGamer.size(); i++) {
		if (!allPos[i].isEnd) return false;
	}
	return true;
}

void CGameDlg::tendProc() {
	auto threadId = std::thread([this]() {

		StatProc procer;
		std::vector<StatProc*> gameProcer;
		std::vector<std::vector<std::vector<Round_t>>> allRoad;
		std::vector<std::vector<std::vector<Round_t>>> allColRoad;
		std::vector<pos_t> allPos;

		for (int i = 0; i < gGamer.size(); i++) {
			StatProc *temp = new StatProc();
			//temp->reset();
			//StatProc *temp = &gGamer[i]->pGameProcer->gameData.allProcer;
			temp->reset();
			gameProcer.push_back(temp);
			std::vector<std::vector<Round_t>>  road;
			allColRoad.push_back(road);
			gGamer[i]->pGameProcer->gameData.getAllRoad(road);
			allRoad.push_back(road);
			pos_t pos;
			allPos.push_back(pos);
		}

		/*uint8_t betType;
		int b1;
		int b2;
		int b3;
		int8_t betId;*/
		Round_t curRound, ansRound;

		while (!isOver(allPos)) {

			for (int i = 0; i < gGamer.size(); i++) {
				if (allPos[i].isEnd) continue;
				if (allRoad[i].size() <= allPos[i].row) {
					allPos[i].isEnd = true;
					continue;
				}
				if (allRoad[i][allPos[i].row].size() <= allPos[i].col) {
					allPos[i].isEnd = true;
					continue;
				}
				GameData data;
				curRound = allRoad[i][allPos[i].row][allPos[i].col];
				allPos[i].col++;
				if (allPos[i].col >= allRoad[i][allPos[i].row].size()) {
					allPos[i].row++;
					if (allPos[i].row >= allRoad[i].size()) {
						allPos[i].isEnd = true;
						continue;
					}
					allPos[i].col = 0;
					if (allRoad[i][allPos[i].row].size() <= allPos[i].col) {
						allPos[i].isEnd = true;
						continue;
					}
				}
				ansRound = allRoad[i][allPos[i].row][allPos[i].col];
				data.addColRoad(curRound, allColRoad[i], gConfig.isOmitMid);

				Round_t round;
				int ret = gRuleConfig.betMoney(allColRoad[i], procer, *gameProcer[i],
					false, round.type, round.b1, round.b2, round.b3, round.betId);
				if (ret <= 0) continue;
				//Round_t round;
				//round.betId = betId;
				//round.type = betType;
				round.payoff = gRuleConfig.betAns(ansRound.cardAns, round.b1, round.b2, round.b3);
				procer.setProfit(round, true);
				gameProcer[i]->setProfit(round);
			}
		}

		for (int i = 0; i < gGamer.size(); i++) delete gameProcer[i];
		procer.getAllKline(allData);
		klineProc();
		isLive = false;

	});
	threadId.detach();
}


void CGameDlg::OnBnClickedRuletendbtn()
{
	// TODO: 在此添加控件通知处理程序代码

	m_AllTimeCheck.SetCheck(0);
	isLive = false;
	if (m_SetInRuleCombox.GetCurSel() < 0 || m_SetBetRuleCombox.GetCurSel() < 0) {
		MessageBox(_T("空的规则"), _T("错误"));
		return;
	}

	gRuleConfig.SetInRule(m_SetInRuleCombox.GetCurSel());
	gRuleConfig.SetBetRule(m_SetBetRuleCombox.GetCurSel());

	if (m_OmitMidCb.GetCheck()) gConfig.isOmitMid = true;
	else gConfig.isOmitMid = false;

	if (m_UseNewGameCb.GetCheck()) gConfig.useNew = true;
	else gConfig.useNew = false;

	CString str;
	m_BetRateEdit.GetWindowText(str);
	gConfig.moneyRate = atof(CT2A(str.GetBuffer()));

	tendProc();

#if 0
	StatProc procer;
	std::vector<StatProc*> gameProcer;
	std::vector<std::vector<std::vector<Round_t>>> allRoad;
	std::vector<std::vector<std::vector<Round_t>>> allColRoad;
	std::vector<pos_t> allPos;

	for (int i = 0; i < gGamer.size(); i++) {
		StatProc *temp = new StatProc();
		//temp->reset();
		//StatProc *temp = &gGamer[i]->pGameProcer->gameData.allProcer;
		temp->reset();
		gameProcer.push_back(temp);
		std::vector<std::vector<Round_t>>  road;
		allColRoad.push_back(road);
		gGamer[i]->pGameProcer->gameData.getAllRoad(road);
		allRoad.push_back(road);
		pos_t pos;
		allPos.push_back(pos);
	}

	/*uint8_t betType;
	int b1;
	int b2;
	int b3;
	int8_t betId;*/
	Round_t curRound, ansRound;

	while (!isOver(allPos)) {

		for (int i = 0; i < gGamer.size(); i++) {
			if (allPos[i].isEnd) continue;
			if (allRoad[i].size() <= allPos[i].row) {
				allPos[i].isEnd = true;
				continue;
			}
			if (allRoad[i][allPos[i].row].size() <= allPos[i].col) {
				allPos[i].isEnd = true;
				continue;
			}
			GameData data;
			curRound = allRoad[i][allPos[i].row][allPos[i].col];
			allPos[i].col++;
			if (allPos[i].col >= allRoad[i][allPos[i].row].size()) {
				allPos[i].row++;
				if (allPos[i].row >= allRoad[i].size()) {
					allPos[i].isEnd = true;
					continue;
				}
				allPos[i].col = 0;
				if (allRoad[i][allPos[i].row].size() <= allPos[i].col) {
					allPos[i].isEnd = true;
					continue;
				}				
			}
			ansRound = allRoad[i][allPos[i].row][allPos[i].col];
			data.addColRoad(curRound, allColRoad[i], gConfig.isOmitMid);

			Round_t round;
			int ret = gRuleConfig.betMoney(allColRoad[i], procer, *gameProcer[i],
				false, round.type, round.b1, round.b2, round.b3, round.betId);
			if (ret <= 0) continue;
			//Round_t round;
			//round.betId = betId;
			//round.type = betType;
			round.payoff = gRuleConfig.betAns(ansRound.cardAns, round.b1, round.b2, round.b3);
			procer.setProfit(round, true);
			gameProcer[i]->setProfit(round);
		}
	}

	procer.getAllKline(allData);
	klineProc();
	isLive = false;
#endif

}
