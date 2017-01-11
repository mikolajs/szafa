#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

std::string execCommand(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
    }

    int main(){
        std::string current = execCommand("/usr/bin/mpc current");
        printf("current list: %s \n", current.c_str());
        if(current.length() < 1) exec("/usr/bin/mpc clear");
        return 0;
    }
