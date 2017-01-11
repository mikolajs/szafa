

import java.sql._
import scala.collection.mutable._

class DBtest {
  //Class.forName("org.postgresql.Driver")
  Class.forName("org.sqlite.JDBC")

  
  var timeStart = System.currentTimeMillis
  //ustanowienie połączenia
  var conn:Connection = _

  def startTimer() { timeStart = System.currentTimeMillis }
  def showTimer() =  System.currentTimeMillis - timeStart

  def initDB() {
    try {
      conn =  DriverManager.getConnection("jdbc:sqlite:./data.db3")
    }
    catch {
      case e:Exception => println("Error: " + e.toString)
        case _ : Throwable => println("Nieznany błąd połączenia z bazą")
    }

  }

  def dbClose() {
    try {
      conn.close
    }
    catch {
      case e:Exception => println("Error: " + e.toString)
        case _  : Throwable => println("Nieznany błąd zamykania bazy")
    }
  }


  def insertData() {
    //try {
     val path = "/var/lib/mpd/music/music"
     val artist = "Artysta "
     val title = "Music nr "

    var n = 0;
      for(i <- 1 to 100){
         n = i % 24 + 1
         val s = if(n < 10) "0" + n.toString else n.toString
         val stat = conn.createStatement
         stat.executeUpdate("INSERT INTO files VALUES( %d, '%s', '%s', '%s', %d)".format(i, artist + s, title + s, path + s + ".mp3", i))
      }
      
    //}
    //catch {
      //case e:Exception => println("Error: " + e.toString)
      //case _ => println("Nieznany błąd zapisu danych")
     //}
    }

//CREATE TABLE files (id INTEGER NOT NULL, artist VARCHAR(100) NOT NULL, title VARCHAR(100) NOT NULL, file VARCHAR(1024) NOT NULL, button_id INTEGER NOT NULL, PRIMARY KEY(id));

  def executeQ(s:String) {
    val stat = conn.createStatement
    stat.executeUpdate(s)
  }

  def testMakeStrings():String = {
    var l:List[String] = List()
    for( i <- (10 to 100000 )) {
     l = ("INSERT INTO test VALUES(" + i.toString + ",'nazwa" + i.toString + "')")::l
    }
    l.mkString(";")
  }
  
}

val dbt = new DBtest()
println("POCZĄTEK")
dbt.initDB
var im = ""
var na = ""
//dbt.startTimer
/*for (i <- (100000  to 200000)) {
  na = "Naz" + i.toString
  dbt.insertData(i,na)
}*/

//dbt.executeQ(dbt.testMakeStrings)

dbt.insertData
//dbt.readData
//println("Czas zapytania: " + dbt.showTimer.toString)
dbt.dbClose
//println("KONIEC")

