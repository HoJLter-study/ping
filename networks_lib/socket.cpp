#pragma comment(lib, "ws2_32.lib")
#include <Winsock2.h>
#include <iostream>
#include <string>
#include "socket.h"
#include <WS2tcpip.h>
#include <vector>

Socket::Socket() {
	WSADATA ws;
	int err = WSAStartup(MAKEWORD(2, 2), &ws);
	if (err != 0) {
		throw std::runtime_error("[ERROR] WSA Startup failed with error: " + std::to_string(err));
	}

	SOCKET sDescriptor = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sDescriptor == INVALID_SOCKET){
		throw std::runtime_error("[ERROR] Socket creation failed with error: " + std::to_string(WSAGetLastError()));
	}

	s = sDescriptor;
}

Socket::Socket(int timeout): Socket(){
	int err = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout));
	if (err != 0) {
		throw std::runtime_error("[ERROR] Socket settings failed with error: " + std::to_string(WSAGetLastError()));
	}
}

Socket::~Socket(){
	closesocket(s);
	WSACleanup();
}

SOCKET Socket::get() {
	return s;
}

ICMP_header::ICMP_header(const char* pl) {
	checksum = 0;
	memcpy(payload, pl, 4);

	uint16_t* words = reinterpret_cast<uint16_t*>(this);

	uint32_t cs = 0;
	for (int i = 0; i < sizeof(ICMP_header) / 2; i++) {
		cs += words[i];
	}

	while (cs >> 16) {
		cs = (cs & 0xFFFF) + (cs >> 16);
	}

	checksum = htons(~cs);
};


addrinfo* dnsRequest(std::string domain) {
	addrinfo hints{};
	hints.ai_family = AF_INET;
	addrinfo* res;

	int err = getaddrinfo(domain.data(), NULL, &hints, &res);
	if (err != 0) {
		throw std::runtime_error("[ERROR] DNS request failed (maybe wrong domain?) with error: " + std::to_string(WSAGetLastError()));
	}
	return res;
}

void ping(std::string domain, int timeout = 1000) {
	Socket s(timeout);

	ICMP_header header("ping");
	addrinfo* destinction = dnsRequest(domain);

	char* buf = reinterpret_cast<char*>(&header);
	for (int i = 0; i < 4; i++) {
		//SENDING
		int bytes = sendto(s.get(), buf, sizeof(header), 0, destinction->ai_addr, destinction->ai_addrlen);
		if (bytes == SOCKET_ERROR) {
			throw std::runtime_error("[ERROR] Sending failed with error: " + std::to_string(WSAGetLastError()));
		}
		std::cout << bytes << " bytes was sent to " + domain << std::endl;

		//RECIEVING
		std::vector<char> resp(1024);
		int addrlen = destinction->ai_addrlen;
		bytes = recvfrom(s.get(), resp.data(), sizeof(header), 0, destinction->ai_addr, &addrlen);
		if (bytes == SOCKET_ERROR) {
			throw std::runtime_error("[ERROR] Recieving failed with error: " + std::to_string(WSAGetLastError()));
		}
		std::cout << bytes << " bytes was recieved from " + domain << std::endl;
	}

}

int main() {
	try {
		ping("google.com", 3000);
	}
	catch (std::exception e) {
		std::cout << e.what();
	}
}