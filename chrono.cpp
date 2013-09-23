/*******************************************************************************
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 * 
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * For more information, please refer to <http://unlicense.org/>
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
