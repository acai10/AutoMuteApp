#include <windows.h>
#include <stdio.h>
#include "config.h"
#include "worker.h"

int WINAPI WinMain(
    HINSTANCE hInst,
    HINSTANCE hPrevInst,
    LPSTR lpCmdLine,
    int nShowCmd
) {
    (void)hPrevInst; 
    (void)lpCmdLine;
    (void)nShowCmd;

    load_config(L"config.json");
    CreateThread(NULL, 0, worker_thread, NULL, 0, NULL);
    Sleep(INFINITE);
}
