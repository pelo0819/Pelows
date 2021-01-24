#include <iostream>
#include <string>
#include <fstream>


int main()
{
    std::cout << "Hello World!\n";

    std::ofstream outputfile;
    std::string path_str = "";
    path_str = "C:/Windows/System32/test_log.dat";

    outputfile.open(path_str, std::ios::app);
    if (outputfile.fail()) 
    {
        std::cout << "failed open file." << std::endl;
    }
    outputfile << "test" << std::endl;
    outputfile.close();
}
