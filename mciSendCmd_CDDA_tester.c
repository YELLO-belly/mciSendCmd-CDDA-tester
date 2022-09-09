/*
	THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
	ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
	PARTICULAR PURPOSE.
*/

// A simple console program to test the mciSendCommand for the mci cdaudio device.

#include <windows.h>
#include <stdio.h>
#pragma comment(lib, "winmm.lib") //For MSVC

int notifymsg=0;
int from = 0;
int to = 0;
int timeformat = 0;
int length_pos_q = 0;
int track = 0;

HANDLE hConsole;
const char g_szClassName[] = "myWindowClass";

HWND hWndNotify = NULL;
	UINT wDeviceID;
	DWORD dwReturn;
	MCI_OPEN_PARMS mciOpenParms;
	MCI_SET_PARMS mciSetParms;
	MCI_PLAY_PARMS mciPlayParms;
	MCI_STATUS_PARMS mciStatusParms;

DWORD statusLengthPosCDaudio(void)
{
	mciStatusParms.dwItem = MCI_STATUS_LENGTH;
	mciStatusParms.dwTrack = track;

	//Length
	if (dwReturn = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID) &mciStatusParms)){
		printf("MCI ERROR CODE: %d\n",dwReturn);
		return (dwReturn);
	}
	else if(timeformat == 2){
		printf("Length: %d\n",mciStatusParms.dwReturn);
	}
	else{
		printf("Full media length: m%d s%d f%d\n",MCI_MSF_MINUTE(mciStatusParms.dwReturn),
								MCI_MSF_SECOND(mciStatusParms.dwReturn),
								MCI_MSF_FRAME(mciStatusParms.dwReturn));
	}

	//Track lenght
	if (dwReturn = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD)(LPVOID) &mciStatusParms)){
		printf("MCI ERROR CODE: %d\n",dwReturn);
		return (dwReturn);
	}
	else if(timeformat == 2){
		printf("Track %d length: %d\n", track, mciStatusParms.dwReturn);
	}
	else{
		printf("Track %d length: m%d s%d f%d\n", track, MCI_MSF_MINUTE(mciStatusParms.dwReturn),
								MCI_MSF_SECOND(mciStatusParms.dwReturn),
								MCI_MSF_FRAME(mciStatusParms.dwReturn));
	}

	mciStatusParms.dwItem = MCI_STATUS_POSITION;
	if (dwReturn = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK, (DWORD)(LPVOID) &mciStatusParms)){
		printf("MCI ERROR CODE: %d\n",dwReturn);
		return (dwReturn);
	}
	else if(timeformat == 2){
		printf("Track %d position: %d\n", track,mciStatusParms.dwReturn);
	}
	else if(timeformat == 1){
		printf("Track %d position: t%d m%d s%d f%d\n", track, MCI_TMSF_TRACK(mciStatusParms.dwReturn),
								MCI_TMSF_MINUTE(mciStatusParms.dwReturn),
								MCI_TMSF_SECOND(mciStatusParms.dwReturn),
								MCI_TMSF_FRAME(mciStatusParms.dwReturn));
	}
	else{
		printf("Track %d position: m%d s%d f%d\n", track, MCI_MSF_MINUTE(mciStatusParms.dwReturn),
								MCI_MSF_SECOND(mciStatusParms.dwReturn),
								MCI_MSF_FRAME(mciStatusParms.dwReturn));
	}
	return (0);
}

DWORD statusTracksCDaudio(void)
{
	mciStatusParms.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
	if (dwReturn = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID) &mciStatusParms)){
		printf("MCI ERROR CODE: %d\n",dwReturn);
		return (dwReturn);
	}
	else{
		SetConsoleTextAttribute(hConsole, 6); // yellow
		printf("Number of tracks: %d\n", mciStatusParms.dwReturn);
		SetConsoleTextAttribute(hConsole, 7); // default color
	}
	return (0);
}

DWORD statusModeCDaudio(void)
{
	mciStatusParms.dwItem = MCI_STATUS_MODE;
	if (dwReturn = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID) &mciStatusParms)){
		printf("MCI ERROR CODE: %d\n",dwReturn);
		return (dwReturn);
	}
	else if(mciStatusParms.dwReturn == MCI_MODE_PLAY){
		printf("Mode: Playing...\n");
	}
	else if(mciStatusParms.dwReturn == MCI_MODE_STOP){
		printf("Mode: Stopped...\n");
	}
	else if(mciStatusParms.dwReturn == MCI_MODE_PAUSE){
		printf("Mode: Paused...\n");
	}
	else if(mciStatusParms.dwReturn == MCI_MODE_NOT_READY){
		printf("Mode: Not ready...\n");
	}
	else if(mciStatusParms.dwReturn == MCI_MODE_OPEN){
		printf("Mode: Open...\n");
	}
	//Omitted commands:
	//MCI_MODE_RECORD
	//MCI_MODE_SEEK
	return (0);
}

DWORD statusPosCDaudio(void)
{
	mciStatusParms.dwItem = MCI_STATUS_POSITION;
	if (dwReturn = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID) &mciStatusParms)){
		printf("MCI ERROR CODE: %d\n",dwReturn);
		return (dwReturn);
	}
	else if(timeformat == 2){
		printf("Position: %d\n",mciStatusParms.dwReturn);
	}
	else if(timeformat == 1){
		printf("Position: t%d m%d s%d f%d\n",MCI_TMSF_TRACK(mciStatusParms.dwReturn),
								MCI_TMSF_MINUTE(mciStatusParms.dwReturn),
								MCI_TMSF_SECOND(mciStatusParms.dwReturn),
								MCI_TMSF_FRAME(mciStatusParms.dwReturn));
	}
	else{
		printf("Position: m%d s%d f%d\n",MCI_MSF_MINUTE(mciStatusParms.dwReturn),
								MCI_MSF_SECOND(mciStatusParms.dwReturn),
								MCI_MSF_FRAME(mciStatusParms.dwReturn));
	}
	return (0);
}

DWORD stopCDaudio(void)
{
	// Stop playback with MCI_STOP

	if (dwReturn = mciSendCommand(wDeviceID, MCI_STOP, 0, (DWORD)(LPVOID) &mciPlayParms)){
		printf("MCI ERROR CODE: %d\n",dwReturn);
		return (dwReturn);
	}
	return (0);
}

DWORD closeCDaudio(void)
{
	// Close device handle with MCI_CLOSE

	if (dwReturn = mciSendCommand(wDeviceID, MCI_CLOSE, 0, (DWORD)(LPVOID) &mciPlayParms)){
		printf("MCI ERROR CODE: %d\n",dwReturn);
		return (dwReturn);
	}
	return (0);
}

DWORD pauseCDaudio(void)
{
	// Pause playback with MCI_PAUSE

	if (dwReturn = mciSendCommand(wDeviceID, MCI_PAUSE, 0, (DWORD)(LPVOID) &mciPlayParms)){
		printf("MCI ERROR CODE: %d\n",dwReturn);
		return (dwReturn);
	}
	return (0);
}

DWORD resumeCDaudio(void)
{
	// Resume playback with MCI_PLAY NULL
	// NOTE: MCI_RESUME was not available in Win9x for cdaudio devices.

	if (dwReturn = mciSendCommand(wDeviceID, MCI_PLAY, 0, (DWORD)(LPVOID) &mciPlayParms)){
		printf("MCI ERROR CODE: %d\n",dwReturn);
		return (dwReturn);
	}
	return (0);
}

DWORD resumeCDaudioto(HWND hWndNotify, BYTE bTrack)
{
	// Resume playback with MCI_PLAY TO
	if(!timeformat)mciPlayParms.dwTo = MCI_MAKE_MSF(bTrack, 0, 0);
	if(timeformat == 1)mciPlayParms.dwTo = MCI_MAKE_TMSF(bTrack, 0, 0, 0);
	if(timeformat == 2)mciPlayParms.dwTo = to;
	mciPlayParms.dwCallback = (DWORD) hWndNotify;

	if (notifymsg){
		if (dwReturn = mciSendCommand(wDeviceID, MCI_PLAY, MCI_TO | MCI_NOTIFY, (DWORD)(LPVOID) &mciPlayParms)){
			printf("MCI ERROR CODE: %d\n",dwReturn);
			return (dwReturn);
		}
	}
	else{
		if (dwReturn = mciSendCommand(wDeviceID, MCI_PLAY, MCI_TO, (DWORD)(LPVOID) &mciPlayParms)){
			printf("MCI ERROR CODE: %d\n",dwReturn);
			return (dwReturn);
		}
	}
	return (0);
}

// https://docs.microsoft.com/en-us/windows/win32/multimedia/playing-a-compact-disc-track
DWORD playCDaudio(HWND hWndNotify, BYTE aTrack, BYTE bTrack)
{
	// Open the CD audio device by specifying the device name.
	mciOpenParms.lpstrDeviceType = "cdaudio";
	//if (dwReturn = mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_SHAREABLE, (DWORD)(LPVOID) &mciOpenParms)){
	if (dwReturn = mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE, (DWORD)(LPVOID) &mciOpenParms)){
		// Failed to open device. Don't close it; just return error.
		printf("MCI ERROR CODE: %d\n",dwReturn);
		return (dwReturn);
	}

	// The device opened successfully; get the device ID.
	wDeviceID = mciOpenParms.wDeviceID;
	SetConsoleTextAttribute(hConsole, 6); // yellow
	printf("cdaudio device ID = %d\n",wDeviceID);
	SetConsoleTextAttribute(hConsole, 7); // default color

	// Default time format is minute/second/frame (MSF).

	if(timeformat == 1){
		// Set the time format to track/minute/second/frame (TMSF).
		mciSetParms.dwTimeFormat = MCI_FORMAT_TMSF;
		if (dwReturn = mciSendCommand(wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)(LPVOID) &mciSetParms)){
			printf("MCI ERROR CODE: %d\n",dwReturn);
			return (dwReturn);
		}
	}

	if(timeformat == 2){
		// Set the time format to Milliseoconds.
		mciSetParms.dwTimeFormat = MCI_FORMAT_MILLISECONDS;
		if (dwReturn = mciSendCommand(wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, (DWORD)(LPVOID) &mciSetParms)){
			printf("MCI ERROR CODE: %d\n",dwReturn);
			return (dwReturn);
		} 
	}

	// Set play params for MSF, TMSF, Milliseconds:
	mciPlayParms.dwFrom = 0L;
	mciPlayParms.dwTo = 0L;
	if(!timeformat)mciPlayParms.dwFrom = MCI_MAKE_MSF(aTrack, 0, 0);
	if(!timeformat)mciPlayParms.dwTo = MCI_MAKE_MSF(bTrack, 0, 0);
	if(timeformat == 1)mciPlayParms.dwFrom = MCI_MAKE_TMSF(aTrack, 0, 0, 0);
	if(timeformat == 1)mciPlayParms.dwTo = MCI_MAKE_TMSF(bTrack, 0, 0, 0);
	if(timeformat == 2)mciPlayParms.dwFrom = from;
	if(timeformat == 2)mciPlayParms.dwTo = to;
	mciPlayParms.dwCallback = (DWORD) hWndNotify;

	if(!from){
		dwReturn = mciSendCommand(wDeviceID, MCI_PLAY, 0, (DWORD)(LPVOID) &mciPlayParms);
		if(dwReturn!=0)printf("MCI ERROR CODE: %d\n",dwReturn);
		return (dwReturn);
	}

	if(!to){
		if (!notifymsg){
			dwReturn = mciSendCommand(wDeviceID, MCI_PLAY, MCI_FROM, (DWORD)(LPVOID) &mciPlayParms);
			if(dwReturn!=0)printf("MCI ERROR CODE: %d\n",dwReturn);
			return (dwReturn);
		}
		if (notifymsg){
			dwReturn = mciSendCommand(wDeviceID, MCI_PLAY, MCI_FROM | MCI_NOTIFY, (DWORD)(LPVOID) &mciPlayParms);
			if(dwReturn!=0)printf("MCI ERROR CODE: %d\n",dwReturn);
			return (dwReturn);
		}
	}

	if (!notifymsg){
		dwReturn = mciSendCommand(wDeviceID, MCI_PLAY, MCI_FROM | MCI_TO, (DWORD)(LPVOID) &mciPlayParms);
		if(dwReturn!=0)printf("MCI ERROR CODE: %d\n",dwReturn);
		return (dwReturn);
	}
	if (notifymsg){
		dwReturn = mciSendCommand(wDeviceID, MCI_PLAY, MCI_FROM | MCI_TO | MCI_NOTIFY, (DWORD)(LPVOID) &mciPlayParms);
		if(dwReturn!=0)printf("MCI ERROR CODE: %d\n",dwReturn);
		return (dwReturn);
	}
	return (0);
}

int cmdInterp (void)
{
	char buffer;

	printf("Slect time format: D=Default MSF, T=TMSF, M=Milliseconds: ");

	while(1){
		
		scanf("%c", &buffer);
		if(buffer == 'D' || buffer == 'd'){
			timeformat = 0;
			printf( "\nUsing default Minutes-SF Time format\n" );
			SetConsoleTextAttribute(hConsole, 8); // gray
			printf( "Ignoring Seconds & Frames for simplicity.\n\n" );
			SetConsoleTextAttribute(hConsole, 7); // default color
			break;
		}
		if(buffer == 'T' || buffer == 't'){
			timeformat = 1;
			printf( "\nTime Format set to Tracks-MSF\n" );
			SetConsoleTextAttribute(hConsole, 8); // gray
			printf( "Ignoring Minutes, Seconds & Frames for simplicity.\n\n" );
			SetConsoleTextAttribute(hConsole, 7); // default color
			break;
		}
		if(buffer == 'M' || buffer == 'm'){
			timeformat = 2;
			printf( "\nTime Format set to Milliseconds\n" );
			SetConsoleTextAttribute(hConsole, 8); // gray
			printf( "First track starts in 2001 milliseconds.\n\n" );
			SetConsoleTextAttribute(hConsole, 7); // default color
			break;
		}
	}

	//Query track length & track position
	printf("Query for track length and position? (Y)es (N)o: ");
	while(1){
		scanf("%c", &buffer);
		if(buffer == 'N' || buffer == 'n'){
			break;
		}
		if(buffer == 'Y' || buffer == 'y'){
			length_pos_q = 1;
			break;
		}
	}
	if(length_pos_q){
		printf("Input track no. for length and position query: ");
		scanf("%d", &track);
		SetConsoleTextAttribute(hConsole, 8); // gray
		printf("Selected track: %d\n\n" ,track);
		SetConsoleTextAttribute(hConsole, 7); // default color
	}

	//Query play from-to and notify message
	printf("0 = MCI_PLAY NULL.\n");
	printf("Input play from number: ");
	scanf("%d", &from);
	SetConsoleTextAttribute(hConsole, 8); // gray
	printf("Play from: %d\n\n" ,from);
	SetConsoleTextAttribute(hConsole, 7); // default color

	if(from){
		printf("0 = MCI_TO NULL.\n");
		printf("Input play to number: ");
		scanf("%d", &to);
		SetConsoleTextAttribute(hConsole, 8); // gray
		printf("Play to: %d\n\n" ,to);
		SetConsoleTextAttribute(hConsole, 7); // default color

		printf("Notify when done? (Y)es (N)o: ");
		while(1){
			scanf("%c", &buffer);
			if(buffer == 'N' || buffer == 'n'){
				notifymsg=0;
				system("cls");
				break;
			}
			if(buffer == 'Y' || buffer == 'y'){
				notifymsg=1;
				system("cls");
				SetConsoleTextAttribute(hConsole, 9); // light blue
				printf("Notify flag is set!\n");
				SetConsoleTextAttribute(hConsole, 7); // default color
				break;
			}
		}
	}

	printf("During playback: P=PAUSE, R=RESUME, S=STOP, Q=CLOSE, E=modE O=pOsition\n");
	SetConsoleTextAttribute(hConsole, 8); // gray
	printf("In Win9x you need to have the console window out of focus (inactive)\n"); 
	printf("for key input to work!\n\n");
	SetConsoleTextAttribute(hConsole, 7); // default color
	playCDaudio(hWndNotify, from, to);
	return 0;
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
	cmdInterp();
	statusTracksCDaudio(); // Get number of tracks
	if(length_pos_q){
		statusLengthPosCDaudio();
		length_pos_q = 0;
		track = 0;
	}
	SetConsoleTextAttribute(hConsole, 6); // yellow
	printf("Playing from: %d to: %d\n", from, to);
	if(!timeformat)printf("Time Format: MSF\n\n");
	if(timeformat == 1)printf("Time Format: TMSF\n\n");
	if(timeformat == 2)printf("Time Format: MILLISECONDS\n\n");
	SetConsoleTextAttribute(hConsole, 7); // default color

	// Message loop and playback controls
	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		// E for modE
		if (GetAsyncKeyState(0x45)){
			while (GetAsyncKeyState(0x45)){
				Sleep(10);
			}
			statusModeCDaudio();
		}

		// O for pOsition
		if (GetAsyncKeyState(0x4F)){
			while (GetAsyncKeyState(0x4F)){
				Sleep(10);
			}
			statusPosCDaudio();
		}

		// S for Stop
		if (GetAsyncKeyState(0x53)){
			while (GetAsyncKeyState(0x53)){
				Sleep(10);
			}
			stopCDaudio();
			printf( "MCI_STOP\n" );
		}

		// P for Pause
		if (GetAsyncKeyState(0x50)){
			while (GetAsyncKeyState(0x50)){
				Sleep(10);
			}
			pauseCDaudio();
			printf( "MCI_PAUSE\n" );
		}

		// R for resume
		if (GetAsyncKeyState(0x52)){
			while (GetAsyncKeyState(0x52)){
				Sleep(10);
			}
			if(!to){
				resumeCDaudio();
				printf( "MCI_PLAY NULL resume\n" );
			}
			else{
				resumeCDaudioto(hWndNotify, to);
				printf( "MCI_PLAY to: %d resume\n", to);
			}
		}

		// Why Q instead of C for MCI_CLOSE?
		// In Win9x for some reason the C key gets registered even 
		// when it is not pressed triggering a false MCI_CLOSE.
		// Possibly something to do with "C"onsole or freopen???
		if (GetAsyncKeyState(0x51)){
			while (GetAsyncKeyState(0x51)){
				Sleep(10);
			}
				closeCDaudio();
				system("cls");
				printf( "MCI_CLOSE\n" );
				cmdInterp();
				statusTracksCDaudio();
				if(length_pos_q){
					statusLengthPosCDaudio();
					length_pos_q = 0;
					track = 0;
				}
				SetConsoleTextAttribute(hConsole, 6); // yellow
				printf("Playing from: %d to: %d\n", from, to);
				if(!timeformat)printf("Time Format: MSF\n\n");
				if(timeformat == 1)printf("Time Format: TMSF\n\n");
				if(timeformat == 2)printf("Time Format: MILLISECONDS\n\n");
				SetConsoleTextAttribute(hConsole, 7); // default color
		}

		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}
