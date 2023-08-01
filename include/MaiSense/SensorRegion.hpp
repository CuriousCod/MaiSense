#ifndef MAISENSE_SENSOR_REGION_HPP
#define MAISENSE_SENSOR_REGION_HPP

#include <vector>

#include <MaiSense/Sensor.hpp>
#include <MaiSense/Point.hpp>

namespace MaiSense
{
	class SensorRegion
	{
	private:
		static constexpr int COLINEAR = 0;
		static constexpr int CLOCKWISE = 1;
		static constexpr int COUNTER_CLOCKWISE = 2;

		std::vector<Point> points;

		static bool IsOnSegment(const Point& p, const Point& q, const Point& r);
		static int FindOrientation(const Point& p, const Point& q, const Point& r);
		static bool DoesSegmentIntersect(const Point& p1, const Point& q1, const Point& p2, const Point& q2);

	public:
		SensorRegion();
		explicit SensorRegion(const std::vector<Point>& points);
		~SensorRegion();

		void AddPoint(const Point& point);
		bool Test(const Point& test_point, float scale_x = 1.0, float scale_y = 1.0f) const;
		std::vector<Point> GetPoints() const;
		void SetPoints(const std::vector<Point>& new_points);
	};
}

#endif
