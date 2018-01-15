#include <iostream>
#include <fstream>
#include <string>


int main()
{
    const std::string funcName = "main";
    const std::string retValue = "42";
   
    const std::string asmContent =
        ".globl " + funcName + "\n"
        "" + funcName + ":\n"
        "movl $" + retValue + ", %eax\n"
        "ret\n";

    std::ofstream out("return_2.s");
    out << asmContent;

    return 0;
}
