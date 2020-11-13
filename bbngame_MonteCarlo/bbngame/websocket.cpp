#include "stdafx.h"
#include<vector>
#include"ansDlg.h"
#include"gameDlg.h"
#include "websocket.h"

int GameProc::proc_amfIsReady() {
	char buf[TEMP_SIZE];
	char tempBuf[TEMP_SIZE];
	int32_t buflen;

	sprintf_s(buf, "{\"gameType\":%d,\"gameCode\":%d,"
		"\"sid\":\"%s\",\"action\":\"login\"}",
		nGameType, nGameCode, gConfig.sid.c_str());
	set_status(AMFISREADY);
	bufToSendStr(buf, tempBuf, buflen);
	if (conner->sendBuf(tempBuf, buflen) < 0) return -1;
	return 1;
}

int GameProc::proc_onLogin(const rapidjson::Value& obj) {

	if (obj[0].HasMember("runEor")) {
		PostMessage(m_dlg->GetSafeHwnd(), WM_UPDATE_MESSAGE, 0, 0);
		return - 1;
	}
	char buf[TEMP_SIZE];
	char tempBuf[TEMP_SIZE];
	int32_t buflen;

	sprintf_s(buf, "{\"gameType\":%d,\"gameCode\":%d,"
		"\"sid\":\"%s\",\"action\":\"userInfo\"}",
		nGameType, nGameCode, gConfig.sid.c_str());
	set_status(LOGINED);
	bufToSendStr(buf, tempBuf, buflen);
	if (conner->sendBuf(tempBuf, buflen) < 0) return -1;
	return 1;
}

int GameProc::proc_onUserInfo(const rapidjson::Value& obj) {

	if (obj[0].HasMember("credit")){
		if(obj[0]["credit"].IsFloat()) 
			gUserData->setCredit(obj[0]["credit"].GetFloat());
		else
			gUserData->setCredit(obj[0]["credit"].GetInt());
		m_dlg->UpdateUserInfo();
	}

	char buf[TEMP_SIZE];
	char tempBuf[TEMP_SIZE];
	int32_t buflen;

	sprintf_s(buf, "{\"gameType\":%d,\"gameCode\":%d,"
		"\"sid\":\"%s\",\"action\":\"tableList\"}",
		nGameType, nGameCode, gConfig.sid.c_str());
	set_status(GETUSERINFO);
	bufToSendStr(buf, tempBuf, buflen);
	if (conner->sendBuf(tempBuf, buflen) < 0) return -1;
	return 1;
}


int GameProc::proc_onTableListn() {

	char buf[TEMP_SIZE];
	char tempBuf[TEMP_SIZE];
	int32_t buflen;

	sprintf_s(buf, "{\"gameType\":%d,\"gameCode\":%d,"
		"\"sid\":\"%s\",\"action\":\"videoLink\"}",
		nGameType, nGameCode, gConfig.sid.c_str());
	set_status(GETTABLE);
	bufToSendStr(buf, tempBuf, buflen);
	if (conner->sendBuf(tempBuf, buflen) < 0) return -1;
	return 1;
}

int GameProc::requestBet(uint64_t time, int bs, int b1, int b2, int b3) {

	std::string strBet = "";
	if (gameData.cur_round.b1 > 0)
		strBet += "\"b1\":" + std::to_string(gameData.cur_round.b1) + ",";
	if (gameData.cur_round.b2 > 0)
		strBet += "\"b2\":" + std::to_string(gameData.cur_round.b2) + ",";
	if (gameData.cur_round.b3 > 0)
		strBet += "\"b3\":" + std::to_string(gameData.cur_round.b3) + ",";
	strBet += "\"pokerID\":\"\"";

	char buf[TEMP_SIZE];
	char tempBuf[TEMP_SIZE];
	int32_t buflen;

	sprintf_s(buf, "{%s,\"roundSerial\":%lld,\"ts\":%lld,\"bs\":%d,"
		"\"gameType\":%d,\"gameCode\":%d,"
		"\"sid\":\"%s\",\"action\":\"bet\"}",
		strBet.c_str(), gameData.cur_round.roundSerial,
		time * 1000, bs, nGameType, nGameCode, gConfig.sid.c_str());

	bufToSendStr(buf, tempBuf, buflen);
	if (conner->sendBuf(tempBuf, buflen) < 0) return -1;
	set_status(TRYBET);
	return 1;
}


void GameProc::set_status(int ID, std::string str) {
	if (gameData.isStop) {
		gameStatus.set(ID, "停止游戏");
	}
	else {
		if(gameData.rateStr != "" && 
			(ID == BETING || ID == BETED || ID == FAKEBET)) gameStatus.set(ID, gameData.rateStr);
		else	gameStatus.set(ID, str);
	}
	m_dlg->m_ans->draw_status(nRoomId);
}


void GameProc::initRoad(std::string roadStr, std::string roundNo) {
	gameData.setInitRoad(roadStr, roundNo);
	m_dlg->m_ans->draw_road(nRoomId);
}

bool GameProc::canStart() {
	if (gameData.isStop) return false;
	if (gUserData->isStop) {
		float allProfit = gUserData->procer.realStat.bet * gConfig.backRate / 100 + gUserData->procer.realStat.profit;
		float tempProfit = gUserData->tempProcer.realStat.bet * gConfig.backRate / 100 + gUserData->tempProcer.realStat.profit;
		if (allProfit >= gConfig.targetProfit ||
			allProfit <= -gConfig.targetLose ||
			(gConfig.winRestart && tempProfit >= gConfig.winValue) ||
			(gConfig.loseRestart && tempProfit <= -gConfig.loseValue))	return false;

		int wait = 0;
		for (int i = 0; i < gGamer.size(); i++)
			if (!gGamer[i]->pGameProcer->gameData.roundIsOver) {
				wait++;
				break;
			}
		if (wait > 0)  return false;
		gUserData->isStop = false;
	}
	return true;
}


void GameProc::proc_bet() {
	int b1, b2, b3;
	int8_t betId;


	if (!canStart()) return;
	uint8_t  stype;

	if (gRuleConfig.getBetMoney(gUserData, &gameData, stype, b1, b2, b3, betId)) {

		gameData.cur_round.b1 = b1;
		gameData.cur_round.b2 = b2;
		gameData.cur_round.b3 = b3;
		gameData.cur_round.betId = betId;
		if (b1 + b2 + b3 < 10) gameData.cur_round.isSmallBet = true;

		if (stype == FAKEBET_TYPE || !gConfig.realMoney || gameData.cur_round.isSmallBet) {

			gameData.roundIsOver = false;
			gameData.cur_round.type = stype;
			if(stype == FAKEBET_TYPE)	set_status(FAKEBET);
			else set_status(BETED);
			m_dlg->m_ans->draw_bet(nRoomId);

			BetData_t data;
			data.bet = b1 + b2 + b3;
			data.gameId = nRoomId;
			data.type = stype;
			data.isSmallBet = gameData.cur_round.isSmallBet;
			data.time = gameData.cur_round.time;

			gUserData->procer.setBet(data);
			gUserData->tempProcer.setBet(data);
			gameData.allProcer.setBet(data);
			gameData.curProcer.setBet(data);
			gameData.tempProcer.setBet(data);
			m_dlg->UpdateUserInfo();			
		}
		else {
			if (gUserData->outOfCredit(b1 + b2 + b3)) {
				set_status(TRYBET, "余额不足");
				return;
			}
			requestBet(gameData.cur_round.time, gameData.allProcer.realStat.betNum + 1, b1, b2, b3);
		}
	}	
}



void GameProc::proc_fakeProfit(float credit, int res) {

	int result = ((res - 1 + 12) % 12) / 4;
	
	/*float payoff = 0;

	switch (result) {
	case BANKER:
		payoff += gameData.cur_round.b1 * 0.95;
		payoff -= gameData.cur_round.b2;
		payoff -= gameData.cur_round.b3;
		break;
	case PLAYER:
		payoff -= gameData.cur_round.b1;
		payoff += gameData.cur_round.b2;
		payoff -= gameData.cur_round.b3;
		break;
	case MID:
		payoff += 7 * gameData.cur_round.b3;
		break;
	default:
		return;
	}*/

	gameData.cur_round.payoff = gRuleConfig.betAns(result, gameData.cur_round.b1, 
		gameData.cur_round.b2, gameData.cur_round.b3);
	gameData.cur_round.userCredit = credit;
}

void GameProc::proc_newGame(std::string str) {
	if (!gameData.newGame(str)) return;
	set_status(NEWGAME);
	//gameData.newGame(str);
	newRound();
	m_dlg->m_ans->draw_road(nRoomId);
}

void GameProc::checkCanStop() {
	float allProfit = gUserData->procer.realStat.bet * gConfig.backRate / 100 + gUserData->procer.realStat.profit;
	float tempProfit = gUserData->tempProcer.realStat.bet * gConfig.backRate / 100 + gUserData->tempProcer.realStat.profit;

	if (allProfit >= gConfig.targetProfit || 
		allProfit <= -gConfig.targetLose ||
		(gConfig.winRestart && tempProfit >= gConfig.winValue) || 
		(gConfig.loseRestart && tempProfit <= -gConfig.loseValue) ) {

		gUserData->isStop = true;
		int wait = 0;
		for (int i = 0; i < gGamer.size(); i++)
			if (!gGamer[i]->pGameProcer->gameData.roundIsOver) {
				wait++;
				break;
			}
		if (wait == 0) {
			if (allProfit >= gConfig.targetProfit) {
				PostMessage(m_dlg->GetSafeHwnd(), WM_UPDATE_MESSAGE, 1, 0);
			}
			else if (allProfit <= -gConfig.targetLose) {
				PostMessage(m_dlg->GetSafeHwnd(), WM_UPDATE_MESSAGE, 2, 0);
			}
			else if (gConfig.winRestart && tempProfit >= gConfig.winValue) {
				PostMessage(m_dlg->GetSafeHwnd(), WM_UPDATE_MESSAGE, 3, 0);
			}
			else {
				PostMessage(m_dlg->GetSafeHwnd(), WM_UPDATE_MESSAGE, 4, 0);
			}
			return;
		}
	}
	float profit = gameData.tempProcer.realStat.profit;
	if ((gConfig.oneWinStop && profit >= gConfig.oneTargetWin) ||
		(gConfig.oneLoseStop && profit <= -gConfig.oneTargetLose)) {
		gameData.isStop = true;
		PostMessage(m_dlg->GetSafeHwnd(), WM_UPDATE_MESSAGE, 5, (LPARAM)&nRoomId);
	}
}


void GameProc::proc_roundOver(const rapidjson::Value& obj) {

	char tempBuf[TEMP_SIZE];

	gameData.cur_round.setAns(obj[0]["roadmap"].GetString());

	if (gameData.cur_round.type == REALBET_TYPE && gConfig.realMoney && !gameData.cur_round.isSmallBet) {
		if (obj[0].HasMember("credit")) {
			if (obj[0]["credit"].IsFloat())
				gameData.cur_round.userCredit = obj[0]["credit"].GetFloat();
			else
				gameData.cur_round.userCredit = obj[0]["credit"].GetInt();
		}
		if (obj[0].HasMember("payoff")) {
			if (obj[0]["payoff"].IsFloat())
				gameData.cur_round.payoff = obj[0]["payoff"].GetFloat();
			else
				gameData.cur_round.payoff = obj[0]["payoff"].GetInt();
		}
	}
	gameData.overRound();
	m_dlg->m_ans->draw_road(nRoomId, 0);
	if (gameData.cur_round.type == LOOK_TYPE) return;

	if (gameData.cur_round.type == REALBET_TYPE) {
		sprintf_s(tempBuf, TEMP_SIZE, "收益%.2f", gameData.cur_round.payoff);
		set_status(RESULT, tempBuf);
		gUserData->setCredit(gameData.cur_round.userCredit);
	}
	else {
		sprintf_s(tempBuf, TEMP_SIZE, "模拟收益 %.2f", gameData.cur_round.payoff);
		set_status(RESULT, tempBuf);
	}

	Round_t data = gameData.cur_round;
	gUserData->procer.setProfit(data, true);
	gUserData->tempProcer.setProfit(data, true);
	gameData.allProcer.setProfit(data);
	gameData.curProcer.setProfit(data);
	gameData.tempProcer.setProfit(data);
	m_dlg->m_ans->draw_profit(nRoomId);
	m_dlg->UpdateUserInfo();

	checkCanStop();
}

void GameProc::newRound() {
	gameData.newRound(gUserData->curPart());
	m_dlg->m_ans->draw_bet(nRoomId);
	m_dlg->m_ans->draw_profit(nRoomId);
}

int GameProc::proc_onBet(const rapidjson::Value& obj) {

	if (obj[0].HasMember("runEor"))
		set_status(BETERR);
	else {
		gameData.roundIsOver = false;
		gameData.cur_round.type = REALBET_TYPE;
		m_dlg->m_ans->draw_bet(nRoomId);
		set_status(BETED);

		if (obj[0].HasMember("credit")) {
			if (obj[0]["credit"].IsFloat())
				gUserData->setCredit(obj[0]["credit"].GetFloat());
			else
				gUserData->setCredit(obj[0]["credit"].GetInt());
		}
		BetData_t data;
		data.bet = gameData.cur_round.b1 + gameData.cur_round.b2 + gameData.cur_round.b3;
		data.gameId = nRoomId;
		data.type = REALBET_TYPE;
		data.isSmallBet = gameData.cur_round.isSmallBet;
		data.time = gameData.cur_round.time;
		gUserData->procer.setBet(data);
		gUserData->tempProcer.setBet(data);
		gameData.allProcer.setBet(data);
		gameData.curProcer.setBet(data);
		gameData.tempProcer.setBet(data);
		m_dlg->UpdateUserInfo();
	}		
	return 1;
}

std::vector<WebSocket*> gGamer;
