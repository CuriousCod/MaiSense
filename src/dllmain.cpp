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
SensorChecker       sensor_checker_p1;
SensorChecker       sensor_checker_p2;
SensorProcessor     processor_p1;
SensorProcessor     processor_p2;
bool                initialized    = false;

void initialize_sensors(SensorChecker& sensor_checker, SensorProcessor& sensor_processor, const bool& is_player2)
{
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

    if (sensor_processor.GetChecker() == nullptr)
    {
        sensor_processor.SetChecker(&sensor_checker);

        if (is_player2)
        {
            // TODO This is a fixed offset for now
            auto offset = Point();
            offset.X = 1080;
            offset.Y = 0;
            sensor_checker.AddOffsetToAllRegions(offset);
        }
    }

    if (sensor_processor.GetSensor() == nullptr)
    {
        Sensor* sensors = is_player2 ? InputManager::GetSensorsP2() : InputManager::GetSensorsP1();

        sensor_processor.SetSensor(sensors);
    }
}

void process_touch_input(const TouchEvent ev)
{
    processor_p1.HandleTouchEvent(ev);
    // processor_p2.HandleTouchEvent(ev);

    if (initialized)
    {
        return;
    }

    initialize_sensors(sensor_checker_p1, processor_p1, false);
    // initialize_sensors(sensor_checker_p2, processor_p2, true);

    initialized = true;
}

void process_mouse_input(const MouseEvent ev)
{
    if (ev.MButton)
    {
        mouse_controller.EmulateTouch();
    }

    processor_p1.HandleMouseEvent(ev);
    // processor_p2.HandleMouseEvent(ev);

    if (initialized)
    {
        return;
    }

    initialize_sensors(sensor_checker_p1, processor_p1, false);
    // initialize_sensors(sensor_checker_p2, processor_p2, true);

    initialized = true;
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
        // InputManager::Install(&mouse_controller); // Disabled mouse events, as they generate a ton of touch events when using a touch screen
    }
    else if (cause == DLL_PROCESS_DETACH)
    {
        InputManager::Unhook();
    }

    return TRUE;
}
