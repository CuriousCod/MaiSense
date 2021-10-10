#include <MaiSense/SensorChecker.hpp>
#include <MaiSense/InputManager.hpp>
#include <MaiSense/Config.hpp>
#include <math.h>

using namespace std;

namespace MaiSense
{
	SensorChecker::SensorChecker()
	{
		this->screenWidth = 0;
		this->screenHeight = 0;
		this->InitRegions();
	}

	SensorChecker::SensorChecker(int screenWidth, int screenHeight)
	{
		this->screenWidth = screenWidth;
		this->screenHeight = screenHeight;
		this->InitRegions();
	}

	SensorChecker::SensorChecker(int screenWidth, int screenHeight, const std::map<SensorId, SensorRegion>& regionMap)
	{
		this->screenWidth = screenWidth;
		this->screenHeight = screenHeight;
		this->InitRegions();
		this->regionMap.insert(regionMap.begin(), regionMap.end());
	}

	SensorChecker::~SensorChecker()
	{
	}

	void SensorChecker::InitRegions()
	{		
		if (config.SensorRegions != NULL ){

			json r = config.SensorRegions;

			try {
				this->AddRegion(Sensor::C, SensorRegion({ {r["C_REGION"][0][0], r["C_REGION"][0][1]}, {r["C_REGION"][1][0], r["C_REGION"][1][1]}, {r["C_REGION"][2][0], r["C_REGION"][2][1]}, {r["C_REGION"][3][0], r["C_REGION"][3][1]}, {r["C_REGION"][4][0], r["C_REGION"][4][1]}, {r["C_REGION"][5][0], r["C_REGION"][5][1]}, {r["C_REGION"][6][0], r["C_REGION"][6][1]}, {r["C_REGION"][7][0], r["C_REGION"][7][1]} }));
				this->AddRegion(Sensor::A1, SensorRegion({ {r["A1_REGION"][0][0], r["A1_REGION"][0][1]}, {r["A1_REGION"][1][0], r["A1_REGION"][1][1]}, {r["A1_REGION"][2][0], r["A1_REGION"][2][1]}, {r["A1_REGION"][3][0], r["A1_REGION"][3][1]} }));
				this->AddRegion(Sensor::A2, SensorRegion({ {r["A2_REGION"][0][0], r["A2_REGION"][0][1]}, {r["A2_REGION"][1][0], r["A2_REGION"][1][1]}, {r["A2_REGION"][2][0], r["A2_REGION"][2][1]}, {r["A2_REGION"][3][0], r["A2_REGION"][3][1]} }));
				this->AddRegion(Sensor::A3, SensorRegion({ {r["A3_REGION"][0][0], r["A3_REGION"][0][1]}, {r["A3_REGION"][1][0], r["A3_REGION"][1][1]}, {r["A3_REGION"][2][0], r["A3_REGION"][2][1]}, {r["A3_REGION"][3][0], r["A3_REGION"][3][1]} }));
				this->AddRegion(Sensor::A4, SensorRegion({ {r["A4_REGION"][0][0], r["A4_REGION"][0][1]}, {r["A4_REGION"][1][0], r["A4_REGION"][1][1]}, {r["A4_REGION"][2][0], r["A4_REGION"][2][1]}, {r["A4_REGION"][3][0], r["A4_REGION"][3][1]} }));
				this->AddRegion(Sensor::A5, SensorRegion({ {r["A5_REGION"][0][0], r["A5_REGION"][0][1]}, {r["A5_REGION"][1][0], r["A5_REGION"][1][1]}, {r["A5_REGION"][2][0], r["A5_REGION"][2][1]}, {r["A5_REGION"][3][0], r["A5_REGION"][3][1]} }));
				this->AddRegion(Sensor::A6, SensorRegion({ {r["A6_REGION"][0][0], r["A6_REGION"][0][1]}, {r["A6_REGION"][1][0], r["A6_REGION"][1][1]}, {r["A6_REGION"][2][0], r["A6_REGION"][2][1]}, {r["A6_REGION"][3][0], r["A6_REGION"][3][1]} }));
				this->AddRegion(Sensor::A7, SensorRegion({ {r["A7_REGION"][0][0], r["A7_REGION"][0][1]}, {r["A7_REGION"][1][0], r["A7_REGION"][1][1]}, {r["A7_REGION"][2][0], r["A7_REGION"][2][1]}, {r["A7_REGION"][3][0], r["A7_REGION"][3][1]} }));
				this->AddRegion(Sensor::A8, SensorRegion({ {r["A8_REGION"][0][0], r["A8_REGION"][0][1]}, {r["A8_REGION"][1][0], r["A8_REGION"][1][1]}, {r["A8_REGION"][2][0], r["A8_REGION"][2][1]}, {r["A8_REGION"][3][0], r["A8_REGION"][3][1]} }));
				this->AddRegion(Sensor::B1, SensorRegion({ {r["B1_REGION"][0][0], r["B1_REGION"][0][1]}, {r["B1_REGION"][1][0], r["B1_REGION"][1][1]}, {r["B1_REGION"][2][0], r["B1_REGION"][2][1]}, {r["B1_REGION"][3][0], r["B1_REGION"][3][1]} }));
				this->AddRegion(Sensor::B2, SensorRegion({ {r["B2_REGION"][0][0], r["B2_REGION"][0][1]}, {r["B2_REGION"][1][0], r["B2_REGION"][1][1]}, {r["B2_REGION"][2][0], r["B2_REGION"][2][1]}, {r["B2_REGION"][3][0], r["B2_REGION"][3][1]} }));
				this->AddRegion(Sensor::B3, SensorRegion({ {r["B3_REGION"][0][0], r["B3_REGION"][0][1]}, {r["B3_REGION"][1][0], r["B3_REGION"][1][1]}, {r["B3_REGION"][2][0], r["B3_REGION"][2][1]}, {r["B3_REGION"][3][0], r["B3_REGION"][3][1]} }));
				this->AddRegion(Sensor::B4, SensorRegion({ {r["B4_REGION"][0][0], r["B4_REGION"][0][1]}, {r["B4_REGION"][1][0], r["B4_REGION"][1][1]}, {r["B4_REGION"][2][0], r["B4_REGION"][2][1]}, {r["B4_REGION"][3][0], r["B4_REGION"][3][1]} }));
				this->AddRegion(Sensor::B5, SensorRegion({ {r["B5_REGION"][0][0], r["B5_REGION"][0][1]}, {r["B5_REGION"][1][0], r["B5_REGION"][1][1]}, {r["B5_REGION"][2][0], r["B5_REGION"][2][1]}, {r["B5_REGION"][3][0], r["B5_REGION"][3][1]} }));
				this->AddRegion(Sensor::B6, SensorRegion({ {r["B6_REGION"][0][0], r["B6_REGION"][0][1]}, {r["B6_REGION"][1][0], r["B6_REGION"][1][1]}, {r["B6_REGION"][2][0], r["B6_REGION"][2][1]}, {r["B6_REGION"][3][0], r["B6_REGION"][3][1]} }));
				this->AddRegion(Sensor::B7, SensorRegion({ {r["B7_REGION"][0][0], r["B7_REGION"][0][1]}, {r["B7_REGION"][1][0], r["B7_REGION"][1][1]}, {r["B7_REGION"][2][0], r["B7_REGION"][2][1]}, {r["B7_REGION"][3][0], r["B7_REGION"][3][1]} }));
				this->AddRegion(Sensor::B8, SensorRegion({ {r["B8_REGION"][0][0], r["B8_REGION"][0][1]}, {r["B8_REGION"][1][0], r["B8_REGION"][1][1]}, {r["B8_REGION"][2][0], r["B8_REGION"][2][1]}, {r["B8_REGION"][3][0], r["B8_REGION"][3][1]} }));

				// Check config for maimai screen layout
				int divider = config.OnePlayerOnly ? 1 : 2;

				REF_SCREEN_WIDTH = config.RefScreenWidth / divider; // Software only supports one maimai screen at the moment, so divide the width by 2 if maimai is in two player mode
				REF_SCREEN_HEIGHT = config.RefScreenHeight;
			}
			catch (exception e) {
				MessageBoxA(NULL, "Could not load sensor region or ref_screen data from the config.", "Config Error", MB_ICONEXCLAMATION);
				exit(EXIT_FAILURE);
			}

		}
		else {
			this->AddRegion(Sensor::C, C_REGION);
			this->AddRegion(Sensor::A1, A1_REGION);
			this->AddRegion(Sensor::A2, A2_REGION);
			this->AddRegion(Sensor::A3, A3_REGION);
			this->AddRegion(Sensor::A4, A4_REGION);
			this->AddRegion(Sensor::A5, A5_REGION);
			this->AddRegion(Sensor::A6, A6_REGION);
			this->AddRegion(Sensor::A7, A7_REGION);
			this->AddRegion(Sensor::A8, A8_REGION);
			this->AddRegion(Sensor::B1, B1_REGION);
			this->AddRegion(Sensor::B2, B2_REGION);
			this->AddRegion(Sensor::B3, B3_REGION);
			this->AddRegion(Sensor::B4, B4_REGION);
			this->AddRegion(Sensor::B5, B5_REGION);
			this->AddRegion(Sensor::B6, B6_REGION);
			this->AddRegion(Sensor::B7, B7_REGION);
			this->AddRegion(Sensor::B8, B8_REGION);
		}
	}

	void SensorChecker::AddRegion(SensorId sensorId, const SensorRegion& region)
	{
		this->regionMap.insert(std::pair<SensorId, SensorRegion>(sensorId, region));
	}

	void SensorChecker::RemoveRegion(SensorId sensorId)
	{
		this->regionMap.erase(sensorId);
	}

	bool SensorChecker::Check(Point position, SensorId sensorId)
	{

		auto  sensor = InputManager::GetSensor();
		float scaleX = this->screenWidth / (float)REF_SCREEN_WIDTH;
		float scaleY = this->screenHeight / (float)REF_SCREEN_HEIGHT;

		if (this->regionMap.count(sensorId) <= 0)
			return false;

		return this->regionMap[sensorId].Test(position, scaleX, scaleY);
	}

	int SensorChecker::GetScreenWidth()
	{
		return this->screenWidth;
	}

	int SensorChecker::GetScreenHeight()
	{
		return this->screenHeight;
	}

	void SensorChecker::SetScreenSize(int width, int height)
	{
		int divider = config.OnePlayerOnly ? 1 : 2;

		this->screenWidth = width / divider; // Software only supports one maimai screen at the moment, so divide the width by 2 if maimai is in two player mode
		this->screenHeight = height;
	}
}