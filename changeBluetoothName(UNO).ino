// 소프트웨어 시리얼을 이용할 때는 Serial3 대신 mySerial 부분을 주석해제 후 사용
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); // RX, TX

void setup() {
  //통신 설정
  Serial.begin(9600); // 시리얼 통신(컴퓨터-USB케이블-아두이노 메가)
  mySerial.begin(9600); // 블루투스 통신(스마트폰 앱-블루투스-아두이노 우노 소프트웨어 시리얼)
  Serial.println("You Can set your Bluetooth by AT Command now");
}

void loop() {
  if (mySerial.available()) {       // 블루투스에서 데이터가 들어올 경우
    Serial.write(mySerial.read()); // 컴퓨터로 보냄
  }
  if (Serial.available()) {        // 컴퓨터에서 데이터가 들어올 경우
  mySerial.write(Serial.read()); // 블루투스로 보냄    
  }
}
