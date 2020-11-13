#include "stdafx.h"
#include"rule.h"
#include"setup.h"

int CSetup::readConfig() {
	std::ifstream cf;

	cf.open(config_path, std::ios::in);
	if (!cf.is_open()) return 0;
	while (!cf.eof()) {
		std::string  key;
		std::string value;
		cf >> key >> value;
		if (cf.fail()) break;
		if (key == "realMoney") realMoney = atoi(value.c_str());
		else if (key == "isOmitMid")  isOmitMid = atoi(value.c_str());
		else if (key == "useNew")  useNew = atoi(value.c_str());
		else if (key == "moneyRate")  moneyRate = atof(value.c_str());
		else if (key == "backRate")  backRate = atof(value.c_str());
		else if (key == "targetProfit")  targetProfit = atof(value.c_str());
		else if (key == "targetLose")  targetLose = atof(value.c_str());
		else if (key == "lossRestart")  loseRestart = atoi(value.c_str());
		else if (key == "lossValue")  loseValue = atof(value.c_str());
		else if (key == "lossTime")  loseTime = atoi(value.c_str());
		else if (key == "winRestart")  winRestart = atoi(value.c_str());
		else if (key == "winValue")  winValue = atof(value.c_str());
		else if (key == "winTime")  winTime = atoi(value.c_str());
		else if (key == "targetLoss")  targetLose = atof(value.c_str());
		else if (key == "oneWinStop")  oneWinStop = atoi(value.c_str());
		else if (key == "oneTargetWin")  oneTargetWin = atof(value.c_str());
		else if (key == "oneLossStop")  oneLoseStop = atoi(value.c_str());
		else if (key == "oneTargetLoss")  oneTargetLose = atof(value.c_str());
		else if (key == "avgLowIsFake")  avgLowIsFake = atoi(value.c_str());
		else if (key == "inRule")  gRuleConfig.inRuleIndex = atoi(value.c_str());
		else if (key == "betRule")  gRuleConfig.betRuleIndex = atoi(value.c_str());
		else if (key == "username")  userName = value;
		else;

	}
	cf.close();
	return 1;
}

int CSetup::writeConfig() {
	std::ofstream f;
	f.open(config_path, std::ios::out | std::ios::trunc);
	if (!f.is_open())  return 0;
	f << "realMoney" << " " << (int)realMoney << std::endl;
	f << "isOmitMid" << " " << (int)isOmitMid << std::endl;
	f << "moneyRate" << " " << moneyRate << std::endl;
	f << "backRate" << " " << backRate << std::endl;
	f << "targetProfit" << " " << targetProfit << std::endl;
	f << "targetLose" << " " << targetLose << std::endl;
	f << "lossRestart" << " " << (int)loseRestart << std::endl;
	f << "lossValue" << " " << loseValue << std::endl;
	f << "lossTime" << " " << loseTime << std::endl;
	f << "winRestart" << " " << (int)winRestart << std::endl;
	f << "winValue" << " " << winValue << std::endl;
	f << "winTime" << " " << winTime << std::endl;
	f << "oneWinStop" << " " << (int)oneWinStop << std::endl;
	f << "oneTargetWin" << " " << oneTargetWin << std::endl;
	f << "oneLossStop" << " " << (int)oneLoseStop << std::endl;
	f << "oneTargetLoss" << " " << oneTargetLose << std::endl;
	f << "avgLowIsFake" << " " << (int)avgLowIsFake << std::endl;
	f << "inRule" << " " << gRuleConfig.inRuleIndex << std::endl;
	f << "betRule" << " " << gRuleConfig.betRuleIndex << std::endl;
	f << "username" << " " << userName << std::endl;
	f.close();
}

CSetup gConfig;