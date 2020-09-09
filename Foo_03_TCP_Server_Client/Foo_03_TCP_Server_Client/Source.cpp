#include "TCP_Server.h"
#include <windows.h>
#include <csignal>

#pragma comment(lib, "Gdi32.lib")

TCP_Server Server;

LONG WINAPI WndProc(HWND, UINT, WPARAM, LPARAM); // ������� ��������� ��������� ����


void INTHandler(int sig)
{
	Server.exit_except();
	return;
}

DWORD WINAPI MakeWindow(LPVOID lpParam)
{
	HWND hwnd; // ���������� ����
	MSG msg;   // ��������� ���������
	WNDCLASS w; // ��������� ������ ����
	memset(&w, 0, sizeof(WNDCLASS)); // ������� ������ ��� ���������
	w.style = CS_HREDRAW | CS_VREDRAW;
	w.lpfnWndProc = (WNDPROC)WndProc;
	w.hInstance = *((HINSTANCE*)lpParam);
	w.hbrBackground = CreateSolidBrush(0x00FFFFFF);
	w.lpszClassName = "MyClass";
	RegisterClass(&w); // ����������� ������ ����
					   // �������� ����
	hwnd = CreateWindow("MyClass", "Sending",
		WS_OVERLAPPEDWINDOW,
		500, 300, 200, 150,
		NULL, NULL, *((HINSTANCE*)lpParam), NULL);
	ShowWindow(hwnd, 10); // ����������� ����
	UpdateWindow(hwnd);         // ����������� ����

								// ���� ��������� ���������
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
// ������� ��������� ���������
LONG WINAPI WndProc(HWND hwnd, UINT Message,
	WPARAM wparam, LPARAM lparam) {
	HDC hdc;
	HINSTANCE hInst;
	PAINTSTRUCT ps;
	static HWND hBtn; // ���������� ������
	static HWND hEdt1;; // ����������� ����� ��������������
	TCHAR StrA[512];
	int len;

	switch (Message) {
	case WM_CREATE: // ��������� �������� ����
		hInst = ((LPCREATESTRUCT)lparam)->hInstance; // ���������� ����������
													 // ������� � ���������� ���� ��������������
		hEdt1 = CreateWindow("edit", "",
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 5, 20, 170, 20,
			hwnd, 0, hInst, NULL);
		ShowWindow(hEdt1, SW_SHOWNORMAL);
		// ������� � ���������� ������
		hBtn = CreateWindow("button", "Send",
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			5, 60, 100, 30, hwnd, 0, hInst, NULL);
		ShowWindow(hBtn, SW_SHOWNORMAL);;
		break;
	case WM_COMMAND:  // ��������� � �������
		if (lparam == (LPARAM)hBtn)    // ���� ������ �� ������
		{
			len = GetWindowText(hEdt1, StrA, 20);
			SetWindowText(hEdt1, "");
			if (len = 0)
				break;
			//�������� ���������.
			Server.log_message(std::string(StrA));
		}
		break;
	case WM_PAINT: // ����������� ����
		hdc = BeginPaint(hwnd, &ps); // ������ �����������
		EndPaint(hwnd, &ps); // ����� �����������
		break;
	case WM_DESTROY: // �������� ����
		PostQuitMessage(0);
		break;
	default: // ��������� ��������� �� ���������
		return DefWindowProc(hwnd, Message, wparam, lparam);
	}
	return 0;
}


int main(void) {

	signal(SIGINT, INTHandler);

	HINSTANCE hinst = (HINSTANCE)GetModuleHandle(NULL);

	CreateThread(NULL, NULL, MakeWindow, &hinst, NULL, NULL);
	Server.server_loop();

	return 0;
}
