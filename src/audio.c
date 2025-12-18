#define COBJMACROS
#include <stdio.h>
#include "audio.h"
#include "config.h"

#include <initguid.h>
#include <windows.h>
#include <audiopolicy.h>
#include <functiondiscoverykeys_devpkey.h>
#include <psapi.h>

static BOOL device_matches(IMMDevice* dev)
{
    IPropertyStore* store;
    PROPVARIANT v;
    BOOL match = FALSE;

    PropVariantInit(&v);
    if (SUCCEEDED(IMMDevice_OpenPropertyStore(dev, STGM_READ, &store)))
    {
        if (SUCCEEDED(IPropertyStore_GetValue(store, &PKEY_Device_FriendlyName, &v)))
        {
            for (int i = 0; i < g_config.device_count; i++)
                if (wcsstr(v.pwszVal, g_config.devices[i]))
                    match = TRUE;
        }
        PropVariantClear(&v);
        IPropertyStore_Release(store);
    }
    return match;
}

static void mute_discord_on_device(IMMDevice* dev)
{
    IAudioSessionManager2* mgr;
    IAudioSessionEnumerator* en;

    if (FAILED(IMMDevice_Activate(dev, &IID_IAudioSessionManager2,
        CLSCTX_ALL, NULL, (void**)&mgr)))
        return;

    if (FAILED(IAudioSessionManager2_GetSessionEnumerator(mgr, &en)))
        goto out;

    int count;
    IAudioSessionEnumerator_GetCount(en, &count);

    for (int i = 0; i < count; i++)
    {
        IAudioSessionControl* ctl;
        IAudioSessionControl2* ctl2;
        ISimpleAudioVolume* vol;
        DWORD pid;
        wchar_t name[MAX_PATH];

        IAudioSessionEnumerator_GetSession(en, i, &ctl);
        IAudioSessionControl_QueryInterface(
            ctl, &IID_IAudioSessionControl2, (void**)&ctl2);

        IAudioSessionControl2_GetProcessId(ctl2, &pid);
        HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

        if (h)
        {
            GetModuleBaseNameW(h, NULL, name, MAX_PATH);
            if (!_wcsicmp(name, g_config.target_app))
            {
                IAudioSessionControl_QueryInterface(
                    ctl, &IID_ISimpleAudioVolume, (void**)&vol);

                BOOL muted;
                ISimpleAudioVolume_GetMute(vol, &muted);
                if (!muted)
                    ISimpleAudioVolume_SetMute(vol, TRUE, NULL);

                ISimpleAudioVolume_Release(vol);
            }
            CloseHandle(h);
        }

        IAudioSessionControl2_Release(ctl2);
        IAudioSessionControl_Release(ctl);
    }

out:
    IAudioSessionEnumerator_Release(en);
    IAudioSessionManager2_Release(mgr);
}

void scan_devices_and_mute(void)
{
    IMMDeviceEnumerator* e;
    IMMDeviceCollection* c;

    CoCreateInstance(&CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL,
        &IID_IMMDeviceEnumerator, (void**)&e);

    IMMDeviceEnumerator_EnumAudioEndpoints(
        e, eAll, DEVICE_STATE_ACTIVE, &c);

    UINT n;
    IMMDeviceCollection_GetCount(c, &n);

    for (UINT i = 0; i < n; i++)
    {
        IMMDevice* d;
        IMMDeviceCollection_Item(c, i, &d);
        if (device_matches(d))
            mute_discord_on_device(d);
        IMMDevice_Release(d);
    }

    IMMDeviceCollection_Release(c);
    IMMDeviceEnumerator_Release(e);
}
