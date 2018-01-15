#include <iostream>
#include <memory>
#include <string>

using namespace std;

int main()
{
    cout << "Main start" << endl;

    string x("0123456789");

    auto y = new int [10];
    delete[] y;

    cout << "Main end" << endl;
    return 0;
}