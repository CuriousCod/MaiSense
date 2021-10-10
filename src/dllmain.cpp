#include <MaiSense/Sensor.hpp>
#include <MaiSense/InputManager.hpp>
#include <MaiSense/TouchController.hpp>
#include <MaiSense/KeyboardController.hpp>
#include <MaiSense/MouseController.hpp>
#include <MaiSense/SensorChecker.hpp>
#include <MaiSense/SensorProcessor.hpp>

#include <string>

#pragma warning (disable : 4996)

using namespace MaiSense;

TouchController touchController;
KeyboardController keyboardController;
MouseController mouseController;
SensorChecker sensorChecker;
SensorProcessor processor;

BOOL APIENTRY DllMain(HMODULE hMod, DWORD cause, LPVOID lpReserved)
{
    if (!InputManager::Ready())
        return TRUE;

    if (cause == DLL_PROCESS_ATTACH)
    {
        AllocConsole();
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);

        touchController.SetCallback([&](TouchEvent ev)
            {
                processor.Handle(ev);
                if (sensorChecker.GetScreenWidth() == 0 || sensorChecker.GetScreenHeight() == 0)
                {
                    RECT clientRect;
                    GetClientRect(InputManager::GetGameWindow(), &clientRect);
                    sensorChecker.SetScreenSize
                    (
                        clientRect.left + clientRect.right,
                        clientRect.top + clientRect.bottom
                    );
                }

                if (processor.GetChecker() == NULL)
                    processor.SetChecker(&sensorChecker);

                if (processor.GetSensor() == NULL)
                    processor.SetSensor(InputManager::GetSensor());
            });

        keyboardController.SetCallback([&](KeyEvent ev)
            {
                // Set keyboard eventIds to start from -3 and decrease, this will avoid conflicts with the touch events
                // Trigger point will be always static (1,1)

                auto sensor = InputManager::GetSensor();
                switch (ev.KeyCode)
                {
                case 0x30: // 0
                    sensor->Queue(Sensor::C, ev.Active, -3, {1, 1});
                    break;
                case 0x31: // 1
                    sensor->Queue(Sensor::A1, ev.Active, -4, { 1, 1 });
                    break;
                case 0x32: // 2
                    sensor->Queue(Sensor::A2, ev.Active, -5, { 1, 1 });
                    break;
                case 0x33: // 3
                    sensor->Queue(Sensor::A3, ev.Active, -6, { 1, 1 });
                    break;
                case 0x34: // 4
                    sensor->Queue(Sensor::A4, ev.Active, -7, { 1, 1 });
                    break;
                case 0x35: // 5
                    sensor->Queue(Sensor::A5, ev.Active, -8, { 1, 1 });
                    break;
                case 0x36: // 6
                    sensor->Queue(Sensor::A6, ev.Active, -9, { 1, 1 });
                    break;
                case 0x37: // 7
                    sensor->Queue(Sensor::A7, ev.Active, -10, { 1, 1 });
                    break;
                case 0x38: // 8
                    sensor->Queue(Sensor::A8, ev.Active, -11, { 1, 1 });
                    break;
                case VK_NUMPAD1: // NUMPAD 1
                    sensor->Queue(Sensor::B1, ev.Active, -12, { 1, 1 });
                    break;
                case VK_NUMPAD2: // NUMPAD 2
                    sensor->Queue(Sensor::B2, ev.Active, -13, { 1, 1 });
                    break;
                case VK_NUMPAD3: // NUMPAD 3
                    sensor->Queue(Sensor::B3, ev.Active, -14, { 1, 1 });
                    break;
                case VK_NUMPAD4: // NUMPAD 4
                    sensor->Queue(Sensor::B4, ev.Active, -15, { 1, 1 });
                    break;
                case VK_NUMPAD5: // NUMPAD 5
                    sensor->Queue(Sensor::B5, ev.Active, -16, { 1, 1 });
                    break;
                case VK_NUMPAD6: // NUMPAD 6
                    sensor->Queue(Sensor::B6, ev.Active, -17, { 1, 1 });
                    break;
                case VK_NUMPAD7: // NUMPAD 7
                    sensor->Queue(Sensor::B7, ev.Active, -18, { 1, 1 });
                    break;
                case VK_NUMPAD8: // NUMPAD 8
                    sensor->Queue(Sensor::B8, ev.Active, -19, { 1, 1 });
                    break;
                }
            });
        mouseController.SetCallback([&](MouseEvent ev)
            {
                // Disabled mouse touch emulation as it won't work with the current setup
                
                /*
                if (ev.MButton)
                    mouseController.EmulateTouch(); 
                */
                
                processor.Handle(ev);
                if (sensorChecker.GetScreenWidth() == 0 || sensorChecker.GetScreenHeight() == 0)
                {
                    RECT clientRect;
                    GetClientRect(InputManager::GetGameWindow(), &clientRect);
                    sensorChecker.SetScreenSize
                    (
                        clientRect.left + clientRect.right,
                        clientRect.top + clientRect.bottom
                    );
                }

                if (processor.GetChecker() == NULL)
                    processor.SetChecker(&sensorChecker);

                if (processor.GetSensor() == NULL)
                    processor.SetSensor(InputManager::GetSensor());
                

            });

        InputManager::Hook();
        InputManager::Install(&touchController);
        InputManager::Install(&keyboardController);
        // InputManager::Install(&mouseController); // Also disabled the rest of the mouse events, as they generate a ton of touch events when using a touch screen
    }
    else if (cause == DLL_PROCESS_DETACH)
    {
        InputManager::Unhook();
    }

    return TRUE;
}
