#include <Mouse.h>
#include <CapacitiveSensor.h>

CapacitiveSensor indexFinger = CapacitiveSensor(7, 9);
CapacitiveSensor middleFinger = CapacitiveSensor(7, 10);
CapacitiveSensor ringFinger = CapacitiveSensor(7, 11);

long valIndexFinger;
long valMiddleFinger;
long valRingFinger;

//Määritellään sisääntulopinnit
const int leftSensor = A0;
const int rightSensor = A1;
const int upSensor = A2;
const int downSensor = A3;

//const int xPin   = A4; Kytketty mutta ei käytettykkään.
const int yPin   = A5;

bool active = true;

int leftAverage = 0;
int rightAverage = 0;
int upAverage = 0;
int downAverage = 0;

//Laita debug 1 jos haluat printtaukset serial monitorille
bool debug = 0;

void setup() {
  pinMode(leftSensor, INPUT);
  pinMode(rightSensor, INPUT);
  pinMode(upSensor, INPUT);
  pinMode(downSensor, INPUT);
  //Serial.begin(19200);
}

void loop() {

  bool rotation;
  //Debug 1 jos haluat printtaukset
  if (debug == 1) {
    Serial.println(active);
  }

  if(active)
  {
    rotation = Rotation();
    if(rotation == 1)
    {
      MouseClickRotation();
    }
    else{
    MouseClick();
    //TiltControl();
    Average(leftSensor, rightSensor, upSensor, downSensor);
    Move(leftAverage, rightAverage, upAverage, downAverage);
    }
  }
  else
  {
    Activate();
  }
}
//////////////////////////////////////////////////
void Move(int leftAverage, int rightAverage, int upAverage, int downAverage)
{
  int xSpeed = rightAverage - leftAverage;
  int ySpeed = upAverage - downAverage;
  //Serial.print(xSpeed);
  //Serial.println(ySpeed);
  xSpeed = map(xSpeed, 0, 30, 0, 10);
  ySpeed = map(ySpeed, 0, 30, 0, 10);
  //Serial.print(xSpeed);
  //Serial.println(ySpeed);
  if (abs(ySpeed) <= abs(xSpeed)) {
    for (int i = 0; i < abs(xSpeed); i++)
    {
      if (xSpeed < 0) {
        Mouse.move(1, 0, 0);
      }
      else
      {
        Mouse.move(-1, 0, 0);
      }
      if (abs(ySpeed) > i) {
        if (ySpeed < 0) {
          Mouse.move(0, -1, 0);
        }
        else
        {
          Mouse.move(0, 1, 0);
        }
      }
    }
  }

  if (abs(xSpeed) < abs(ySpeed)) {
    for (int i = 0; i < abs(ySpeed); i++)
    {
      if (ySpeed < 0) {
        Mouse.move(0, -1, 0);
      }
      else
      {
        Mouse.move(0, 1, 0);
      }
      if (abs(xSpeed) > i) {
        if (xSpeed < 0) {
          Mouse.move(1, 0, 0);
        }
        else
        {
          Mouse.move(-1, 0, 0);
        }
      }
    }
  }
}

void Average(int leftSensor, int rightSensor, int upSensor, int downSensor)
{
  float leftAverages[3];
  float rightAverages[3];
  float upAverages[3];
  float downAverages[3];
  int leftValues[20];
  int rightValues[20];
  int upValues[20];
  int downValues[20];

  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 20; j++)
    {
      leftValues[j] = analogRead(leftSensor);
      rightValues[j] = analogRead(rightSensor);
      upValues[j] = analogRead(upSensor);
      downValues[j] = analogRead(downSensor);
    }
    leftAverages[i] = MaxMinAverage(leftValues);
    rightAverages[i] = MaxMinAverage(rightValues);
    upAverages[i] = MaxMinAverage(upValues);
    downAverages[i] = MaxMinAverage(downValues);
  }

  for (int i = 0; i < 3; ++i)
  {
    int a = 0;
    for (int j = i + 1; j < 3; ++j)
    {
      if (leftAverages[i] > leftAverages[j])
      {
        a =  leftAverages[i];
        leftAverages[i] = leftAverages[j];
        leftAverages[j] = a;
      }
    }
  }
  for (int i = 0; i < 3; ++i)
  {
    int a = 0;
    for (int j = i + 1; j < 3; ++j)
    {
      if (rightAverages[i] > rightAverages[j])
      {
        a =  rightAverages[i];
        rightAverages[i] = rightAverages[j];
        rightAverages[j] = a;
      }
    }
  }
  for (int i = 0; i < 3; ++i)
  {
    int a = 0;
    for (int j = i + 1; j < 3; ++j)
    {
      if (downAverages[i] > downAverages[j])
      {
        a =  downAverages[i];
        downAverages[i] = downAverages[j];
        downAverages[j] = a;
      }
    }
  }
  for (int i = 0; i < 3; ++i)
  {
    int a = 0;
    for (int j = i + 1; j < 3; ++j)
    {
      if (upAverages[i] > upAverages[j])
      {
        a =  upAverages[i];
        upAverages[i] = upAverages[j];
        upAverages[j] = a;
      }
    }
  }
  leftAverage = leftAverages[1];
  rightAverage = rightAverages[1];
  upAverage = upAverages[1];
  downAverage = downAverages[1];
}
///////////////////////////////////////////////
int MaxMinAverage(int sensorValues[])
{
  int average;
  int maximum = 0;
  int minimum = 1024;
  for (int i = 0; i  < 20; i++)
  {
    if (sensorValues[i] > maximum )
    {
      maximum = sensorValues[i];
    }
    if (sensorValues[i] < minimum )
    {
      minimum = sensorValues[i];
    }
  }
  average = maximum - minimum;

  //Debug 1 jos haluat printtaukset
  if (debug == 1)
  {
    Serial.print("Average: ");
    Serial.println(average);
  }
  return average;
}
/////////////////////////////////////////
bool Rotation()
{
  int y;
  float ay;
  int yVertical = 0;

  y = analogRead(yPin);
  ay = 0.1439 * y - 48.204;

  if (ay < -7.5) {
    yVertical = 1;
  }
  if (ay > -5) {
    yVertical = 0;
  }
  //Serial.println(ay);
  return yVertical;
}

/////////////////////////////////
void MouseClick()
{
  valIndexFinger = indexFinger.capacitiveSensor(30);
  valMiddleFinger = middleFinger.capacitiveSensor(30);
  valRingFinger = ringFinger.capacitiveSensor(30);
  //Serial.println(valMiddleFinger);
  //Serial.println(valIndexFinger);

  if(valIndexFinger >= 10000 && valMiddleFinger >= 10000)
  {
    Mouse.press(MOUSE_LEFT);
    delay(50);
  }
  else if (valIndexFinger >= 20000)
  {
    Mouse.click();
    delay(50);
  }
  else if (valMiddleFinger >= 50000)
  {
    Mouse.click(MOUSE_RIGHT);
    delay(50);
  }

  else if (valRingFinger >= 20000)
  {
    Mouse.click(MOUSE_MIDDLE);
    delay(50);
  }
  else
  {
    Mouse.release(MOUSE_LEFT);
  }

  if(valIndexFinger >= 25000 && valMiddleFinger >= 25000 && valRingFinger >= 25000)
  {
    active = !active;
  }
}
///////////////////////////////////////
void MouseClickRotation()
{
  char page_Up = KEY_PAGE_UP;
  char page_Down = KEY_PAGE_DOWN;
  valIndexFinger = indexFinger.capacitiveSensor(30);
  valMiddleFinger = middleFinger.capacitiveSensor(30);
  valRingFinger = ringFinger.capacitiveSensor(30);

  if (valIndexFinger >= 10000 && valMiddleFinger >= 10000)
  {
    Mouse.move(0, 0, 10);
    delay(100);
  }
    else if (valIndexFinger >= 20000)
  {
    Mouse.move(0, 0, 1);
    delay(100);
  }
  if (valRingFinger >= 10000 && valMiddleFinger >= 10000)
  {
    Mouse.move(0, 0, -10);
    delay(100);
  }
  else if (valRingFinger >= 50000)
  {
    Mouse.move(0, 0, -1);
    delay(100);
  }
  if (valIndexFinger >= 25000 && valMiddleFinger >= 25000 && valRingFinger >= 25000)
  {
    active = !active;
  }
}
///////////////////////////////////////////////////////
void Activate()
{
  valIndexFinger = indexFinger.capacitiveSensor(30);
  valMiddleFinger = middleFinger.capacitiveSensor(30);
  valRingFinger = ringFinger.capacitiveSensor(30);
  
  if(valIndexFinger >= 25000 && valMiddleFinger >= 25000 && valRingFinger >= 25000)
  {
    active = !active;
  }
}
///////////////////////////////////////
/*void TiltControl()
{
  float ay;
  int y = 0;
  int yArr[3];

  for (int i = 0; i < 3; i++)
  {
    yArr[i] = analogRead(yPin);
  }

  for (int i = 0; i < 3; i++)
  {
    int a = 0;

    for (int j = i + 1; j < 3; ++j)
    {
      if (yArr[i] > yArr[j])
      {
        a =  yArr[i];
        yArr[i] = yArr[j];
        yArr[j] = a;
      }
    }
  }

  y = yArr[1];
  ay = (0.1439 * y - 48.204) + 0.7;

  //Debug 1 jos haluat printtaukset
  if (debug == 1)
  {
    //Serial.print("ay:");
    //Serial.println(ay);
  }

  if (ay > 2.5)
  {
    Mouse.move(0, 0, -1);
    delay(100);
  }

  if (ay < -2.5)
  {
    Mouse.move(0, 0, 1);
    delay(100);
  }
}*/
