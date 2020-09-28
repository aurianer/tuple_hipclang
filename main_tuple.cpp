#include <iostream>
#include "try_tuple.hpp"

int main()
{
    //std::array<int const, 3> arr{4, 3, 1};
    //std::array<int, 3> const arr{4, 3, 1};
    std::array<int const, 3> const arr{4, 3, 1};
    //std::cout << hpx::get<0, decltype(arr)>(std::move(arr)) << '\n';
    std::cout << hpx::get<0, decltype(arr)>(arr) << '\n';
}
