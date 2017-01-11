#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <sqlite3.h>



class DataBase {
public:
   DataBase(std::string path);
   std::string lookForPath(int);
   int lookForCost();
   int lookForSongButton(std::string);
   ~DataBase();
private:
  sqlite3 *db;
  //extractor methods
  void lookForPathH(int);
  void lookForCostH();
  void lookForSongButtonH(std::string);

};


#endif
