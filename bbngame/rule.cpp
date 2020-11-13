#include "stdafx.h"
#include<vector>
//#include"ansDlg.h"
//#include"websocket.h"
//#include"gameDlg.h"
#include"rule.h"



/*void RuleConfig::SetInRule(int index) {
	inRuleIndex = index;
	std::vector<inRule_t> rules;
	readInRuleFile(inRuleFileList[index], rules);
	for (int i = 0; i < gGamer.size(); i++) {
		gGamer[i]->pGameProcer->ruler.inRule.setRule(rules);
	}
}
void RuleConfig::SetBetRule(int index) {
	betRuleIndex = index;
	std::vector<betRule_t> rules;
	readBetRuleFile(betRuleFileList[index], rules);
	for (int i = 0; i < gGamer.size(); i++) {
		gGamer[i]->pGameProcer->ruler.betRule.setRule(rules);
	}
}*/


RuleConfig gRuleConfig;