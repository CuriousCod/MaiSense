#include <MaiSense/Sensor.hpp>
#include <future>
#include <execution>

namespace MaiSense
{

    Sensor::Sensor() :
        activeFlags(0),
        inactiveFlags(0)
    {
        // Initialize touch input tracking
        for (const auto& sensorId : sensors) {
            //touchPoints[sensorId] = -1;
            SetSensorState(sensorId, false);
            touchPoints[sensorId] = {};
        }
    }

    Sensor::~Sensor()
    {
    }

    bool Sensor::Connect()
    {
        // Process hook is created, no need to recreate
        if (activeFlags && inactiveFlags)
            return true;

        // Lookup for sensor addresses
        int input = *(int*)(TOUCH_POINTER_ADDRESS);
        if (input)
        {
            // Read sensor flag from given address
            // TODO: Add P2 Support(?)
            activeFlags = (int*)(input + P1_OFFSET_ACTIVE_ADDRESS);
            inactiveFlags = (int*)(input + P1_OFFSET_INACTIVE_ADDRESS);

            return true;
        }

        return false;
    }

    bool Sensor::SetSensorState(SensorId sensorId, bool value)
    {
        // Attempt to read sensor state in case not initialized yet
        if (!Connect())
            return false;

        // Validate Sensor Id (there's still so much gap tho)
        //if (sensorId < Sensor::A1 || sensorId > Sensor::C)
        if (std::find(std::begin(sensors), std::end(sensors), sensorId) == std::end(sensors))
            return false;

        // No need to trigger the sensor if sensor already in desired state
        if (states.count(sensorId) > 0 && states[sensorId] == value)
            return true;

        // Update sensor states
        states[sensorId] = value;

        // Write sensor bits into memory
        if (value)
        {
            *activeFlags |= sensorId;
            *inactiveFlags &= ~sensorId;
        }
        else
        {
            *activeFlags &= ~sensorId;
            *inactiveFlags |= sensorId;
        }

        return true;
    }

    bool Sensor::GetSensorState(SensorId sensorId) {
        return states[sensorId];
    }

    bool Sensor::Remove(SensorId sensorId, bool value)
    {
        // Attempt to read sensor state in case not initialized yet
        if (!Connect())
            return false;

        // Validate Sensor Id (there's still so much gap tho)
        //if (sensorId < Sensor::A1 || sensorId > Sensor::C)
        if (std::find(std::begin(sensors), std::end(sensors), sensorId) == std::end(sensors))
            return false;

        // Remove the flag
        states.erase(sensorId);
        *activeFlags &= ~sensorId;
        *inactiveFlags &= ~sensorId;

        return true;
    }

    // Debug
    /*
    SensorId prevId = -1;
    bool prevValue = false;
    int prevEventId = 0;
    */

    void Sensor::Queue(SensorId sensorId, bool value, int eventId, const Point& point)
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
            
        auto message = Message();
        message.SensorId = sensorId;
        message.Value = value;
        message.eventId = eventId; // Current event id, the id is unique for each touch input
        message.point = point; // Current input position on the screen

        queue.push_back(message); // Add event to vector, queue will be processed in ProcessQueue()
    }

    bool Sensor::Activate(SensorId sensorId)
    {
        return SetSensorState(sensorId, true);
    }

    bool Sensor::Deactivate(SensorId sensorId)
    {
        return SetSensorState(sensorId, false);
    }

    bool Sensor::ProcessQueue()
    {
        int evCount = 0;
        auto it = queue.begin();
        bool assisted = false;
        
        // Process all current input events
        while (it != queue.end())
        {
            const auto& message = *it;

            // Check if the touch input is hitting the screen. If this is false the touch input is gone
            if (message.Value) {

                bool isInnerSensor = std::find(std::begin(innerSensors), std::end(innerSensors), message.SensorId) != std::end(innerSensors);

                // Add input to the sensor's vector
                if (!InVector(touchPoints[message.SensorId], message.eventId)){
                    touchPoints[message.SensorId].push_back(message.eventId);

                    if (config.SlideRetrigger)
                        touchPosition[message.eventId] = message.point; // Add input's current position to the map

                    if (isInnerSensor){
                        SetSensorState(message.SensorId, false); // Retrigger inner sensor every time, this helps with slides
                    }
                    /*
                    else if (touchPoints[message.SensorId].size() > 2) {
                        SetSensorState(message.SensorId, false); // Retrigger sensor when more than two inputs are hitting it, this helps with slides
                    }*/

                    std::async(std::launch::async, [&]() { SetSensorState(message.SensorId, true); }); // Activate sensor
                    // SetSensorState(message.SensorId, true); // Activate sensor

                    // Clear touch event from other sensors, the event should be hitting only one sensor at a time
                    for (const auto& sensorId : sensors) {
                        if (sensorId != message.SensorId) {
                            int index = FindIndex(touchPoints[sensorId], message.eventId);
                            if (index != -1) {
                                touchPoints[sensorId].erase(touchPoints[sensorId].begin() + index);
                                if (touchPoints[sensorId].size() == 0)
                                    SetSensorState(sensorId, false); // Deactivate sensor if there are no more inputs hitting it
                                break; // Event should only exist on one sensor
                            }
                            //touchPoints[sensorId].remove(message.eventId);
                        }
                    }
                }
                else {

                    // Check if input has moved enough to retrigger sensor
                    if (config.SlideRetrigger){
                        if (abs(abs(touchPosition[message.eventId].X) - abs(message.point.X)) > config.SlideRetriggerDistance) {
                            if (abs(abs(touchPosition[message.eventId].Y) - abs(message.point.Y)) > config.SlideRetriggerDistance) {
                                
                                std::async(std::launch::async, [&]() {  
                                    SetSensorState(message.SensorId, false);
                                    SetSensorState(message.SensorId, true);
                                    });
                                touchPosition[message.eventId] = message.point; // Reset comparison point
                            }
                        }
                    }
                }

                // Trigger adjacent sensors if Slide Assist is enabled and one of the inner sensors is touched
                if (config.SlideAssist) {
                    if (isInnerSensor) {
                        assisted = true;
                        std::async(std::launch::async, [&]() { SlideAssist(message.SensorId); }); // Activate adjacent sensors
                        //SlideAssist(message.SensorId);
                    }
                }
            }
            
            // When the touch input is no longer hitting the screen, remove the input event id from all sensors
            else {
                touchPosition.erase(message.eventId); // Erase input position information from the map

                // Remove event from sensors' vector
                for (const auto& sensorId : sensors) { 
                    int index = FindIndex(touchPoints[sensorId], message.eventId);
                    if (index != -1){
                        touchPoints[sensorId].erase(touchPoints[sensorId].begin() + index);
                        if (touchPoints[sensorId].size() == 0)
                            SetSensorState(sensorId, false); // Deactivate sensor if there are no more inputs hitting it
                        break; // Event should only exist on one sensor
                    }
                    //touchPoints[sensorId].remove(message.eventId); 
                }
            }
            
            // Clear the event from the process queue
            it = queue.erase(it);

            evCount++;
        }

        // Clear the extra sensor activations generated by Slide Assist
        if (config.SlideAssist && assisted){
            for (const auto& sensorId : sensors) {
                if (touchPoints[sensorId].size() == 0) {
                    SetSensorState(sensorId, false); // Deactivate sensor if there are no inputs hitting it
                }
            }
        }

        // Old code -------------------------------------------------------------------------
        /*
        std::unordered_map<SensorId, bool> processed;
        while (it != queue.end())
        {
            auto message = *it;
            SetSensorState(message.SensorId, message.Value);

            it = queue.erase(it);
            processed[message.SensorId] = message.Value;

            evCount++;
        }
        

        // Set the leftover states due touch move.
        // When the touch is moved and out of sensor region bound it give no signal to former sensor that it no longer active
        // Therefore we need to clear those sensor flag manually

        // Using this will break hold notes
        
        for (auto state : states)
        {
            if (state.second && (processed.count(state.first) == 0 || !processed[state.first]))
                SetSensorState(state.first, false);
        }
        */

        return evCount > 0;
    }

    void Sensor::Reset()
    {
        // Attempt to read sensor state in case not initialized yet
        if (!Connect())
            return;

        // Reset sensor bitflag each game frame
        *activeFlags = 0;
        *inactiveFlags = 0;
    }

    int Sensor::FindIndex(std::vector<int> v, int K) {
        const auto& it = std::find(v.begin(), v.end(), K);

        // If element was found
        if (it != v.end())
        {
            int index = it - v.begin();
            return index;
        }
        else 
            return -1;       
    }

    bool Sensor::InVector(std::vector<int> v, int K) {
        // Check if value is in list (This looks so beautiful in C++) https://stackoverflow.com/questions/24139428/check-if-element-is-in-the-list-contains
        return std::find(v.begin(), v.end(), K) != v.end();
    }

    void Sensor::SlideAssist(SensorId sensorId) {
        // Triggers sensors adjacent to the current sensor
        // Helps with inaccurate touch screens

        switch (sensorId) {
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

        return;
    }

}