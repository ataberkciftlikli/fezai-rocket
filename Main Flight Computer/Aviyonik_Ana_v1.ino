#include <SoftwareSerial.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "MPU9250.h"

MPU9250 IMU(Wire,0x68);
int status;


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

int rakim = 858; //RAKIM GİRİLECEK!!!

boolean apogee = 0;
boolean sistem = 1;
float durum = 0;

int buzzer = 28;
int led = 30;
int role1 = 32;
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

  IMU.readSensor();

  if(loopsayi==0) {
    //Serial.println("HALLEY'İN ÇIRAKLARI ROKET TAKIMI");
    digitalWrite(role1,HIGH);
    digitalWrite(role2,HIGH);
    digitalWrite(buzzer,HIGH);
    digitalWrite(led,HIGH);
    delay(5000);
    digitalWrite(buzzer,LOW);
  }

  irtifa = bmp.readAltitude(1013.25)-rakim;

  if(irtifa>=500 && durum==0) { //DEĞİŞECEK
    durum=1;
  }
  
if (durum==1 && irtifaonceki-irtifa>=1) {
    digitalWrite(role1,LOW);
    digitalWrite(buzzer,HIGH);
    Serial.print("o");
    delay(1200); //DİRENÇ TELİ SÜRESİ
    Serial.print("o");
    digitalWrite(role1,HIGH);
    digitalWrite(buzzer,LOW);
    apogee=1;
    durum=2;
}

if(durum==2 && irtifa<=500) { //DEĞİŞECEK
    digitalWrite(role2,LOW);
    digitalWrite(buzzer,HIGH);
    Serial.print("o");
    delay(2000); //LINEER SÜRESİ
    Serial.print("o");
    digitalWrite(role2,HIGH);
    digitalWrite(buzzer,LOW);
    Serial.print("o");
    durum=3;
}

if(durum==3 && irtifa<=10 && 1<=irtifaonceki-irtifa<=1) { //DEĞİŞECEK
    Serial.print("o");
    durum=4;
}

if(durum==4) {
    Serial.print("o");
    digitalWrite(led,LOW);
    digitalWrite(buzzer,HIGH);
    Serial.print("o");
    delay(800);
    Serial.print("o");
    digitalWrite(led,HIGH);
    digitalWrite(buzzer,LOW);
    Serial.print("o");
    delay(500);
    Serial.print("o");
    digitalWrite(led,LOW);
    digitalWrite(buzzer,HIGH);
    Serial.print("o");
}

Serial.print("o");

  GPSdinle();
  enlemdouble=enlem.toDouble();
  boylamdouble=boylam.toDouble();
  ivme=IMU.getAccelY_mss(),6;

  Array[0] = enlemdouble;  
  Array[1] = boylamdouble;
  Array[2] = irtifa;  
  Array[3] = ivme;
  Array[4] = bmp.readTemperature();
  Array[5] = apogee;
  Array[6] = sistem;
  Array[7] = durum;
  
  //GÖNDERİLMEYECEK!!
  /*if(durum!=4) {
myFile.println("---- VERİLER ----");
  myFile.print("Saat: ");
  myFile.println(saat);
  myFile.print("Enlem: ");
  myFile.println(enlem);
  myFile.print("Boylam: ");
  myFile.println(boylam);
  myFile.print("GPSirtifa: ");
  myFile.println(gpsirtifa);
  myFile.println();

   myFile.print(F("Sıcaklık = "));
   myFile.print(bmp.readTemperature());
   myFile.println(" *C");

   myFile.print(F("Basınç = "));
   myFile.print(bmp.readPressure());
   myFile.println(" Pa");

   myFile.print(F("BMP İrtifa = "));
   myFile.print(irtifa); 
   myFile.println(" m");

   myFile.print(F("Rakım = "));
   myFile.print(rakim);
   myFile.println(" m");

   myFile.println();
   
   myFile.print(F("Önceki İrtifa = "));
   myFile.print(irtifaonceki);
   myFile.println(" m");
   
   myFile.print(F("Sistem = "));
   if(sistem==1) {
   myFile.println("Ana Uçuş Bilgisayarı");
   } else {
    myFile.println("Yedek Uçuş Bilgisayarı");
   }

   myFile.print("Durum: ");
    if (durum==0.00) {
    myFile.println("Yerde.");
    } else if (durum==1.00) {
    myFile.println("Uçuş Devam Ediyor.");
    } else if (durum==2.00) {
    myFile.println("Tepe Noktasına Ulaşıldı, Faydalı Yük Fırlatıldı, Paraşüt Açıldı.");
    } else if (durum==3.00) {
    myFile.println("Ana Paraşüt Açıldı.");
    } else if (durum==4.00) {
    myFile.println("İniş Tamamlandı.");
    }
  }*/

   
  /*Serial.println("--------GÖNDERİLECEK VERİLER---------");
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
  loopsayi=loopsayi + 1;
  Serial.print("o");
  delay(1000);
  Serial.print("o");
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
