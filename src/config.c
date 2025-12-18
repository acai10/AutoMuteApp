#include "config.h"
#include "cJSON.h"
#include <windows.h>
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>

Config g_config;

// Lade eine UTF-8 JSON Datei
static char* load_file_utf8(const wchar_t* path)
{
    FILE* f = _wfopen(path, L"rb, ccs=UTF-8");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char* buf = malloc(size + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }

    size_t read_count = fread(buf, 1, size, f);
    fclose(f);
    if (read_count != (size_t)size) {
        free(buf);
        return NULL;
    }

    buf[size] = '\0';
    return buf;
}

void load_config(const wchar_t* path)
{
    // Defaults
    wcscpy(g_config.target_app, L"Discord.exe");
    g_config.poll_interval_ms = 2000;
    g_config.device_count = 0;

    char* json_utf8 = load_file_utf8(path);
    if (!json_utf8) return;

    cJSON* root = cJSON_Parse(json_utf8);
    free(json_utf8);

    if (!root) return;

    // poll_interval_ms
    cJSON* j_poll = cJSON_GetObjectItemCaseSensitive(root, "poll_interval_ms");
    if (cJSON_IsNumber(j_poll))
        g_config.poll_interval_ms = j_poll->valueint;

    // target_application
    cJSON* j_target = cJSON_GetObjectItemCaseSensitive(root, "target_application");
    if (cJSON_IsString(j_target) && j_target->valuestring)
        mbstowcs(g_config.target_app, j_target->valuestring, MAX_NAME);

    // devices array
    cJSON* j_devices = cJSON_GetObjectItemCaseSensitive(root, "devices");
    if (cJSON_IsArray(j_devices)) {
        int count = 0;
        cJSON* item = NULL;
        cJSON_ArrayForEach(item, j_devices) {
            if (count >= MAX_DEVICES) break;

            cJSON* j_name = cJSON_GetObjectItemCaseSensitive(item, "name_contains");
            if (cJSON_IsString(j_name) && j_name->valuestring) {
                wchar_t dev_name[MAX_NAME];
                mbstowcs(dev_name, j_name->valuestring, MAX_NAME);
                wcscpy(g_config.devices[count++], dev_name);
            }
        }
        g_config.device_count = count;
    }

    cJSON_Delete(root);
}
