#include <Windows.h>
#include <Dbt.h>
#include <string>
#include <iostream>

// 函数声明
bool RenameDriveIfCPH(char driveLetter);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 注册窗口类
    const char CLASS_NAME[] = "USBRenameMonitorClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // 创建隐藏窗口
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

    // 注册设备通知
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

    HDEVNOTIFY hDevNotify = RegisterDeviceNotification(
        hWnd,
        &NotificationFilter,
        DEVICE_NOTIFY_WINDOW_HANDLE
    );

    // 消息循环
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// 如果U盘名称为"CPH"，重命名为"XPH"
bool RenameDriveIfCPH(char driveLetter) {
    char rootPath[] = { driveLetter, ':', '\\', '\0' };
    char volumeName[MAX_PATH + 1] = { 0 };

    // 获取U盘名称
    if (GetVolumeInformationA(rootPath, volumeName, MAX_PATH, NULL, NULL, NULL, NULL, 0)) {
        if (strcmp(volumeName, "CPH") == 0) {
            // 重命名U盘
            if (SetVolumeLabelA(rootPath, "XPH")) {
                // 记录到事件查看器（可选）
                // OutputDebugStringA(("Renamed " + std::string(1, driveLetter) + ": to XPH").c_str());
                return true;
            }
        }
    }
    return false;
}

// 窗口消息处理
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DEVICECHANGE) {
        if (wParam == DBT_DEVICEARRIVAL) {  // 设备插入
            DEV_BROADCAST_VOLUME* vol = (DEV_BROADCAST_VOLUME*)lParam;
            if (vol && vol->dbcv_devicetype == DBT_DEVTYP_VOLUME) {
                // 遍历所有可能的驱动器
                for (char drive = 'C'; drive <= 'Z'; drive++) {
                    // 检查哪个驱动器被激活
                    if (vol->dbcv_unitmask & (1 << (drive - 'A'))) {
                        RenameDriveIfCPH(drive);
                    }
                }
            }
        }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}