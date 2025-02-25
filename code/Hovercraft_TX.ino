/* 
    호버크래프트 송신기
    by team DCA , kang sungjun
    Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
int b1 = 2;

RF24 radio(7, 8);   // nRF24L01 (CE, CSN)
const byte address[6] = "00001"; // 주소

// 데이터 저장
struct Data_Package {
  byte j1PotX; //조이스틱 X축
  byte j1PotY; //조이스틱 Y축
  byte j1Button; //조이스틱 버튼
  byte pot1; // 슬라이드 저항 
};
Data_Package data;

void setup() {


  //RF24 설정
  radio.begin();
  radio.openWritingPipe(address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening(); 
  //스위치 설정
  pinMode(b1, INPUT_PULLUP);

  // 조이스틱(가변저항) 값 0~1023 에서 중간 값인 512를 맵핑해서 0~255까지의 중간 값인 127로 설정
  data.j1PotX = 127;
  data.j1PotY = 127;
  data.j1Button = 1;
  data.pot1 = 0;

}

void loop() {
 //아날로그 인풋
  data.j1PotX = map(analogRead(A2), 0, 1023, 0, 255); 
  data.j1PotY = map(analogRead(A1), 0, 1023, 0, 255);
  data.pot1 = map(analogRead(A0), 0, 1023, 0, 255);
 //디지털 인풋
  data.j1Button = digitalRead(b1);
 //송신
  radio.write(&data, sizeof(Data_Package));

}

/* 
SCK D13
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
servo.attach(10);//우*/

