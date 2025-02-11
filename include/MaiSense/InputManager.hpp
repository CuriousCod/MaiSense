#ifndef MAISENSE_INPUT_MANAGER_HPP
#define MAISENSE_INPUT_MANAGER_HPP

#include <windows.h>
#include <vector>
#include <thread>

#include <MaiSense/Sensor.hpp>
#include <MaiSense/Config.hpp>

namespace MaiSense
{
    class InputController;
    class InputManager
    {
    private:
        static HHOOK   hHook;
        static HWND    hWnd;
        static Sensor *p1_sensor;
        static Sensor *p2_sensor;
        static std::vector<InputController*> controllers;

        static LRESULT WINAPI GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam);
        static DWORD   WINAPI HookGameInput();
        static HWND    WINAPI HookCreateWindowExA(
            DWORD     dwExStyle,
            LPCSTR    lpClassName,
            LPCSTR    lpWindowName,
            DWORD     dwStyle,
            int       X,
            int       Y,
            int       nWidth,
            int       nHeight,
            HWND      hWndParent,
            HMENU     hMenu,
            HINSTANCE hInstance,
            LPVOID    lpParam
        );

    public:
        static bool Ready();
        static void Hook();
        static void Unhook();

        static bool Install(InputController *controller);

        static Sensor *GetSensorsP1();
        static Sensor *GetSensorsP2();
        static HHOOK   GetHookHandle();
        static HWND    GetGameWindow();
    };
}

#endif