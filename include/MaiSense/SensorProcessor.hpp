#ifndef MAISENSE_SENSOR_PROCESSOR_HPP
#define MAISENSE_SENSOR_PROCESSOR_HPP

#include <unordered_map>
#include <vector>

#include <MaiSense/Sensor.hpp>
#include <MaiSense/SensorChecker.hpp>
#include <MaiSense/Point.hpp>
#include <MaiSense/TouchEvent.hpp>
#include <MaiSense/MouseEvent.hpp>

namespace MaiSense
{
    class SensorProcessor
    {
    private:
        SensorChecker* checker_;
        Sensor* sensor_;

    public:
        SensorProcessor();
        virtual ~SensorProcessor();

        SensorChecker* GetChecker() const;
        Sensor* GetSensor() const;

        void SetChecker(SensorChecker* checker);
        void SetSensor(Sensor* sensor);

        bool HandleTouchEvent(TouchEvent ev) const;
        bool HandleMouseEvent(MouseEvent ev) const;
        bool HandleSensorEvent(const Point& pointer, bool active, int event_id) const;

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
    };
}

#endif