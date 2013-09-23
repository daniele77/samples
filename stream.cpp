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
#include<iomanip>
#include <sstream>
 
using namespace std;

void MyOutputFunction( const string& s )
{
    cout << s << endl;
}


class my_buffer : public std::stringbuf
{
public:
    ~my_buffer()
    {
       sync(); // can be avoided
    }
 
    int sync() // virtual function called when sync
    {
        MyOutputFunction( str() );
        str("");
        return 0;
    }
};



int main()
{
    my_buffer buf;
    ostream myview( &buf );
    myview << "Formatted string with numbers " << 2 << " and " << setprecision(3) << 10.001 << endl;
    // only one string is sent to the View, as wanted

    // Wait for exit
    cout << "Press Enter to end the program." << endl;
    cin.get();

    return 0;
}
