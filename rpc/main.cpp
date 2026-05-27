#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
int main(){
    std::fstream file;
    file.open("a.txt", std::ios::in);
    if(file.fail()){
        std::cout << "open file failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string line;
    std::getline(file, line);
    replace(line.begin(), line.end(), '=', ' ');
    std::istringstream iss(line);
    
    std::string key, value;
    iss >> key >> value;
    std::cout << key << std::endl;
    std::cout << value << std::endl;
    file.close();
    return 0;
}