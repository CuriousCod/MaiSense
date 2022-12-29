#ifndef CONFIG
#define CONFIG

#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <fstream>
#pragma once

using namespace nlohmann;
//using namespace std;

namespace MaiSense
{
	// Holds data for the sensor regions and additional settings
	// Include the sensor.config file in the same location as the MaiSense.Launcher.exe
	// Example configs are included with the source

	class Config
	{
	private:
		std::string settingsFile = "sensor.config";
		bool configExists = false;
		json config;

		std::string configKeys[7] = { "regions", "ref_screen", "1p_only",
						 "slide_retrigger", "slide_retrigger_distance", "slide_assist", "prefer_accuracy"};

		static bool FileExists(std::string filename)
		{
			std::ifstream infile(filename);
			return infile.good();
		}

		static json LoadConfig(std::string filename)
		{
			json j;

			std::ifstream i(filename);

			try
			{
				i >> j;
			}
			catch (std::exception e)
			{
				MessageBoxA(nullptr, "Could not parse sensor.config. Please verify that the json is valid.", "Config Error", MB_ICONEXCLAMATION);
				return NULL; 
			}

			return j;
		}

	public:

		json SensorRegions = NULL;			// Sensor regions
		int RefScreenWidth = 1000;			// maimai screen width
		int RefScreenHeight = 1000;			// maimai screen height
		bool OnePlayerOnly = true;			// Adjusts sensor positions based on if two or one maimai screens are displayed
		bool SlideRetrigger = false;		// Enables Slide Retrigger
		int SlideRetriggerDistance = 0;		// Minimum amount of movement required to register touch slide as a new input
		bool SlideAssist = false;			// Toggle Slide Assist on or off
		bool PreferAccuracy = true;			// Changes between TWF_FINETOUCH (more accurate) and TWF_WANTPALM (less latency) touch methods https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registertouchwindow

		explicit Config(const bool debug_print = true)
		{
			configExists = FileExists(settingsFile);

			if (!configExists)
			{
				if (debug_print)
				{
					printf("No sensor.config found, using default configuration values \n\n");
					printf("Width: %d\nHeight: %d\nOnePlayer: %d\nSlideRetrigger: %d\nSlideRetriggerDistance: %d\nSlideAssist: %d\nPreferAccuracy: %d\n\n", RefScreenWidth, RefScreenHeight, OnePlayerOnly, SlideRetrigger, SlideRetriggerDistance, SlideAssist, PreferAccuracy);
				}

				return;
			}


			config = LoadConfig(settingsFile);

			if (config == NULL)
			{
				configExists = false;
				return;
			}

			for (std::string key : configKeys)
			{
				if (!config.contains(key))
				{
					MessageBoxA(nullptr, ("Could not find key: " + key + " from sensor.config. Skipping config.").c_str(), "Config Error", MB_ICONEXCLAMATION);
					configExists = false;
				}
			}

			if (configExists)
			{
				try
				{
					SensorRegions = config["regions"];
					RefScreenWidth = config["ref_screen"]["width"];
					RefScreenHeight = config["ref_screen"]["height"];
					OnePlayerOnly = config["1p_only"];
					SlideRetrigger = config["slide_retrigger"];
					SlideRetriggerDistance = config["slide_retrigger_distance"];
					SlideAssist = config["slide_assist"];
					PreferAccuracy = config["prefer_accuracy"];

					if (debug_print)
					{
						printf("Config loaded:\nWidth: %d\nHeight: %d\nOnePlayer: %d\nSlideRetrigger: %d\nSlideRetriggerDistance: %d\nSlideAssist: %d\nPreferAccuracy: %d\n\n", RefScreenWidth, RefScreenHeight, OnePlayerOnly, SlideRetrigger, SlideRetriggerDistance, SlideAssist, PreferAccuracy);
					}
				}
				catch (std::exception e)
				{
					MessageBoxA(NULL, "Could not load data from the config. Please ensure that the config json is valid", "Config Error", MB_ICONEXCLAMATION);
					exit(EXIT_FAILURE);
				}
			}
		}
	};
}

#endif
