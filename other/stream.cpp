/*******************************************************************************
 * Copyright Daniele Pallastrelli 2013.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
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
