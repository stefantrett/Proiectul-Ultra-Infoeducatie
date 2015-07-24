#include <Servo.h> 
#include "SoftwareSerial.h"; 

Servo  clawServo;  

const int bluetoothTx = 4;
const int bluetoothRx = 2; 

SoftwareSerial bluetooth(bluetoothTx, bluetoothRx); 

unsigned int MOTOR1_PIN1 = 6;
unsigned int MOTOR1_PIN2 = 9;

const int buttonPin_motor = 3; 
const int buttonPin_claw = 5;  
const int buttonPin_disco = 8; 
const int pressButton = 7;  

const int led_yellow = 11;
const int led_green = 13;
const int led_blue = 12;

//analog
const int pin_temp = 0;
const int pin_umid = 2;
const int pin_lumi = 1;


int lastEmotion = 0;

int pos = 160;          

void setup() {

  clawServo.attach(10); 

  pinMode(led_yellow, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_blue, OUTPUT);
  
  pinMode(MOTOR1_PIN1, OUTPUT);
  pinMode(MOTOR1_PIN2, OUTPUT);
  
  pinMode(pressButton, INPUT);
  pinMode(buttonPin_claw, INPUT);
  pinMode(buttonPin_motor, INPUT);
  pinMode(buttonPin_disco, INPUT);
  
  Serial.begin(9600);
  bluetooth.begin(115200); 
}

void loop() {
  
  int buttonState_motor = digitalRead(buttonPin_motor);
  int buttonState_claw  = digitalRead(buttonPin_claw);
  int buttonState_disco  = digitalRead(buttonPin_disco);

  //Button MOTOR
  if (buttonState_motor == HIGH){
    Serial.println("motor pe 3");
    go(50);
    delay(5000);
    lastEmotion = 1;
  }
  else{
    go(0);
  }

  //Button CLAW
  if(buttonState_claw == HIGH)
  {
    Serial.println("claw pe 5");
    claw_grab();
    delay(20000);
    lastEmotion = 2;
  }
  else
  {
    clawServo.write(pos);
    delay(100);
  }

  //Button DISCO
  if(buttonState_disco == HIGH)
  {
    Serial.println("disco pe 8");
    for(int i=0;i<40;++i)
      disco();
      lastEmotion = 0;
  }
  else
  {
    for(int i=11;i<=13;++i)
      digitalWrite(i, LOW);
    delay(100);
  }

  transferBluetooth();
}



void transferBluetooth()
{
  switch (lastEmotion){
      case 0: 
      bluetooth.println("0");
      break;
      
      case 1: 
      bluetooth.println("1");
      break;
      
      case 2:
      bluetooth.println("2");
      break;
    }

    float temp = readTempInCelsius(10, pin_temp);
    int temperatura = (int)temp;
    bluetooth.print("temperature");
    bluetooth.print(temperatura);
    bluetooth.println(" C");

    float umid = citesteUmid();
    int U = (int)umid;
    bluetooth.print("humidity");
    bluetooth.print(U);
    bluetooth.println(" %");

    float lumi = analogRead(pin_lumi)/10.24;
    int lumin = (int)lumi;
    bluetooth.print("luminosity");
    bluetooth.print(lumin);
    bluetooth.println(" %");

    delay(830);
}



float citesteUmid()
{
  float nivelU;
  float nivelUmiditate; 

  for(int i=0;i<10;++i)
  {
    nivelU = analogRead(pin_umid);
    nivelUmiditate += 100 - nivelU/10.24; 
  }
  return nivelUmiditate /=(float)10;
}

float readTempInCelsius(int count, int pin) {
  float temperaturaMediata = 0;
  float sumaTemperatura;
  for (int i =0; i < count; i++) {
    int reading = analogRead(pin);
    float voltage = reading * 5.0;
    voltage /= 1024.0;
    float temperatureCelsius = (voltage - 0.5) * 100 ;
    sumaTemperatura = sumaTemperatura + temperatureCelsius;
  }
  return sumaTemperatura / (float)count;
}

void claw_grab()
{
  int i;
  int b = digitalRead(pressButton);

  for(i=pos;i>5 && b == LOW ;--i)
  {
    clawServo.write(i);
    b = digitalRead(pressButton);
    delay(10);
  }
  
  clawServo.write(i+5); 
}

void disco()
{
    for(int i=11;i<=13;++i){
      digitalWrite(i, HIGH);
      for(int j=11;j<=13;++j)
       {
         if(i != j)
            digitalWrite(j, LOW);
         }
       delay(100);
    }
}


void go(int speed)
{
  if (speed > 0)
  {
    analogWrite(MOTOR1_PIN1, speed);
    analogWrite(MOTOR1_PIN2, 0);
  }
  else
  {
    analogWrite(MOTOR1_PIN1, 0);
    analogWrite(MOTOR1_PIN2, -speed);
  }
}
