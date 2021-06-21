void setup() {
  //통신 설정
  Serial.begin(9600); // 시리얼 통신(컴퓨터-USB케이블-아두이노 메가)
  Serial3.begin(9600); // 블루투스 통신(스마트폰 앱-블루투스-아두이노 메가)
  Serial.println("You Can set your Bluetooth by AT Command now");
}

void loop() {
  if (Serial3.available())        // 블루투스에서 데이터가 들어올 경우
    Serial.write(Serial3.read()); // 컴퓨터로 보냄
  if (Serial.available())         // 컴퓨터에서 데이터가 들어올 경우
    Serial3.write(Serial.read()); // 블루투스로 보냄
}
