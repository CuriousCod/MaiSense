#include <MaiSense/SensorRegion.hpp>
#include <cmath>

namespace MaiSense
{
	SensorRegion::SensorRegion() = default;

    SensorRegion::SensorRegion(const std::vector<Point>& points)
	{
		this->points.reserve(points.size());
		this->points.insert(this->points.end(), points.begin(), points.end());
	}

	SensorRegion::~SensorRegion() = default;

    void SensorRegion::AddPoint(const Point& point)
	{
		this->points.push_back(point);
	}

	bool SensorRegion::Test(const Point& test_point, const float scale_x, const float scale_y) const
    {
        if (this->points.size() < 3)
        {
            return false;
        }

        std::vector<Point> scaled_points;

        for (unsigned int i = 0; i < this->points.size(); i++)
        {
            const int scaled_x = (int)ceil(this->points[i].X * scale_x);
            const int scaled_y = (int)ceil(this->points[i].Y * scale_y);
            scaled_points.push_back({ scaled_x, scaled_y });
        }

        const Point extreme = { 9999, test_point.Y };

        int count = 0, i = 0, n = scaled_points.size();
        do
        {
            const int next = (i + 1) % n;

            if (DoesSegmentIntersect(scaled_points[i], scaled_points[next], test_point, extreme))
            {
                if (FindOrientation(scaled_points[i], test_point, scaled_points[next]) == COLINEAR)
                {
                    return IsOnSegment(scaled_points[i], test_point, scaled_points[next]);
                }

                count++;
            }

            i = next;
        } while (i != 0);

        return count & 1;
	}

    bool SensorRegion::IsOnSegment(const Point& p, const Point& q, const Point& r)
    {
        if (q.X <= max(p.X, r.X) && q.X >= min(p.X, r.X) && q.Y <= max(p.Y, r.Y) && q.Y >= min(p.Y, r.Y))
        {
            return true;
        }

        return false;
    }

    int SensorRegion::FindOrientation(const Point& p, const Point& q, const Point& r)
    {
        const int val = (q.Y - p.Y) * (r.X - q.X) - (q.X - p.X) * (r.Y - q.Y);

        if (val == 0)
        {
            return COLINEAR;
        }

        return val > 0 ? CLOCKWISE : COUNTER_CLOCKWISE;
    }

    bool SensorRegion::DoesSegmentIntersect(const Point& p1, const Point& q1, const Point& p2, const Point& q2)
    {
        const int o1 = FindOrientation(p1, q1, p2);
        const int o2 = FindOrientation(p1, q1, q2);
        const int o3 = FindOrientation(p2, q2, p1);
        const int o4 = FindOrientation(p2, q2, q1);

        if (o1 != o2 && o3 != o4)
        {
            return true;
        }

        if (o1 == COLINEAR && IsOnSegment(p1, p2, q1))
        {
            return true;
        }

        if (o2 == COLINEAR && IsOnSegment(p1, q2, q1))
        {
            return true;
        }

        if (o3 == COLINEAR && IsOnSegment(p2, p1, q2))
        {
            return true;
        }

        if (o4 == COLINEAR && IsOnSegment(p2, q1, q2))
        {
            return true;
        }

        return false;
    }

    std::vector<Point> SensorRegion::GetPoints() const
    {
        return this->points;
    }

    void SensorRegion::SetPoints(const std::vector<Point>& new_points)
    {
        points = new_points;
    }
}