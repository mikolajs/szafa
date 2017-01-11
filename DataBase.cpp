
#include <string>
#include <stdexcept>
#include <stdio.h>

#include "DataBase.h"

///glolbal data
int DB_cost;
int DB_songButton;
std::string DB_file;

///global function
int DataBase_getCost(void *d, int argc, char **argv, char **azColName) {
  int i = 0;
  for(i = 0; i < argc; i++) {
    std::string colName(azColName[i]);
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    if(colName == "cost" ) {
      printf("found cost \n");
      DB_cost = std::stoi(argv[i]);
    }
  }
  printf("cost of pulse: %d \n", DB_cost);
  return 0;
}

int DataBase_getPath(void *d, int argc, char **argv, char **azColName) {
  int i = 0;
  for(i = 0; i < argc; i++) {
    std::string colName(azColName[i]);
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    if(colName == "file" ) {
      printf("found file \n");
      std::string tmp(argv[i]);
      DB_file = tmp;
    }
  }
  printf("file of button is  %s \n", DB_file.c_str());
  return 0;
}

int DataBase_getButton(void *d, int argc, char **argv, char **azColName) {
  int i = 0;
  for(i = 0; i < argc; i++) {
    std::string colName(azColName[i]);
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    if(colName == "button_id" ) {
      printf("found button_id \n");
      std::string tmp(argv[i]);
      DB_songButton = std::stoi(tmp);
    }
  }
  printf("to del button is  %d \n", DB_songButton);
  return 0;
}


DataBase::DataBase(std::string path){
  int rc = -1;
  rc = sqlite3_open(path.c_str(), &db);
  if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
  } else {
      fprintf(stderr, "Opened database successfully\n");
  }
}

DataBase::~DataBase(){
  sqlite3_close(db);
}




void DataBase::lookForPathH(int buttonNr) {
  const char *data = "Look for path";
  char *zErrMsg = 0;
  int rc;
  std::string sql = "SELECT * from files WHERE button_id =  ";
  sql += std::to_string(buttonNr);
  printf("Zapytanie: %s \n", sql.c_str()) ;
  rc = sqlite3_exec(db, sql.c_str(), DataBase_getPath, (void*) data, &zErrMsg);
  if( rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  else {
    fprintf(stdout, "Operation done successfully\n");
  }
}

void DataBase::lookForCostH() {
  char *zErrMsg = 0;
  int rc;
  const char* data = "Look for cost";
  DB_songButton = -1;

  std::string sql = "SELECT * from song_cost";
  printf("Zapytanie: %s \n", sql.c_str()) ;

  rc = sqlite3_exec(db, sql.c_str(), DataBase_getCost, (void*)data, &zErrMsg);
  if( rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  else {
    fprintf(stdout, "Operation done successfully\n");
  }
}

void DataBase::lookForSongButtonH(std::string aFile){
  char *zErrMsg = 0;
  int rc;
  const char* data = "Look for button";

  std::string sql = "SELECT * from files WHERE file = '/opt/jukebox/" + aFile  + "'" ;
  printf("Zapytanie: %s \n", sql.c_str()) ;

  rc = sqlite3_exec(db, sql.c_str(), DataBase_getButton, (void*)data, &zErrMsg);
  if( rc != SQLITE_OK ) {
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
  }
  else {
    fprintf(stdout, "Get button SQL done successfully\n");
  }
}

std::string DataBase::lookForPath(int buttonNr){
  DB_file = "";
  lookForPathH(buttonNr);
  return DB_file;
}
int DataBase::lookForCost(){
  DB_cost = -1;
  lookForCostH();
  return DB_cost;
}
int DataBase::lookForSongButton(std::string aFile){
  DB_songButton = -1;
  lookForSongButtonH(aFile);
  return DB_songButton;
}
