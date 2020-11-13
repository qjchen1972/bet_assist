#pragma once
#include<string>
#include <mutex>
#include <atomic>
#include<vector>
#include <iostream>
#include <fstream>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "userdata.h"
#include "setup.h"
#include "rule.h"
#include"notify.h"
#include"myssl.h"

#pragma pack(push, 8)
struct packet_head_t {
	uint8_t  type = 0x81;
	uint8_t  length = 0x0;

	inline uint8_t get_fin() { return type >> 7; }
	inline void set_fin(uint8_t fin) { type = (type & 0x7f) | (fin << 7); }
	inline uint8_t get_opcode() { return type & 0x0f; }
	inline void set_opcode(uint8_t opcode) { type = (type & 0xf0) | (opcode & 0x0f); }
	inline uint8_t get_mask() { return length >> 7; }
	inline void set_mask(uint8_t mask) { length = (length & 0x7f) | (mask << 7); }
	inline uint8_t get_length() { return length & 0x7f; }
	inline void set_length(uint8_t len) { length = (length & 0x80) | (len & 0x7f); }
};
#pragma pack(pop)

class Mybuf {

public:
	Mybuf(int len){
		buffer = new char[len];
		size = len;
		clear();
	}

	~Mybuf() {
		if(buffer) delete[] buffer;
	}

	void clear() {
		begin = end = 0;
	}

	int get_onepkg(char* buf, int& retlen) {
		std::lock_guard<std::mutex> guard(mtx);

		packet_head_t head;

		if (begin > end - sizeof(head)) return 0;
		int pos = begin;
		memcpy(&head, buffer + pos, sizeof(head));
		pos += sizeof(head);

		int temp = head.get_fin();
		temp = head.get_opcode();
		temp = head.get_mask();
		uint16_t length = head.get_length();

		if (head.get_length() == 126) {
			if (pos > end - sizeof(uint16_t)) return 0;
			memcpy(&length, buffer + pos, sizeof(uint16_t));
			length = ntohs(length);
			pos += sizeof(uint16_t);
		}
		if (pos > end - length) return 0;
		memcpy(buf, buffer + pos, length);
		pos += length;
		begin = pos;
		retlen = length;
		return 1;
	}

	int put_buf(char* buf, int len) {
		std::lock_guard<std::mutex> guard(mtx);
		if (begin >= size / 2) _reset();
		if (size - end < len) return -1;
		memcpy(buffer + end, buf, len);
		end += len;
		return 1;
	}	

private:
	int size;
	char *buffer = nullptr;
	int begin = 0;
	int end = 0;
	std::mutex mtx;

	void _reset() {
		if (end != begin) 	memmove(buffer, buffer + begin, end - begin);
		end -= begin;
		begin = 0;
	}
};



#define TEMP_SIZE 5012


class CGameDlg;
class WebSocket;

class GameProc {
public:
	GameProc(WebSocket*web, uint16_t gameType, uint16_t gameCode, int roomId) {
		conner = web;
		nGameType = gameType;
		nGameCode = gameCode;
		nRoomId = roomId;
		gameData.roomId = roomId;
	}

	~GameProc() {
	}

	int proc_buffer(char *recvBuf) {

		rapidjson::Document dom;
		if (dom.Parse(recvBuf).HasParseError()) return 1;
		if (!dom.HasMember("NetStatusEvent") || !dom["NetStatusEvent"].IsString())
			return 1;

		std::string strEvent = dom["NetStatusEvent"].GetString();
		if (strEvent == "NetConnection.Connect.Closed")
			return -1;
		if (strEvent == "Connected.amfIsReady")
			return proc_amfIsReady();
		if (strEvent == "Data") {
			if (!dom.HasMember("args") || !dom["args"].IsArray())
				return 1;
			const rapidjson::Value& obj = dom["args"];
			if (!obj[0].IsObject()) return 1;
			if (!obj[0].HasMember("action") || !obj[0]["action"].IsString())
				return 1;
			if (!obj[0].HasMember("action") || !obj[0]["action"].IsString())
				return 1;

			std::string strAction = obj[0]["action"].GetString();
			if (strAction == "onLogin")
				return proc_onLogin(obj);
			if (strAction == "onUserInfo")
				return proc_onUserInfo(obj);
			if (strAction == "onTableList")
				return proc_onTableListn();
			if (strAction == "onUpdate")
				return proc_onUpdate(obj);
			if (strAction == "onBet")
				return proc_onBet(obj);
			return 1;
		}
		return 1;
	}
	
	
	void set_status(int ID, std::string str = "");

	GameStatus gameStatus;
	GameData gameData;
	//Rule ruler;
	CGameDlg *m_dlg = nullptr;

private:
	uint16_t nGameType;
	uint16_t nGameCode;
	WebSocket* conner;
	int nRoomId;

	
	void checkCanStop();
	bool canStart();

	int proc_amfIsReady();
	int proc_onLogin(const rapidjson::Value& obj);
	int proc_onUserInfo(const rapidjson::Value& obj);
	int proc_onTableListn();
	void initRoad(std::string roadStr, std::string roundNo);
	void proc_bet();
	void proc_fakeProfit(float credit, int result);
	void proc_newGame(std::string str);
	void proc_roundOver(const rapidjson::Value& obj);
	void newRound();
	int proc_onBet(const rapidjson::Value& obj);

	int requestBet(uint64_t time, int bs, int b1, int b2, int b3);


	int proc_onUpdate(const rapidjson::Value& obj ) {


		if (obj[0].HasMember("srcDown")) {
			if (gameStatus.status != GETTABLE) return 1;

			if (obj[0].HasMember("roundNo") && obj[0]["roundNo"].IsString()) {
				proc_newGame(obj[0]["roundNo"].GetString());
			}

			newRound();
			if (obj[0].HasMember("roundSerial") && obj[0]["roundSerial"].IsInt64())
				gameData.cur_round.roundSerial = obj[0]["roundSerial"].GetInt64();
			if (obj[0].HasMember("roundNo") && obj[0]["roundNo"].IsString()) 
				gameData.cur_round.roundNo = obj[0]["roundNo"].GetString();
			if (obj[0].HasMember("status") && obj[0]["status"].IsString())
				set_status(gameStatus.str2status(obj[0]["status"].GetString()));
			if (obj[0].HasMember("roadmap") && obj[0]["roadmap"].IsString())
				initRoad(obj[0]["roadmap"].GetString(), gameData.cur_round.roundNo);
			if (gameStatus.status == BETING ) {
				//可以下注
			}
			return 1;
		}

		if (gameStatus.status == GETTABLE) return -1;

		if (obj[0].HasMember("countDown") && obj[0].HasMember("roundSerial")) {

			if (obj[0].HasMember("roundNo") && obj[0]["roundNo"].IsString()) {
				proc_newGame(obj[0]["roundNo"].GetString());
			}
			newRound();
			if (obj[0]["roundSerial"].IsInt64())
				gameData.cur_round.roundSerial = obj[0]["roundSerial"].GetInt64();
			if (obj[0].HasMember("roundNo") && obj[0]["roundNo"].IsString()) 
				gameData.cur_round.roundNo = obj[0]["roundNo"].GetString();
			if (obj[0].HasMember("status") && obj[0]["status"].IsString())
				set_status(gameStatus.str2status(obj[0]["status"].GetString()));
			if (gameStatus.status == BETING) {
				if(!gConfig.isStop) proc_bet();
				if (obj[0]["countDown"].IsString() && gameData.cur_round.type == LOOK_TYPE)
					set_status(BETING, obj[0]["countDown"].GetString());
			}
			return 1;
		}	
		

		if (obj[0].HasMember("result") && obj[0]["result"].IsInt()) {
			//int money = gameData.cur_round.b1 + gameData.cur_round.b2 + gameData.cur_round.b3;
			if (gameData.cur_round.type == FAKEBET_TYPE || !gConfig.realMoney || gameData.cur_round.isSmallBet) {
				if (obj[0].HasMember("credit")) {
					if (obj[0]["credit"].IsFloat())
						proc_fakeProfit(obj[0]["credit"].GetFloat(), obj[0]["result"].GetInt());
					else
						proc_fakeProfit(obj[0]["credit"].GetInt(), obj[0]["result"].GetInt());
				}
			}
			return 1;
		}

		if (obj[0].HasMember("roadmap") && obj[0].HasMember("roundNo")) {
			proc_newGame(obj[0]["roundNo"].GetString());
			return 1;
		}

		if (obj[0].HasMember("roadmap") && obj[0]["roadmap"].IsString()) {
			proc_roundOver(obj);
			return 1;
		}

		if (obj[0].HasMember("status") && obj[0]["status"].IsString()) {
			set_status(gameStatus.str2status(obj[0]["status"].GetString()));
			return 1;
		}

		if (obj[0].HasMember("poker") && obj[0]["poker"].IsString()) {
			gameData.cur_round.cardStr = obj[0]["poker"].GetString();
			return 1;
		}

		if (obj[0].HasMember("countDown") && obj[0]["countDown"].IsString() &&
			gameData.cur_round.type == LOOK_TYPE) {
			if (obj[0]["countDown"].IsString() && gameData.cur_round.type == LOOK_TYPE)
				set_status(BETING, obj[0]["countDown"].GetString());
			return 1;
		}

		return 1;
	}
	
	void bufToSendStr(char *src, char* dst, int &dstlen) {
		int len = strlen(src);
		packet_head_t head;
		dstlen = 0;
		if (len < 126) {
			head.set_length(len);
			memcpy(dst, &head, sizeof(head));
			dstlen += sizeof(head);
		}
		else {
			head.set_length(126);
			memcpy(dst, &head, sizeof(head));
			dstlen += sizeof(head);
			uint16_t buflen = len;
			buflen = htons(buflen);
			memcpy(dst + dstlen, &buflen, sizeof(uint16_t));
			dstlen += sizeof(uint16_t);
		}
		memcpy(dst + dstlen, src, len);
		dstlen += len;
	}
};



#define BUF_SIZE  501200
class WebSocket{

public:
	WebSocket(std::string host, uint16_t port,std::string path, 
		uint16_t gameType, uint16_t gameCode, int roomId, int noshow) {
		m_hostDns = host;
		m_hostPort = port;
		m_connPath = path;
		m_recvBuf = new Mybuf(BUF_SIZE);
		pGameProcer = new GameProc(this,gameType, gameCode, roomId);
		loginStr = connect_str();	
		noShow = noshow;
	}

	~WebSocket() {
		bStop = true;
		if (m_recv_thread.joinable()) m_recv_thread.join();
		if (m_proc_thread.joinable()) m_proc_thread.join();
		reset();
		if (m_sockFd > 0) {
			closesocket(m_sockFd);
			m_sockFd = -1;
		}
		if (m_recvBuf) delete m_recvBuf;
		if (m_psslSSL) {
			SSL_free(m_psslSSL);
			m_psslSSL = nullptr;
		}
		if (pGameProcer) {
			delete pGameProcer;
			pGameProcer = nullptr;
		}
	}	

	void stop() {
		auto del_thread = std::thread([this]() {
			this->bStop = true;
			if (this->m_recv_thread.joinable()) this->m_recv_thread.join();
			if (this->m_proc_thread.joinable()) this->m_proc_thread.join();
			this->reset();
			this->pGameProcer->set_status(INIT, "停止");
		});
		del_thread.detach();
	}

	void start() {
		bStop = false;
		recv_start();
		proc_start();		
	}

	

	void reset() {
		std::lock_guard<std::mutex> guard(mtx);
		if (m_sockFd > 0) {
			closesocket(m_sockFd);
			m_sockFd = -1;
		}
		if (m_psslSSL) {
			SSL_free(m_psslSSL);
			m_psslSSL = nullptr;
		}
	}

	void set_gameDlg(CGameDlg *dlg) {
		pGameProcer->m_dlg = dlg;
	}		

	int sendBuf(char *buf, int buflen) {
		std::lock_guard<std::mutex> guard(mtx);
		if (!m_psslSSL) return -1;
		if (SSL_write(m_psslSSL, buf, buflen) <= 0) return -1;
		return 1;
	}	
	
	
	GameProc *pGameProcer = nullptr;
	bool bStop = false;
	int noShow=0;
private:	

	std::thread m_recv_thread;
	std::thread m_proc_thread;

	Notifier notifier;
	int m_sockFd = -1;
	int64_t last_livetime;

	
	std::string m_hostDns;
	uint16_t m_hostPort;
	std::string m_connPath;

	std::mutex mtx;
	
	Mybuf  *m_recvBuf = nullptr;
	SSL *m_psslSSL = nullptr;
	SSLsocket ssl;
	std::string loginStr;	
	

	std::string  connect_str() {
		std::string strSendData;

		strSendData = "GET " + m_connPath + " HTTP/1.1\r\n";
		strSendData += "Accept-Encoding: gzip, deflate, br\r\n";
		strSendData += "Accept-Language: zh-TW,zh;q=0.9,en-US; q=0.8,en;q=0.7\r\n";
		strSendData += "Cache-Control: no-cache\r\n";
		strSendData += "Connection: Upgrade\r\n";
		strSendData += "Host: " + m_hostDns + "\r\n";
		strSendData += "Origin: https://" + m_hostDns + "\r\n";
		strSendData += "Pragma: no-cache\r\n";
		strSendData += "Sec-WebSocket-Extension: permessage-deflate; client_max_window_bits\r\n";
		strSendData += "Sec-WebSocket-Key: 50NlQ2aWKjzUGTlilomHag==\r\n";
		//strSendData += "Sec-WebSocket-Protocol: casino.bin\r\n";
		strSendData += "Sec-WebSocket-Version: 13\r\n";
		strSendData += "Upgrade: websocket\r\n";
		strSendData += "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4044.129 Safari/537.36\r\n";
		strSendData += "\r\n";
		return strSendData;
	}	

	

	int recvBuf(char *buf, int maxsize, int&len) {
		std::lock_guard<std::mutex> guard(mtx);
		if (!m_psslSSL) return -1;
		len = SSL_read(m_psslSSL, buf, maxsize);
		if (len <= 0) return -1;
		return 1;
	}

	int login() {
		std::lock_guard<std::mutex> guard(mtx);
		if (m_sockFd > 0) {
			closesocket(m_sockFd);
			m_sockFd = -1;
		}
		if (m_psslSSL) {
			SSL_free(m_psslSSL);
			m_psslSSL = nullptr;
		}
		m_sockFd = ssl.ssl_connect(m_hostDns, m_hostPort, &m_psslSSL);
		if (m_sockFd < 0) return -1;
		if (SSL_write(m_psslSSL, loginStr.c_str(), loginStr.length()) <= 0) return -1;
		last_livetime = time(NULL);
		return 1;
	}

	int recv_buffer() {
		int len;
		char tempBuf[TEMP_SIZE];

		if (recvBuf(tempBuf, TEMP_SIZE, len) < 0) return -1;
		int pos = 0;
		if (!strncmp(tempBuf, "HTTP", 4)) {
			std::string temp = tempBuf;
			pos = temp.find("\r\n\r\n") + 4;
			len = len - pos;
		}
		return m_recvBuf->put_buf(tempBuf + pos, len);
	}

	int proc_buffer() {

		packet_head_t head;
		char buf[TEMP_SIZE];
		int32_t buflen;		
		while (true) {
			int ret = m_recvBuf->get_onepkg(buf, buflen);
			if (!ret) return 1;
			buf[buflen] = 0;
			ret = pGameProcer->proc_buffer(buf);
			if (ret < 0) return ret;
		}
		return 1;
	}

	void recv_start() {
		m_recv_thread = std::thread([this]() {
			while (!bStop) {
				recv_loop();
			}
		});
	}

	void proc_start() {
		m_proc_thread = std::thread([this]() {
			while (!bStop) {
				proc_loop();
			}
		});
	}

	void recv_loop() {
		

		if (m_sockFd < 0) {
			if (login() < 0) {
				pGameProcer->set_status(INIT, "登录失败");
				//Sleep(1000);
				return;
			}
		}

		fd_set read_fds, err_fds;
		FD_ZERO(&read_fds);
		FD_ZERO(&err_fds);

		FD_SET(m_sockFd, &read_fds);
		FD_SET(m_sockFd, &err_fds);
     
		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 50 * 1000;

		int nfds = select(m_sockFd + 1, &read_fds, NULL, &err_fds, &timeout);
		time_t now = time(NULL);
		if (nfds > 0) {
			if (FD_ISSET(m_sockFd, &err_fds)) {
				reset();
				pGameProcer->set_status(INIT, "连接出错");
			}
			else {
				if (FD_ISSET(m_sockFd, &read_fds)) {
					if (recv_buffer() >= 0) {
						notifier.notify(0);
						last_livetime = now;
					}
					else {
						reset();
						pGameProcer->set_status(INIT, "连接被关闭");
					}
				}
			}
			return;
		}
		if (now > last_livetime + 10) {
			reset();
			pGameProcer->set_status(INIT, "接受数据超时");
		}
	}

	void proc_loop() {
		int index = notifier.wait_for(50);
		if (index < 0) return;
		if(proc_buffer() < 0){
			reset();
			pGameProcer->set_status(INIT, "处理数据错误");
		}
	}

};

extern std::vector<WebSocket*> gGamer;