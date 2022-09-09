#include <windows.h>
#include <stdio.h>

char buffer[1024] = "";
char ret_buffer[1024] = "";
char ret_error[1024] = "";

MCIERROR error;
HWND hWndNotify = NULL;
HANDLE hConsole;
const char g_szClassName[] = "myWindowClass";

int cmdInterp (void){
	scanf(" %[^\n]", buffer);
	error = mciSendStringA(buffer, ret_buffer, 1024, hWndNotify);
	printf("Buffer: %s\n", buffer);
	printf("Return: %s ", ret_buffer);
	mciGetErrorString (error, ret_error, 1024);
	printf("| %s\n", ret_error);
}

// Message handling window stuff:
LRESULT CALLBACK WndProc(HWND hWndNotify, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case MM_MCINOTIFY:
		{
			if (msg==MM_MCINOTIFY && wParam==MCI_NOTIFY_SUCCESSFUL){
				SetConsoleTextAttribute(hConsole, 0xA); // light green
				printf("Notify msg: Successful\n");
				SetConsoleTextAttribute(hConsole, 7); // default color
			}
			if (msg==MM_MCINOTIFY && wParam==MCI_NOTIFY_ABORTED){
				SetConsoleTextAttribute(hConsole, 0xC); // light red
				printf("Notify msg: Aborted\n");
				SetConsoleTextAttribute(hConsole, 7); // default color
			}
			if (msg==MM_MCINOTIFY && wParam==MCI_NOTIFY_FAILURE){
				SetConsoleTextAttribute(hConsole, 0xC); // light red
				printf("Notify msg: Failure\n");
				SetConsoleTextAttribute(hConsole, 7); // default color
			}
			if (msg==MM_MCINOTIFY && wParam==MCI_NOTIFY_SUPERSEDED){
				SetConsoleTextAttribute(hConsole, 0xE); // light yellow
				printf("Notify msg: Superseded\n");
				SetConsoleTextAttribute(hConsole, 7); // default color
			}
		}
		break;

		case WM_CLOSE:
			DestroyWindow(hWndNotify);
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
		break;

		default:
			return DefWindowProc(hWndNotify, msg, wParam, lParam);

	}
	return 0;
}

// WinMain for message handling:
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	MSG Msg;

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc)){
		MessageBox(NULL, "Window Registration Failed!", "Error!",
					MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hWndNotify = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"mciSendCmd tester wnd",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
		NULL, NULL, hInstance, NULL);

	if(hWndNotify == NULL){
		MessageBox(NULL, "Window Creation Failed!", "Error!",
					MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	SetTimer(hWndNotify, 1, 10, NULL); // Set a timer so we do not deadlock

	// Setup console
	AllocConsole();
	freopen("CONOUT$", "w", stdout); // Direct output to console
	freopen("CONIN$", "r", stdin); // Direct input to console
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // For text color updates
	
	SetConsoleTextAttribute(hConsole, 6); // yellow
	printf("NOTE: Certain commands like 'play' will disable text input.\n");
	printf("When that happens press the 'E' key to regain control.\n");
	printf("Type in a valid mciSendString and press ENTER:\n\n");

	SetConsoleTextAttribute(hConsole, 7); // default color
	cmdInterp();

	// Message loop and playback controls
	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		// E for enter string
		if (GetAsyncKeyState(0x45)){
			while (GetAsyncKeyState(0x45)){
				Sleep(10);
			}
			cmdInterp();
		}

		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}
