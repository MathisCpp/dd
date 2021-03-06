#include "utils.h"

void ClearConsole() {
	HANDLE hCons = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwConSize, nCharsWritten;
	_CONSOLE_SCREEN_BUFFER_INFO cbinfo;
	COORD coord_screen = { 0,0 };
	GetConsoleScreenBufferInfo(hCons, &cbinfo);
	dwConSize = cbinfo.dwSize.X * cbinfo.dwSize.Y;
	FillConsoleOutputCharacter(hCons, (CHAR)'\0', dwConSize, coord_screen, &nCharsWritten);
	FillConsoleOutputAttribute(hCons, cbinfo.wAttributes, dwConSize, coord_screen, &nCharsWritten);
	SetConsoleCursorPosition(hCons, coord_screen);
}

bool FileExist(LPCSTR lpFileName)
{
	HANDLE hFile = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	DWORD dwLastError = GetLastError();
	bool exist = dwLastError != ERROR_FILE_NOT_FOUND && dwLastError != ERROR_INVALID_NAME && dwLastError != ERROR_PATH_NOT_FOUND;
	CloseHandle(hFile);
	return exist;
}

DWORD GetConsolePID() {

	HWND hwndCons = GetConsoleWindow();
	DWORD pid;
	GetWindowThreadProcessId(hwndCons, &pid);
	return pid;
	
}

byte RotateBitsLeft(byte n, byte subject)
{
	return (subject << n) | (subject >> (8 - n));
}

byte RotateBitsRight(byte n, byte subject)
{
	return (subject >> n) | (subject << (8 - n));
}

bool IsLetter(const char ch) {
	return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

bool IsNumber(const char ch) {
	return ch <= '9' && ch >= '0';
}

QWORD Remainder(QWORD a, QWORD b) {
	QWORD qwRemainder = a / b;
	return a - qwRemainder * b;
}

bool IsPathValid(LPCSTR lpPath) {
	if (GetChars(lpPath, ':') > 1 &&
		FindFirstChar(lpPath, ':') != 1 &&
		strlen(lpPath) > MAX_PATH) {
		return false;
	}
	return true;
}

void ShowLastError(LPCSTR lpCaption) {
	char szFullMessage[256];
	GetErrorString(szFullMessage, sizeof szFullMessage);
	MessageBoxA(NULL, szFullMessage, lpCaption, MB_OK | MB_ICONERROR);
}

DWORD SetFileSize(HANDLE hFile, QWORD qwSize) {
	LARGE_INTEGER li;
	li.QuadPart = qwSize;
	if (!SetFilePointerEx(hFile, li, NULL, FILE_BEGIN)) {
		return GetLastError();
	}
	if (!SetEndOfFile(hFile)) {
		return GetLastError();
	}
	return NO_ERROR;
}

QWORD GetFilePointer(HANDLE hFile) {
	LARGE_INTEGER li, _li = { 0 };
	SetFilePointerEx(hFile, _li, &li, FILE_CURRENT);
	return li.QuadPart;
}

void GetErrorString(LPSTR lpszError, DWORD dwBufferLength) {
	DWORD dwError = GetLastError();
	DWORD dwMessageLength = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwError, LANG_USER_DEFAULT, lpszError, dwBufferLength, NULL);
	if (dwMessageLength == 0) {
		_ultoa(dwError, lpszError, 10);
		strcat(lpszError, "\n");
	}
}

WORD GetLine(char* lpRawText, char* lpLineBuf, LPDWORD lpdwIndex, DWORD dwFileSize) {

	while (lpRawText[*lpdwIndex] == '\n' || lpRawText[*lpdwIndex] == '\r' || lpRawText[*lpdwIndex] == ' ' || lpRawText[*lpdwIndex] == '\t') {
		(*lpdwIndex)++;
	}
	if (*lpdwIndex >= dwFileSize) {
		return 0;
	}
	WORD wLength = 0;
	while (lpRawText[*lpdwIndex] != '\n' && lpRawText[*lpdwIndex] != '\r') {
		*(lpLineBuf++) = lpRawText[*lpdwIndex];
		(*lpdwIndex)++;
		wLength++;
		if (*lpdwIndex >= dwFileSize) {
			break;
		}
	}
	*(lpLineBuf) = 0;
	return wLength;
}
