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

typedef int sensor_id;
typedef int event_id;

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

        struct sensor_event
        {
            sensor_id sensor_id;
            bool state;
            int event_id;
            Point touch_point;
        };

        int* active_sensor_flags_;
        int* inactive_sensor_flags_;
        std::unordered_map<sensor_id, bool> sensor_states_;

        // Stores the input events currently triggering the sensors
        std::unordered_map<sensor_id, std::vector<event_id>> sensor_touch_event_map_;

        // Stores the position of the input event
        std::unordered_map<event_id, Point> touch_point_map_;

        std::vector<sensor_event> sensor_event_queue_;

        Config config_;

        const int sensors_[17] = {
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

        const int outer_sensors_[8] = {
          Sensor::A1,
          Sensor::A2,
          Sensor::A3,
          Sensor::A4,
          Sensor::A5,
          Sensor::A6,
          Sensor::A7,
          Sensor::A8,
        };

        const int inner_sensors_[9] = {
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
        static const sensor_id
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
        bool SetSensorState(sensor_id sensor_id, bool state);
        void SetSensorStateAsync(sensor_id sensor_id, bool value);
        bool GetSensorState(sensor_id sensorId);
        bool GetSensorActiveStateFromPointerFlag(sensor_id sensor_id) const;
        bool GetSensorInactiveStateFromPointerFlag(sensor_id sensor_id) const;
        void Queue(sensor_id sensorId, bool value, int eventId, const Point& point);

        bool Activate(sensor_id sensorId);
        bool Deactivate(sensor_id sensorId);
        bool ProcessTouchEvent(const std::vector<Sensor::sensor_event>::value_type& sensor_event);
        void ClearTouchEventFromSensors(const std::vector<Sensor::sensor_event>::value_type& sensor_event);
        bool Remove(sensor_id sensorId, bool value);

        bool ProcessTouchEventQueue();
        void CheckForRetrigger(const std::vector<Sensor::sensor_event>::value_type& sensor_event);
        void RegisterTouchEvent(const std::vector<Sensor::sensor_event>::value_type& sensor_event,
                                bool is_inner_sensor);
        void Reset();

        void ApplySlideAssist(sensor_id sensorId);
        void DisplayDebug() const;
    };
}

#endif