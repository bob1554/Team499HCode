/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       VEX                                                       */
/*    Created:      Thu Sep 26 2019                                           */
/*    Description:  Competition Template                                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Controller1          controller                    
// Frontleft            motor         1               
// backleft             motor         2               
// frontright           motor         8               
// backright            motor         4               
// intake               motor         9               
// OutputSpin           motor         11              
// Outspin2             motor         17              
// Intake2              motor         19              
// Controller2          controller                    
// Endgame              digital_out   A               
// Flicker              digital_out   B               
// rightEncoder         encoder       E, F            
// LeftEncoder          encoder       G, H            
// centerEncoder        encoder       C, D            
// Inertial             inertial      18              
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"
#include <iostream>
#include <cmath>


using namespace vex;


// A global instance of competition
competition Competition;

// define your global instances of motors and other devices here

/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/
/*class Button
{
  public:
  int x, y, width, height;
  std::string text;
  
  Button(int x, int y, int width, int height, std::string text):
  x(x), y(y), height(height), text(text){}

  void render(){
    Brain.Screen.drawRectangle(x, y, width, height);
    Brain.Screen.printAt(x + 10, y + 10, false, text.c_str());
  }

  bool isClicked(){
    if(Brain.Screen.pressing() && Brain.Screen.xPosition() >= x && Brain.Screen.xPosition() <= x + width && Brain.Screen.yPosition() >= y && Brain.Screen.yPosition() <= y + height){
      return true;
    }
    return false; 
  }


};

Button autonButtons[] = {
  Button(10, 10, 150, 50, "Left"),
  Button(170, 10, 150, 50, "Right"),

};*/
void pre_auton(void) {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  /*while(1)
  {
    Brain.Screen.clearScreen(vex::white);

    if(!Competition.isEnabled()){
      for(int i = 0; i < 2; i++){
        autonButtons[i].render();
        if(autonButtons[i].isClicked()){
          autonButtons[autonToRun]
        }
      }
    }
  }*/

  
  

  // All activities that occur before the competition starts
  // Example: clearing encoders, setting servo positions, ...
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/



//PID for Linear Movement (back and forth)
void linearPID(double rot, double reduction, bool drift){
  double linearkP = 0.8;
  double linearkI = 0.004;
  double linearkD = 0.00000;

  int linearError = 0;
  int linearPrevError = 0;
  int linearDerivative;
  int linearTotalError = 0;


  int stopTollerance = 10;
  double kDrift = 10;

  LeftEncoder.resetRotation();
  rightEncoder.resetRotation();


  double intOrientation = Inertial.rotation();

  do{
    int leftEncoderPosition = LeftEncoder.position(rotationUnits::deg);
    int rightEncoderPosition = rightEncoder.position(rotationUnits::deg);

    int avgPosition = (leftEncoderPosition + rightEncoderPosition) / 2;

    std::cout << linearError << "\n";

    linearError = rot - avgPosition;
    linearDerivative = linearError - linearPrevError;
    linearTotalError += linearError;

    double driveTrainMotorPower = linearError * linearkP + linearDerivative * linearkD + linearTotalError * linearkI;

    linearPrevError = linearError; 

    double driftCounter;

    if(drift){
      if(rot < 0){
        driftCounter = (Inertial.rotation() + intOrientation) * kDrift;
      }else {
        driftCounter = -1 * (Inertial.rotation() - intOrientation) * kDrift; 
      }
    } else {
      driftCounter = 0;
    }
    Frontleft.spin(forward, (driveTrainMotorPower + driftCounter) / (50 * reduction), voltageUnits::volt);
    backleft.spin(forward,(driveTrainMotorPower + driftCounter) / (50 * reduction), voltageUnits::volt);
    frontright.spin(forward, (driveTrainMotorPower - driftCounter) / (50 * reduction), voltageUnits::volt);
    backright.spin(forward, (driveTrainMotorPower - driftCounter) / (50 * reduction), voltageUnits::volt); 

    wait(5, timeUnits::msec);
  }
  while(linearError > stopTollerance || linearError < -1 * stopTollerance);
    Frontleft.stop(brake);
    backleft.stop(brake);
    frontright.stop(brake);
    backright.stop(brake);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////




void turnPID(double degrees){
   
  double turnkP = 0.5;
  double turnkI = 0.004;
  double turnkD = 0.0;
  

  int turnError = 0;
  int turnPrevError = 0;
  int turnDerivative;
  int turnTotalError = 0;

  int stopTollerance = 2;

  LeftEncoder.resetRotation();
  rightEncoder.resetRotation();

  do{
    int currentPosition = Inertial.rotation();

    std::cout << turnError << "\n";
    
    turnError = degrees - currentPosition;

    turnDerivative = turnError - turnPrevError;
    turnTotalError += turnError;

    double driveTrainTurnPower = turnError * turnkP + turnDerivative * turnkD + turnTotalError * turnkI;

    turnPrevError = turnError;

    Frontleft.spin(forward, driveTrainTurnPower/ 10, voltageUnits::volt);
    backleft.spin(forward, driveTrainTurnPower / 10, voltageUnits::volt);
    frontright.spin(reverse, driveTrainTurnPower /  10, voltageUnits::volt);
    backright.spin(reverse, driveTrainTurnPower / 10, voltageUnits::volt);

    wait(5, timeUnits::msec);
  }
  while(std::abs(turnError) > stopTollerance);

  Frontleft.stop(brake);
  backleft.stop(brake);
  frontright.stop(brake);
  backright.stop(brake);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////




void strafePID(double rot, bool drift){
  
  double strafekP = 0.8;
  double strafekI = 0.000003;
  double strafekD = 0.02;

  int strafeError = 0;
  int strafePrevError = 0;
  int strafeDerivative;
  int strafeTotalError = 0;

  int stopTollerance = 10;
  double kDrift = 8;

  centerEncoder.resetRotation();
  rightEncoder.resetRotation();
  LeftEncoder.resetRotation();

  double intOrientation = Inertial.rotation();

  double driftCounter;

    if(drift){
      
      if(rot < 0){
        driftCounter = (Inertial.rotation() + intOrientation) * kDrift;
      }else {
        driftCounter = -1 * (Inertial.rotation() + intOrientation) * kDrift; 
      }
    } else {
      driftCounter = 0;
    }

  

  do{
    int centerPosition = centerEncoder.position(rotationUnits::deg);

    std::cout << strafeError << "\n";
    
    strafeError = rot - centerPosition;

    strafeDerivative = strafeError - strafePrevError;
    strafeTotalError += strafeError;

    double driveTrainTurnPower = strafeError * strafekP + strafeDerivative * strafekD + strafeTotalError * strafekI;

    strafePrevError = strafeError;
    

    Frontleft.spin(reverse, driveTrainTurnPower + driftCounter / 10, voltageUnits::volt);
    backleft.spin(forward, driveTrainTurnPower + driftCounter / 10, voltageUnits::volt);
    frontright.spin(forward, driveTrainTurnPower + driftCounter / 10, voltageUnits::volt);
    backright.spin(reverse, driveTrainTurnPower + driftCounter / 10, voltageUnits::volt);

    wait(5, timeUnits::msec);
  }
  while(strafeError > stopTollerance || strafeError < -1 * stopTollerance);
  Frontleft.stop(brake);
    backleft.stop(brake);
    frontright.stop(brake);
    backright.stop(brake);

}

void intakeStart(bool inverse){
  if(inverse == true){
  intake.spin(reverse, 100, pct);
  Intake2.spin(reverse, 100, pct);
  }
  else{
    intake.spin(forward, 100, pct);
  Intake2.spin(forward, 100, pct);
  }

}
void intakeStop(){
  intake.stop(brake);
  Intake2.stop(brake);

}

void trn(bool dir, double deg){
int tim = deg * 6;

if(dir){
  Frontleft.spin(forward, 200,rpm);
    backleft.spin(forward,  200,rpm);
    frontright.spin(reverse, 200, rpm);
    backright.spin(reverse, 200, rpm);
    wait(tim, msec);
}
else{
Frontleft.spin(reverse, 200,rpm);
    backleft.spin(reverse,  200,rpm);
    frontright.spin(forward, 200, rpm);
    backright.spin(forward, 200, rpm);
    wait(tim, msec);
}
  Frontleft.stop(brake);
  backleft.stop(brake);
  frontright.stop(brake);
  backright.stop(brake);
}

void braking(){
  Frontleft.stop(brake);
  frontright.stop(brake);
  backright.stop(brake);
  backleft.stop(brake);
}

int flickerForAuton(){
  Flicker.set(false);
    wait(0.5, sec);
    Flicker.set(true);

    return 0;
}

void shooting(int shots, int speed){
  int taken = 1;
  OutputSpin.spin(forward, speed, rpm);
  Outspin2.spin(forward, speed, rpm);
  wait(1, sec);
  
  while(taken <= shots){
    flickerForAuton();
    wait(1, sec);
    taken++;
  }
  OutputSpin.stop(coast);
  Outspin2.stop(coast);

}

void autonomous(void) {
  /*linearPID(-200, 1, true);
  wait(0.5, timeUnits::sec);
  strafePID(730, true);
  wait(0.5, timeUnits::sec);
  linearPID(160, 1, true);
  wait(0.5, timeUnits::sec);
  intakeStart(true);
  wait(0.20, timeUnits::sec);
  intakeStop();*/

  
  Frontleft.spin(fwd, 50, pct);
  frontright.spin(fwd, 50, pct);
  backright.spin(fwd, 50, pct);
  backleft.spin(fwd, 50, pct);
  wait(0.25, timeUnits::sec);
  braking();
  wait(0.5, timeUnits::sec);
  intakeStart(true);
  wait(.2, timeUnits::sec);
  intakeStop();
  


   

//Roller One 
/*Endgame.set(false);
Flicker.set(true);
wait(.5, timeUnits::sec);
  Frontleft.spin(fwd, 50, pct);
  frontright.spin(fwd, 50, pct);
  backright.spin(fwd, 50, pct);
  backleft.spin(fwd, 50, pct);
  wait(0.2, timeUnits::sec);
  braking();
  intakeStart(true);
  wait(.4, timeUnits::sec);
  intakeStop();
  wait(.2, timeUnits::sec);
  
  //Roller 2
  linearPID(-740, 2, false);
  wait(.5, timeUnits::sec);
  trn(true, 90);
  wait(.5, timeUnits::sec);
  linearPID(825, 2, true);
  wait(.5, timeUnits::sec);
  intakeStart(false);
  wait(.4, timeUnits::sec);
  intakeStop();
  wait(0.5, timeUnits::sec);

//Move from roller to middle 
  wait(1, timeUnits::sec);
  linearPID(-2000, 5, false);
  wait(.5, timeUnits::sec);
  trn(false, 90);

//from middle to other side 
  wait(1, timeUnits::sec);
  linearPID(-3010, 7, false);
  wait(.5, timeUnits::sec);
  trn(false, 83);
  wait(.5, timeUnits::sec);
  shooting(2, 120);
  wait(.5, timeUnits::sec);
  trn(false, 305);
  


//Roler 3
 wait(.5, timeUnits::sec);
  linearPID(2000, 7, false);
  wait(.5, timeUnits::sec);
  intakeStart(false);
  wait(.35, timeUnits::sec);
  intakeStop();


//Roller 4
  wait(1, timeUnits::sec);
  linearPID(-750, 2, false);
  wait(.5, timeUnits::sec);
  trn(false, 90);
  wait(.5, timeUnits::sec);
  linearPID(825, 2, true);
  wait(.5, timeUnits::sec);
  intakeStart(false);
  wait(.25, timeUnits::sec);
  intakeStop();
  wait(.5, timeUnits::sec);

//Endgame 
linearPID(-700, 2, false);
wait(.5, timeUnits::sec);
strafePID(-100, false);
wait(.5, timeUnits::sec);
trn(true, 50);
wait(.5, timeUnits::sec);
Endgame.set(true);*/






}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

//Print Encoder Values to Brain
int rightPosition(){
  Brain.Screen.setCursor(1,1);
  Brain.Screen.print("Right %f", rightEncoder.position(degrees));
  return 1;
}

int leftPosition(){
  Brain.Screen.setCursor(2,1);
  Brain.Screen.print("Left %f", LeftEncoder.position(degrees));
  return 1;
}

int turnPosition(){
  Brain.Screen.setCursor(3,1);
  Brain.Screen.print("turn %f", centerEncoder.position(degrees));
  return 1;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//Constant for drive train speed 
double numCutoff(double num, double cutoff){
    if(num > cutoff)
     num = cutoff;
    else if(num < -cutoff)
     num = -cutoff;
    return num;
  }


int flicker(){
  if(Controller1.ButtonUp.pressing()){
    Flicker.set(false);
    wait(0.5, sec);
    Flicker.set(true);
  }

  
  return 0;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////




void usercontrol(void) {

  //Multiplier for drive train speed
  int jvaluesLFM;
  int jvaluesRFM;
  int jvaluesLBM;
  int jvaluesRBM;


  int FlyWheelRPM; //Controlls speed of flywheel 

  while (1) {

    //prints encoder values to brain screen 
  rightPosition();
  leftPosition(); 
  turnPosition();

//Sets pneumatic pistion to put pressed out 
  Flicker.set(true);
  Endgame.set(false);

  if(Controller1.ButtonDown.pressing()){
  Endgame.set(true);
  }
  

  //Take controller axises for drivetrain speeds
  jvaluesLFM = Controller1.Axis3.position() + Controller1.Axis4.position() + (Controller1.Axis1.position() * 0.8);
  jvaluesRFM = Controller1.Axis3.position() -  Controller1.Axis4.position() - (Controller1.Axis1.position() * 0.8);
  jvaluesLBM = Controller1.Axis3.position() - Controller1.Axis4.position() + (Controller1.Axis1.position() * 0.8);
  jvaluesRBM = Controller1.Axis3.position() + Controller1.Axis4.position() - (Controller1.Axis1.position() * 0.8);

  Frontleft.spin(fwd, numCutoff(jvaluesLFM, 100), pct);
  backleft.spin(fwd, numCutoff(jvaluesLBM, 100), pct);
  frontright.spin(fwd, numCutoff(jvaluesRFM, 100), pct);
  backright.spin(fwd, numCutoff(jvaluesRBM, 100), pct);
  


//speed of flywheel presets  
  if(Controller1.ButtonA.pressing()){
    FlyWheelRPM = 130;
    
  }else if(Controller1.ButtonB.pressing()){
    FlyWheelRPM = 70;
    
  }else if(Controller1.ButtonX.pressing()){
    FlyWheelRPM = 75;

  }else if(Controller1.ButtonY.pressing()){
    FlyWheelRPM = 65;
    
  }else {
    FlyWheelRPM = 126; 
    
  }
  

//Starts flywheel when bottom right bumper is pressed 
  if(Controller1.ButtonR2.pressing()){
    OutputSpin.spin(forward, FlyWheelRPM, rpm);
    Outspin2.spin(forward, FlyWheelRPM, rpm);
    flicker();
  }

  
  
  else{
    OutputSpin.stop(coast);
    Outspin2.stop(coast);
  }
  

  //starts intake when top right button is pressed
  if(Controller1.ButtonR1.pressing()){
    intake.spin(forward, 100, pct);
    Intake2.spin(forward, 100, pct);
    
  }

  //starts intake when top left button is pressed, but in reverse
  else if(Controller1.ButtonL1.pressing()){
    intake.spin(reverse, 100, pct);
    Intake2.spin(reverse, 100, pct);
    
  }
  else{
    intake.stop(coast);
    Intake2.stop(coast);
  }

    wait(5, msec); // Sleep the task for a short amount of time to
                    // prevent wasted resources.
  }
}

//
// Main will set up the competition functions and callbacks.
//
int main() {
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(15, msec);
  }
}



