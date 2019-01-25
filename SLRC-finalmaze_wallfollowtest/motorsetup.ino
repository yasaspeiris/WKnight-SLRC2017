/*Motor driver functions -
  contains -
  motorsetup() - always run at setup
  SetMasterPower(int MasterPower) - set Master(left) pwm + front - back
  SetSlavePower (int SlavePower) - set Slave(right) pwm + front - back
  inchtravel(int distance,int +-power) - encoded movementfor provided inches/10
  MaintainMovement() - only run inside mmtravel,inchtravel,sharpleft or sharpright,angleturn
  StopMovement() - stop motors
*/

void motorsetup() {
  // put your setup code here, to run once:
  pinMode(LMOTORSPEED, OUTPUT); pinMode(RMOTORSPEED, OUTPUT);
  analogWrite(LMOTORSPEED, 0); analogWrite(RMOTORSPEED, 0);

  pinMode(RMOTOR1, OUTPUT); pinMode(RMOTOR2, OUTPUT);
  digitalWrite(RMOTOR1, LOW); digitalWrite(RMOTOR2, LOW);

  pinMode(LMOTOR1, OUTPUT); pinMode(LMOTOR2, OUTPUT);
  digitalWrite(LMOTOR1, LOW); digitalWrite(LMOTOR2, LOW);

  pinMode(MOTOREN, OUTPUT);
  digitalWrite(MOTOREN, HIGH);

  pinMode(MOTOREN2, OUTPUT);
  digitalWrite(MOTOREN2, HIGH);

}

/*===========================================================

  Set PWM - Set Master and Slave PWM - Master - Left Wheel

   ===========================================================
*/
void SetMasterPower(int MasterPower) {
  if (MasterPower < 0) {
    analogWrite(LMOTORSPEED, abs(MasterPower));
    digitalWrite(LMOTOR1, HIGH); digitalWrite(LMOTOR2, LOW);

  } else if (MasterPower > 0) {
    analogWrite(LMOTORSPEED, abs(MasterPower));
    digitalWrite(LMOTOR1, LOW); digitalWrite(LMOTOR2, HIGH);

  } else {
    digitalWrite(LMOTOR1, LOW ); digitalWrite(LMOTOR2, LOW);
  }
}

void SetSlavePower(int SlavePower) {
  if (SlavePower < 0) {
    analogWrite(RMOTORSPEED, abs(SlavePower));

    digitalWrite(RMOTOR1, HIGH); digitalWrite(RMOTOR2, LOW);

  } else if (SlavePower > 0) {
    analogWrite(RMOTORSPEED, abs(SlavePower));
    digitalWrite(RMOTOR1, LOW); digitalWrite(RMOTOR2, HIGH);

  } else {
    digitalWrite(RMOTOR1, LOW); digitalWrite(RMOTOR2, LOW);
  }
}



/*===================================================================

          // Line - nodetravel,nodetravelnostop, linetravel,alligntoline

  ===================================================================== */

char nodetravel(int power) {
  nodeflag = 0;
  char doaction2;
  MasterENC.write(0) ; SlaveENC.write(0); totalTicks = 0; error = 0; lasterror = 0; SensorPanelVal = 0;
  MaxPower = power;
  MasterPower = power; SlavePower = power;
  while (nodeflag == 0) {
    doaction2 = MaintainLineMovement();
  }
  setBrakes(255, 255);
  analogWrite(11, 120);
  delay(20);
  analogWrite(11, 0);
  MaxPower = 0;
  return doaction2;
}


void linetravel(int distance, int power) { //Travel along the line for some distance
  int tickGoal = (128 * (distance)); int i;
  MasterENC.write(0) ; SlaveENC.write(0); totalTicks = 0; error = 0; lasterror = 0;
  MaxPower = power;
  MasterPower = power; SlavePower = power;

  while (abs(totalTicks) <  tickGoal) {
    MaintainLineMovement();
  }


  setBrakes(200, 200); //StopMotors
}


/*===================================================================

          // Encoder travel - inchtravel, inchtravelnostop,mmtravel

  ===================================================================== */
void inchtravel(float distance, int power) {
  int tickGoal = (128 * (distance)); int i;
  MasterENC.write(0) ; SlaveENC.write(0); totalTicks = 0; error = 0; lasterror = 0;
  MaxPower = power;
  MasterPower = power; SlavePower = power;

  while (abs(totalTicks) <  tickGoal) {
    MaintainMovement();
  }
  setBrakes(230, 230);
}

/*===================================================================

          // Degree Travel - degreetravel

  ===================================================================== */

void degreetravel(int degree, int power) {
  int tickGoal = abs(degree) * 9.89 ;//compensation
  MasterENC.write(0) ; SlaveENC.write(0); totalTicks = 0;
  if (degree > 0) { //left turn
    SetMasterPower(-1 * power); SetSlavePower(power);
    while (MasterENC.read() > (-1 * tickGoal) || SlaveENC.read() < tickGoal) {
    }
    setBrakes(200, 200);
  } else if (degree < 0) {
    MasterENC.write(0) ; SlaveENC.write(0); totalTicks = 0;
    SetMasterPower( power); SetSlavePower(-power);
    while (MasterENC.read() < tickGoal || SlaveENC.read() > (-1 * tickGoal)) {
    }
    setBrakes(200, 200);

  }
}

void linedegreetravel(char degree, int power) {
  int tickGoal = 50 * 9.89 ;//compensation
  MasterENC.write(0) ; SlaveENC.write(0);
  if (degree == 'L') { //left turn
    SetMasterPower(-1 * power); SetSlavePower(power);
    while (MasterENC.read() > (-1 * tickGoal) || SlaveENC.read() < tickGoal) {
      QTRCRead();
    }
    SetMasterPower(-1 * power / 2); SetSlavePower(power / 2);
    while (1) {
      QTRCRead();
      if (sensorRead[5]) {
        break;
      }
    }
    setBrakes(255, 255);
  } else if (degree == 'R') {
    SetMasterPower( power); SetSlavePower(-power);
    while (MasterENC.read() < tickGoal || SlaveENC.read() > (-1 * tickGoal)) {
      QTRCRead();
    }
    SetMasterPower( power / 2); SetSlavePower(-power / 2);
    while (1) {
      QTRCRead();
      if (sensorRead[3 ]) {
        break;
      }
    }
    setBrakes(255, 255);

  }
}

void reverseline(int power) {
  MasterENC.write(0) ; SlaveENC.write(0); totalTicks = 0;
  MaxPower = -power;
  MasterPower = -power; SlavePower = -power;
  SetMasterPower(MasterPower); SetSlavePower(SlavePower);
  while (MasterENC.read() > -1 * REVERSETICKS) {

  }
  setBrakes(200, 200);
  analogWrite(11, 120);
  delay(20);
  analogWrite(11, 0);
  MaxPower = 0;
}






/*===========================================================

  Line Movement - maintainlinemovment

   ===========================================================
*/
char MaintainLineMovement() { //Only run inside nodetravel. Left only and Right only turns are not considered as Nodes
  boolean found_left = 0;
  boolean found_straight = 0;
  boolean found_right = 0;
  SetMasterPower(MasterPower); SetSlavePower(SlavePower);
  QTRCRead();
  //Serial.println(position);
  error =  2500 - (int)position;
  //Serial.println(error);
  int delta = error - lasterror;
  int change = (error * Kp) + (delta * Kd) + (sumerr * Ki);
  lasterror = error;
  sumerr += error;

  MasterPower = MaxPower - change;
  MasterPower = constrain(MasterPower, 0, 255);
  SlavePower  = MaxPower + change;
  SlavePower = constrain(SlavePower, 0, 255);

  int frontdis = sonar3.ping() / 57;
  Serial.println(frontdis);

  if (sensorRead[0] || sensorRead[1]) {
    found_left = 1;
  }
  if (sensorRead[8] || sensorRead[9]) {
    found_right = 1;
  }

  if (found_left || found_right) {
    //run for a wee bit
    SetMasterPower(5); SetSlavePower(5);
    MasterENC.write(0); SlaveENC.write(0);
    while (MasterENC.read() < OVERSHOOT_TICKS && SlaveENC.read() < OVERSHOOT_TICKS) {
      QTRCRead();
      if (sensorRead[0] || sensorRead[1]) {
        found_left = 1;
      }
      if (sensorRead[8] || sensorRead[9]) {
        found_right = 1;
      }

    }
    SetMasterPower(2); SetSlavePower(2);
    MasterENC.write(0); SlaveENC.write(0);
    while (1) {
      if(MasterENC.read() > OVERSHOOT_TICKS){
        setMasterBrake(255);
        SetSlavePower(10);
        while(SlaveENC.read() < MasterENC.read()){
          
        }
        setSlaveBrake(255);
        break;
      }else if(SlaveENC.read() > OVERSHOOT_TICKS){
        setSlaveBrake(255);
        SetMasterPower(10);
        while(MasterENC.read() < SlaveENC.read()){
          
        }
        setMasterBrake(255);
        break;
      }
      
    }
    QTRCRead();
    if (sensorRead[2] || sensorRead[3] || sensorRead[4] || sensorRead[5] || sensorRead[6] || sensorRead[7]) {
      found_straight = 1;
      digitalWrite(YELLOWLED, HIGH);

    }
    if (found_left && !found_straight && !found_right) {
      delay(200);
      linedegreetravel('L', TURNSPEED);
      delay(200);
      MasterPower = MaxPower; SlavePower = MaxPower;
    } else if (found_right && !found_straight && !found_left) {
      delay(200);
      linedegreetravel('R', TURNSPEED);
      delay(200);
      MasterPower = MaxPower; SlavePower = MaxPower;
    } else if (found_left && (found_straight || found_right)) {
      nodeflag = 1;
      return 'L';
    } else if (found_straight && !found_left) {
      nodeflag = 1;
      return 'F';
    }

  } else if (SensorPanelVal == 0) {
    setBrakes(255, 255);
    nodeflag = 1;
    int caught = 0;
    for (int l = 0 ; l < 5; l++) {
      front = sonar3.ping_median() / 57;
      if (front != 0) {
        return 'C';
        caught = 1;
      }
    }
    if (caught == 0) {
      return 'B';
    }
  }else if(frontdis >7 && frontdis <10 && which_turn == 2 ){
    setBrakes(250,250);
    nodeflag = 1;
    return 'U';
  }  else {
    return 'W';
  }

}

/*===========================================================

  Encoder Movement - MaintainMovement

   ===========================================================
*/


void MaintainMovement() { //Only run inside inchtravel,mmtravel,sharpleft,sharpright
  
  SetMasterPower(MasterPower); SetSlavePower(SlavePower);
  error =  (MasterENC.read()) - (SlaveENC.read());
  int delta = error - lasterror;
  int change = (error * 0.215) + (delta * 0.4);
  totalTicks += MasterENC.read(); MasterENC.write(0); SlaveENC.write(0);
  lasterror = error;
  sumerr += error;

  MasterPower = MaxPower - change;
  MasterPower = constrain(MasterPower, 0, 255);
  SlavePower  = MaxPower + change;
  SlavePower = constrain(SlavePower, 0, 255);
}

/*===========================================================

  Stop Commands

   ===========================================================
*/


void setMasterBrake(int brake) {
  // normalize brake
  if (brake < 0)
  {
    brake = -brake;
  }
  if (brake > 255)  // Max brake
    brake = 255;
  digitalWrite(LMOTOR1, LOW);
  digitalWrite(LMOTOR2, LOW);
  if (brake == 255) {
    digitalWrite(LMOTORSPEED, HIGH);
  } else {
    analogWrite(LMOTORSPEED, brake); // default to using analogWrite, mapping 400 to 255
  }
}

void setSlaveBrake(int brake) {
  // normalize brake
  if (brake < 0)
  {
    brake = -brake;
  }
  if (brake > 255)  // Max brake
    brake = 255;
  digitalWrite(RMOTOR1, LOW);
  digitalWrite(RMOTOR2, LOW);
  if (brake == 255) {
    digitalWrite(RMOTORSPEED, HIGH);
  } else {
    analogWrite(RMOTORSPEED, brake); // default to using analogWrite, mapping 400 to 255
  }
}

void setBrakes(int m1Brake, int m2Brake)
{
  setMasterBrake(m1Brake);
  setSlaveBrake(m2Brake);
}
