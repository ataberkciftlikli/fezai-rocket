#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(9, 10); // CE, CSN
float Array[8];
const byte address[6] = "34101";
void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  Serial.println("Halley'in Çırakları Roket Takımı - Yer İstasyonu");
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

    Serial.print("İvme: ");
    Serial.print(Array[3]);
    Serial.println(" m/sn²");

    Serial.print("Sıcaklık: ");
    Serial.print(Array[4]);
    Serial.println("°C");

    Serial.print("Tepe Noktasına Ulaştı mı?: ");
    if (Array[5]==0.00) {
    Serial.println("Hayır");
    } else if (Array[5]==1.00) {
    Serial.println("Evet");
    }

    Serial.print("Uçuş Bilgisayarı: ");
    if (Array[6]==1.00) {
    Serial.println("Ana Uçuş Bilgisayarı");
    } else if (Array[6]==0.00) {
    Serial.println("Yedek Uçuş Bilgisayarı");
    }

    Serial.print("Durum: ");
    if (Array[7]==0.00) {
    Serial.println("Yerde.");
    } else if (Array[7]==1.00) {
    Serial.println("Uçuş Devam Ediyor.");
    } else if (Array[7]==2.00) {
    Serial.println("Tepe Noktasına Ulaşıldı, Faydalı Yük Fırlatıldı, Paraşüt Açıldı.");
    } else if (Array[7]==3.00) {
    Serial.println("Ana Paraşüt Açıldı.");
    } else if (Array[7]==4.00) {
    Serial.println("İniş Tamamlandı.");
    }
    Serial.println();
  }
}
