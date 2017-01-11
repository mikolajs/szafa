#include <string>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fstream>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <deque>

#include "DataBase.h"
#include "BananaPins.h"
#include "Commands.h"

int credits = 0;
bool stop = false;
std::string file = "";
int cost = 0;
int toDelButton = -1;
bool isRolled = false;

struct music {
  std::string path;
  int button;
};

std::deque<music> playlist;

/////g++ -std=c++11 -o demo main.cpp DataBase.cpp BananaPins.cpp  Commands.cpp -l sqlite3 -l wiringPi

/*
//old
void deleteFirstFromPlayList(int des, DataBase &db){
  std::string first = Commands::getFirstFileOnPlaylist();
  printf("kasuję pozycję 1 z playlisty. Plik: %s", first.c_str());
  Commands::deleteFirstFromPlayList();
  if(first.length() > 0) {
    toDelButton = db.lookForSongButton(first);
    std::string command = std::to_string(toDelButton) + ":4";
    printf("%s\n", command.c_str() );
    serialPuts(des, command.c_str());
 }
}
///old

///kasuję wszystkie piosenki z wcześniejszych pozycji niż aktualna
void deleteOldFromPlaylist(int des, DataBase& db) {
      int current = Commands::getCurrentPos();
      printf("Current song::::: %d\n", current);
      if(current > 1)   deleteFirstFromPlayList(des, db);
      else  if(current < 0){
          Commands::clearList();
          serialPuts(des, "0:9");
      }

  }
//old
void runSong(std::string songPath,  int button, int des) {
  printf("play song: %s\n", songPath.c_str() );
  music m;
  m.path = songPath;
  m.button = button;
  playlist.push_back(m);
  Commands::playSong(songPath);
  std::string command = std::to_string(button) + ":3";
  printf("Led commad in run song %s\n", command.c_str());
  serialPuts(des, command.c_str());
}
*/
void addSongToPlaylist(std::string song, int button, int des){
  music m;
  m.path = song;
  m.button = button;
  playlist.push_back(m);
  std::string command = std::to_string(button) + ":3";
  printf("Led commad in run song %s\n", command.c_str());
  serialPuts(des, command.c_str());
}


void checkIfDelButton(int des){
  if(toDelButton > 0){
    std::string command = std::to_string(toDelButton) + ":4";
    printf("Kasuje przycisk %s\n", command.c_str() );
    serialPuts(des, command.c_str());
    toDelButton = -1;
  }
}

void checkIfEmpty(int des, BananaPins& bp){
  if(Commands::notPlaying()){
    checkIfDelButton(des);
    if(playlist.size() > 0) {
      music m = playlist.front();
      toDelButton = m.button;
      Commands::clearList();
      Commands::playSong(m.path);
      if(!isRolled) {
        bp.startRoll();
        isRolled = true;
      }
      playlist.pop_front();
    } else if(isRolled) {
      bp.stopRoll();
      isRolled = false;
    }
  }
}

void  readCredits(int des) {
  if(des >= 0) {
      int a = serialDataAvail(des);
      //printf("aivalible: %d\n", a);
      for(int i = 0; i < a; i++) {
          char ch = (char) serialGetchar(des);
          //printf("char %c\n", ch);
          if(ch ==  '*') {
              credits++;
              printf("new credit: %d\n", credits);
              }
          }
      }
  }

void isSignal(int s) {
  printf("Caught signal %d\n", s);
  stop = true;
}


int main(int argc, char* argv[]) {
  //int rc;
  Commands::clearUSB();
  DataBase db("/home/szafa/html/app/data.db3");
  int lastButton = 0;
  int currentButton = 0;

  int counter = 0;
///czytam koszt piosenki
  cost = db.lookForCost();
  ///ustawianie pinów do odczytu
  BananaPins bp;

  signal (SIGINT, isSignal);


  int desLeds = serialOpen("/dev/ttyUSB0", 9600);
  if(desLeds < 0) desLeds = serialOpen("/dev/ttyUSB1", 9600);
  if(desLeds < 0) desLeds = serialOpen("/dev/ttyUSB2", 9600);
  printf(  "descriptor for USB %d \n", desLeds);
  serialFlush(desLeds);
  delay(500);
  int desCredits = serialOpen("/dev/ttyACM0", 9600);
  if(desCredits < 0) desCredits = serialOpen("/dev/ttyACM1", 9600);
  if(desCredits < 0) desCredits = serialOpen("/dev/ttyACM2", 9600);
  printf(  "descriptor for ACM %d \n", desCredits);
  serialFlush(desCredits);
  delay(500);

  serialPuts(desLeds, "1:9");
  delay(200);

  while(!stop) {
    ///pytamy o kredyty i odczytujemy ich ilość
      readCredits(desCredits);
      ///sprawdzam czy jest kredyt i włączam piosenkę, zmniejszam kredyt
      if(credits > 0) {
          //printf(" have credits: %d \n", credits);
          currentButton  = bp.isButtonPressed();
          delay(10);
          if(currentButton != lastButton && currentButton > 0) {
                  file = db.lookForPath(currentButton);
                  credits -= cost;
                  if(credits < 0) credits = 0;
                  if(file.size() > 13)
                  /// old: runSong(file.substr(13), currentButton, desLeds);
                  addSongToPlaylist(file.substr(13), currentButton, desLeds);
                  lastButton = currentButton;
              }
      }
      checkIfEmpty(desLeds, bp);
      /*
      counter++;
      if(counter > 100){
        deleteOldFromPlaylist(desLeds, db);
        counter = 0;
      }
      */

      delay(100);
    }


    serialFlush(desLeds);
    serialClose(desLeds);
    delay(100);
    serialFlush(desCredits);
    serialClose(desCredits);

  }
