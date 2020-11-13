#pragma once
#include <thread>
#include "notify.h"
#include "websocket.h"
#include"setup.h"

class MyNet {
public:

	MyNet(){
#ifdef _WIN32
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
		
	}

	~MyNet(){
		stop();
#ifdef _WIN32
		WSACleanup();
#endif
	}


	void stop() {
		bStop = true;
		if (m_recv_thread.joinable()) m_recv_thread.join();
		if (m_proc_thread.joinable()) m_proc_thread.join();
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


private:

	bool bStop = false;
	std::thread m_recv_thread;
	std::thread m_proc_thread;
	Notifier notifier;
	
	void recv_loop() {
		//char buf[512];

		int32_t fdmax = -1;
		fd_set read_fds, err_fds;
		FD_ZERO(&read_fds);
		FD_ZERO(&err_fds);

		for (int i = 0; i < gGamer.size(); i++) {
			if (gConfig.gameSite[i].isstop || gConfig.gameStop){
				if (gGamer[i]->status != INIT) {
					gGamer[i]->set_status(INIT, "停止");
					gGamer[i]->reset();
				}
				continue;
			}
			if (gGamer[i]->status == CONNECTING)
				continue;
			if (gGamer[i]->m_sockFd < 0)
				gGamer[i]->asyn_login();
			else{
				FD_SET(gGamer[i]->m_sockFd, &read_fds);
				FD_SET(gGamer[i]->m_sockFd, &err_fds);
				if (gGamer[i]->m_sockFd > fdmax) fdmax = gGamer[i]->m_sockFd;
			}
		}

		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 50 * 1000;

		int nfds = select(fdmax + 1, &read_fds, NULL, &err_fds, &timeout);
		time_t now = time(NULL);
		if (nfds > 0) {			
			for (int i = 0; i < gGamer.size(); i++) {
				if (gGamer[i]->m_sockFd < 0 || gGamer[i]->status == CONNECTING) continue;
				if (FD_ISSET(gGamer[i]->m_sockFd, &err_fds)) {
					gGamer[i]->reset();
					gGamer[i]->set_status(INIT, "连接出错");
				}
				else {
					if (FD_ISSET(gGamer[i]->m_sockFd, &read_fds)) {
						if (gGamer[i]->recv_buffer() >= 0) {
							notifier.notify(i);
							gGamer[i]->last_livetime = now;
						}
						else {
							gGamer[i]->reset();
							gGamer[i]->set_status(INIT, "连接被关闭");
						}
					}
				}
			}
		}

		//处理超时
		for (int i = 0; i < gGamer.size(); i++) {
			if (gGamer[i]->m_sockFd < 0 || gGamer[i]->status == CONNECTING) continue;
			if (now > gGamer[i]->last_livetime + 10) {
				gGamer[i]->reset();
				gGamer[i]->set_status(INIT, "接受数据超时");
			}
		}
	}

	void proc_loop() {
		int index = notifier.wait_for(50);
		if (index < 0) return;
		char buf[512];
		if (gGamer[index]->proc_buffer() < 0) {
			gGamer[index]->reset();
			gGamer[index]->set_status(INIT, "处理数据错误");
		}
	}
};