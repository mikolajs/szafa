
class Commands {
public:
  static std::string execCommand(const char*);
  static std::string ltrim(std::string str);
  static int getCurrentPos();
  static std::string getFirstFileOnPlaylist();
  static void deleteFirstFromPlayList();
  static void playSong(std::string songPath);
  static void clearUSB();
  static void clearList();
  static bool notPlaying();
};
