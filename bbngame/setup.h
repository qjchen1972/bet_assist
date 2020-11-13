#pragma once
#include<string>
#include<vector>
#include <iostream>
#include <fstream>
#include<thread>


struct gamesite_t {
	int32_t roomid;
	std::string name;
	std::string dns;
	uint16_t port;
	std::string path;
	uint16_t  gameType;
	uint16_t gameCode;
};

#define REAL_MODE 0
#define AVG_MODE 1
#define MORE_MODE 2



class CSetup {
public:

	int getNetConfig() {
		std::ifstream cf;
		cf.open(netconfig_path, std::ios::in);
		if (!cf.is_open()) return 0;
		while (!cf.eof()) {
			std::string  key;
			std::string value;
			cf >> key >> value;
			if (cf.fail()) break;
			if (key == "dns") dns = value;
			else if (key == "port")  port = atoi(value.c_str());
			else;
		}
		return 1;
	}	

	int get_gamesite() {

		std::ifstream cf;
		cf.open(gamesite_path, std::ios::in);
		if (!cf.is_open()) return 0;
		gameSite.clear();
		while (!cf.eof()) {
			gamesite_t web;
			cf >> web.name >> web.dns >> web.port >>
				web.path >> web.gameType >> web.gameCode >> web.roomid;
			if (cf.fail()) break;
			gameSite.push_back(web);
		}
		if (gameSite.size() == 0) return 0;
		return 1;
	}

	int readConfig();
	int writeConfig();

	std::vector<gamesite_t> gameSite;
	std::string sid = "";

	int profitStatNum = 5;
	int avgNum = 5;
	

	bool realMoney = false; //真钱
	bool isOmitMid = true; //和去掉
	bool useNew = true; //使用新局来判定
	float moneyRate = 1.0;	
	float backRate = 0.0;

	float targetProfit = 500;
	float targetLose = 500;

	bool loseRestart = false;
	float loseValue = 100;
	int loseTime = 10;

	bool winRestart = false;
	float winValue = 100;
	int winTime = 10;

	bool oneWinStop = false;
	float oneTargetWin = 0;
	bool oneLoseStop = false;
	float oneTargetLose = 0;

	bool avgLowIsFake = false;
	bool avgRate = false;
	bool isStop = true;

	uint64_t stopTime = 0;
	int needStart = 0;

	std::string dns = "";
	int port = 443;

	std::string ver = "V0.131";
	uint64_t validDate = 0;
	std::string  userName = "";
	std::string loginId = "0";
	int userType = 0;

private:
	std::string  gamesite_path = "conf/gamesite.txt";
	std::string  config_path = "conf/config.txt";
	std::string  netconfig_path = "conf/netconfig.txt";

};

extern CSetup gConfig;