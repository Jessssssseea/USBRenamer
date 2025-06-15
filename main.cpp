#include <Windows.h>
#include <Dbt.h>
#include <string>
#include <iostream>

// ��������
bool RenameDriveIfCPH(char driveLetter);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // ע�ᴰ����
    const char CLASS_NAME[] = "USBRenameMonitorClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // �������ش���
    HWND hWnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "USB Rename Monitor",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hWnd == NULL) {
        return 0;
    }

    // ע���豸֪ͨ
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

    HDEVNOTIFY hDevNotify = RegisterDeviceNotification(
        hWnd,
        &NotificationFilter,
        DEVICE_NOTIFY_WINDOW_HANDLE
    );

    // ��Ϣѭ��
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// ���U������Ϊ"CPH"��������Ϊ"XPH"
bool RenameDriveIfCPH(char driveLetter) {
    char rootPath[] = { driveLetter, ':', '\\', '\0' };
    char volumeName[MAX_PATH + 1] = { 0 };

    // ��ȡU������
    if (GetVolumeInformationA(rootPath, volumeName, MAX_PATH, NULL, NULL, NULL, NULL, 0)) {
        if (strcmp(volumeName, "CPH") == 0) {
            // ������U��
            if (SetVolumeLabelA(rootPath, "XPH")) {
                // ��¼���¼��鿴������ѡ��
                // OutputDebugStringA(("Renamed " + std::string(1, driveLetter) + ": to XPH").c_str());
                return true;
            }
        }
    }
    return false;
}

// ������Ϣ����
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DEVICECHANGE) {
        if (wParam == DBT_DEVICEARRIVAL) {  // �豸����
            DEV_BROADCAST_VOLUME* vol = (DEV_BROADCAST_VOLUME*)lParam;
            if (vol && vol->dbcv_devicetype == DBT_DEVTYP_VOLUME) {
                // �������п��ܵ�������
                for (char drive = 'C'; drive <= 'Z'; drive++) {
                    // ����ĸ�������������
                    if (vol->dbcv_unitmask & (1 << (drive - 'A'))) {
                        RenameDriveIfCPH(drive);
                    }
                }
            }
        }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}