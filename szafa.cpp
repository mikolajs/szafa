#include <sqlite3.h>
#include <string>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fstream>
#include <wiringPi.h>
#include <mcp23017.h>
#include <wiringSerial.h>
///g++ -std=c++11 -o demo main.cpp -l sqlite3 -l wiringPi
int credits = 0;
int cost = 0;
std::string file;
bool stop = false;

int getCost(void *d, int argc, char **argv, char **azColName) {
  int i = 0;
  for(i = 0; i < argc; i++) {
      std::string colName(azColName[i]);
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      if(colName == "cost" ) {
          printf("found cost \n");
          cost = std::stoi(argv[i]);
          }
      }
  printf("cost of pulse: %d \n", cost);
  return 0;
  }

int getPath(void *d, int argc, char **argv, char **azColName) {
  int i = 0;
  for(i = 0; i < argc; i++) {
      std::string colName(azColName[i]);
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      if(colName == "file" ) {
          printf("found file \n");
          std::string tmp(argv[i]);
          file = tmp;
          }
      }
  printf("file of button is  %s \n", file.c_str());
  return 0;
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
              //printf("new credit: %d\n", credits);
              }
          }
      }
  }

void lookForPath(sqlite3 *db, int buttonNr) {
  const char *data = "Look for path";
  char *zErrMsg = 0;
  int rc;
  std::string sql = "SELECT * from files WHERE button_id =  ";
  sql += std::to_string(buttonNr);
  printf("Zapytanie: %s \n", sql.c_str()) ;
  rc = sqlite3_exec(db, sql.c_str(), getPath, (void*) data, &zErrMsg);
  if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      }
  else {
      fprintf(stdout, "Operation done successfully\n");
      }
  }

void lookForCost(sqlite3 *db) {
  char *zErrMsg = 0;
  int rc;
  const char* data = "Look for cost";

  std::string sql = "SELECT * from song_cost";
  printf("Zapytanie: %s \n", sql.c_str()) ;

  rc = sqlite3_exec(db, sql.c_str(), getCost, (void*)data, &zErrMsg);
  if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      }
  else {
      fprintf(stdout, "Operation done successfully\n");
      }
  }

void setupWiringPi() {
  wiringPiSetup () ;
  mcp23017Setup (100, 0x20) ;
  mcp23017Setup (200, 0x21) ;
  printf ("Test przyciskow\n") ;
  int i;

  for (i = 0 ; i < 18 ; ++i) {
      pinMode (100 + i, INPUT) ;
      pullUpDnControl (100 + i, PUD_DOWN) ;
      }

  for (i = 0 ; i < 8 ; ++i) {
      pinMode (200 + i, OUTPUT) ;
      digitalWrite(200 + i , LOW);
      }

  }

int isButtonPressed() {
  int bit;
  bool isPressed = false;
  int pressed = 0;
  int nr = 0;
  for(int j = 0; j < 8; j++) {
      digitalWrite(200 + j, HIGH);
      if(j < 2 || j > 5) nr = 13;
      else nr = 12;
      for (bit = 0 ; bit < nr ; ++bit) {
          if(digitalRead (100 + bit) == 1) {
              printf ("%i\n", bit + 1) ;
              pressed = bit + 1;
              isPressed = true;
              }
          }
      digitalWrite(200 + j, LOW);
      if(isPressed) return pressed;
      }
  return 0;
  }

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
      }
  catch (...) {
      pclose(pipe);
      throw;
      }
  pclose(pipe);
  return result;
  }

///kasuję wszystkie piosenki z wcześniejszych pozycji niż aktualna
void deleteOldFromPlaylist() {
  bool deleted = true;
  while(deleted) {
      std::string current = execCommand("/usr/bin/mpc current -f %position%");
      if(current.length() > 0) {
          int pos = std::stoi(current.c_str());
          if(pos > 1) {
              printf("kasuję pozycję 1 z playlisty:");
              system("/usr/bin/mpc del  1" );
              }
          else deleted = false;
          }
      else deleted = false;

      }
  }

void runSong(std::string songPath) {
  std::string command("/usr/bin/sudo /usr/bin/mpc add ");
  command += songPath;
  system(command.c_str());
  command = "/usr/bin/sudo /usr/bin/mpc play ";
  system(command.c_str());
  delay(5);
  deleteOldFromPlaylist();
  }

void isSignal(int s) {
  printf("Caught signal %d\n", s);
  stop = true;
  }


int main(int argc, char* argv[]) {
  sqlite3 *db;
  int rc;
  int lastButton = 0;
  int currentButton = 0;
///otwieram bazę danych
  rc = sqlite3_open("/home/szafa/html/app/data.db3", &db);
  if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
      }
  else {
      fprintf(stderr, "Opened database successfully\n");
      }
  lookForCost(db);
  ///ustawianie pinów do odczytu

  setupWiringPi();
  signal (SIGINT,isSignal);

  int des = serialOpen("/dev/ttyACM0", 9600);
  if(des < 0) des = serialOpen("/dev/ttyACM1", 9600);
  if(des < 0) des = serialOpen("/dev/ttyACM2", 9600);
  printf(  "descriptor %d \n", des);
  serialFlush(des);
  delay(3000);
//tu będzie pętla
  while(!stop) {
///pytamy o kredyty i odczytujemy ich ilość
      readCredits(des);
      //std::cout << "credits = " << credits << std::endl;
///zwiększamy ilość kredytów jeśli ktoś wrzucił monety

//printf(" credits: %d \n",credits);
      ///sprawdzam czy jest kredyt i włączam piosenkę, zmniejszam kredyt
      if(credits > 0) {
          //printf(" credts: %d \n", credits);
          currentButton  = isButtonPressed();
          if(currentButton != lastButton) {
              if(currentButton > 0) {
                  lookForPath(db, currentButton);
                  credits -= cost;
                  if(credits < 0) credits = 0;
                  if(file.size() > 13) runSong(file.substr(13));
                  }
              lastButton = currentButton;
              }
          }
      delay(50);
// koniec pętli, zamknij bazę danych
      }
  sqlite3_close(db);
  serialClose(des);
  }

