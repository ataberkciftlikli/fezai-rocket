#include <SoftwareSerial.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP085.h>;
#include <nRF24L01.h>
#include <RF24.h>
#include "DHT.h"

Adafruit_BMP085 bmp; // I2C

SoftwareSerial GPS(10, 4); // Rx, Tx

RF24 radio(7, 8); // CE, CSN
const byte address[6] = "34102";

#define DHTPIN 9     // what pin we're connected to
#define DHTTYPE DHT11   // DHT 11 
DHT dht(DHTPIN, DHTTYPE, 6);

String saat = "";    // hhmmss.00
String enlem = "";   // DDMM.MMMM Kuzey/Güney N/S
String boylam = "";  // DDMM.MMMM Doğu/Batı E/W
String gpsirtifa = "";  // metre

float ivme;
float irtifa;
double enlemdouble;
double boylamdouble;
float irtifaonceki;

int sensorValue;
int rakim = 0; //RAKIM GİRİLECEK!!!

float durum = 0;

int buzzer = 6;
int led = 5;

int loopsayi = 0;

float Array[8];

void setup() {
  Serial.begin(9600);
  bmp.begin();
  dht.begin();
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  GPS.begin(9600);



pinMode(buzzer,OUTPUT);
pinMode(led,OUTPUT);

}

void loop() {

  if(loopsayi==0) {
    //Serial.println("HALLEY'İN ÇIRAKLARI ROKET TAKIMI");
    digitalWrite(buzzer,HIGH);
    digitalWrite(led,HIGH);
    delay(5000);
    digitalWrite(buzzer,LOW);
  }

  irtifa = bmp.readAltitude(bmp.readSealevelPressure())-rakim;

  if(irtifa>=10 && durum==0) {
    durum=1;
  }
  
if (durum==1 && irtifaonceki-irtifa>=1) {
    durum=2;
}

if(durum==2 && irtifa<=6 && 1<=irtifaonceki-irtifa<=1) {
    durum=2;
}

if(durum==3) {
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

  Array[0] = enlemdouble;  
  Array[1] = boylamdouble;
  Array[2] = irtifa;  
  Array[3] = bmp.readTemperature();
  Array[4] = durum;
  
  //GÖNDERİLMEYECEK!!
  Serial.println("----SD KARTA KAYEDEİLECEK VERİLER----");
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

   sensorValue = analogRead(0);
   Serial.print("Hava Kalitesi = ");
   Serial.print(sensorValue, DEC);
   Serial.println(" PPM");

   Serial.print("DHT Sıcaklık = ");
   Serial.print(dht.readTemperature());
   Serial.println(" *C");

   Serial.print("Nem = ");
   Serial.print(dht.readHumidity());
   Serial.println(" %");

  VeriGonder();
  
  irtifaonceki=irtifa;
  loopsayi=loopsayi + 1;
  delay(1000);
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
