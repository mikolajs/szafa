#ifndef _BANANAPINS_H_
#define _BANANAPINS_H_

class BananaPins {
public:
  BananaPins();
  int isButtonPressed();
  void startRoll();
  void stopRoll();
private:
  void setupWiringPi();
};

#endif
