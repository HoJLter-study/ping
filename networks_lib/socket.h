#pragma once
#include <Winsock2.h>
#include <string>

class Socket {
private:
	SOCKET s;

public:
	Socket();
	~Socket();

	SOCKET get();
};

struct ICMP_header {
	uint8_t type = 8;
	uint8_t code = 0;
	uint16_t checksum = 0;
	char payload[4];


	ICMP_header(const char*);
};