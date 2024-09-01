#include <windows.h>
#include <stdint.h>
#include <inttypes.h>
#include "native.h"
#include "bofdefs.h"
#include "beacon.h"

// Define printf format specifier based on architecture
#if _WIN64
#define PFORMAT "%lld"
#else
#define PFORMAT "%d"
#endif

void go(char* args, int length)
{
    // Extract filename
    datap parser;
    BeaconDataParse(&parser, args, length);
    char* filename = BeaconDataExtract(&parser, NULL);

    // Open handle to file
    HANDLE hFile = CreateFileA(filename, 0, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if( hFile == INVALID_HANDLE_VALUE)
    {
        DWORD dwGLE = GetLastError();
        if (dwGLE == ERROR_FILE_NOT_FOUND || dwGLE == ERROR_PATH_NOT_FOUND)
            BeaconPrintf(CALLBACK_ERROR, "Cannot locate %s!", filename);
        else
            BeaconPrintf(CALLBACK_ERROR, "CreateFile failed! GLE: %d", GetLastError());
        return;
    }

    // Allocate buffer + call NtQueryInformationFile
    int iMaxProc = 16;
    int iAlloc = sizeof(INT_PTR) + (sizeof(INT_PTR) * iMaxProc);
    void* pPidsBuffer = calloc(iAlloc, sizeof(char));
    IO_STATUS_BLOCK ioStatus = {0};
    NTSTATUS ntResult = NtQueryInformationFile(hFile, &ioStatus, pPidsBuffer, iAlloc, 47);

    // If our buffer wasn't big enough, reallocate and try again. 
    while (ntResult == STATUS_INFO_LENGTH_MISMATCH)
    {
        iMaxProc *= 2;
        iAlloc = sizeof(INT_PTR) + (sizeof(INT_PTR) * iMaxProc);
        free(pPidsBuffer);
        pPidsBuffer = calloc(iAlloc, sizeof(char));
        memset(&ioStatus, 0, sizeof(IO_STATUS_BLOCK));
        ntResult = NtQueryInformationFile(hFile, &ioStatus, pPidsBuffer, iAlloc, 47);
    }

    // Close file handle
    CloseHandle(hFile);

    if (ntResult != STATUS_SUCCESS)
    {
        BeaconPrintf(CALLBACK_ERROR, "NtQueryInformationFile failed! GLE: %d", GetLastError());
        free(pPidsBuffer);
        return;
    }

    // Set up output buffer
    formatp buffer;
    BeaconFormatAlloc(&buffer, 12388);

    // Grab number of PIDS in list from first value in buffer
    INT_PTR numPids = *(INT_PTR *)(pPidsBuffer);
    BeaconFormatPrintf(&buffer, "Number of processes with open handles to file: " PFORMAT "\n", numPids);

    // Iterate over each PID and resolve the associated image
    for (int i = 0; i < numPids * sizeof(INT_PTR); i += sizeof(INT_PTR))
    {
        // Extract PID
        INT_PTR pid = *(INT_PTR *)(pPidsBuffer + sizeof(INT_PTR) + i);

        // Set index
        int index = i / sizeof(INT_PTR) + 1;

        // Open PID
        HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, (DWORD)pid);
        if (hProc)
        {
            // Resolve image associated with PID
            CHAR imageName[MAX_PATH] = {0};
            DWORD bufLen = MAX_PATH;
            if (QueryFullProcessImageNameA(hProc, 0, imageName, &bufLen))
                BeaconFormatPrintf(&buffer, "%d. PID " PFORMAT " -> %s\n", index, pid, imageName);
            else
                BeaconFormatPrintf(&buffer, "%d. PID " PFORMAT " -> ERROR UNABLE TO RESOLVE PROCESS NAME! GLE: %d\n", index, pid, GetLastError());

            // Close handle
            CloseHandle(hProc);
        }
        else
            BeaconFormatPrintf(&buffer, "%d. PID " PFORMAT " -> ERROR UNABLE TO OPEN PROCESS! GLE: %d\n", index, pid, GetLastError());
    }

    // Return output
    BeaconPrintf(CALLBACK_OUTPUT, "%s\n", BeaconFormatToString(&buffer, NULL));

    // Free buffers
    BeaconFormatFree(&buffer);
    memset(pPidsBuffer, 0, ioStatus.Information);
    free(pPidsBuffer);
}