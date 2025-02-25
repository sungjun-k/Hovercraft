/* 
    호버크래프트 본체 수신기
    by team DCA , kang sungjun
    Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <Servo.h>
#include <RF24.h>

RF24 radio(7, 8);   // nRF24L01 (CE, CSN)
const byte address[6] = "00001";
unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

Servo esc1;  
Servo esc2;
Servo esc3;
Servo servo1; 
Servo servo2;
int led = 2 ;
int buzzer = 4 ;
int esc1Value, esc2Value, esc3Value, servo1Value , servo2Value;
float vout = 0.0; 
float vin = 0.0;
float R1 = 30000.0; //30k  
float R2 = 7500.0;  //7.5k
int value = 0;

// 데이터 저장
struct Data_Package {
  byte j1PotX; //조이스틱 X축
  byte j1PotY; //조이스틱 Y축
  byte j1Button; //조이스틱 버튼
  byte pot1; // 슬라이드 저항 
  byte servo1;// 좌측 서보모터
  byte servo2;// 우측 서보모터

};
Data_Package data;

void setup() {
  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening(); //  Set the module as receiver
  resetData();
  esc1.attach(6); //하단
  esc2.attach(5); //좌측
  esc3.attach(3); //우측
  servo1.attach(9);//좌측 서보
  servo2.attach(10);//우측 서보
  pinMode(buzzer , OUTPUT);
  pinMode(led , OUTPUT); 
  }

void loop() {
  // 송수신기 사이의 연결 확인
  currentTime = millis();
  if ( currentTime - lastReceiveTime > 1000 ) { // 마지막 데이터를 수신한 후 현재 시간이 1초 이상인 경우 연결이 끊어진 것
    resetData(); //연결이 끊어지면 재설정
  }
  // 수신할 데이터가 있는지 확인
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package)); // 전체 데이터를 읽어 'data' 구조에 저장
    lastReceiveTime = millis(); // 데이터를 받았다.
  }

  // ESC로 모터 제어
  // 하단 팬
  esc1Value = map(data.pot1, 0, 255, 1000, 2000); // Map 
  esc1.writeMicroseconds(esc1Value); // PWM 신호를 ESC로 보낸다.
  
  // 좌측 모터
  esc2Value = constrain(data.j1PotY, 130, 255); 
  esc2Value = map(esc2Value, 130, 255, 1000, 2000);
  esc2.writeMicroseconds(esc2Value);
  // 우측 모터
  esc3Value = constrain(data.j1PotY, 130, 255);
  esc3Value = map(esc3Value, 130, 255, 1000, 2000);
  esc3.writeMicroseconds(esc3Value);
  // 좌측 서보모터
  servo1Value = map(data.j1PotX, 0, 255, 0, 50);
  servo1.write(servo1Value);
  // 우측 서보모터
  servo2Value = map(data.j1PotX, 0, 255, 0, 50);
  servo1.write(servo2Value);

  // 배터리 전압 측정
  value = analogRead(A0);
  vout =  value * (4.93/ 1023.00);//(value * 4.0) / 1024.0;
  vin = vout / ( R2 / ( R1 + R2) );

  //Serial.println(vin);
 
  // 11v가 되면 led 켜기
  if (vin < 11) {
    digitalWrite(led, HIGH);
    tone(buzzer , 850);
  }
  else {
    digitalWrite(led, LOW);
    noTone(buzzer);
  }
}
void resetData() {
  // 통제불능 일때 기본 값으로 재설정
  data.j1PotX = 127;
  data.j1PotY = 127;
  data.j1Button = 1;
  
}

/* SCK D13
MISO D12
MOSI D11
CE D7
CSN D8

led = 2 
buzzer = 4

esc1.attach(3); //밑
esc2.attach(5); //좌 
esc3.attach(6); //우
servo.attach(9);//좌
servo.attach(10);//우 */ 