#pragma once
#ifndef _WIN32
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/socket.h>
#ifdef EPOLL
#include <sys/epoll.h>
#endif
#define closesocket close
#else
#include <winsock2.h>
#include<WS2tcpip.h>
#define socklen_t int
#endif

#define USE_SSL
#ifdef USE_SSL
#include<openssl\ssl.h>
#endif

#include<string>
#include <mutex>
#include <atomic>
#include<vector>


class SSLsocket {
public:
	SSLsocket() {
		init_ssl();
	}
	~SSLsocket() {
		exit_ssl();
	}


	int ssl_connect(std::string  dns, int16_t port, SSL **m_psslSSL) {

		ADDRINFOA aiHints;
		ZeroMemory(&aiHints, sizeof(ADDRINFOT));
		aiHints.ai_family = AF_INET;
		aiHints.ai_flags = AI_PASSIVE;
		aiHints.ai_protocol = 0;
		aiHints.ai_socktype = SOCK_STREAM;
		PADDRINFOA paiResult;
		if (GetAddrInfoA(dns.c_str(), NULL, &aiHints, &paiResult)) return -1;

		SOCKADDR_IN hostAddr;
		hostAddr.sin_addr = ((LPSOCKADDR_IN)paiResult->ai_addr)->sin_addr;
		hostAddr.sin_family = AF_INET;
		hostAddr.sin_port = htons(port);

		int  fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (fd < 0) return -1;
		if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char*)&recvBuf, sizeof(int)) != 0)
			return -1;
		if (setSocketTimeout(fd, timieout) != 0) return -1;
		if (connect(fd, (const sockaddr*)&hostAddr, sizeof(hostAddr)) != 0)
			return -1;

		*m_psslSSL = SSL_new(m_pctxSSL);
		if (m_psslSSL == NULL) return -1;
		if (!SSL_set_fd(*m_psslSSL, fd))  return -1;
		if (SSL_connect(*m_psslSSL) != 1)  return -1;
		return fd;
	}


private:
	SSL_CTX *m_pctxSSL = nullptr;
	int32_t timieout = 10;
	int32_t recvBuf = 64 * 1024;

	int init_ssl() {

		if (!SSL_library_init()) return 0;
		//SSLeay_add_ssl_algorithms();
		if (!SSL_load_error_strings()) return 0;
		m_pctxSSL = SSL_CTX_new(TLSv1_2_client_method());
		if (m_pctxSSL == NULL) return 0;
		return 1;
	}

	void exit_ssl() {
		if (m_pctxSSL) {
			SSL_CTX_free(m_pctxSSL);
			m_pctxSSL = nullptr;
		}
	}

	int setSocketTimeout(int fd, int sec) {
#ifdef WIN32
		int timeout = sec * 1000;
		return setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
#else
		struct timeval timeout;
		timeout.tv_sec = sec;
		timeout.tv_usec = 0;
		return setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,
			(char *)&timeout, sizeof(struct timeval));
#endif
	}
	void _set_noneblock(int sock) {
#ifdef WIN32
		unsigned long ul = 1;
		ioctlsocket(sock, FIONBIO, (unsigned long *)&ul);
#else
		int flags = fcntl(sock, F_GETFL, 0);
		fcntl(sock, F_SETFL, flags | O_NONBLOCK);
#endif
	}


	int _noblock_connect(int sock, SOCKADDR_IN &hostAddr, int timeout) {

		int ret = connect(sock, (struct sockaddr *)(&hostAddr), sizeof(hostAddr));
		if (ret == 0) return 1;

#ifdef WIN32

		if (WSAGetLastError() != WSAEWOULDBLOCK) return 0;
#else
		if (errno != EINPROGRESS) return 0;
#endif
		fd_set writefds, expectfds;
		struct timeval tv;
		tv.tv_sec = timeout;
		tv.tv_usec = 0;
		FD_ZERO(&writefds);
		FD_ZERO(&expectfds);
		FD_SET(sock, &writefds);
		FD_SET(sock, &expectfds);
		ret = select(sock + 1, NULL, &writefds, &expectfds, &tv);
		if (ret > 0) {
			if (FD_ISSET(sock, &writefds))
				return 1;
			if (FD_ISSET(sock, &expectfds))
				return 0;
		}
		else
			return 0;
	}
};
