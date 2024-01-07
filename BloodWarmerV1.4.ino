#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

byte tempIcon[] = {
  B01110,
  B01010,
  B01010,
  B11011,
  B11111,
  B11111,
  B11111,
  B01110
};
byte setIcon[] = {
  B11111,
  B11111,
  B11000,
  B11111,
  B11111,
  B00011,
  B11111,
  B11111
};
byte degIcon[] = {
  B00110,
  B01001,
  B01001,
  B01001,
  B00110,
  B00000,
  B00000,
  B00000
};

#define sensorPin A0
#define heaterPin 11 //digital pin support pwm
#define buttonUp 2
#define buttonDown 4
#define buttonEnter 7
#define ledPin 12
#define buzzerPin 10 

//OneWire oneWire(thermoPin); //Membuat variabel oneWire berdasarkan PIN yang telah didefinisikan
//DallasTemperature sensor(&oneWire);
float temp;
float vRef=4.97;
int setTemp;
int counter=32;
bool status=false;
//int prevTemp;
//int prevSetTemp;
//int prevStatus;
int ledStat;
int heaterStat;
int buzzerStat;
//int pwm=255; //nilai 0-255 semakin tinggi semakin cepat panas
String lcdHeatStat;
unsigned long buttonTimer;
unsigned long sensorTimer;
unsigned long dispTimer;

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
  pinMode(heaterPin, OUTPUT);
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonEnter, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(buzzerPin, HIGH); //matikan buzzer saat awal nyala

  lcd.begin();
  lcd.backlight();
  lcd.clear(); 
  lcd.setCursor(6,0);
  lcd.print("ZULL");
  lcd.setCursor(2,1);
  lcd.print("BLOOD WARMER");
  delay(1000);
  lcd.clear();

  lcd.createChar(0, tempIcon);
  lcd.createChar(1, setIcon);
  lcd.createChar(2, degIcon);
  setTemp=counter;
}

void loop() {
  if(millis()-sensorTimer>=100){
    temp=(analogRead(sensorPin)/1023.0)*vRef/0.01;
  }

  if(millis()-buttonTimer>=350){ //memeriksa apakah tombol ditekan, tiap 50ms
    if(digitalRead(buttonUp)==LOW && counter<40){
      counter++;
      setTemp=counter;
    }
    else if(digitalRead(buttonDown)==LOW && counter>32){
      counter--;
      setTemp=counter;
    }
    else if(digitalRead(buttonEnter)==LOW){
      status=true;
    }
    else if(digitalRead(buttonEnter)==HIGH){
      status=false;
    }
    buttonTimer=millis();
  }

  if(status==true){
    ledStat=0; //led menyala
    lcdHeatStat="ON";
    if(status==true && temp<setTemp){
      heaterStat=0; //heater nyala
    }
    else if(status==true && temp>=setTemp){
      heaterStat=1; //heater mati
    }
  }
  else if(status==false){
    lcdHeatStat="OFF";
    ledStat=1; //led mati
    heaterStat=1; //heater mati
  }
  
  if(temp<0 || temp>40){
    buzzerStat=0; //buzzer nyala
  }
  else{
    buzzerStat=1; //buzzer mati
  }

  display();

  //if (temp != prevTemp || setTemp != prevSetTemp || status != prevStatus) { //melakukan update layar hanya jika ada perubahan nilai pada variabel
  //  display();
  //  prevTemp = temp;
  //  prevSetTemp = setTemp;
  //  prevStatus = status;
  //}
  digitalWrite(ledPin, ledStat);
  digitalWrite(heaterPin, heaterStat);
  digitalWrite(buzzerPin, buzzerStat);
}

void display(){
  if(millis() - dispTimer >= 100){ //membatasi refresh lcd maksimal 100ms
    dispTimer=millis();
    lcd.setCursor(0,0);
    lcd.write(byte(0)); //menampilkan icon temperatur
    lcd.setCursor(0,1);
    lcd.write(byte(1)); //menampilkan icon set temperatur
    lcd.setCursor(8,0);
    lcd.write(byte(2)); //menampilkan icon derajat
    lcd.setCursor(9,0);
    lcd.print("C"); 

    lcd.setCursor(13,0);
    lcd.print("   ");
    lcd.setCursor(13,0);
    lcd.print(lcdHeatStat); //menampilkan kondisi pemanas mati/menyala

    lcd.setCursor(2,0);
    lcd.print("   ");
    lcd.setCursor(2,0);
    lcd.print(temp); //menampilkan nilai temperatur dengan 0.xxx
    Serial.print(temp);

    lcd.setCursor(2,1);
    lcd.print("   ");
    lcd.setCursor(2,1);
    lcd.print(setTemp); //menampilkan nilai set
  }
}
