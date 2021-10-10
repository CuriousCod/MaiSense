# MaiSense #

- **Original Author**: SirusDoma (a.k.a CXO2)
- **Modified By**: CuriousCod
- **Version**: 1.1.0-CC

Touch Sensor Input for SDEY, touchlaundry disco 💦. 

Changes since 1.0.0:

- Fixed hold notes by reimplementing touch event processing
- Added a configuration file to adjust various settings (sensor positions, screen size, touch adjustments, etc).
- Various configuration options to improve slide note accuracy
- Changed the default touch input trigger method to improve accuracy (adjustable from the config)

- Mouse input is disabled as it was causing several issues and wasn't very useful in general

- Sensors positions can be visualized for testing purposes with separate software: https://github.com/CuriousCod/MaiVisualizeSensors

## Config ##

Default configs are in SensorConfigs folder.  
If no config is used, default values will be loaded.  

The config contains the following settings:
1. `regions`
    - Contains the positions for all the touch sensors (A1-A8, B1-B8, C)
    - Default sensors are 4 point polygons excluding sensor C which has 8 points
    - A point consists of screen X and Y values
    - See the default values and layout from the SensorConfigs\original_sensor.config file
2. `ref_screen`
    - Contains keys for maimai window width and height
    - Set the values to match the values in maimai SystemConfig.txt
    - Width is automatically halved, if `1p_only` is set to false, to compensate the width of the second maimai display
    - Values accepted: integer
    - Default values: {`width`: 1000, `height`: 1000}
3. `1p_only`
    - Used to divide the screen width for sensor positioning when in 2p mode
    - The setting should match with maimai's GrooveMaster.ini
    - Values accepted: true/false
    - Enabled by default
4. `slide_retrigger`
    - Toggles Slide Retrigger feature
    - Retriggers sensor when slide_retrigger_distance value is exceeded 
    - By default, the sensor will stay on as long as at least one touch input is on it, this can cause problems when hitting slide notes (input only registers once)
        - Enabling this option can alleviate the issue
    - Values accepted: true/false
    - Disabled by default
5. `slide_retrigger_distance`
    - Minimum amount of touch movement (pixels) required to retrigger the sensor
    - Values accepted: integer 
    - Disabled by default
6. `slide_assist`
    - Toggles Slides Assist feature
    - Retriggers all adjacent inner sensors (B1-B8, C) when hitting one of the inner sensors
    - Helps with slide notes
    - Values accepted: true/false
    - Disabled by default
7. `prefer_accuracy`
    - Alternates between two different touch methods TWF_FINETOUCH (more accurate) and TWF_WANTPALM (less latency)
    - Enabling this usually results in better score with IR touch panels, but your mileage may wary
    - Values accepted: true/false
    - Enabled by default


## How it works ##

It makes use of `SetWindowsHookEx` to listen `WH_GETMESSAGE` and hook any input events.  
In order to register touch input, `CreateWindowExA` is detoured to perform `RegisterTouchWindow` after the actual `CreateWindowExA` is called.  

The game did **not** emulate any hardware input such as JVS and touch sensors instead, it writes input flag directly into the game memory.

## Usage ##

1. Download the release or build the project
2. Copy `MaiSense.Launcher.exe`, `MaiSense.dll` and `sensor.config` into the game directory
3. Adjust sensor.config values to your setup (You can use [separate application](https://github.com/CuriousCod/MaiVisualizeSensors "MaiVisualizeSensors") to test sensor positions )
4. Run `MaiSense.Launcher.exe`

### Touch Hook ###

Multi touch input is supported.  

Currently only support 1P with DEV mode only  
- You can play in 2P mode, but the sensors will only work for player 1

### Keyboard Hook ###

- Key 1\~8 as for A1\~A8 Sensors respectively, 0 for C Sensor
- Numpad 1\~8 as for B1\~B8 Sensors respectively

## License ##

This is an open-sourced library licensed under the [MIT License](http://github.com/SirusDoma/MaiSense/blob/master/LICENSE)
