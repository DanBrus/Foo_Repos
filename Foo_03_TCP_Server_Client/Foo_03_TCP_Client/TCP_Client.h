#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <iostream> 
#include <string>
#include <ctime>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <limits>
#include <fstream>
// Директива линковщику: использовать библиотеку сокетов
#pragma comment(lib, "ws2_32.lib")


class TCP_Client
{
private:
	int s;
	bool bizy_for_send, ctrl_c;
	std::fstream log_file;

	void schedule_send(std::string message);
	void schedule_read();
	std::string ip;
public:
	void sending_thread(void);
	void client_loop();
	void exit_except();
	TCP_Client();
	~TCP_Client();
};

