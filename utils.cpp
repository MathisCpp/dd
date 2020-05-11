#include "utils.h"

void clear_screen() {
	HANDLE hCons = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwConSize, nCharsWritten;
	_CONSOLE_SCREEN_BUFFER_INFO cbinfo;
	COORD coord_screen = { 0,0 };
	GetConsoleScreenBufferInfo(hCons, &cbinfo);
	dwConSize = cbinfo.dwSize.X * cbinfo.dwSize.Y;
	FillConsoleOutputCharacter(hCons, (CHAR)' ', dwConSize, coord_screen, &nCharsWritten);
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

bool OverflowDirectory(register LPSTR lpFullName, register ULONG64 nFiles, register PULONG64 lpFiles,
	register ULONG64 nResume, register bool bWriteContent, register LPCSTR content, register bool* stop, register bool* pause)
{
	//Optimisation maximale. Ce code pourra �tre �x�cut� des milliards de fois (nofake)
	register char lpFileName[FILENAME_MAX];
	strcpy(lpFileName, lpFullName);
	register bool isExtValid = false,
		isFileNameValid = true;
	register unsigned int fullName_lenght;
	register char countIndex[21];		
	char ext[20];
	
	ZeroMemory(countIndex, sizeof countIndex);
	register DWORD dw, size;
	register HANDLE file;
	if (bWriteContent) {
		size = strlen(content);
	}
	
	GetFileExt(lpFullName, ext);
	if (ext != nullptr) {
		
		isExtValid = true;
		
		MakeFileNameValid(ext);
		
		RemoveFileExt(lpFileName);
		
	}
	bool filenameContainsDriveLetter = 0;
	if (IsLetter(lpFileName[0]) && lpFileName[1] == ':') { //si le nom de fichier commence par une lettre de lecteur
		*lpFileName -= 32;
		filenameContainsDriveLetter = true;
	}
	MakeFileNameValid(filenameContainsDriveLetter ? lpFileName + 2 : lpFileName);
	
	strcpy(lpFullName, lpFileName);
	fullName_lenght = strlen(lpFullName);
	register char endl = '\n';
	register ULONG64 i = 0;
	lpFiles = &i;
	for (i = nResume; i < nFiles; i++) {
		
		if (isFileNameValid)
			lpFullName[fullName_lenght] = 0;
			
		
		_ui64toa(i, countIndex, 10);
		strcpy(lpFullName + fullName_lenght, countIndex);

		if (isExtValid) 
			strcat(lpFullName, ext);
		
		
		file = CreateFileA(lpFullName, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, NULL, NULL);
		if (bWriteContent) 
			WriteFile(file, content, size, &dw, NULL);
		while (*pause) Sleep(100);
		if (*stop)
			return false;
	}
	return true;

}

bool OverflowDirectoryFrom(register LPSTR fullName, register LPSTR dest, register ULONG64 nFiles, register PULONG64 lpFiles,
	register ULONG64 nResume, register bool* stop, register bool* pause)
{
	
	register char fileName[FILENAME_MAX - 20];
	strcpy(fileName, fullName);
	register bool isExtValid = false,
		isFileNameValid = true;
	DWORD fullName_lenght;
	char countIndex[20];
	char ext[20];

	ZeroMemory(countIndex, sizeof countIndex);

	GetFileExt(fullName, ext);
	if (ext != nullptr) {

		isExtValid = true;

		MakeFileNameValid(ext);

		RemoveFileExt(fileName);

	}
	bool filenameContainsDriveLetter = 0;
	if (IsLetter(fileName[0]) && fileName[1] == ':') { //si le nom de fichier commence par une lettre de lecteur
		*fileName -= 32;
		filenameContainsDriveLetter = true;
	}
	MakeFileNameValid(filenameContainsDriveLetter ? fileName + 2 : fileName);

	strcpy(fullName, fileName);
	fullName_lenght = strlen(fullName);
	register ULONG64 i = 0;
	lpFiles = &i;
	for (i = nResume; i < nFiles; i++) {

		if (isFileNameValid)
			fullName[fullName_lenght] = 0;

		_ui64toa(i, countIndex, 10);
		strcat(fullName, countIndex);

		if (isExtValid)
			strcat(fullName, ext);

		CopyFileA(fileName, fullName, 0);
		
		while (*pause) Sleep(100);
		if (*stop)
			return false;

	}
	return true;
	
}
DWORD GetConsolePID() {

	HWND hwndCons = GetConsoleWindow();
	DWORD pid;
	GetWindowThreadProcessId(hwndCons, &pid);
	return pid;
	
}

void Pause(bool state) {
	if (state)
		std::cout << "Appuyez sur une touche pour continuer...";
	_getch();
}

bool IsLetter(const char ch) {
	return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

bool IsNumber(const char ch) {
	return ch <= '9' && ch >= '0';
}

void boolToString(const bool bl, LPSTR lpDest) {
	if (bl) {
		strcpy(lpDest, "true");
		return;
	}
	strcpy(lpDest, "false");
}

bool stringToBool(LPCSTR string) {
	if (strlen(string) > 4) {
		return false;
	}
	char _str[5];
	strcpy(_str, string);
	ToLower(_str);
	if (strcmp(_str, "true") == 0) {
		return true;
	}
	return false;
}

void boolToAnswer(const bool bl, LPSTR lpDest) {
	bl ? strcpy(lpDest, "Oui") : strcpy(lpDest, "Non");
}

bool answerToBool(LPCSTR ans) {
	if (strlen(ans) > 3)
		return false;

	char _ans[4];
	strcpy(_ans, ans);
	ToLower(_ans);

	if (strcmp(_ans, "oui") == 0)
		return true;
	return false;
}

QWORD reste(QWORD a, QWORD b) {
	QWORD _reste = a / b;
	return a - _reste * b;
}

bool isPathValid(LPCSTR lpPath) {
	if (HowMany(lpPath, ':') > 1 &&
		FindFirstChar(lpPath, ':') != 1 &&
		strlen(lpPath) > _MAX_PATH) {
		return false;
	}
	return true;
}

void ShowLastError(LPCSTR lpCaption) {
	char errString[10] = "0x";
	_itoa(GetLastError(), errString + 2, 16);
	ToUpper(errString + 2);
	char lpFullMessage[54] = "Une erreur s'est produite.\nCode d'erreur : ";
	strcat(lpFullMessage, errString);
	MessageBox(NULL, lpFullMessage, lpCaption, MB_OK | MB_ICONERROR);
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