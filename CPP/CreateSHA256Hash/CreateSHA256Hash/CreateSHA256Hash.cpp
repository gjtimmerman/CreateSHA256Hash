// CreateSHA256Hash.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "CreateSHA256Hash.h"

#define MAX_LOADSTRING 100
#define MAX_FILENAME 1024
#define MAX_READBUFFER 0x1000
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


    return 0;
}

int evaluateBStatus(NTSTATUS status)
{
    if (status == ERROR_SUCCESS)
        return 0;
    switch (status)
    {
    case STATUS_INVALID_HANDLE:
        throw std::runtime_error("Cryptographic function returned error code: STATUS_INVALID_HANDLE");
    case STATUS_INVALID_PARAMETER:
        throw std::runtime_error("Cryptographic function returned error code: STATUS_INVALID_PARAMETER");
    case STATUS_NO_MEMORY:
        throw std::runtime_error("Cryptographic function returned error code: STATUS_NO_MEMORY");
    default:
        throw std::runtime_error("Cryptographic function returned unknown error code");
    }

}

void CalculateHashOfFile(wchar_t *fileName, unsigned char *hash, size_t hashLength)
{
    std::ifstream inputFile;
    inputFile.open(fileName,std::ios::in | std::ios::binary);
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
        throw std::domain_error("Wrong hash size!");
    }
    BCRYPT_HASH_HANDLE hashHandle;
    bStatus = BCryptCreateHash(algHandle, &hashHandle, NULL, 0, NULL, 0, 0);
    evaluateBStatus(bStatus);
    bStatus = BCryptCloseAlgorithmProvider(algHandle, 0);
    evaluateBStatus(bStatus);
    unsigned char buffer[MAX_READBUFFER];
    inputFile.read((char *)buffer, MAX_READBUFFER);
    while (!inputFile.eof())
    {
        bStatus = BCryptHashData(hashHandle, buffer, MAX_READBUFFER, 0);
        evaluateBStatus(bStatus);
        inputFile.read((char *)buffer, MAX_READBUFFER);
    }
    if (inputFile.gcount() != 0)
    {
        bStatus = BCryptHashData(hashHandle, buffer, (ULONG)inputFile.gcount(), 0);
        evaluateBStatus(bStatus);
    }
    inputFile.close();
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
            wchar_t fileName[MAX_FILENAME+1] = {};
            memset(&myOpenFileSettings, 0, sizeof(OPENFILENAME));
            myOpenFileSettings.hwndOwner = hWnd;
            myOpenFileSettings.hInstance = hInst;
            myOpenFileSettings.lStructSize = sizeof(OPENFILENAME);
            myOpenFileSettings.nMaxFile = MAX_FILENAME;
            myOpenFileSettings.lpstrFile = fileName;
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
            std::basic_stringstream<wchar_t> hashStr;
            for (int i = 0; i < HASH_LENGTH; i++)
            {
                hashStr.width(2);
                hashStr.fill('0');
                hashStr << std::hex << hash[i];
            }
            std::basic_string<wchar_t> result = hashStr.str();
            SetDlgItemText(hWnd, IDC_EDIT2, result.c_str());
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
