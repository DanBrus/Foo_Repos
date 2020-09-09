#include <signal.h>
#include "TCP_Client.h"

TCP_Client client[1];

void INTHandler(int sig)
{
	client->exit_except();
	return;
}

DWORD WINAPI  my_thread(LPVOID lpParam) {
	client->sending_thread();
	return 0;
}

int main(int argc, char *argv[])
{
	signal(SIGINT, INTHandler);

	CreateThread(NULL, NULL, my_thread, NULL, NULL, NULL);
	client->client_loop();

	return 0;
}