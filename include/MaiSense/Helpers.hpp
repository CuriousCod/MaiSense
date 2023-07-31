#ifndef MAISENSE_HELPERS_HPP
#define MAISENSE_HELPERS_HPP

#include <vector>

namespace MaiSense
{
    class Helpers
    {
    public:
        /**
         * \brief Find the index of a value in a int vector
         * \param v Vector to search
         * \param k Value to search for
         * \return Index of the value in the vector, or -1 if not found
         */
        static int FindIndexFromVector(std::vector<int> v, const int k);

        /**
         * \brief Check if a value is in a int vector
         * \param v Vector to search
         * \param k Value to search for
         * \return True if the value is in the vector, false otherwise
         */
        static bool IsInVector(std::vector<int> v, const int k);
    };
}

#endif
