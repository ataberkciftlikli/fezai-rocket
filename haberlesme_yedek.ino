#include <SoftwareSerial.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "MPU9250.h"

MPU9250 IMU(Wire,0x68);
int status;

int gelenyanit;

Adafruit_BMP280 bmp; // I2C

SoftwareSerial GPS(10, 11); // Rx, Tx

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "34101";

String saat = "";    // hhmmss.00
String enlem = "";   // DDMM.MMMM Kuzey/Güney N/S
String boylam = "";  // DDMM.MMMM Doğu/Batı E/W
String gpsirtifa = "";  // metre

float ivme;
float irtifa;
double enlemdouble;
double boylamdouble;
float irtifaonceki;

int baglantideneme = 0;
int rakim = 863; //RAKIM GİRİLECEK!!!

boolean apogee = 0;
boolean sistem = 0;
float durum = 0;

boolean sistemdevrede=0;
int buzzer = 28;
int led = 30;
int role1 = 33;
int role2 = 34;

int loopsayi = 0;

float Array[8];

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  GPS.begin(9600);

    if (!bmp.begin()) {
    //Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

   status = IMU.begin();
  if (status < 0) {
    /*Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);*/
    while(1) {}
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

pinMode(buzzer,OUTPUT);
pinMode(led,OUTPUT);
pinMode(role1,OUTPUT);
pinMode(role2,OUTPUT);
digitalWrite(role1,HIGH);
digitalWrite(role2,HIGH);
}

void loop() {
  
  
 if(loopsayi==0) {
    //Serial.println("HALLEY'İN ÇIRAKLARI ROKET TAKIMI");
    digitalWrite(role1,HIGH);
    digitalWrite(role2,HIGH);
    delay(5000);
    digitalWrite(buzzer,HIGH);
    digitalWrite(led,HIGH);
    delay(5000);
    digitalWrite(buzzer,LOW);
    digitalWrite(led,LOW);
    loopsayi=loopsayi + 1;
  }

  if (sistemdevrede==1) {

    digitalWrite(buzzer,HIGH);
    
  IMU.readSensor();

 

  irtifa = bmp.readAltitude(1013.25)-rakim;

  if(irtifa>=10 && durum==0) {
    durum=1;
  }
  
if (durum==1 && irtifaonceki-irtifa>=1) {
    digitalWrite(role1,LOW);
    digitalWrite(buzzer,HIGH);
    delay(1200);
    digitalWrite(role1,HIGH);
    digitalWrite(buzzer,LOW);
    apogee=1;
    durum=2;
}

if(durum==2 && irtifa<=500) {
    digitalWrite(role2,LOW);
    digitalWrite(buzzer,HIGH);
    delay(2000);
    digitalWrite(role2,HIGH);
    digitalWrite(buzzer,LOW);
    durum=3;
}

if(durum==3 && irtifa<=10 && 1<=irtifaonceki-irtifa<=1) {
    durum=4;
}

if(durum==4) {
    digitalWrite(led,LOW);
    digitalWrite(buzzer,HIGH);
    delay(1500);
    digitalWrite(led,HIGH);
    digitalWrite(buzzer,LOW);
    delay(500);
    digitalWrite(led,LOW);
    digitalWrite(buzzer,HIGH);
}

  GPSdinle();
  enlemdouble=enlem.toDouble();
  boylamdouble=boylam.toDouble();
  ivme=IMU.getAccelY_mss(),6;

  Array[0] = enlemdouble;  
  Array[1] = boylamdouble;
  Array[2] = irtifa;  
  Array[3] = ivme;
  Array[4] = irtifa;
  Array[5] = apogee;
  Array[6] = sistem;
  Array[7] = durum;
  
  //GÖNDERİLMEYECEK!!
 /* Serial.println("----SD KARTA KAYEDEİLECEK VERİLER----");
  Serial.print("Saat: ");
  Serial.println(saat);
  Serial.print("Enlem: ");
  Serial.println(enlem);
  Serial.print("Boylam: ");
  Serial.println(boylam);
  Serial.print("GPSirtifa: ");
  Serial.println(gpsirtifa);
  Serial.println();

   Serial.print(F("Sıcaklık = "));
   Serial.print(bmp.readTemperature());
   Serial.println(" *C");

   Serial.print(F("Basınç = "));
   Serial.print(bmp.readPressure());
   Serial.println(" Pa");

   Serial.print(F("BMP İrtifa = "));
   Serial.print(irtifa); 
   Serial.println(" m");

   Serial.print(F("Rakım = "));
   Serial.print(rakim);
   Serial.println(" m");

   Serial.println();
   
   Serial.print(F("Önceki İrtifa = "));
   Serial.print(irtifaonceki);
   Serial.println(" m");
   
   Serial.print(F("Sistem = "));
   if(sistem==1) {
   Serial.println("Ana Uçuş Bilgisayarı");
   } else {
    Serial.println("Yedek Uçuş Bilgisayarı");
   }

   Serial.print("Durum: ");
    if (durum==0.00) {
    Serial.println("Yerde.");
    } else if (durum==1.00) {
    Serial.println("Uçuş Devam Ediyor.");
    } else if (durum==2.00) {
    Serial.println("Tepe Noktasına Ulaşıldı, Faydalı Yük Fırlatıldı, Paraşüt Açıldı.");
    } else if (durum==3.00) {
    Serial.println("Ana Paraşüt Açıldı.");
    } else if (durum==4.00) {
    Serial.println("İniş Tamamlandı.");
    }
   
  Serial.println("--------GÖNDERİLECEK VERİLER---------");
  Serial.print("Enlem: ");
  Serial.println(enlemdouble);
  Serial.print("Boylam: ");
  Serial.println(boylamdouble);
  Serial.print("İrtifa: ");
  Serial.println(irtifa);
  Serial.print("İvme: ");
  Serial.println(ivme);*/

  VeriGonder();
  
  irtifaonceki=irtifa;
digitalWrite(buzzer,LOW);
  delay(1000);
  } else {

    if (Serial.available() && Serial.read()==111) {
      Serial.println(Serial.read());
      digitalWrite(led,HIGH);
      delay(1000);
    }
     else {
    if(baglantideneme>3000)
    {
    digitalWrite(led,LOW);
    sistemdevrede=1;
    digitalWrite(buzzer,HIGH);
    delay(150);
    digitalWrite(buzzer,LOW);
    delay(50);
    digitalWrite(buzzer,HIGH);
    delay(150);
    digitalWrite(buzzer,LOW);
    delay(50);
    digitalWrite(buzzer,HIGH);
    delay(150);
    digitalWrite(buzzer,LOW);
    } else {
baglantideneme = baglantideneme + 1;
}
}
}
}

void GPSdinle() {

  // $GPGGA arıyoruz
  if ( GPS.find("$GPGGA,") ) {

    // Gelen veriyi parçalıyoruz
    saat = GPS.readStringUntil(',');
    enlem = GPS.readStringUntil(',');
    enlem.concat(GPS.readStringUntil(','));
    boylam = GPS.readStringUntil(',');
    boylam.concat(GPS.readStringUntil(','));

    // irtifaya kadar olan kısmı atlıyoruz
    for ( int i = 0; i < 3; i++ ) {
      GPS.readStringUntil(',');
    }

    // irtifa verisini parçala
    gpsirtifa = GPS.readStringUntil(',');
    gpsirtifa.concat(GPS.readStringUntil(','));

    // Verinin geri kalanını atlıyoruz
    GPS.readStringUntil('\r');
  }
} 

void VeriGonder() {
  //Serial.println("------------GÖNDERİLEN VERİ-------------");
  
  radio.write(Array, sizeof(Array));
  
  /*Serial.print("Gönderilen Veri Boyutu(byte): ");
  Serial.println(sizeof(Array));
  Serial.println("Veri Gönderildi!");
  Serial.println("");*/
}
