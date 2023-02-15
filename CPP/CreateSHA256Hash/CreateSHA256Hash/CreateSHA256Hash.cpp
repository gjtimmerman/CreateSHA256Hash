// CreateSHA256Hash.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "CreateSHA256Hash.h"

#define MAX_LOADSTRING 100
#define MAX_FILENAME 900
#define MAX_READBUFFER 0x100
#define HASH_LENGTH 32

// Global Variables:
HINSTANCE hInst;                                // current instance

LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    DialogBox(hInst, MAKEINTRESOURCE(IDD_FORMVIEW), nullptr, &WndProc);

    ExitProcess(0);
    return 0;
}



int evaluateBStatus(NTSTATUS status)
{
    if (status == ERROR_SUCCESS)
        return 0;
    switch (status)
    {
    case STATUS_INVALID_HANDLE:
        return 1;
    case STATUS_INVALID_PARAMETER:
        return 1;
    case STATUS_NO_MEMORY:
        return 1;
    default:
        return 1;
    }

}

void CalculateHashOfFile(wchar_t *fileName, unsigned char *hash, size_t hashLength)
{
    HANDLE inputFile;
    inputFile = CreateFile(fileName, FILE_GENERIC_READ,0,nullptr,0,0,0);
    BCRYPT_ALG_HANDLE algHandle;
    NTSTATUS bStatus;
    bStatus = BCryptOpenAlgorithmProvider(&algHandle, BCRYPT_SHA256_ALGORITHM, NULL, 0);
    evaluateBStatus(bStatus);
    ULONG actualHashLength;
    ULONG cbHashLength = sizeof(ULONG);
    ULONG cbHashLengthReturned;
    bStatus = BCryptGetProperty(algHandle, BCRYPT_HASH_LENGTH, (PUCHAR)&actualHashLength, cbHashLength, &cbHashLengthReturned, 0);
    evaluateBStatus(bStatus);
    if (actualHashLength != hashLength)
    {
        return;
    }
    BCRYPT_HASH_HANDLE hashHandle;
    bStatus = BCryptCreateHash(algHandle, &hashHandle, NULL, 0, NULL, 0, 0);
    evaluateBStatus(bStatus);
    bStatus = BCryptCloseAlgorithmProvider(algHandle, 0);
    evaluateBStatus(bStatus);
    unsigned char buffer[MAX_READBUFFER];
    DWORD bytesRead;
    ReadFile(inputFile, buffer, MAX_READBUFFER, &bytesRead,nullptr);
    while (bytesRead == MAX_READBUFFER)
    {
        bStatus = BCryptHashData(hashHandle, buffer, MAX_READBUFFER, 0);
        evaluateBStatus(bStatus);
//        inputFile.read((char *)buffer, MAX_READBUFFER);
        ReadFile(inputFile, buffer, MAX_READBUFFER, &bytesRead, nullptr);

    }
   if (bytesRead > 0)
    {
        bStatus = BCryptHashData(hashHandle, buffer, (ULONG)bytesRead, 0);
        evaluateBStatus(bStatus);
    }
    CloseHandle(inputFile);
    bStatus = BCryptFinishHash(hashHandle, hash, (ULONG)hashLength, 0);
    evaluateBStatus(bStatus);
    bStatus = BCryptDestroyHash(hashHandle);
    evaluateBStatus(bStatus);
    bStatus = BCryptCloseAlgorithmProvider(algHandle, 0);
}


OPENFILENAME myOpenFileSettings;


INT_PTR CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    DWORD error;
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hWnd, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam == IDC_BUTTON1))
        {
            wchar_t fileName[MAX_FILENAME+1];
            myOpenFileSettings.hwndOwner = hWnd;
            myOpenFileSettings.hInstance = hInst;
            myOpenFileSettings.lStructSize = sizeof(OPENFILENAME);
            myOpenFileSettings.nMaxFile = MAX_FILENAME;
            myOpenFileSettings.lpstrFile = fileName;
            myOpenFileSettings.lpstrFilter = L"Executable Files\0*.exe\0All Files\0*.*\0";
            if (GetOpenFileName(&myOpenFileSettings) != 0)
            {
                SetDlgItemText(hWnd, IDC_EDIT1, myOpenFileSettings.lpstrFile);
            }
            else
            {
                error = CommDlgExtendedError();
            }
        }
        else if (LOWORD(wParam == IDC_BUTTON2))
        {
            wchar_t fileName[MAX_FILENAME+1];
            GetDlgItemText(hWnd, IDC_EDIT1, fileName, MAX_FILENAME);
            unsigned char hash[HASH_LENGTH];
            CalculateHashOfFile(fileName, hash, HASH_LENGTH);
            wchar_t hashStr[(HASH_LENGTH) + 1 * 2];
            hashStr[0] = '\0';
            for (int i = 0; i < HASH_LENGTH; i++)
            {
                wchar_t tmp[4];
                wsprintf(tmp, L"%2x",hash[i]);
                wcscat(hashStr, tmp);
            }
            SetDlgItemText(hWnd, IDC_EDIT2, hashStr);
        }
        break;

    }
    return (INT_PTR)FALSE;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
