#pragma once
#include <Winsock2.h>
#include <string>

class Socket {
private:
	SOCKET s;

public:
	Socket();
	Socket(int);
	~Socket();

	SOCKET get();
};
#pragma pack(push, 1)
struct ICMP_header {
	uint8_t type = 8;
	uint8_t code = 0;
	uint16_t checksum = 0;
	char payload[4] = {'p', 'i', 'n', 'g'};
	uint16_t identifier = GetCurrentProcessId();
	uint16_t sequence = 0;
};
#pragma pack(pop)