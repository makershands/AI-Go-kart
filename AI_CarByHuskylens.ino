/* 
*  - 통신 :  
아두이노 MEGA - 블루투스 모듈(HC-06 or HM-10)  
         RX31 - TXD  
         TX31 - RXD  
         
- 조향 :  
아두이노 UNO - 모터 드라이버(점퍼는 사진 확인 후 적용)  
         p10 - enA 
         p11 - PUL(펄스)  
         p12 - DIR(디렉션)  
모터 드라이버 - 스텝모터(2번, 5번 미사용)  
         A+ - 모터 결선 좌측 1번째  
         A- - 모터 결선 좌측 3번째  
         B+ - 모터 결선 좌측 4번째  
         B- - 모터 결선 좌측 6번째  
모터 드라이버의 점퍼를 조정해 전류를 제어할 수 있음  
모터는 K6G50C 1:50 기어박스가 포함되어 있음  
점퍼는 off-on-off(200pulse/rev)로 세팅합니다. 
- 페달모드 :
아두이노 - 페달
     A0 - 노랑
     p7  - 스위치-전진
     p6  - 스위치-후진
- 구동 :  
아두이노 MEGA - 모터 드라이버  
전진후진(P8) - dir입력(모터 회전 방향)  
PWM출력(p9) - PWM입력(파워)  
*/

// 허스키렌즈를 사용하기 위해서 라이브러리 설치가 필요합니다. 
// 소프트웨어시리얼 핀은 아두이노 메가의 52, 53번을 사용합니다. 
#include <SoftwareSerial.h>
#include <HUSKYLENS.h>
HUSKYLENS huskylens;
SoftwareSerial mySerial(52, 53); // RX, TX

// 스텝 모터 제어
const int enA = 10;  // 구동 여부 결정
const int stepPin = 11; // 스텝 펄스
const int dirPinLR = 12;  // 좌우 회전
const int rst = 5; // 리셋, LOW 상태로 유지함

const int STEPS_PER_REV = 400; // 모터 2회전, 점퍼는 off-on-off로 세팅함(200pulse/rev)
int rotatePos = 10;
int rotateMid = 10;
int rotateLeftLimit = 7;
int rotateRightLimit = 13;

// 드라이브 모터 제어
const int DIR = 8; // 파워
const int PWM = 9; // 신호 1 

int valocity = 40;

// 페달 제어_전진, 후진 스위치 센싱
int pedalF = 6;
boolean pedalFVal = 0;

int pedalB = 7;
boolean pedalBVal = 0;

const int ground = 4;

// 페달 제어_페달 센싱
const int pedalSensor = A0;
int pedalVal = 0;

// 수동 모드, 앱 제어모드 변경, 0은 앱제어모드, 1은 수동 모드
boolean modeState = 1;

// 속도값 초기화
int i = 0;

// 입력 문자, 입력 문자 백업
char cmd = "";
char cmdM = "s";

void setup() {
  //모니터링을 위한 시리얼 통신 설정
  Serial.begin(9600); // 시리얼 통신
  
  //mySerial은 소프트웨어 시리얼입니다. 
  mySerial.begin(9600); // 허스키렌즈와 시리얼 통신을 할 때 사용합니다. 

  //Serial3는 아두이노 메가를 사용할 때 사용하는 시리얼입니다. 아두이노 우노를 사용하면 주석처리 하세요.
  Serial3.begin(9600);           

  // 스텝모터 핀 모드 설정
  pinMode(dirPinLR,OUTPUT);
  pinMode(stepPin,OUTPUT); 
  pinMode(enA, OUTPUT);
  digitalWrite(enA, HIGH);
  pinMode(rst, OUTPUT);
  digitalWrite(rst, LOW);

  // 드라이브모터 핀 모드 설정
  pinMode(PWM,OUTPUT); 
  pinMode(DIR, OUTPUT);
         
  // 페달모드 전진 후진 신호
  pinMode(pedalF, INPUT_PULLUP);
  pinMode(pedalB, INPUT_PULLUP);
  pinMode(ground, OUTPUT);
  digitalWrite(ground, LOW);

  while (!huskylens.begin(mySerial)){
    Serial.println(F("Begin failed!"));
    Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>Serial 9600)"));
    Serial.println(F("2.Please recheck the connection."));
    delay(100);
  }
       
  Serial.println("Huskylens Go-Kart is Ready!");
  delay(1000);
}

void loop() {
  //  modestate가 1이면 페달제어 모드로 수행
  if(modeState == 1) {       
    // 전진, 후진 스위치 값 저장
    pedalFVal = digitalRead(pedalF);
    pedalBVal = digitalRead(pedalB);

    // 페달값 센싱-매핑-한계범위설정
    pedalVal = analogRead(pedalSensor);
    pedalVal = map(pedalVal, 230, 850, 0, 255);
    pedalVal = constrain(pedalVal, 0, 255);

    // 페달 값 변화 시리얼 모니터 링
    Serial.print(pedalFVal);
    Serial.print("  ");
    Serial.print(pedalBVal);
    Serial.print("  ");
    Serial.print(pedalVal);
    Serial.print("  ");

    // 페달 신호가 0이면 브레이킹
    if (pedalVal == 0) {
      digitalWrite(DIR,LOW);   
      analogWrite(PWM, 0);
      delay(100);
    }

    // 전진, 후진 스위치 값에 따른 페달 동작
    if (pedalFVal == 1 && pedalBVal == 0) {
      digitalWrite(DIR,LOW); 
      analogWrite(PWM, pedalVal);
      Serial.println("RRRR");
    } else if (pedalFVal == 1 && pedalBVal == 1) {
      digitalWrite(DIR,HIGH);
      analogWrite(PWM, pedalVal);
      Serial.println("FFFF");
    } else {
      digitalWrite(DIR,LOW);  
      analogWrite(PWM, 0);
      Serial.println("SSSS");
    }
  }

  // 아두이노 메가를 쓸 때는 Serial3를 그대로 사용하고, 아두이노 우노를 쓸 때는 Serial3를 mySerial로 수정해주세요. 
  // 아두이노 메가를 쓸 때는 SW6 스위치를 3번쪽으로 옮기고, 아두이노 우노를 쓸 때는 SW6 스위치를 1번쪽으로 옮겨주세요.
  if (Serial3.available() ){        // 블루투스 통신에 데이터가 있을 경우
    cmd = Serial3.read();     // 블루투스의 데이터(문자 한 글자)를 'cmd' 변수에 저장
  
    // cmd 변수의 데이터가 m이면 수동모드로, i면 앱모드로 modeState 변수의 상태를 바꿈
    if (cmd == 'm') {
      modeState = 1;
      Serial3.println("input 'm'");
      Serial3.println("the mode is : manual control");
    }
    
    if (cmd == 'i') {
      modeState = 0;
      Serial3.println("input 'i'");
      Serial3.println("the mode is : huskylens control");
    } 
  }   
    
  // modestate가 0이면 허스키렌즈제어 모드로 수행
  if (modeState == 0) {
      // 아래는 허스키 렌즈로 제어할 때 사용
    if (!huskylens.request()) Serial3.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
    else if(!huskylens.isLearned()) Serial3.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
    else if(!huskylens.available()) Serial3.println(F("No block or arrow appears on the screen!"));
    else
    {
      while (huskylens.available())
      {
        HUSKYLENSResult result = huskylens.read();
        printResult(result);
        
        if (cmd == 'w' ){               // 만약 w가 입력되면 이전 입력값'cmdM'을 확인하고, cmdM이 w이면 전진유지, 아니면 천천히 가속하여 전진
          Serial.println(cmd);
          if(cmdM == 'w'){
            forward();
          } else {
            i = 0;
            forward();
          }
          cmdM = 'w';
        } else if ( cmd == 'x') {        // 만약 x가 입력되면 이전 입력값'cmdM'을 확인하고, cmdM이 x이면 후진유지, 아니면 천천히 가속하여 후진
          Serial.println(cmd);
          if(cmdM == 'x') {
            backward();
          } else {
            i = 0;
            backward();
          }
          cmdM = 'x';
        } else if ( cmd == 'a' ) {       // 아니고 만약 'cmd' 변수의 데이터가 a면 좌회전
          right();
        } else if ( cmd == 'd' ) {       // 아니고 만약 'cmd' 변수의 데이터가 d면 우회전
          left();
        } else if ( cmd == 's' ) {       // 아니고 만약 'cmd' 변수의 데이터가 s면 정지
          motorStop();
        } else if ( cmd == 'o' ) {       // 아니고 만약 'cmd' 변수의 데이터가 o면 속도 줄이기
          valocity -= 30;
          if (valocity < 30) {
            valocity = 30;
          }
          Serial.print("Speed Down! Current Speed is ");
          Serial.println(valocity);
        } else if ( cmd == 'p' ) {       // 아니고 만약 'cmd' 변수의 데이터가 p면 속도 높이기
          valocity += 30;
          if (valocity > 250) {
            valocity = 250;
          }
          Serial.print("Speed Up! Current Speed is ");
          Serial.println(valocity);
        }
      }    
    }
  }
}



void left() {
  // 조향 모터가 '반시계방향'으로 회전하도록 신호부여
  digitalWrite(dirPinLR,LOW); 
  
  if (rotatePos > rotateLeftLimit) {
    // 1000마이크로초 주기로 모터 축이 1.5회전하는 코드
    // 1:50 기어박스 내장되어 있으므로, 모터 1회전에 바퀴 7.2도 회전함
    // 따라서, 모터가 1.5회전하면 바퀴가 10.8도 회전함
    for(int x = 0; x < STEPS_PER_REV*0.3; x++) {
      digitalWrite(enA,HIGH);
      digitalWrite(stepPin,HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin,LOW);
      delayMicroseconds(500); 
    }
    rotatePos = rotatePos - 1;
  } else {
    rotatePos = rotateLeftLimit;
  }
  Serial.println(rotatePos);
}

void right() {
  // 조향 모터가 '시계방향'으로 회전하도록 신호부여
  digitalWrite(dirPinLR,HIGH); 
  
  if (rotatePos < rotateRightLimit) {
    for(int x = 0; x < STEPS_PER_REV*0.3; x++) {
      digitalWrite(enA,HIGH);
      digitalWrite(stepPin,HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin,LOW);
      delayMicroseconds(500); 
    }
    rotatePos = rotatePos + 1;
  } else {
    rotatePos = rotateRightLimit;
  }
  Serial.println(rotatePos);
}

void forward() {
  //드라이브 모터가 앞으로 회전하도록 신호부여
  digitalWrite(DIR,LOW); 
  analogWrite(PWM, i);

  if (i != valocity) {
    for (i = 0; i < valocity; i = i + 10) {
      analogWrite(PWM, i);
      delay(100);
    }
  }

  if (rotateMid > rotatePos) {
    int j = rotateMid - rotatePos;
    for (int k = 0; k < j; k++) {
      right();
    }
  } else if (rotateMid == rotatePos) {
    
  } else if (rotateMid < rotatePos) {
    int j = rotatePos - rotateMid;
    for (int k = 0; k < j; k++) {
      left();
    }
  }
  Serial.println("forward");
}

void motorStop() {
  digitalWrite(DIR,LOW);
  
  analogWrite(PWM, 0);
  delay(100);
  Serial.println("motorStop");

  cmdM = 's';
}

void backward() {
  ////드라이브 모터가 뒤로 회전하도록 신호부여
  digitalWrite(DIR,HIGH); 
  analogWrite(PWM, i);
  
  if(i != valocity) {
    for (i = 0; i < valocity; i = i + 10) {
      analogWrite(PWM, i);
      delay(100);
    }
  }
  Serial.println("backward");
}

void printResult(HUSKYLENSResult result){
    if (result.command == COMMAND_RETURN_BLOCK){
      Serial.println(String()+F("Block:xCenter=")+result.xCenter+F(",yCenter=")+result.yCenter+F(",width=")+result.width+F(",height=")+result.height+F(",ID=")+result.ID);
      
      if(result.xCenter > 180) {
        if(cmdM == 'x'){
          cmd = 'a';
          Serial.println(String() + cmd + F("  Backward right"));   
        } else {
          cmd = 'd';
          Serial.println(String() + cmd + F("  right"));   
        }  
      }
      if(result.xCenter < 140) {
        if(cmdM == 'x'){
          cmd = 'd';
          Serial.println(String() + cmd + F("  left"));   
        } else {
          cmd = 'a';
          Serial.println(String() + cmd + F("  Backward left"));   
        }
      }
       if(result.xCenter <= 180 && result.xCenter >=140) {
        cmd = 's';
        Serial.println(String() + cmd + F("  left"));   
      }
      if(result.width < 60 && result.height < 60) {
        cmd = 'w';
        cmdM = 'w';
        Serial.println(String() + cmd + F("  forward"));      
      } 
      if(result.width > 90 && result.height > 90) {
        cmd = 'x';
        cmdM = 'x';
        Serial.println(String() + cmd + F("  backward"));  
      }
    }
    else if (result.command == COMMAND_RETURN_ARROW){
      Serial.println(String()+F("Arrow:xOrigin=")+result.xOrigin+F(",yOrigin=")+result.yOrigin+F(",xTarget=")+result.xTarget+F(",yTarget=")+result.yTarget+F(",ID=")+result.ID);
      cmd = 'w';
      
      if(result.xOrigin < result.xTarget && result.xOrigin - result.xTarget < -20) {
        cmd = 'd';
        Serial.println(String() + cmd + F("  right"));  
        if(result.xOrigin < 100) {
          cmd = 'w';
          Serial.println(String() + cmd + F("  forward"));   
        } 
      }
      else if(result.xOrigin > result.xTarget && result.xOrigin - result.xTarget > 20) {
        cmd = 'a';
        Serial.println(String() + cmd + F("  left"));   
        if(result.xOrigin > 220) {
          cmd = 'w';
          Serial.println(String() + cmd + F("  forward"));   
        } 
      }
      else if(result.xOrigin < 120 && result.xTarget < 120) {
        cmd = 'a';
        Serial.println(String() + cmd + F("  left"));
      }
      else if(result.xOrigin > 200 && result.xTarget > 200) {
        cmd = 'd';
        Serial.println(String() + cmd + F("  right"));
      }
      else {
        cmd = 'w';
        Serial.println(String() + cmd + F("  forward"));   
      } 
    }
    else{
      Serial.println("Object unknown!");
    }
}
