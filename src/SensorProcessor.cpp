#include <MaiSense/SensorProcessor.hpp>

namespace MaiSense
{
    SensorProcessor::SensorProcessor() :
        checker(0),
        sensor(0)
    {
    }

    SensorProcessor::~SensorProcessor()
    {
    }

    SensorChecker* SensorProcessor::GetChecker()
    {
        return this->checker;
    }

    Sensor* SensorProcessor::GetSensor()
    {
        return this->sensor;
    }

    void SensorProcessor::SetChecker(SensorChecker* checker)
    {
        this->checker = checker;
    }

    void SensorProcessor::SetSensor(Sensor* sensor)
    {
        this->sensor = sensor;
    }

    bool SensorProcessor::Handle(TouchEvent ev)
    {
        bool active = !(ev.Flag & POINTER_FLAG_UP || ev.Flag & POINTER_FLAG_CANCELED); // Check if the touch input entered the touch area / is moving on the touch area / left the touch area

        //return Handle({ev.X, ev.Y}, ev.Flag);
        return Handle({ ev.X, ev.Y }, active, ev.Id); // This will break mouse support if touch emulation is used
    }

    bool SensorProcessor::Handle(MouseEvent ev)
    {
        // Set mouse click eventId as static -2, so it won't conflict with the incrementing touch event IDs
        return Handle({ ev.X, ev.Y }, ev.LButton, -2); 
    }

    bool SensorProcessor::Handle(const Point& pointer, bool active, int eventId) // Added eventId as an argument to track the movement of the input
    {
        if (this->checker == NULL)
            return false;

        if (this->sensor == NULL)
            return false;

        for (const auto& sensorId : sensors)
        {
            if (checker->Check(pointer, sensorId))
            {
                // Input event is hitting a sensor, send sensor and status to the queue
                // active bool can be false here, if the input is lifted on top of the sensor
                sensor->Queue(sensorId, active, eventId, pointer);

                return true;
            }
        }

        // Input event is not hitting any sensors, send sensor as -1 and bool as false to the queue
        sensor->Queue(-1, false, eventId, pointer);

        return false;
    }
}