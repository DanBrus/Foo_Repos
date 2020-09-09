#include "TCP_Server.h"

void TCP_Server::server_loop()
{
	events = (WSAEVENT*) malloc(sizeof(WSAEVENT) + 1);

	events[0] = WSACreateEvent();
	
	//��������� ������� 0 � ������� �����������
	WSAEventSelect(sockets[0], events[0], FD_ACCEPT);

	while (1)
	{
		WSANETWORKEVENTS ne;
		//�������� ������� � ������� 0.1 ���
		DWORD ev_code = WSAWaitForMultipleEvents(connect_ctr, events, false, 100, false);

		if (ev_code != WSA_WAIT_TIMEOUT) {			   //�� ����� �������� ��������� �������
				//����������, ����� ������� ������ �� �����
			WSAEnumNetworkEvents(sockets[ev_code - WSA_WAIT_EVENT_0], events[ev_code - WSA_WAIT_EVENT_0], &ne);
			if (ne.lNetworkEvents & FD_ACCEPT) {
				// �������� ����������� � ������ �������� ����������
				schedule_accept();
				add_accepted_connection();
			}
			if (ne.lNetworkEvents & FD_READ) {	//������ ������ �� �����. ����� ���������.
				schedule_read(clients[sockets[ev_code - WSA_WAIT_EVENT_0]]); 
			}
			if (ne.lNetworkEvents & FD_WRITE) { //����� ����� ��� ������, ����� ���������� ������.
				clients[ev_code - WSA_WAIT_EVENT_0].bizy_for_send = false;
			}
			if (ne.lNetworkEvents & FD_CLOSE) { // ��������� ������� ������� ����������, ����� ������� �����.
				schedule_disconnect(ev_code - WSA_WAIT_EVENT_0);
			}
		}
		if (ctrl_c) {
			if (connect_ctr == 1) {
				closesocket(sockets[0]);
				WSACloseEvent(events[0]);
				free(events);
				free(sockets);
				break;
			}

			shutdown(sockets[0], SD_BOTH);
			for (int i = 0; i < connect_ctr; i++) {
				shutdown(sockets[i], SD_SEND);
			}
		}

		
	}
	
}

void TCP_Server::exit_except()
{
	ctrl_c = true;
}

void TCP_Server::print_log(const std::string &ip, std::string message, bool to_console)
{
	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	//����� IP ������
	file << ip << " | ";
	//����� ���� � �������
	file << (1900 + timeinfo->tm_year) << '.' << timeinfo->tm_mon << '.' << timeinfo->tm_mday << ' ' << timeinfo->tm_hour << ':' << timeinfo->tm_min << " | ";
	//����� ���������
	file << message << std::endl;

	if (to_console) {
		//����� IP ������
		std::cout << ip << " | ";
		//����� ���� � �������
		std::cout << (1900 + timeinfo->tm_year) << '.' << timeinfo->tm_mon << '.' << timeinfo->tm_mday << ' ' << timeinfo->tm_hour << ':' << timeinfo->tm_min << " | ";
		//����� ���������
		std::cout << message << std::endl;
	}
}

int TCP_Server::sock_err(const std::string & function, int s)
{
	int err = WSAGetLastError();
	std::cerr << function << " : socket error: " << err;
	return -1;
}

void TCP_Server::add_accepted_connection()
{
	//�������� ��������� � ����������� � �����������
	client_ctx cur;
	memset(&cur, 0, sizeof(cur));
	clients.insert(std::pair<SOCKET, client_ctx>(g_accepted_socket, cur));
	std::map<SOCKET, client_ctx>::iterator i_client = clients.find(g_accepted_socket);
	i_client->second.ip = std::string(inet_ntoa(new_client.sin_addr));
	i_client->second.socket = g_accepted_socket;
	i_client->second.bizy_for_send = false;
	//����� ���������� � ����������� � ����
	print_log(i_client->second.ip, std::string("connected"), 0);

	//�������� ������� ��� ������ � ����������� � ���������� �������� ������� � ������.
	events = (WSAEVENT*) realloc(events, ++connect_ctr * sizeof(WSAEVENT));
	events[connect_ctr - 1] = WSACreateEvent();
	sockets = (SOCKET*) realloc(sockets, connect_ctr * sizeof(SOCKET));
	sockets[connect_ctr - 1] = g_accepted_socket;
	schedule_read(i_client->second);
	//���������� ������� � ������
	WSAEventSelect(sockets[connect_ctr - 1], events[connect_ctr - 1], FD_READ | FD_WRITE | FD_CLOSE);
	return;

}


void TCP_Server::schedule_read(TCP_Server::client_ctx & client)
{
	//���������� � ������� ������ ��� �������� ���������
	char buffer[512];
	memset(buffer, 0, 512);
	int curlen = 0;
	int rcv, rcvd = 0;
	//���� ������ ��������, ���������
	do {
		rcv = recv(client.socket, buffer, 512, 0);
		rcvd += rcv;
	} while (rcv > 0);
	//���� �������� ������, ������� �� � ������� � � ����
	if(rcvd > 0)
		print_log(client.ip, std::string(buffer, rcvd + 1), 1);
}

void TCP_Server::schedule_write(std::string &message)
{
	std::map<SOCKET, client_ctx>::iterator client_i = clients.begin();
	while (clients.end() != client_i) {
		int sent = 0;
		int flags = 0;

		while (sent < message.length())
		{
			// �������� ���������� ����� ������ 
			int res = send(client_i->first, &message[sent], message.length() - sent, flags);
			if (res == SOCKET_ERROR) {
				if (WSAGetLastError() == WSAEWOULDBLOCK) {
					client_i->second.bizy_for_send = true;
					while (client_i->second.bizy_for_send)
						Sleep(200);
				}
				else {
					//std::cerr << "Error of sending" << std::endl;
					break;
				}
			}
			sent += res;
		}

		client_i++;
	}
}

void TCP_Server::schedule_disconnect(int s_index)
{
	print_log(clients[sockets[s_index]].ip, std::string("disconnected"), 0);
	//���������� ���� ��������� ����������� � �������� ��������� � �������� ����������
	shutdown(sockets[s_index], SD_SEND);
	//�������� ������
	clients.erase(sockets[s_index]);
	closesocket(sockets[s_index]);
	//�������� ������� ��������� � ��� ������� �� �������.
	WSACloseEvent(events[s_index]);
	events[s_index] = events[--connect_ctr];
	sockets[s_index] = sockets[connect_ctr];
	events = (WSAEVENT*)realloc(events, sizeof(WSAEVENT) * connect_ctr);
	sockets = (SOCKET*)realloc(sockets, sizeof(SOCKET) * connect_ctr);
}

void TCP_Server::schedule_accept()
{
	g_accepted_socket = accept(sockets[0], (sockaddr*)&new_client, &addrlen);
	if (g_accepted_socket == INVALID_SOCKET)
		std::cerr << WSAGetLastError();
}

void TCP_Server::log_message(std::string message)
{
	schedule_write(message);
	print_log(std::string("SEND"), message, 0);
}

TCP_Server::TCP_Server()
{
	ctrl_c = false;
	// ������������� ����� � �����������
	file.open("server_messages.txt", std::ios_base::out);

	WSADATA wsa_data;
	//�������������
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data)) {
		std::cerr << "WSAStartup error." << std::endl;
		ctrl_c = true;
		return;
	}

	//�������� ������ �������������
	sockets = (SOCKET*)malloc(sizeof(SOCKET));
	sockets[0] = WSASocketA(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
	if (sockets[0] < 0) {
		std::cerr << "Socket creation error." << std::endl;
		free(sockets);
		ctrl_c = true;
		return;
	}
	
	//������� ������ � ������������� �����
	
	unsigned long mode = 1;
	ioctlsocket(sockets[0], FIONBIO, &mode);
	
	//���������� ��������� ��� �������������
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9000);					// ���� � ��������� ��������������� ����� 
	addr.sin_addr.s_addr = htonl(INADDR_ANY);		// ���� ��������� �� ���� �������

													//���������� ������ � ������ ������������� 
	if (bind(sockets[0], (struct sockaddr*) &addr, sizeof(addr)) < 0) {
		sock_err(std::string("bind"), sockets[0]);
		closesocket(sockets[0]);
		free(sockets);
		ctrl_c = true;
		return;
	}
	std::cout << "Bind is OK." << std::endl;

	//������� ����� ��� ������������
	if (listen(sockets[0], SOMAXCONN) < 0)
	{
		sock_err(std::string("listen"), sockets[0]);
		closesocket(sockets[0]);
		free(sockets);
		ctrl_c = true;
		return;
	}
	else
		std::cout << "Listening TCP port: " << ntohs(addr.sin_port) << std::endl;
	
	connect_ctr = 1;
}


TCP_Server::~TCP_Server()
{
	closesocket(sockets[0]);
	WSACleanup();
}

