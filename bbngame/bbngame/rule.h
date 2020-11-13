#pragma once
#include <iostream>
#include <fstream>
#include<io.h>
#include<time.h>
#include"userdata.h"

struct inRule_t {
	int id=0;
	std::string limit="";
	std::string scene="";
	std::string action="";
};

#define BIGRULE 0   //单列大于
#define SMALLRULE 1 //单列小于
#define STARRULE 2 //*规则
#define MATCHRULE 3 //全列匹配
#define PARTMATCHRULE 4  // 末尾开始匹配
#define MAXSMALLRULE  5 // 前面的列最大长度小于
#define MAXBIGRULE  6 //前面的列最大长度大于
#define MINSMALLRULE  7 //前面的列最小长度小于
#define MINBIGRULE  8 //前面的列最小长度大于
#define NONERULE 9

struct procedInRule_t {
	uint8_t type;
	uint8_t cardAns;
	uint8_t number;
	std::string scene;
};


class InRule {
public:
	InRule() {}
	~InRule() {}

	void setRule(std::vector<inRule_t> rule) {
		std::lock_guard<std::mutex> guard(mtx);
		inRule = rule;
		procInRule();
	}	

	int matchGame(std::vector<std::vector<Round_t>> game_data, std::string &action) {
		std::lock_guard<std::mutex> guard(mtx);
		if (procedInRule.size() == 0) return false;
		for (int i = 0; i < procedInRule.size(); i++) {
			if (gameMatchOneRule(game_data, procedInRule[i], procedLimitRule[i])) {
				action = inRule[i].action;
				return 1;
			}
		}
		return 0;
	}

	std::vector<std::vector<procedInRule_t>> procedInRule;
	std::vector<std::vector<procedInRule_t>> procedLimitRule;

private:
	std::vector<inRule_t> inRule;

	int maxNum = 100;
	std::mutex mtx;
	
	void procInRule() {
		procedInRule.clear();
		procedLimitRule.clear();
		for (int i = 0; i < inRule.size(); i++) {
			std::vector<procedInRule_t> rule;
			procScene(inRule[i].scene, rule);
			procedInRule.push_back(rule);
			std::vector<procedInRule_t> limit;
			procLimit(inRule[i].limit, limit);
			procedLimitRule.push_back(limit);
		}
	}

	int roadMinSize(std::vector<std::vector<Round_t>> &data, uint8_t color, int start, int end) {
		int min = 1000;
		for (int i = start; i <= end; i++) {
			if (color != data[i][0].cardAns) continue;
			if (data[i].size() < min) min = data[i].size();
		}
		if (min == 1000) return -1;
		return min;
	}

	int roadMaxSize(std::vector<std::vector<Round_t>> &data, uint8_t color, int start, int end) {
		int max = 0;
		for (int i = start; i <= end; i++) {
			if (color != data[i][0].cardAns) continue;
			if (data[i].size() > max) max = data[i].size();
		}
		if (max == 0) return -1;
		return max;
	}

	// match one rule
	int gameMatchOneRule(
		std::vector<std::vector<Round_t>> game_data, 
		std::vector<procedInRule_t> scene,
		std::vector<procedInRule_t> limit) {
		
		int pos = 0;
		int max, min;
		while (pos < limit.size()) {
			if (limit[pos].type == MAXSMALLRULE) {
				max = roadMaxSize(game_data, limit[pos].cardAns, 0, game_data.size() - 2);
				if (max >= 0 && max >= limit[pos].number) return 0;
			}
			else if (limit[pos].type == MAXBIGRULE) {
				max = roadMaxSize(game_data, limit[pos].cardAns, 0, game_data.size() - 2);
				if (max >= 0 &&  max <= limit[pos].number) return 0;
			}
			else if (limit[pos].type == MINSMALLRULE) {
				min = roadMinSize(game_data, limit[pos].cardAns, 0, game_data.size() - 2);
				if (min >= 0 && min >= limit[pos].number) return 0;
			}
			else if (limit[pos].type == MINBIGRULE) {
				min = roadMinSize(game_data, limit[pos].cardAns, 0, game_data.size() - 2);
				if (min >= 0 && min <= limit[pos].number) return 0;
			}
			else;
			pos++;
		}				

		int game_size = game_data.size() - 1;
		int  scene_size = scene.size() - 1;
		if (game_size < scene_size) return 0;
		pos = 0;
		while (pos <= scene_size) {
			if (!matchOneCol(game_data[game_size - pos], scene[scene_size - pos]))
				return 0;
			pos++;
		}		
		return 1;
	}

	//用于逐个比对
	int matchColAll(std::vector<Round_t> gameCol, std::string scene, bool isPart = false) {
		if (gameCol.size() < scene.size()) return 0;
		if (!isPart)
			if (gameCol.size() != scene.size()) return 0;

		/*for (int i = scene.size() - 1; i >= 0; i--) {
			if (gameCol[i].cardAns == ERR) return 0;
			if (scene[i] != 'p' && scene[i] != 'b' && scene[i] != 'h') return 0;
			if (scene[i] == 'b' && gameCol[i].cardAns != BANKER) return 0;
			if (scene[i] == 'p' && gameCol[i].cardAns != PLAYER) return 0;
			if (scene[i] == 'h' && gameCol[i].cardAns != MID) return 0;
		}*/

		int gameSize = gameCol.size() - 1;
		int sceneSize = scene.size() - 1;
		for (int i = 0; i < scene.size(); i++) {
			if (gameCol[gameSize - i].cardAns == ERR) return 0;
			if (scene[sceneSize - i] != 'p' && scene[sceneSize - i] != 'b' && scene[sceneSize - i] != 'h') return 0;
			if (scene[sceneSize - i] == 'b' && gameCol[gameSize - i].cardAns != BANKER) return 0;
			if (scene[sceneSize - i] == 'p' && gameCol[gameSize - i].cardAns != PLAYER) return 0;
			if (scene[sceneSize - i] == 'h' && gameCol[gameSize - i].cardAns != MID) return 0;
		}
		return 1;
	}

	//match one col of rule
	int matchOneCol(std::vector<Round_t> gameCol, procedInRule_t ruleCol) {
		if (ruleCol.type == NONERULE) return 0;
		if (ruleCol.type == STARRULE) {
			if (gameCol[0].cardAns != ruleCol.cardAns) return 0;
			return 1;
		}
		if (ruleCol.type == BIGRULE) {
			if (gameCol[0].cardAns != ruleCol.cardAns) return 0;
			if (gameCol.size() <= ruleCol.number) return 0;
			return 1;
		}
		if (ruleCol.type == SMALLRULE) {
			if (gameCol[0].cardAns != ruleCol.cardAns) return 0;
			if (gameCol.size() >= ruleCol.number) return 0;
			return 1;
		}
		if (ruleCol.type == MATCHRULE) {
			if (gameCol[0].cardAns != ruleCol.cardAns) return 0;
			return matchColAll(gameCol, ruleCol.scene, false);
		}
		if (ruleCol.type == PARTMATCHRULE)
			return matchColAll(gameCol, ruleCol.scene, true);
		return 0;
	}

	//输入场景的单列转成规则
	void procSceneCol(std::string col, procedInRule_t &one, bool isFirst=false) {

		if (col.size() == 2 && col[1] == '*') {
			one.type = STARRULE;
			if (col[0] == 'b') one.cardAns = BANKER;
			else if(col[0] == 'p') one.cardAns = PLAYER;
			else one.type = NONERULE;
			return;
		}
		if (col.size() >= 3 && col[1] == '>' && isdigit(col[2])) {
			one.type = BIGRULE;
			int temp = col[2] - '0';
			for (int i = 3; i < col.size(); i++) {
				if (!isdigit(col[i])) break;
				temp = temp * 10 + col[i] - '0';
			}
			if (temp > maxNum) temp = maxNum;
			one.number = temp;
			if (col[0] == 'b') one.cardAns = BANKER;
			else if (col[0] == 'p') one.cardAns = PLAYER;
			else one.type = NONERULE;
			return;
		}
		if (col.size() >= 3 && col[1] == '<' && isdigit(col[2])) {
			one.type = SMALLRULE;
			int temp = col[2] - '0';
			for (int i = 3; i < col.size(); i++) {
				if (!isdigit(col[i])) break;
				temp = temp * 10 + col[i] - '0';
			}
			if (temp > maxNum) temp = maxNum;
			one.number = temp;
			if (col[0] == 'b') one.cardAns = BANKER;
			else if (col[0] == 'p') one.cardAns = PLAYER;
			else one.type = NONERULE;
			return;
		}
		if (col.size() >= 4 && col[1] == 'a' && col[2] == '<' && isdigit(col[3])) {
			one.type = MAXSMALLRULE;
			int temp = col[3] - '0';
			for (int i = 4; i < col.size(); i++) {
				if (!isdigit(col[i])) break;
				temp = temp * 10 + col[i] - '0';
			}
			if (temp > maxNum) temp = maxNum;
			one.number = temp;
			if (col[0] == 'b') one.cardAns = BANKER;
			else if (col[0] == 'p') one.cardAns = PLAYER;
			else one.type = NONERULE;
			return;
		}
		if (col.size() >= 4 && col[1] == 'a' && col[2] == '>' && isdigit(col[3])) {
			one.type = MAXBIGRULE;
			int temp = col[3] - '0';
			for (int i = 4; i < col.size(); i++) {
				if (!isdigit(col[i])) break;
				temp = temp * 10 + col[i] - '0';
			}
			if (temp > maxNum) temp = maxNum;
			one.number = temp;
			if (col[0] == 'b') one.cardAns = BANKER;
			else if (col[0] == 'p') one.cardAns = PLAYER;
			else one.type = NONERULE;
			return;
		}
		if (col.size() >= 4 && col[1] == 'i' && col[2] == '<' && isdigit(col[3])) {
			one.type = MINSMALLRULE;
			int temp = col[3] - '0';
			for (int i = 4; i < col.size(); i++) {
				if (!isdigit(col[i])) break;
				temp = temp * 10 + col[i] - '0';
			}
			if (temp > maxNum) temp = maxNum;
			one.number = temp;
			if (col[0] == 'b') one.cardAns = BANKER;
			else if (col[0] == 'p') one.cardAns = PLAYER;
			else one.type = NONERULE;
			return;
		}
		if (col.size() >= 4 && col[1] == 'i' && col[2] == '>' && isdigit(col[3])) {
			one.type = MINBIGRULE;
			int temp = col[3] - '0';
			for (int i = 4; i < col.size(); i++) {
				if (!isdigit(col[i])) break;
				temp = temp * 10 + col[i] - '0';
			}
			if (temp > maxNum) temp = maxNum;
			one.number = temp;
			if (col[0] == 'b') one.cardAns = BANKER;
			else if (col[0] == 'p') one.cardAns = PLAYER;
			else one.type = NONERULE;
			return;
		}
		if (!isFirst) {
			one.type = MATCHRULE;
			if (col[0] == 'b') one.cardAns = BANKER;
			else if (col[0] == 'p') one.cardAns = PLAYER;
			else one.type = NONERULE;
			one.scene = col;
			return;
		}
		else {
			one.type = PARTMATCHRULE;
			one.scene = col;
			return;
		}
		one.type = NONERULE;
		return;
	}

	//把设置场景变成规则
	void procScene(std::string scene, std::vector<procedInRule_t> &procedScene) {

		procedScene.clear();
		char color;
		std::string  temp;
		for (int i = 0; i < scene.size(); i++) {
			if (i == 0) {
				color = scene[i];
				temp.push_back(scene[i]);
				continue;
			}
			if (color != 'b' && color != 'p') {
				if (scene[i] != 'b' && scene[i] != 'p') 
					temp.push_back(scene[i]);
				else {
					if (temp.size() > 0) {
						procedInRule_t one;
						if (procedScene.size() == 0)
							procSceneCol(temp, one, true);
						else
							procSceneCol(temp, one);
						procedScene.push_back(one);
						temp.clear();
					}
					color = scene[i];
					temp.push_back(scene[i]);
				}
				continue;
			}

			if (scene[i] == color || scene[i] == 'h' || scene[i] == '*' || 
				isdigit(scene[i]) || scene[i] == '>' || scene[i] == '<')
				temp.push_back(scene[i]);
			else {
				if (temp.size() > 0) {
					procedInRule_t one;
					if (procedScene.size() == 0)
						procSceneCol(temp, one, true);
					else
						procSceneCol(temp, one);
					procedScene.push_back(one);
					temp.clear();
				}
				color = scene[i];
				temp.push_back(scene[i]);
			}
		}
		if (temp.size() > 0) {
			procedInRule_t one;
			if (procedScene.size() == 0)
				procSceneCol(temp, one, true);
			else
				procSceneCol(temp, one);
			procedScene.push_back(one);
		}
	}

	//把设置限制条件变成规则
	void procLimit(std::string scene, std::vector<procedInRule_t> &procedScene) {

		procedScene.clear();
		if (scene.size() == 0) return;
		char color = scene[0];
		if (color != 'b' && color != 'p') return;

		std::string  temp;
		temp.push_back(scene[0]);
		for (int i = 1; i < scene.size(); i++) {
			if (scene[i] == 'a' || scene[i] == 'i' || isdigit(scene[i]) || scene[i] == '>' || scene[i] == '<')
				temp.push_back(scene[i]);
			else if (scene[i] != 'b' && scene[i] != 'p')
				return;
			else {
				if (temp.size() > 0) {
					procedInRule_t one;
					procSceneCol(temp, one);
					procedScene.push_back(one);
					temp.clear();
				}
				temp.push_back(scene[i]);
			}
		}
		if (temp.size() > 0) {
			procedInRule_t one;
			procSceneCol(temp, one);
			procedScene.push_back(one);
		}
	}
};


struct betRule_t {
	int id;
	int money;
	int winId;
	int loseId;
	int midId;
};


class BetRule {
public:
	BetRule() {}
	~BetRule() {}

	void setRule(std::vector<betRule_t> rule) {
		std::lock_guard<std::mutex> guard(mtx);
		betRule = rule;
	}

	int getBetMoney(uint8_t betType, GameData *data, int8_t &id) {
		std::lock_guard<std::mutex> guard(mtx);

		Round_t round;
		id = 0;
		int ret;
		if (gConfig.useNew)
			ret = data->curProcer.getOneProfit(round, betType);
		else
			ret = data->allProcer.getOneProfit(round, betType);

		if (ret > 0 ){
			if (round.payoff > 0) id = betRule[round.betId].winId;
			else if (round.payoff < 0) id = betRule[round.betId].loseId;
			else id = betRule[round.betId].midId;			
		}
		return betRule[id].money;
	}	

	int getBetMoney(uint8_t betType, StatProc &procer, int8_t &id) {
		std::lock_guard<std::mutex> guard(mtx);

		Round_t round;
		id = 0;
		int ret = procer.getOneProfit(round, betType);
	
		if (ret > 0) {
			if (round.payoff > 0) id = betRule[round.betId].winId;
			else if (round.payoff < 0) id = betRule[round.betId].loseId;
			else id = betRule[round.betId].midId;
		}
		return betRule[id].money;
	}

private:
	std::mutex mtx;
	std::vector<betRule_t> betRule;	
};

/*class CGameDlg;

class Rule {
public:
	InRule inRule;
	BetRule betRule;

	int getBetMoney(CGameDlg *dlg, UserStat *user, GameData *data, uint8_t &betType, int &b1, int &b2, int &b3, int8_t &betId) {
		std::string action;

		if (gConfig.useNew) {
			if (!inRule.matchGame(data->curColRoad, action)) return 0;
		}
		else {
			if (!inRule.matchGame(data->allColRoad, action)) return 0;
		}
		betType = getGameMode(user, data);

		int money = int(betRule.getBetMoney(betType, data, betId) * gConfig.moneyRate);
		if (money < 10) return 0;		
		b1 = b2 = b3 = 0;
		if (action == "b") b1 = money;
		if (action == "p") b2 = money;
		if (action == "h") b3 = money;
		return 1;
	}	

	float getAvg(std::vector<Kline_t> data, int size) {
		float total = 0;
		int num = 0;
		for (int i = data.size() -1; i >= 0; i--) {
			total += data[i].end;
			num++;
			if (num == size) break;
		}
		total = total / num;
		return total;
	}	

	uint8_t getGameMode(UserStat *user, GameData *data) {
		if (!gConfig.avgLowIsFake) return REALBET_TYPE;

		std::vector<Kline_t> klineSet;
		user->tempProcer.getAllKline(klineSet);

		if (klineSet.size() == 0) return REALBET_TYPE;
		float avg = getAvg(klineSet, gConfig.avgNum);
		if (klineSet[klineSet.size() - 1].end >= avg) return  REALBET_TYPE;
		return FAKEBET_TYPE;
	}	
};

*/

class RuleConfig{

public:
	RuleConfig() {}
	~RuleConfig() {}

	/*int getBetMoney(UserStat *user, GameData *data, uint8_t &betType, int &b1, int &b2, int &b3, int8_t &betId) {
		std::string action;

		if (gConfig.useNew) {
			if (!inRule.matchGame(data->curColRoad, action)) return 0;
		}
		else {
			if (!inRule.matchGame(data->allColRoad, action)) return 0;
		}
		betType = getGameMode(user->tempProcer);

		int money = int(betRule.getBetMoney(betType, data, betId) * gConfig.moneyRate);
		if (money < 10) return 0;
		b1 = b2 = b3 = 0;
		if (action == "b") b1 = money;
		if (action == "p") b2 = money;
		if (action == "h") b3 = money;
		return 1;
	}*/

	int getBetMoney(UserStat *user, GameData *data, uint8_t &betType, int &b1, int &b2, int &b3, int8_t &betId) {
	
		std::vector<std::vector<Round_t>> *colRoad;
		StatProc *gameProcer;

		if (gConfig.useNew) {
			colRoad = &(data->curColRoad);
			gameProcer = &(data->curProcer);
		}
		else {
			colRoad = &data->allColRoad;
			gameProcer = &(data->allProcer);
		}

		return betMoney(*colRoad, user->tempProcer, *gameProcer, gConfig.avgLowIsFake,
			betType, b1, b2, b3, betId);
	}



	float getAvg(std::vector<Kline_t> data, int size) {
		float total = 0;
		int num = 0;
		for (int i = data.size() - 1; i >= 0; i--) {
			total += data[i].end;
			num++;
			if (num == size) break;
		}
		total = total / num;
		return total;
	}

	int betMoney(std::vector<std::vector<Round_t>> &colRoad, 
		            StatProc &allProcer, 
		            StatProc &gameProcer,
		            bool useAvg,
		            uint8_t &betType, int &b1, int &b2, int &b3, int8_t &betId) {

		std::string action;

		if (!inRule.matchGame(colRoad, action)) return 0;
		
		betType = getGameMode(allProcer, useAvg);

		int money = int(betRule.getBetMoney(betType, gameProcer, betId) * gConfig.moneyRate);
		if (money < 10) return 0;
		b1 = b2 = b3 = 0;
		if (action == "b") b1 = money;
		if (action == "p") b2 = money;
		if (action == "h") b3 = money;
		return 1;
	}

	uint8_t getGameMode(StatProc &procer, bool useAvg) {
		//if (!gConfig.avgLowIsFake) return REALBET_TYPE;
		if(!useAvg) return REALBET_TYPE;
		std::vector<Kline_t> klineSet;
		procer.getAllKline(klineSet);

		if (klineSet.size() == 0) return REALBET_TYPE;
		float avg = getAvg(klineSet, gConfig.avgNum);
		if (klineSet[klineSet.size() - 1].end >= avg) return  REALBET_TYPE;
		return FAKEBET_TYPE;
	}

	float betAns(int result, int b1, int b2, int b3) {

		float payoff = 0;

		switch (result) {
		case BANKER:
			payoff += b1 * 0.95;
			payoff -= b2;
			payoff -= b3;
			break;
		case PLAYER:
			payoff -= b1;
			payoff += b2;
			payoff -= b3;
			break;
		case MID:
			payoff += 7 * b3;
			break;
		default:
			break;
		}
		return payoff;
	}	
	

	void SetInRule(int index) {
		inRuleIndex = index;
		std::vector<inRule_t> rules;
		readInRuleFile(inRuleFileList[index], rules);
		inRule.setRule(rules);
	}

	void SetBetRule(int index) {
		betRuleIndex = index;
		std::vector<betRule_t> rules;
		readBetRuleFile(betRuleFileList[index], rules);
		betRule.setRule(rules);
	}

	void init() {
		getFileList(inRuleFilePath, inRuleFileList);
		getFileList(betRuleFilePath, betRuleFileList);
	}

	void loadInRuleFileList() {
		getFileList(inRuleFilePath, inRuleFileList);
	}

	void loadBetRuleFileList() {
		getFileList(betRuleFilePath, betRuleFileList);
	}


	void delInRuleFile(std::string name) {
		std::string filepath = inRuleFilePath + "/" + name + ".txt";
		remove(filepath.c_str());
	}

	void delBetRuleFile(std::string name) {
		std::string filepath = betRuleFilePath + "/" + name + ".txt";
		remove(filepath.c_str());
	}


	int readInRuleFile(std::string name, std::vector<inRule_t> &rule) {
		std::ifstream cf;
		std::string filepath = inRuleFilePath + "/" + name + ".txt";

		cf.open(filepath, std::ios::in);
		if (!cf.is_open()) return 0;
		rule.clear();
		Tool tool;
		while (!cf.eof()) {
			inRule_t one;
			cf >> one.id >> one.scene >> one.action;
			if (cf.fail()) break;
			std::vector<std::string> substr;
			tool.my_split(one.scene, ',', substr);
			if (substr.size() >= 2) {
				one.limit = substr[0];
				one.scene = substr[1];
			}
			rule.push_back(one);
		}
		cf.close();
		if (rule.size() == 0) return 0;
		return 1;
	}

	int writeInRuleFile(std::string name, std::vector<inRule_t> &rule) {
		std::ofstream f;
		std::string filepath = inRuleFilePath + "/" + name + ".txt";
		f.open(filepath, std::ios::out | std::ios::trunc);
		if (!f.is_open())  return 0;
		for (int i = 0; i < rule.size(); i++) {
			f << rule[i].id << " " << rule[i].scene << " " << rule[i].action << std::endl;
		}
		f.close();
	}

	int readBetRuleFile(std::string name, std::vector<betRule_t> &rule) {
		std::ifstream cf;
		std::string filepath = betRuleFilePath + "/" + name + ".txt";

		cf.open(filepath, std::ios::in);
		if (!cf.is_open()) return 0;
		rule.clear();
		while (!cf.eof()) {
			betRule_t one;
			cf >> one.id >> one.money >> one.winId >> one.loseId >>one.midId;
			if (cf.fail()) break;
			rule.push_back(one);
		}
		cf.close();
		if (rule.size() == 0) return 0;
		return 1;
	}
	int writeBetRuleFile(std::string name, std::vector<betRule_t> &rule) {
		std::ofstream f;
		std::string filepath = betRuleFilePath + "/" + name + ".txt";
		f.open(filepath, std::ios::out | std::ios::trunc);
		if (!f.is_open())  return 0;
		for (int i = 0; i < rule.size(); i++) {
			f << rule[i].id << " " << rule[i].money << " "
				<< rule[i].winId << " " << rule[i].loseId << " "
				<< rule[i].midId << std::endl;
		}
		f.close();
	}	

	std::vector<std::string> inRuleFileList;	
	std::vector<std::string> betRuleFileList;

	int inRuleIndex = 0;
	int betRuleIndex = 0;
	InRule inRule;
	BetRule betRule;

private:
	std::string  inRuleFilePath = "rule";
	std::string betRuleFilePath = "bet";
	std::mutex mtx;

	int  getFileList(std::string path, std::vector<std::string>&files) {

		_finddata_t f;
		intptr_t handle;
		files.clear();
		path += "/*.txt";
		handle = _findfirst(path.c_str(), &f);
		if (handle == -1L) return 0;
		do {

			if (f.attrib & _A_SUBDIR) continue;
			std::string temp = f.name;
			temp = temp.substr(0, temp.size() - 4);
			files.push_back(temp);
		} while (_findnext(handle, &f) == 0);
		_findclose(handle);
		return 1;
	}
};

extern RuleConfig gRuleConfig;