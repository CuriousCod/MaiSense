#include <MaiSense/Sensor.hpp>
#include <MaiSense/InputManager.hpp>
#include <MaiSense/TouchController.hpp>
#include <MaiSense/KeyboardController.hpp>
#include <MaiSense/MouseController.hpp>
#include <MaiSense/SensorChecker.hpp>
#include <MaiSense/SensorProcessor.hpp>

#pragma warning (disable : 4996)

using namespace MaiSense;

TouchController     touch_controller;
KeyboardController  keyboard_controller;
MouseController     mouse_controller;
SensorChecker       sensor_checker;
SensorProcessor     processor;

void process_touch_input(const TouchEvent ev)
{
    processor.HandleTouchEvent(ev);
    if (sensor_checker.GetScreenWidth() == 0 || sensor_checker.GetScreenHeight() == 0)
    {
        RECT client_rect;
        GetClientRect(InputManager::GetGameWindow(), &client_rect);
        sensor_checker.SetScreenSize
        (
            client_rect.left + client_rect.right,
            client_rect.top + client_rect.bottom
        );
    }

    if (processor.GetChecker() == nullptr)
    {
        processor.SetChecker(&sensor_checker);
    }

    if (processor.GetSensor() == nullptr)
    {
        processor.SetSensor(InputManager::GetSensor());
    }
}

void process_mouse_input(const MouseEvent ev)
{
    // Disabled mouse touch emulation as it won't work with the current setup

    /*
    if (ev.MButton)
    {
        mouseController.EmulateTouch();
    }
    */

    processor.HandleMouseEvent(ev);

    if (sensor_checker.GetScreenWidth() == 0 || sensor_checker.GetScreenHeight() == 0)
    {
        RECT client_rect;
        GetClientRect(InputManager::GetGameWindow(), &client_rect);
        sensor_checker.SetScreenSize
        (
            client_rect.left + client_rect.right,
            client_rect.top + client_rect.bottom
        );
    }

    if (processor.GetChecker() == nullptr)
    {
        processor.SetChecker(&sensor_checker);
    }

    if (processor.GetSensor() == nullptr)
    {
        processor.SetSensor(InputManager::GetSensor());
    }
}

BOOL APIENTRY DllMain(HMODULE hMod, DWORD cause, LPVOID lpReserved)
{
    if (!InputManager::Ready())
    {
        return TRUE;
    }

    if (cause == DLL_PROCESS_ATTACH)
    {
        AllocConsole();
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);

        touch_controller.SetCallback([&](const TouchEvent ev)    { process_touch_input(ev); });
        keyboard_controller.SetCallback([&](const KeyEvent ev)   { KeyboardController::ProcessKeyboardInput(ev);});
        mouse_controller.SetCallback([&](const MouseEvent ev)    { process_mouse_input(ev); });

        InputManager::Hook();
        InputManager::Install(&touch_controller);
        InputManager::Install(&keyboard_controller);
        // InputManager::Install(&mouseController); // Also disabled the rest of the mouse events, as they generate a ton of touch events when using a touch screen
    }
    else if (cause == DLL_PROCESS_DETACH)
    {
        InputManager::Unhook();
    }

    return TRUE;
}
