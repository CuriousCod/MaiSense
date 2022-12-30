#include <MaiSense/SensorProcessor.hpp>

namespace MaiSense
{
    SensorProcessor::SensorProcessor() : checker_(nullptr), sensor_(nullptr) {}

    SensorProcessor::~SensorProcessor() = default;

    SensorChecker* SensorProcessor::GetChecker() const
    {
        return this->checker_;
    }

    Sensor* SensorProcessor::GetSensor() const
    {
        return this->sensor_;
    }

    void SensorProcessor::SetChecker(SensorChecker* checker)
    {
        this->checker_ = checker;
    }

    void SensorProcessor::SetSensor(Sensor* sensor)
    {
        this->sensor_ = sensor;
    }

    bool SensorProcessor::HandleTouchEvent(const TouchEvent ev) const
    {
        const bool active = !(ev.Flag & POINTER_FLAG_UP || ev.Flag & POINTER_FLAG_CANCELED); // Check if the touch input entered the touch area / is moving on the touch area / left the touch area

        //return Handle({ev.X, ev.Y}, ev.Flag);
        return HandleSensorEvent({ ev.X, ev.Y }, active, ev.Id); // This will break mouse support if touch emulation is used
    }

    bool SensorProcessor::HandleMouseEvent(const MouseEvent ev) const
    {
        // Set mouse click eventId as static -2, so it won't conflict with the incrementing touch event IDs
        return HandleSensorEvent({ ev.X, ev.Y }, ev.LButton, -2);
    }

    bool SensorProcessor::HandleSensorEvent(const Point& pointer, const bool active, const int event_id) const // Added eventId as an argument to track the movement of the input
    {
        if (this->checker_ == nullptr)
        {
            return false;
        }

        if (this->sensor_ == nullptr)
        {
            return false;
        }

        for (const auto& sensor_id : sensors)
        {
            if (!checker_->Check(pointer, sensor_id))
            {
                continue;
            }

            // Input event is hitting a sensor, send sensor and status to the queue
            // active bool can be false here, if the input is lifted on top of the sensor
            sensor_->Queue(sensor_id, active, event_id, pointer);

            return true;
        }

        // Input event is not hitting any sensors, send sensor as -1 and bool as false to the queue
        sensor_->Queue(-1, false, event_id, pointer);

        return false;
    }
}