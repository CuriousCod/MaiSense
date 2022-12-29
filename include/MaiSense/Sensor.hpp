#ifndef MAISENSE_MAIN_HPP
#define MAISENSE_MAIN_HPP

#include <string>
#include <list>
#include <windows.h>
#include <unordered_map>
#include <queue>
#include <vector>

#include <MaiSense/Config.hpp>
#include <MaiSense/Point.hpp>

typedef int SensorId;

namespace MaiSense
{
    class Process;
    class Sensor
    {
    private:
        const int TOUCH_POINTER_ADDRESS = 0xF40D28;
        const int P1_OFFSET_ACTIVE_ADDRESS = 52;
        const int P2_OFFSET_ACTIVE_ADDRESS = 56;
        const int P1_OFFSET_INACTIVE_ADDRESS = 60;
        const int P2_OFFSET_INACTIVE_ADDRESS = 64;

        struct Message
        {
            SensorId SensorId;
            bool Value;
            int eventId;
            Point point;
        };

        int* activeFlags;
        int* inactiveFlags;
        std::unordered_map<SensorId, bool> states;

        // Add a new variable to track the inputs the sensors are currently receiving
        std::unordered_map<SensorId, std::vector<int>> touchPoints;

        // Position of the input event
        std::unordered_map<int, Point> touchPosition;

        std::vector<Message> queue;

        MaiSense::Config config;

        const int sensors[17] = {
          Sensor::A1,
          Sensor::A2,
          Sensor::A3,
          Sensor::A4,
          Sensor::A5,
          Sensor::A6,
          Sensor::A7,
          Sensor::A8,
          Sensor::B1,
          Sensor::B2,
          Sensor::B3,
          Sensor::B4,
          Sensor::B5,
          Sensor::B6,
          Sensor::B7,
          Sensor::B8,
          Sensor::C
        };

        const int outerSensors[8] = {
          Sensor::A1,
          Sensor::A2,
          Sensor::A3,
          Sensor::A4,
          Sensor::A5,
          Sensor::A6,
          Sensor::A7,
          Sensor::A8,
        };

        const int innerSensors[9] = {
          Sensor::B1,
          Sensor::B2,
          Sensor::B3,
          Sensor::B4,
          Sensor::B5,
          Sensor::B6,
          Sensor::B7,
          Sensor::B8,
          Sensor::C
        };

    public:
        static const SensorId
            A1 = 1 << 0,
            B1 = 1 << 1,
            A2 = 1 << 2,
            B2 = 1 << 3,
            A3 = 1 << 5,
            B3 = 1 << 6,
            A4 = 1 << 7,
            B4 = 1 << 8,
            A5 = 1 << 10,
            B5 = 1 << 11,
            A6 = 1 << 12,
            B6 = 1 << 13,
            A7 = 1 << 15,
            B7 = 1 << 16,
            A8 = 1 << 17,
            B8 = 1 << 18,
            C = 1 << 19;

        Sensor();

        ~Sensor();

        bool Connect();
        bool SetSensorState(SensorId sensorId, bool value);
        void SetSensorStateAsync(SensorId sensor_id, bool value);
        bool GetSensorState(SensorId sensorId);
        void Queue(SensorId sensorId, bool value, int eventId, const Point& point);

        bool Activate(SensorId sensorId);
        bool Deactivate(SensorId sensorId);
        bool Remove(SensorId sensorId, bool value);

        bool ProcessQueue();
        void Reset();

        int FindIndex(std::vector<int> v, int k) const;
        bool InVector(std::vector<int> v, int k) const;
        void SlideAssist(SensorId sensorId);

    };
}

#endif