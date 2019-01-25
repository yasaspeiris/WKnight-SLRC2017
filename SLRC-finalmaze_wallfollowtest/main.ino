void setup() {
  Serial.begin(115200);
  motorsetup();
  servosetup();
  lightsandswsetup();
  colorsensorsetup();
  nrfsetup();
  //vlxsetup();
}

void loop() {
  int val1 = digitalRead(STARTSWITCH);
  int val2 = digitalRead(MODESW);
  int val3 = digitalRead(CALSW);
  
  if ( val1 == 0) {

    digitalWrite(BLUELED, HIGH);
    maze_solve();
    delay(200);
    gotostart();

    MasterENC.write(0); SlaveENC.write(0); //overshoot into box
    SetMasterPower(SPEED); SetSlavePower(SPEED);
    while (MasterENC.read() < BOXOVERSHOOT) {

    }
    setBrakes(200, 200);
    wallfollow(60);
    delay(PADDING);
    lastpart();
    while (1) {
      int val1 = digitalRead(STARTSWITCH);
      if ( val1 == 0) {
        MasterENC.write(0); SlaveENC.write(0); //overshoot out of box
        SetMasterPower(SPEED); SetSlavePower(SPEED);
        while (MasterENC.read() < BOXOVERSHOOT) {

        }

        travelmappedmaze();
        delay(170);

        gotostart();

        MasterENC.write(0); SlaveENC.write(0); //overshoot into box
        SetMasterPower(SPEED); SetSlavePower(SPEED);
        while (MasterENC.read() < BOXOVERSHOOT) {

        }

        wallfollow(60);
        delay(PADDING);
        lastpart();

        //follow line and find other box
        //overshoot into other box

      }
    }


  } if (val3 == 0) {
    digitalWrite(YELLOWLED, HIGH);
    QTRCcallibrate();
    digitalWrite(YELLOWLED, LOW);
  }
  if (val2 == 0) {
    nodetravel(SPEED);
  }


}


