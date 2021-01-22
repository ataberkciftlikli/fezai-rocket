#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10); // CE, CSN
float Array[8];
const byte address[6] = "34102";
void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  Serial.println("Halley'in Çırakları Roket Takımı - Yer İstasyonu(Payload)");
}
void loop() {
  if (radio.available()) {
    radio.read(Array, sizeof(Array));
    Serial.println("-------- GELEN VERİ --------");
    Serial.print("Enlem: ");
    Serial.println(Array[0]);

    Serial.print("Boylam: ");
    Serial.println(Array[1]);

    Serial.print("İrtifa: ");
    Serial.print(Array[2]);
    Serial.println(" m");

    Serial.print("Sıcaklık: ");
    Serial.print(Array[3]);
    Serial.println("°C");

    Serial.print("Durum: ");
    if (Array[4]==0.00) {
    Serial.println("Yerde.");
    } else if (Array[4]==1.00) {
    Serial.println("Uçuş Devam Ediyor.");
    } else if (Array[4]==2.00) {
    Serial.println("Tepe Noktasına Ulaşıldı, Paraşüt Açıldı, İnişte...");
    } else if (Array[4]==3.00) {
    Serial.println("İniş Tamamlandı.");
    }
    Serial.println();
  }
}
