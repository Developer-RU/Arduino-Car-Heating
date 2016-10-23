#include <Stepper.h>
#include <OneWire.h>
#include <TimerOne.h>

const int count_steps = 50;  //  колличество шагов которое нужно отмотать вперед
const int stepsPerRevolution = 8; // шагов открытия заслонки 
const int spped_manual = 120; // скорость ручного режима

OneWire ds(10);
Stepper myStepper(stepsPerRevolution, 2, 3, 4, 5);

boolean mode = true;
boolean is_null = false;

void setup()
{   
    Serial.begin(9600);
    Serial.println("Go settings");
    Serial.println("");
    
    for(int i = 2; i <= 5; i++) pinMode(i, OUTPUT);
    for(int i = 7; i <= 9; i++) pinMode(i, INPUT_PULLUP);
    
    myStepper.setSpeed(600); 

    Serial.println("Go searching null position");
    while(digitalRead(7))
    {
        myStepper.step(-stepsPerRevolution);
        Serial.println("Step -");
    }
    Serial.println("End searching null position");
    Serial.println("");
    
    Serial.print("Go works position to ");
    Serial.print(count_steps);
    Serial.println("steps");
    for(int i = 0; i < count_steps; i++) 
    {
        myStepper.step(stepsPerRevolution);
        Serial.print("Step "); 
        Serial.println(i);
    }
    Serial.println("End works position");
    Serial.println("");
    
    mode = (digitalRead(8) && digitalRead(9)) ? true : false;
    if(mode) myStepper.setSpeed(5); // время прогрева без датчика
     
    Timer1.initialize();
    Timer1.attachInterrupt(timer_action);

    Serial.println("End settings");
    Serial.println("");

    Serial.println("Start program");
    Serial.println("");
}

void timer_action() 
{
     if(mode)
     {
          int livetemp = get_temp();
          int maxtemp = 90;

          int ostatokTemp = 90 - livetemp; // получаем разность темпиратуры (если отрицательная то сложение будет)
          int ostatokSpeed = 35 - 2; // получаем разность скорости от минимума к максимуму

          int a = ostatokTemp / 100;
          int b = ostatokSpeed / 100;

          int setspeed = ostatokTemp * ostatokSpeed;
          
          myStepper.setSpeed(setspeed);
          
          mode = (digitalRead(8) && digitalRead(9)) ? true : false;
          
          if(!mode) 
          {
              myStepper.setSpeed(35);
              Serial.println("Manual mode start");
              Serial.println("");
          }
     }
}

int get_temp() 
{
    byte data[2]; ds.reset(); 
    ds.write(0xCC); ds.write(0x44);
    delay(750); ds.reset();
    ds.write(0xCC); ds.write(0xBE);
    data[0] = ds.read(); data[1] = ds.read();
    int Temp = (data[1]<< 8)+data[0];
    Temp = Temp>>4;
    Serial.println(Temp);
    return Temp;
}

void loop()
{
    if(mode)
    {
        if(digitalRead(7)) 
        {
            myStepper.step(-stepsPerRevolution);
            Serial.println("Auto Step -");
        }
        else
        {
         mode = !mode;    
            Serial.println("End program");
        }
    }
    else if(!mode)
    {
        myStepper.setSpeed(spped_manual); // скорость ручного режима 
    
        if(!digitalRead(8)) 
        {
            myStepper.step(stepsPerRevolution);
            Serial.println("Manual Step +");
        }
        else if(!digitalRead(9) && digitalRead(7))
        {
            myStepper.step(-stepsPerRevolution);
            Serial.println("Manual Step -");
        }
    }
}
