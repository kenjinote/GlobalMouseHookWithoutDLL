#include <windows.h>

TCHAR szClassName[] = TEXT("Window");
HHOOK hHook;
HWND hMainWnd;
POINT pt;

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	MSLLHOOKSTRUCT* pMS = (MSLLHOOKSTRUCT*)lParam;
	switch (wParam) {
	case WM_MOUSEMOVE:
		pt = pMS->pt;
		InvalidateRect(hMainWnd, 0, TRUE);
		break;
	}
	return CallNextHookEx(hHook, nCode, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		hMainWnd = hWnd;
		hHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, NULL, 0);
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			TCHAR szText[256];
			wsprintf(szText, TEXT("x=%d, y=%d"), pt.x, pt.y);
			TextOut(hdc, 10, 10, szText, lstrlen(szText));
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_DESTROY:
		UnhookWindowsHookEx(hHook);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("DLLを使わないグローバルマウスフック"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}