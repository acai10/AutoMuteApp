#include "worker.h"
#include "audio.h"
#include "config.h"
#include <windows.h>

DWORD WINAPI worker_thread(LPVOID param)
{
    (void)param;
    CoInitialize(NULL);

    while (1)
    {
        scan_devices_and_mute();
        Sleep(g_config.poll_interval_ms);
    }
}
