#include <MaiSense/Sensor.hpp>
#include <MaiSense/Helpers.hpp>
#include <future>

namespace MaiSense
{

    Sensor::Sensor() : active_sensor_flags_(nullptr), inactive_sensor_flags_(nullptr)
    {
        // Initialize touch input tracking
        for (const auto& sensor_id : sensors_)
        {
            //touchPoints[sensorId] = -1;
            SetSensorState(sensor_id, false);
            sensor_touch_event_map_[sensor_id] = {};
        }
    }

    Sensor::~Sensor() = default;

    bool Sensor::Connect()
    {
        // Process hook is created, no need to recreate
        if (active_sensor_flags_ && inactive_sensor_flags_)
        {
            return true;
        }

        // Lookup for sensor addresses
        int input = *(int*)TOUCH_POINTER_ADDRESS;

        if (input)
        {
            // Read sensor flag from given address
            // TODO: Add P2 Support(?)
            active_sensor_flags_ = (int*)(input + P1_OFFSET_ACTIVE_ADDRESS);
            inactive_sensor_flags_ = (int*)(input + P1_OFFSET_INACTIVE_ADDRESS);

            return true;
        }

        return false;
    }

    bool Sensor::SetSensorState(const sensor_id sensor_id, const bool state)
    {
        // Attempt to read sensor state in case not initialized yet
        if (!Connect())
        {
            return false;
        }

        // Validate Sensor Id (there's still so much gap tho)
        //if (sensorId < Sensor::A1 || sensorId > Sensor::C)
        if (std::find(std::begin(sensors_), std::end(sensors_), sensor_id) == std::end(sensors_))
        {
            return false;
        }

        // No need to trigger the sensor if sensor already in desired state
        if (sensor_states_.count(sensor_id) > 0 && sensor_states_[sensor_id] == state)
        {
            return true;
        }

        // Update sensor states
        sensor_states_[sensor_id] = state;

        // Write sensor bits into memory
        if (state)
        {
            *active_sensor_flags_ |= sensor_id;
            *inactive_sensor_flags_ &= ~sensor_id;
        }
        else
        {
            *active_sensor_flags_ &= ~sensor_id;
            *inactive_sensor_flags_ |= sensor_id;
        }

        return true;
    }

    void Sensor::SetSensorStateAsync(const sensor_id sensor_id, const bool value)
    {
        std::async(std::launch::async, [&]()
        {
            SetSensorState(sensor_id, value);
        });
    }

    bool Sensor::GetSensorState(const sensor_id sensorId)
    {
        return sensor_states_[sensorId];
    }

    bool Sensor::GetSensorActiveStateFromPointerFlag(const sensor_id sensor_id) const
    {
        return (*active_sensor_flags_ & sensor_id) != 0;
    }

    bool Sensor::GetSensorInactiveStateFromPointerFlag(const sensor_id sensor_id) const
    {
        return (*inactive_sensor_flags_ & sensor_id) != 0;
    }

    bool Sensor::Remove(sensor_id sensorId, bool value)
    {
        // Attempt to read sensor state in case not initialized yet
        if (!Connect())
        {
            return false;
        }

        // Validate Sensor Id (there's still so much gap tho)
        //if (sensorId < Sensor::A1 || sensorId > Sensor::C)
        if (std::find(std::begin(sensors_), std::end(sensors_), sensorId) == std::end(sensors_))
        {
            return false;
        }

        // Remove the flag
        sensor_states_.erase(sensorId);
        *active_sensor_flags_ &= ~sensorId;
        *inactive_sensor_flags_ &= ~sensorId;

        return true;
    }

    // Debug
    /*
    SensorId prevId = -1;
    bool prevValue = false;
    int prevEventId = 0;
    */

    void Sensor::Queue(sensor_id sensorId, bool value, int eventId, const Point& point)
    {
        // Debug
        /*
        if (prevId != sensorId || prevValue != value || prevEventId != eventId)
           std::fprintf(stdout, "sensorId=%d status=%d eventId=%d\n", sensorId, value, eventId); 
        */

        // Debug
        /*
        prevId = sensorId;
        prevValue = value;
        prevEventId = eventId;
        */
            
        auto event = sensor_event();
        event.sensor_id = sensorId;
        event.state = value;
        event.event_id = eventId; // Current event id, the id is unique for each touch input
        event.touch_point = point; // Current input position on the screen

        sensor_event_queue_.push_back(event); // Add event to vector, queue will be processed in ProcessQueue()
    }

    bool Sensor::Activate(sensor_id sensorId)
    {
        return SetSensorState(sensorId, true);
    }

    bool Sensor::Deactivate(sensor_id sensorId)
    {
        return SetSensorState(sensorId, false);
    }

    bool Sensor::ProcessTouchEventQueue()
    {
        int ev_count = 0;
        auto it = sensor_event_queue_.begin();
        bool applied_touch_assist = false;

        // Process all current input events
        while (it != sensor_event_queue_.end())
        {
            const auto& sensor_event = *it;

            // Check if the touch input is hitting the screen. If this is false the touch input is gone
            if (sensor_event.state)
            {
                applied_touch_assist = ProcessTouchEvent(sensor_event);
            }

            // When the touch input is no longer hitting the screen, remove the input event id from all sensors
            else
            {
                ClearTouchEventFromSensors(sensor_event);
            }

            // Clear the event from the process queue
            it = sensor_event_queue_.erase(it);

            ev_count++;
        }

        // Clear the extra sensor activations generated by Slide Assist
        if (applied_touch_assist)
        {
            for (const auto& sensor_id : sensors_)
            {
                if (!sensor_touch_event_map_[sensor_id].empty())
                {
                    continue;
                }

                SetSensorState(sensor_id, false); // Deactivate sensor if there are no inputs hitting it
            }
        }

        return ev_count > 0;
    }

    bool Sensor::ProcessTouchEvent(const std::vector<Sensor::sensor_event>::value_type& sensor_event)
    {
        const bool is_inner_sensor = std::find(std::begin(inner_sensors_), std::end(inner_sensors_), sensor_event.sensor_id) != std::end(inner_sensors_);

        // Add input to the sensor's vector
        if (!Helpers::IsInVector(sensor_touch_event_map_[sensor_event.sensor_id], sensor_event.event_id))
        {
            RegisterTouchEvent(sensor_event, is_inner_sensor);
        }
        else
        {
            // Check if input has moved enough to retrigger the sensor
            CheckForRetrigger(sensor_event);
        }

        // Trigger adjacent sensors if Slide Assist is enabled and one of the inner sensors is touched
        const bool apply_slide_assist = config_.SlideAssist && is_inner_sensor;

        if (apply_slide_assist)
        {
            ApplySlideAssist(sensor_event.sensor_id);
        }

        return apply_slide_assist;
    }

    void Sensor::RegisterTouchEvent(const std::vector<Sensor::sensor_event>::value_type& sensor_event, const bool is_inner_sensor)
    {
        sensor_touch_event_map_[sensor_event.sensor_id].push_back(sensor_event.event_id);

        if (config_.SlideRetrigger)
        {
            touch_point_map_[sensor_event.event_id] = sensor_event.touch_point; // Add input's current position to the map
        }

        if (is_inner_sensor)
        {
            SetSensorState(sensor_event.sensor_id, false); // Retrigger inner sensor every time, this helps with slides
        }

        /*
        else if (touchPoints[message.SensorId].size() > 2)
        {
            SetSensorState(message.SensorId, false); // Retrigger sensor when more than two inputs are hitting it, this helps with slides
        }
        */

        SetSensorState(sensor_event.sensor_id, true); // Activate sensor

        // Clear touch event from other sensors, the event should be hitting only one sensor at a time
        for (const auto& sensor_id : sensors_)
        {
            if (sensor_id == sensor_event.sensor_id)
            {
                continue;
            }

            int index = Helpers::FindIndexFromVector(sensor_touch_event_map_[sensor_id], sensor_event.event_id);

            if (index == -1)
            {
                continue;
            }

            sensor_touch_event_map_[sensor_id].erase(sensor_touch_event_map_[sensor_id].begin() + index);

            if (sensor_touch_event_map_[sensor_id].empty())
            {
                SetSensorState(sensor_id, false); // Deactivate sensor if there are no more inputs hitting it
            }

            break; // Event should only exist on one sensor

            //touchPoints[sensorId].remove(message.eventId);
        }
    }

    void Sensor::CheckForRetrigger(const std::vector<Sensor::sensor_event>::value_type& sensor_event)
    {
        if (!config_.SlideRetrigger)
        {
            return;
        }

        if (abs(abs(touch_point_map_[sensor_event.event_id].X) - abs(sensor_event.touch_point.X)) > config_.SlideRetriggerDistance)
        {
            if (abs(abs(touch_point_map_[sensor_event.event_id].Y) - abs(sensor_event.touch_point.Y)) > config_.SlideRetriggerDistance)
            {
                SetSensorState(sensor_event.sensor_id, false);
                SetSensorState(sensor_event.sensor_id, true);

                touch_point_map_[sensor_event.event_id] = sensor_event.touch_point; // Reset comparison point
            }
        }
    }

    void Sensor::ClearTouchEventFromSensors(const std::vector<sensor_event>::value_type& sensor_event)
    {
        touch_point_map_.erase(sensor_event.event_id); // Erase input position information from the map

        // Remove event from sensors' vector
        for (const auto& sensor_id : sensors_)
        {
            const int index = Helpers::FindIndexFromVector(sensor_touch_event_map_[sensor_id], sensor_event.event_id);

            if (index == -1)
            {
                continue;
            }

            sensor_touch_event_map_[sensor_id].erase(sensor_touch_event_map_[sensor_id].begin() + index);

            if (sensor_touch_event_map_[sensor_id].empty())
            {
                SetSensorState(sensor_id, false); // Deactivate sensor if there are no more inputs hitting it
            }

            break; // Event should only exist on one sensor

            //touchPoints[sensorId].remove(message.eventId);
        }
    }

    void Sensor::Reset()
    {
        // Attempt to read sensor state in case not initialized yet
        if (!Connect())
        {
            return;
        }

        // Reset sensor bitflag each game frame
        *active_sensor_flags_ = 0;
        *inactive_sensor_flags_ = 0;
    }

    void Sensor::ApplySlideAssist(sensor_id sensorId)
    {
        // Triggers sensors adjacent to the current sensor
        // Helps with inaccurate touch screens

        switch (sensorId)
        {
            case Sensor::B1:
                //SetSensorState(Sensor::A1, false);
                //SetSensorState(Sensor::A1, true);
                SetSensorState(Sensor::B8, true);
                SetSensorState(Sensor::B2, true);
                break;
            case Sensor::B2:
                //SetSensorState(Sensor::A2, false);
                //SetSensorState(Sensor::A2, true);
                SetSensorState(Sensor::B1, true);
                SetSensorState(Sensor::B3, true);
                break;
            case Sensor::B3:
                //SetSensorState(Sensor::A3, false);
                //SetSensorState(Sensor::A3, true);
                SetSensorState(Sensor::B2, true);
                SetSensorState(Sensor::B4, true);
                break;
            case Sensor::B4:
                //SetSensorState(Sensor::A4, false);
                //SetSensorState(Sensor::A4, true);
                SetSensorState(Sensor::B3, true);
                SetSensorState(Sensor::B5, true);
                break;
            case Sensor::B5:
                //SetSensorState(Sensor::A5, false);
                //SetSensorState(Sensor::A5, true);
                SetSensorState(Sensor::B4, true);
                SetSensorState(Sensor::B6, true);
                break;
            case Sensor::B6:
                //SetSensorState(Sensor::A6, false);
                //SetSensorState(Sensor::A6, true);
                SetSensorState(Sensor::B5, true);
                SetSensorState(Sensor::B7, true);
                break;
            case Sensor::B7:
                //SetSensorState(Sensor::A7, false);
                //SetSensorState(Sensor::A7, true);
                SetSensorState(Sensor::B6, true);
                SetSensorState(Sensor::B8, true);
                break;
            case Sensor::B8:
                //SetSensorState(Sensor::A8, false);
                //SetSensorState(Sensor::A8, true);
                SetSensorState(Sensor::B1, true);
                SetSensorState(Sensor::B7, true);
                break;
            /*case Sensor::C: // Activate all surrounding sensors
                for (SensorId sensorId : innerSensors)
                    SetSensorState(sensorId, true);
                break;*/
            default:
                break;
        }

        SetSensorState(Sensor::C, true); // Always activate C sensor
    }

    void Sensor::DisplayDebug() const
    {
        for (const auto& sensor_id : sensors_)
        {
            if (GetSensorActiveStateFromPointerFlag(sensor_id))
            {
                std::printf("Sensor %d: Active\n", sensor_id);
            }

            if (GetSensorInactiveStateFromPointerFlag(sensor_id))
            {
                std::printf("Sensor %d: Inactive\n", sensor_id);
            }
        }
    }

}