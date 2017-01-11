//#include <algorithms>
#include <string>
#include <stdio.h>
#include <stdexcept>

#include "Commands.h"

  std::string Commands::execCommand(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
              result += buffer;
            }
        }
    catch (...) {
        pclose(pipe);
        throw;
        }
    pclose(pipe);
    return result;
    }

    std::string Commands::ltrim(std::string str) {
      size_t f = str.find_first_not_of(' ');
      if (f == std::string::npos)  return "";
      else return str.substr(f, str.length());
    }

    int Commands::getCurrentPos() {
      std::string pos = execCommand("/usr/bin/mpc current -f %position%");
      pos = ltrim(pos);
      if(pos.length() > 0) return std::stoi(pos);
      else return -1;
    }

    std::string Commands::getFirstFileOnPlaylist(){
      std::string list = execCommand("/usr/bin/mpc playlist -f %file%");
      std::string first = "";
      for(std::string::iterator it = list.begin(); it != list.end(); ++it){
        if(*it == '\n') break;
        first += *it;
      }
      return ltrim(first);
    }
void Commands::deleteFirstFromPlayList() {
      system("/usr/bin/mpc del  1" );
    }

  void Commands::playSong(std::string songPath) {
      std::string commandAdd("/usr/bin/sudo /usr/bin/mpc add " + songPath);
      std::string commandPlay("/usr/bin/sudo /usr/bin/mpc play ");
      system(commandAdd.c_str());
      system(commandPlay.c_str());
    }

    void Commands::clearList(){
      std::string commandPlay("/usr/bin/sudo /usr/bin/mpc clear");
    }

    bool Commands::notPlaying() {
      if(Commands::getCurrentPos() > 0) return false;
      else return true;
    }

    void Commands::clearUSB() {
      //printf("%s\n", "/home/szafa/usbreset /dev/bus/usb/001/004");
      system("/home/szafa/usbreset /dev/bus/usb/001/006");
      system("/home/szafa/usbreset /dev/bus/usb/001/003");
    }
