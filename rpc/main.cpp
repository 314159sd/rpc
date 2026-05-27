#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>

int main(){
    std::string line = "name=yoyo";
    replace(line.begin(), line.end(), '=', ' ');
    std::istringstream iss(line);
    
    std::string key, value;
    iss >> key >> value;
    std::cout << key << std::endl;
    std::cout << value << std::endl;
    return 0;
}