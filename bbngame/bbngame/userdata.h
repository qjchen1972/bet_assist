#pragma once
#include<string>
#include<vector>
#include <iostream>
#include <fstream>
#include<thread>
#include <mutex>
#include"setup.h"
#include<time.h>

class Tool {
public:
	Tool() {}
	~Tool() {}

	std::string getTime(uint64_t time) {
		time_t now = time;
		char stTmp[64];
		struct tm tmTmp;

		localtime_s(&tmTmp, &now);
		sprintf_s(stTmp, "%d-%02d-%02d %02d:%02d:%02d",
			1900 + tmTmp.tm_year,
			tmTmp.tm_mon + 1,
			tmTmp.tm_mday,
			tmTmp.tm_hour,
			tmTmp.tm_min,
			tmTmp.tm_sec);
		std::string str = stTmp;
		return str;
	}

	std::string getDate(uint64_t time) {
		time_t now = time;
		char stTmp[64];
		struct tm tmTmp;

		localtime_s(&tmTmp, &now);
		sprintf_s(stTmp, "%d年%02d月%02d日%02d时%02d分",
			1900 + tmTmp.tm_year,
			tmTmp.tm_mon + 1,
			tmTmp.tm_mday,
			tmTmp.tm_hour,
			tmTmp.tm_min);
		std::string str = stTmp;
		return str;
	}

	//黑桃s, 红桃h 方片d, 梅花c
	std::string cardChn(std::string str) {
		std::string ret = "";
		std::string sub = str.substr(2, str.size() - 2);
		if (str[0] == 'S')  ret += "黑桃" + sub;
		else if (str[0] == 'H') ret += "红桃" + sub;
		else if (str[0] == 'D') ret += "方块" + sub;
		else if (str[0] == 'C') ret += "梅花" + sub;
		else;
		return ret;
	}

	int  getLastRoad(std::string  strRoad, int &first, int &second) {

		std::vector<std::string> allans;
		if (my_split(strRoad, ',', allans) < 0) return 0;
		if (!splitnum(allans[allans.size() - 1], first, second)) return 0;
		first = ((first - 1 + 12) % 12) / 4; 
		second = second;
		return 1;
	}

	int getRoundNo(std::string  str, int &first, int &second) {
		if (!splitnum(str, first, second)) return 0;
		return 1;
	}

	int  splitnum(std::string str, int& first, int& second) {

		std::vector<std::string> vec;
		if (str.length() == 0) return 0;
		if (my_split(str, '-', vec) < 0) return 0;
		if (vec.size() < 2) return 0;
		if (vec[0].length() == 0 || vec[1].length() == 0) return 0;
		first = atoi(vec[0].c_str());
		second = atoi(vec[1].c_str());
		return 1;
	}
	int my_split(const std::string& src, const char& delim, std::vector<std::string>& vec) {
		int src_len = src.length();
		int find_cursor = 0;
		int read_cursor = 0;

		vec.clear();
		if (src_len <= 0) return -1;
		while (read_cursor < src_len) {

			find_cursor = src.find(delim, find_cursor);
			//1.找不到分隔符
			if (-1 == find_cursor) {
				//最后一个子串, src结尾没有分隔符
				if (read_cursor < src_len) {
					vec.push_back(src.substr(read_cursor, src_len - read_cursor));
					return 0;
				}
			}
			//2.有连续分隔符的情况
			else if (find_cursor == read_cursor) {
				//字符串开头为分隔符, 也按空子串处理, 如不需要可加上判断&&(read_cursor!=0)
				vec.push_back(std::string(""));
			}
			//3.找到分隔符
			else
				vec.push_back(src.substr(read_cursor, find_cursor - read_cursor));

			read_cursor = ++find_cursor;
			if (read_cursor == src_len) {
				//字符串以分隔符结尾, 如不需要末尾空子串, 直接return
				vec.push_back(std::string(""));
				return 0;
			}
		}//end while()
		return 0;
	}
};
 


#define LOOK_TYPE 0
#define REALBET_TYPE 1
#define FAKEBET_TYPE 2
#define INIT_TYPE 3


#define BANKER 0
#define PLAYER 1
#define MID  2
#define ERR 3


struct Round_t {
	uint64_t roundSerial = 0;
	int16_t game = 0;
	std::string roundNo = "";
	std::string cardStr = "";
	uint8_t  cardAns = 0;
	uint8_t  cardNum = 0;
	uint64_t time = 0;
	uint8_t  type = LOOK_TYPE;
	int32_t b1 = 0;
	int32_t b2 = 0;
	int32_t b3 = 0;
	float payoff = 0.0f;
	float userCredit = 0.0f;
	int8_t betId = -1;
	int8_t gameId = -1;
	float profit = 0;
	int  partId = 0;

	void setAns(std::string  strRoad) {
		int first, second;
		Tool tool;
		if (!tool.getLastRoad(strRoad, first, second)) return;
		cardAns = first;
		cardNum = second;
	}
	void getCard(std::string &bankerCard, std::string &playerCard) {
		Tool tool;
		std::vector<std::string> allans;
		tool.my_split(cardStr, ',', allans);
		bankerCard = "";
		playerCard = "";
		for (int m = 0; m < allans.size(); m++) {
			if (allans[m].size() == 0) continue;
			if (m % 2 == 0) {
				if (m > 0) playerCard += ",";
				playerCard += tool.cardChn(allans[m]);
			}
			else {
				if (m > 1) bankerCard += ",";
				bankerCard += tool.cardChn(allans[m]);
			}
		}
	}
};

struct BetData_t {
	uint64_t time=0;
	int8_t gameId=-1;
	float bet=0;
	uint8_t type=LOOK_TYPE;
};

struct Stat_t{
	float profit = 0;
	float bet = 0;
	int32_t betNum = 0;
	int32_t winNum = 0;
	int32_t loseNum = 0;
	float maxProfit = 0;
	float minProfit = 0;
	int32_t maxContinueWinNum = 0;
	int32_t maxContinueLoseNum = 0;
	float maxContinueWinProfit = 0;
	float maxContinueLoseProfit = 0;
	int32_t continueWinNum = 0;
	int32_t continueLoseNum = 0;
	float continueWinProfit = 0;
	float continueLoseProfit = 0;
	uint64_t startTime = 0;
	uint64_t endTime = 0;

	void setStartTime(uint64_t time) {
		if (startTime == 0) startTime = time;
	}

	void setProfit(uint64_t time, float payoff) {

		endTime = time;
		if (payoff == 0.0) return;
		profit += payoff;		
		if (payoff > 0.0) {
			winNum++;
			continueLoseNum = 0;
			continueLoseProfit = 0;
			continueWinNum++;
			continueWinProfit += payoff;
			if (maxContinueWinNum < continueWinNum)	maxContinueWinNum = continueWinNum;
			if (maxContinueWinProfit < continueWinProfit) maxContinueWinProfit = continueWinProfit;
		}
		else {
			loseNum++;
			continueWinNum = 0;
			continueWinProfit = 0;
			continueLoseNum++;
			continueLoseProfit += payoff;
			if (maxContinueLoseNum < continueLoseNum) maxContinueLoseNum = continueLoseNum;
			if (maxContinueLoseProfit > continueLoseProfit) maxContinueLoseProfit = continueLoseProfit;
		}


		if (maxProfit < profit) maxProfit = profit;
		if (minProfit > profit) minProfit = profit;
	}
	

	void setBet(uint64_t time,  float money) {
		endTime = time;
		bet += money;
		betNum++;
	}
};


struct Kline_t {
	uint64_t startTime = 0;
	uint64_t endTime = 0;
	float high = 0;
	float low = 0;
	float start = 0;
	float end = 0;
	int vol = 0;
	int win = 0;
	int lose = 0;
	int  partId = 0;
};

class StatProc{

public:

	void reset() {
		std::lock_guard<std::mutex> guard(mtx);
		Stat_t temp;
		allStat = temp;
		realStat = temp;
		profitSet.clear();
		kLineSet.clear();
	}

	void setStartTime(uint64_t time) {
		allStat.setStartTime(time);
		realStat.setStartTime(time);
	}

	void setProfit(Round_t &data, bool needKline=false, bool tempStart=false) {
		std::lock_guard<std::mutex> guard(mtx);
		if (data.type == LOOK_TYPE) return;
		allStat.setProfit(data.time, data.payoff);
		if (data.type == REALBET_TYPE)
			realStat.setProfit(data.time, data.payoff);
		data.profit = allStat.profit;
		profitSet.push_back(data);
		if (needKline) 	addKline(gConfig.profitStatNum);		
	}

	void setBet(BetData_t &data) {
		std::lock_guard<std::mutex> guard(mtx);
		if (data.type == LOOK_TYPE) return;
		allStat.setBet(data.time, data.bet);
		if (data.type == REALBET_TYPE) 
			realStat.setBet(data.time, data.bet);
	}

	int  getOneProfit(Round_t &round, uint8_t type) {
		std::lock_guard<std::mutex> guard(mtx);
		if (profitSet.size() == 0) return 0;
		for (int i = profitSet.size() - 1; i >= 0; i--) {
			if (profitSet[i].type == type) {
				round = profitSet[i];
				return 1;
			}
		}
		return 0;
	}

	void getProfitSet(std::vector<Round_t> &set) {
		std::lock_guard<std::mutex> guard(mtx);
		set = profitSet;
	}

	void getAllKline(std::vector<Kline_t> &set) {
		std::lock_guard<std::mutex> guard(mtx);
		set = kLineSet;
	}


	Stat_t allStat;
	Stat_t realStat;

private:
	std::mutex mtx;
	std::vector<Round_t> profitSet;
	std::vector<Kline_t> kLineSet;

	void addKline(int blknum) {
		if (profitSet.size() == 0) return;		
		int index = profitSet.size() - 1;
		Kline_t data;
		if (kLineSet.size() == 0 || 
			kLineSet[kLineSet.size() - 1].vol == blknum || 
			kLineSet[kLineSet.size() - 1].partId != profitSet[index].partId) {
			data.start = profitSet[index].profit;
			data.end = profitSet[index].profit;
			data.high = profitSet[index].profit;
			data.low = profitSet[index].profit;
			data.startTime = profitSet[index].time;
			data.vol = 1;
			data.partId = profitSet[index].partId;
			data.endTime = profitSet[index].time;
			if (profitSet[index].payoff > 0) data.win = 1;
			if (profitSet[index].payoff < 0) data.lose = 1;
			kLineSet.push_back(data);			
			return;
		}

		if (profitSet[index].profit > kLineSet[kLineSet.size() - 1].high) 
			kLineSet[kLineSet.size() - 1].high = profitSet[index].profit;
		if (profitSet[index].profit < kLineSet[kLineSet.size() - 1].low) 
			kLineSet[kLineSet.size() - 1].low = profitSet[index].profit;
		if (profitSet[index].payoff > 0) kLineSet[kLineSet.size() - 1].win++;
		if (profitSet[index].payoff < 0) kLineSet[kLineSet.size() - 1].lose++;
		kLineSet[kLineSet.size() - 1].vol++;
		kLineSet[kLineSet.size() - 1].end = profitSet[index].profit;
		kLineSet[kLineSet.size() - 1].endTime = profitSet[index].time;
	}

	/*void createOneKline(Kline_t &data, int start, int size) {

		data.start = profitSet[start].profit;
		data.end = profitSet[start].profit;
		data.high = profitSet[start].profit;
		data.low = profitSet[start].profit;
		data.startTime = profitSet[start].time;
		data.vol = 1;
		if (profitSet[start].payoff > 0) data.win = 1;
		if (profitSet[start].payoff < 0) data.lose = 1;
		int pos = start + size;
		if (pos > profitSet.size()) pos = profitSet.size();
		for (int i = start + 1; i < pos; i++) {
			if (profitSet[i].profit > data.high) data.high = profitSet[i].profit;
			if (profitSet[i].profit < data.low) data.low = profitSet[i].profit;
			if (profitSet[i].payoff > 0) data.win++;
			if (profitSet[i].payoff < 0) data.lose++;
			data.vol++;
		}
		data.end = profitSet[pos - 1].profit;
		data.endTime = profitSet[pos - 1].time;
	}*/

};




class UserStat {
public:
	UserStat() {}
	~UserStat() {}

	void init() {
		std::lock_guard<std::mutex> guard(mtx);
		isStop = false;
		procer.reset();
		tempProcer.reset();
		partStat.clear();
	}

	void initTemp() {
		std::lock_guard<std::mutex> guard(mtx);
		isStop = false;
		tempProcer.reset();
	}

	void addTemp() {
		std::lock_guard<std::mutex> guard(mtx);
		partStat.push_back(tempProcer.realStat);
	}

	int curPart() {
		std::lock_guard<std::mutex> guard(mtx);
		return partStat.size();
	}

	void setCredit(float credit) {
		std::lock_guard<std::mutex> guard(mtx);
		userCredit = credit;
	}

	float getCredit() {
		std::lock_guard<std::mutex> guard(mtx);
		return userCredit;
	}

	bool outOfCredit(int bet) {
		std::lock_guard<std::mutex> guard(mtx);
		return bet > userCredit;
	}

	void getPartStat(std::vector<Stat_t> &set) {
		std::lock_guard<std::mutex> guard(mtx);
		set = partStat;
	}

	bool isStop = false;
	StatProc procer;
	StatProc tempProcer;

	std::vector<Stat_t> partStat;
private:

	float userCredit = 0;
	std::mutex mtx;	
};





class GameData {

public:
	GameData() {}
	~GameData() {}	

	void init() {
		std::lock_guard<std::mutex> guard(mtx);
		isStop = false;
		allProcer.reset();
		curProcer.reset();
		tempProcer.reset();
	}

	void initTemp() {
		std::lock_guard<std::mutex> guard(mtx);
		isStop = false;
		curProcer.reset();
		tempProcer.reset();
	}

	void setInitRoad(std::string strRoad, std::string roundNo="") {
		std::lock_guard<std::mutex> guard(mtx);
		std::vector<Round_t>  road;
		int temp = getGame(roundNo);
		loadAllRoad(strRoad, road, roundNo);

		if(gameNum != temp) allRoad.push_back(road);
		else allRoad[allRoad.size() - 1] = road;
		gameNum = temp;
		roadToColRoad(road, curColRoad, gConfig.isOmitMid);
		roadToColRoad(allRoad, allColRoad, gConfig.isOmitMid);
	}

	void newGame(std::string roundNo) {
		std::lock_guard<std::mutex> guard(mtx);
		std::vector<Round_t>  road;
		allRoad.push_back(road);
		curColRoad.clear();
		curProcer.reset();
		gameNum = getGame(roundNo);
	}

	void newRound(int partId) {
		std::lock_guard<std::mutex> guard(mtx);
		Round_t round;
		cur_round = round;
		cur_round.game = gameNum;
		cur_round.time = time(NULL);
		cur_round.gameId = roomId;
		cur_round.partId = partId;
	}

	void overRound() {
		std::lock_guard<std::mutex> guard(mtx);
		allRoad[allRoad.size() - 1].push_back(cur_round);
		addColRoad(cur_round, curColRoad, gConfig.isOmitMid);
		addColRoad(cur_round, allColRoad, gConfig.isOmitMid);
		roundIsOver = true;
	}


	void getLastRounds(std::vector<Round_t> &road, int size ) {
		std::lock_guard<std::mutex> guard(mtx);
		int num = 0;
		road.clear();
		for (int i = allRoad.size() - 1; i >= 0; i--) 
			for (int j = allRoad[i].size() - 1; j >= 0; j--) {
				road.push_back(allRoad[i][j]);
					num++;
					if (num >= size) return;
			}	
	}
	
	/*int getLastRound(Round_t &round) {
		std::lock_guard<std::mutex> guard(mtx);
		if (allRoad.size() == 0) return 0;
		for (int i = allRoad.size() - 1; i >= 0; i--)
			for (int j = allRoad[i].size() - 1; j >= 0; j--) {
				round = allRoad[i][j];
				if (i == allRoad.size() - 1) return 1;
				else return 2;
			}			
	}*/

	void getNewRound(std::vector<Round_t> &road) {
		std::lock_guard<std::mutex> guard(mtx);
		road.clear();
		if (allRoad.size() == 0) return;
		int end = allRoad.size() - 1;
		for (int i = allRoad[end].size() - 1; i >= 0; i--) 
			road.push_back(allRoad[end][i]);		
	}

	void getAllRoad(std::vector<std::vector<Round_t>>  &road) {
		std::lock_guard<std::mutex> guard(mtx);
		road = allRoad;
	}

	void addColRoad(Round_t round, std::vector<std::vector<Round_t>> &colRoad, bool delMid = true) {

		if (colRoad.size() == 0 && round.cardAns == MID) return;
		if (delMid && round.cardAns == MID) return;
		std::vector<Round_t> temp;
		if (colRoad.size() == 0) {
			temp.push_back(round);
			colRoad.push_back(temp);
			return;
		}
		if (round.cardAns != MID && round.cardAns != colRoad[colRoad.size() - 1][0].cardAns) {
			temp.push_back(round);
			colRoad.push_back(temp);
			return;
		}
		colRoad[colRoad.size() - 1].push_back(round);
	}


	Round_t cur_round;
	std::vector<std::vector<Round_t>> curColRoad;
	std::vector<std::vector<Round_t>> allColRoad;


	StatProc allProcer;
	StatProc curProcer;
	StatProc tempProcer;

	bool isStop = false;
	bool roundIsOver = true;
	int gameNum = -1;
	int roomId = -1;

private:	

	std::mutex mtx;
	std::vector<std::vector<Round_t>>  allRoad;

	int getGame(std::string  str) {
		int first, second;
		Tool tool;
		if (!tool.getRoundNo(str, first, second)) return 0;
		return first;
	}

	void roadToColRoad(std::vector<Round_t> src, std::vector<std::vector<Round_t>> &colRoad, bool delMid=true) {
		colRoad.clear();
		uint8_t color = ERR;
		std::vector<Round_t> temp;
		for (int i = 0; i < src.size(); i++) {
			if (color == ERR && src[i].cardAns == MID) continue;
			if (delMid && src[i].cardAns == MID) continue;
			if (src[i].cardAns != color && src[i].cardAns != MID) {
				if (temp.size() > 0) {
					colRoad.push_back(temp);
					temp.clear();
				}
				color = src[i].cardAns;
				temp.push_back(src[i]);
			}
			else temp.push_back(src[i]);
		}
		if (temp.size() > 0) colRoad.push_back(temp);
	}

	void roadToColRoad(std::vector<std::vector<Round_t>> src, std::vector<std::vector<Round_t>> &colRoad, bool delMid = true) {
		colRoad.clear();
		uint8_t color = ERR;
		std::vector<Round_t> temp;
		for (int i = 0; i < src.size(); i++)
			for (int j = 0; j < src[i].size(); j++) {
				if (color == ERR && src[i][j].cardAns == MID) continue;
				if (delMid && src[i][j].cardAns == MID) continue;
				if (src[i][j].cardAns != color && src[i][j].cardAns != MID) {
					if (temp.size() > 0) {
						colRoad.push_back(temp);
						temp.clear();
					}
					color = src[i][j].cardAns;
					temp.push_back(src[i][j]);
				}
				else temp.push_back(src[i][j]);
			}
		if (temp.size() > 0) colRoad.push_back(temp);
	}
	

	/*void addColRoad(Round_t round, std::vector<std::vector<Round_t>> &colRoad, bool delMid=true) {
		
		if (colRoad.size() == 0 && round.cardAns == MID) return;
		if (delMid && round.cardAns == MID) return;
		std::vector<Round_t> temp;
		if (colRoad.size() == 0) {
			temp.push_back(round);
			colRoad.push_back(temp);
			return;
		}
		if (round.cardAns != MID && round.cardAns != colRoad[colRoad.size() - 1][0].cardAns) {
			temp.push_back(round);
			colRoad.push_back(temp);
			return;
		}
		colRoad[colRoad.size() - 1].push_back(round);
	}
	*/

	void loadAllRoad(std::string strRoad, std::vector<Round_t> &roadmap, std::string roundNo = "") {

		Tool tool;
		roadmap.clear();
		std::vector<std::string> allans;
		if (tool.my_split(strRoad, ',', allans) < 0) return;
		int first, second;
		int game = getGame(roundNo);
		for (int i = 0; i < allans.size(); i++) {
			if (!tool.splitnum(allans[i], first, second)) continue;
			Round_t  round;
			round.cardAns = ((first - 1 + 12) % 12) / 4;
			round.cardNum = second;
			round.type = INIT_TYPE;
			round.roundNo = roundNo;
			round.game = game;
			roadmap.push_back(round);
		}
	}
};


#define INIT  0
#define CONNECTING 1
#define CONNECTED 2
#define AMFISREADY 3
#define LOGINED 4
#define GETUSERINFO 5
#define GETTABLE 6
#define BETING 7
#define DEALING  8
#define RESULT  9
#define WAITING  10
#define NEWGAME 11
#define TRYBET 12
#define FAKEBET 13
#define BETED  14
#define BETERR 15

class GameStatus {
public:
	GameStatus(){}
	~GameStatus() {}

	void set(int Id, std::string str="") {
		std::lock_guard<std::mutex> guard(mtx);
		status = Id;
		if (str != "") strStatus = str;
		else strStatus = get_strstatus(Id);		
	}

	std::string get_strStatus() {
		std::lock_guard<std::mutex> guard(mtx);	
		return strStatus;
	}

	int str2status(std::string str) {
		if (str == "dealing") return DEALING;
		if (str == "waiting") return WAITING;
		if (str == "betting") return BETING;
		return INIT;
	}

	int status = INIT;
private:
	std::string strStatus;
	std::mutex mtx;

	

	std::string get_strstatus(int sta) {

		switch (sta) {
		case INIT:
			return "关闭";
		case CONNECTING:
			return "正在连接";
		case CONNECTED:
			return "连接成功";
		case AMFISREADY:
			return "登录服务器";
		case LOGINED:
			return "用户登录成功";
		case GETUSERINFO:
			return "得到用户信息";
		case GETTABLE:
			return "得到桌面信息";
		case BETING:
			return "可以下注";
		case DEALING:
			return "开牌中";
		case RESULT:
			return "得到结果";
		case WAITING:
			return "等待中";
		case NEWGAME:
			return "开始新局";
		case TRYBET:
			return "下注";
		case FAKEBET:
			return "模拟下注";
		case BETED:
			return "下注成功";
		case BETERR:
			return "下注失败";
		default:
			return "未知状态";
		}
	}
};


extern UserStat *gUserData;