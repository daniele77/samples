/*******************************************************************************
 * Copyright Daniele Pallastrelli 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#include <iostream>
#include <boost/chrono.hpp>

void do_work( void )
{
    const size_t COUNT = 999999;
    for ( size_t i = 0; i < COUNT; ++i )
        int x = i * i - ( i / 2 );
}

int main()
{
    using namespace std;
    using namespace boost::chrono;

    high_resolution_clock::time_point t0 = high_resolution_clock::now();
    do_work();
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    cout << duration_cast< milliseconds > ( t1 - t0 ).count() << " msec\n";
    cout << duration_cast< nanoseconds > ( t1 - t0 ).count() << " nsec\n";

    // Wait for exit
    std::cout << "Press Enter to end the program." << std::endl;
    std::cin.get();

    return 0;
}
