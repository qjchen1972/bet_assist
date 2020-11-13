#pragma once
#include<string>
#include <mutex>
#include <atomic>
#include<vector>
#include <iostream>
#include <fstream>
#include "setup.h"
#include"myssl.h"
#include"userdata.h"

#define HTTPMAXSIZE  1024

class HttpSocket {
public:

	HttpSocket(std::string host, uint16_t port) {
		m_hostDns = host;
		m_hostPort = port;
	}

	~HttpSocket() {
		reset();
	}

	void reset() {
		if (m_sockFd > 0) {
			closesocket(m_sockFd);
			m_sockFd = -1;
		}
		if (m_psslSSL) {
			SSL_free(m_psslSSL);
			m_psslSSL = nullptr;
		}
	}

	int getResult(std::string str, std::vector<std::string> &result) {
		
		Tool tool;

		int pos = str.find("\r\n\r\n") + 4;
		std::string temp = str.substr(pos, str.size() - pos);
		pos = temp.find("\r\n") + 2;
		temp = temp.substr(pos, temp.size() - pos);
		pos = temp.find("\r\n");
		temp = temp.substr(0, pos);
		tool.my_split(temp, ',', result);
		return 0;
	}

	int login(std::string name, std::string pwd) {
		reset();
		m_sockFd = ssl.ssl_connect(m_hostDns, m_hostPort, &m_psslSSL);
		if (m_sockFd < 0) return -1;

		//char * buf = new char[HTTPMAXSIZE];
		char buf[HTTPMAXSIZE];
		sprintf_s(buf, HTTPMAXSIZE, "GET /login.php?name=%s&pwd=%s HTTP/1.1\r\n", name.c_str(), pwd.c_str());
		std::string strRequest;
		strRequest += buf;
		strRequest += "Host: " + m_hostDns + "\r\n";
		strRequest += "Origin: https://" + m_hostDns + "\r\n";
		strRequest += "Upgrade-Insecure-Requests: 1";
		strRequest += "\r\n\r\n";
		if (SSL_write(m_psslSSL, (void*)strRequest.c_str(), strRequest.length()) <= 0) {
			reset();
			return -1;
		}
		int len = SSL_read(m_psslSSL, buf, HTTPMAXSIZE);
		if (len <= 0) {
			reset();
			return -1;
		}
		std::string str;
		std::vector<std::string> result;
		str += buf;
		getResult(str, result);
		reset();
		gConfig.loginId = result[1];
		gConfig.validDate = atol(result[2].c_str());
		gConfig.userType = atoi(result[3].c_str());
		return atoi(result[0].c_str());
	}

	int query(std::string name, std::string loginId) {

		reset();
		m_sockFd = ssl.ssl_connect(m_hostDns, m_hostPort, &m_psslSSL);
		if (m_sockFd < 0) return -1;

		//char * buf = new char[HTTPMAXSIZE];
		char buf[HTTPMAXSIZE];
		sprintf_s(buf, HTTPMAXSIZE, "GET /query.php?name=%s&id=%s HTTP/1.1\r\n", name.c_str(), loginId.c_str());
		std::string strRequest;
		strRequest += buf;
		strRequest += "Host: " + m_hostDns + "\r\n";
		strRequest += "Origin: https://" + m_hostDns + "\r\n";
		strRequest += "Upgrade-Insecure-Requests: 1";
		strRequest += "\r\n\r\n";
		if (SSL_write(m_psslSSL, (void*)strRequest.c_str(), strRequest.length()) <= 0) {
			reset();
			return -1;
		}
		int len = SSL_read(m_psslSSL, buf, HTTPMAXSIZE);
		if (len <= 0) {
			reset();
			return -1;
		}
		//reset();
		std::string str;
		std::vector<std::string> result;
		str += buf;
		getResult(str, result);
		int ret = atoi(result[0].c_str());
		return  ret;
	}

private:
	std::string m_hostDns;
	uint16_t m_hostPort;
	int m_sockFd = -1;
	SSL *m_psslSSL = nullptr;
	SSLsocket ssl;
};