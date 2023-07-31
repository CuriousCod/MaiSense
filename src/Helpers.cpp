#include <MaiSense/Helpers.hpp>
#include <vector>

namespace MaiSense
{
    int Helpers::FindIndexFromVector(std::vector<int> v, const int k)
    {
        const auto& it = std::find(v.begin(), v.end(), k);

        // If element was found
        if (it != v.end())
        {
            int index = it - v.begin();
            return index;
        }

        return -1;
    }

    bool Helpers::IsInVector(std::vector<int> v, const int k)
    {
        // https://stackoverflow.com/questions/24139428/check-if-element-is-in-the-list-contains
        return std::find(v.begin(), v.end(), k) != v.end();
    }
}

