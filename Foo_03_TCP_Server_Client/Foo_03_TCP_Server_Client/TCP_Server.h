#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#define WIN32_LEAN_AND_MEAN
// Директива линковщику: использовать библиотеку сокетов 

#include <list>
#include <iostream> 
#include <string>
#include <windows.h> 
#include <fstream>
#include <winsock2.h> 
#include <mswsock.h>
#include <ctime>
#include <map>
#include <thread>

#pragma comment(lib, "ws2_32.lib") 
#pragma comment(lib, "mswsock.lib")

class TCP_Server
{
private:
	struct client_ctx
	{
		bool bizy_for_send;
		std::string ip;
		int socket;
	};

	HANDLE g_io_port;						//Общий порт сервера
	std::fstream file;						//Файл для вывода сообщений
	int g_accepted_socket;
	std::map<SOCKET, client_ctx> clients;
	sockaddr_in new_client;
	WSAEVENT *events;
	SOCKET *sockets;
	int connect_ctr;
	int addrlen = 16;
	bool ctrl_c;

	void print_log(const std::string &ip, std::string message, bool to_console);
	int sock_err(const std::string &function, int s);
	void add_accepted_connection();
	void schedule_read(TCP_Server::client_ctx &client);
	void schedule_write(std::string &message);
	void schedule_disconnect(int s_index);
	void schedule_accept();

public:
	void server_loop();
	void exit_except();
	void log_message(std::string message);
	TCP_Server();
	~TCP_Server();
};

