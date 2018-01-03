#include <max6675.h>
#include <LiquidCrystal.h>
//=====pin definitions=====
const int thermoDO = 4;
const int thermoCS = 6;
const int thermoCLK = 5;
const int encoderPinA = 3;
const int encoderPinB = 2;
const int sleep_sensor = 12;
const int heater = 13;
unsigned volatile int tempset = 0;
int temp = 0;
volatile int n = LOW;
volatile int encoderPinALast = LOW;
bool state;
bool pid_enable = true;
const int stand_temp = 175;

//=====object definitions ====
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

void setup() {
	pinMode(heater, OUTPUT);
	pinMode(encoderPinA, INPUT_PULLUP);
	pinMode(encoderPinB, INPUT_PULLUP);
	pinMode(sleep_sensor, INPUT_PULLUP);
	pinMode(13, OUTPUT);		
	pinMode(13, OUTPUT);
	attachInterrupt(digitalPinToInterrupt(encoderPinA), updateEncoder, CHANGE);
	attachInterrupt(digitalPinToInterrupt(encoderPinB), updateEncoder, CHANGE);
	delay(500);//max6675 sttabilise
	lcd.begin(16, 2);
	Serial.begin(9600);

	lcd.setCursor(0, 1);//initial display draw 
	lcd.print("set=");	
	lcd.setCursor(0, 0);
	lcd.print("temp=");
	
}

void loop() {
  
	digitalWrite(heater, LOW);// to get themp  heater must be off
    delay(400);
	temp = thermocouple.readCelsius();

	lcd.setCursor(5, 0);// curent temp display update 
	lcd.print("   ");
	lcd.setCursor(5, 0);
	lcd.print(temp);

	
	state = digitalRead(sleep_sensor);//"parking sensor" 
	if (state == 0)
	{
		pid_enable = false;	
		
	}
	else {
		//when out of stand the system will start
		pid_enable = true;
	}


	if (pid_enable == true) {		
		 if (temp < tempset) {
			  digitalWrite(heater, HIGH);
		}
		if (temp > tempset) {
			digitalWrite(heater, LOW);
		}
	}
	else
	{
		if (temp < stand_temp ){
			digitalWrite(heater, HIGH);
		}
		if (temp > stand_temp) {
			digitalWrite(heater, LOW);
		}
	}//when parked a secont pid loop takes over and keeps the temp at 175c for tip protection
	

	Serial.print(temp);
	Serial.print("|");
	Serial.print(tempset);
	Serial.print("|");
	Serial.println(millis());//test
delay(900);
}

void updateEncoder() {
	n = digitalRead(encoderPinA);
	if ((encoderPinALast == LOW) && (n == HIGH)) {
		if (digitalRead(encoderPinB) == LOW) {
			if (tempset>0) {
				tempset = tempset - 10;
			}
		}
		else {
			if (tempset<400) {
				tempset = tempset + 10;
			}
		}
		lcd.print("   ");
		lcd.setCursor(4, 1);
		lcd.print(tempset);
	}
	encoderPinALast = n;
}
