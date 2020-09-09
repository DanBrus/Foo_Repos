#include "TCP_Client.h"



void TCP_Client::schedule_send(std::string message)
{
	int sent = 0;
	int flags = 0;

	while (sent < message.length())
	{
		// Отправка очередного блока данных 
		int res = send(s, &message[sent], message.length() - sent, flags);
		if (res == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAEWOULDBLOCK) {
				bizy_for_send = true;
				while (bizy_for_send)
					Sleep(200);
			}
			else {
				std::cerr << "Error of sending" << std::endl;
				return;
			}
		}
		sent += res;
	}


	time_t seconds = time(NULL);
	tm* timeinfo = localtime(&seconds);
	//Вывод IP адреса
	log_file << ip << " | ";
	//Вывод даты и времени
	log_file << (1900 + timeinfo->tm_year) << '.' << timeinfo->tm_mon << '.' << timeinfo->tm_mday << ' ' << timeinfo->tm_hour << ':' << timeinfo->tm_min << " | ";
	//Вывод сообщения
	log_file << message << std::endl;

	return;
}

void TCP_Client::schedule_read()
{
	//Объявление и очистка буфера для принятия сообщения
	char buffer[512];
	memset(buffer, 0, 512);
	int curlen = 0;
	int rcv, rcvd = 0;
	//Пока данные приходят, принимать
	do {
		rcv = recv(s, buffer, 512, 0);
		rcvd += rcv;
	} while (rcv > 0);
	//Если подучены данные, вывести их в консоль и в файл
	if (rcvd > 0) {
		time_t seconds = time(NULL);
		tm* timeinfo = localtime(&seconds);
		//Вывод IP адреса
		log_file << ip << " | ";
		//Вывод даты и времени
		log_file << (1900 + timeinfo->tm_year) << '.' << timeinfo->tm_mon << '.' << timeinfo->tm_mday << ' ' << timeinfo->tm_hour << ':' << timeinfo->tm_min << " | ";
		//Вывод сообщения
		log_file << buffer << std::endl;

		//Вывод IP адреса
		std::cout << ip << " | ";
		//Вывод даты и времени
		std::cout << (1900 + timeinfo->tm_year) << '.' << timeinfo->tm_mon << '.' << timeinfo->tm_mday << ' ' << timeinfo->tm_hour << ':' << timeinfo->tm_min << " | ";
		//Вывод сообщения
		std::cout << buffer << std::endl;
	}
}

void TCP_Client::client_loop()
{
	WSAEVENT ev = WSACreateEvent();
	WSAEventSelect(s, ev, FD_READ | FD_CLOSE | FD_WRITE);

	while (1)
	{
		WSANETWORKEVENTS ne;
		//Ожидание событий в течение 0.1 сек
		DWORD ev_code = WSAWaitForMultipleEvents(1, &ev, false, 100, false);

		if (ev_code != WSA_WAIT_TIMEOUT) {			   //За время ожидания произошло событие
														//Определить, какое событие пришло на сокет
			WSAEnumNetworkEvents(s, ev, &ne);
			if (ne.lNetworkEvents & FD_READ) {	//Пришли данные на сокет. Можно принимать.
				schedule_read();
			}
			if (ne.lNetworkEvents & FD_WRITE) { //Сокет готов для записи, можно отправлять данные.
				bizy_for_send = false;
			}
			if (ne.lNetworkEvents & FD_CLOSE) { // Удаленная сторона закрыла соединение
				//Завершение исходящих отправлений и отправление сигнала о завершении.
				shutdown(s, SD_SEND);
				break;
			}
		}
		//Пришёл сигнал о завершении работы
		if (ctrl_c) {
			//Завершение исходящих отправлений и завершение соединения
			shutdown(s, SD_SEND);

			//Цикл завершения приёма
			while (1) {
				WSANETWORKEVENTS ne;
				//Ожидание событий в течение 0.1 сек
				DWORD ev_code = WSAWaitForMultipleEvents(1, &ev, false, 100, false);

				if (ev_code != WSA_WAIT_TIMEOUT) {			   //За время ожидания произошло событие
															   //Определить, какое событие пришло на сокет
					WSAEnumNetworkEvents(s, ev, &ne);
					if (ne.lNetworkEvents & FD_READ) {	//Пришли данные на сокет. Можно принимать.

					}
					if (ne.lNetworkEvents & FD_CLOSE)  // Удаленная сторона закрыла соединение, можно закрыть сокет.
						break;
				}
			}
			break;
		}
	}
	
	closesocket(s);
	WSACloseEvent(ev);
	WSACleanup();
	return;
}

void TCP_Client::sending_thread(void)
{
	while (1) {
		std::string message;
		std::getline(std::cin, message);

		schedule_send(message);

		if (ctrl_c)
			return;
	}
}

void TCP_Client::exit_except()
{
	ctrl_c = true;
}

TCP_Client::TCP_Client()
{
	
	std::string tmp("Client_log");
	tmp += GetCurrentProcessId();
	tmp += std::string(".txt");
	log_file.open(tmp, std::ios_base::out);

	if (!log_file.is_open()) {
		std::cerr << "Cannot make log file." << std::endl;
		ctrl_c = true;
		return;
	}
	struct sockaddr_in addr;
	ctrl_c = false;

	WSADATA wsa_data;
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data)) {
		std::cerr << "WSAStartup error." << std::endl;
		ctrl_c = true;
		return;
	}

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
		std::cerr << "Socket creation error." << std::endl;

	std::cout << "Socket created successful." << std::endl;


	std::cout << "Enter IP address of your server." << std::endl;
	std::cin >> ip;
	char server[16];
	int i = 0;
	for (std::string::iterator iter = ip.begin(); iter != ip.end(); iter++) {
		server[i] = *iter;
		i++;
	}
	server[i] = '\0';
	std::cin.clear();
	std::cin.ignore(100, '\n');
	// Заполнение структуры с адресом удаленного узла 
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9000);
	addr.sin_addr.s_addr = inet_addr(server);

	// Установка соединения с удаленным хостом 
	for (i = 0; i < 10; i++)
	{
		if (connect(s, (struct sockaddr*) &addr, sizeof(addr)) == 0)
			break;
	}
	if (i == 10)
	{
		closesocket(s);
		ctrl_c = true;
		return;
	}
	printf("Connect.\n");

}


TCP_Client::~TCP_Client()
{
}
