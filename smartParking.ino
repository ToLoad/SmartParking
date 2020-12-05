#include <SoftwareSerial.h> // 블루투스
#include <ThreeWire.h> // RTC
#include <RtcDS1302.h> // RTC

SoftwareSerial btSerial(5, 6);
ThreeWire myWire(2, 3, 1);
RtcDS1302<ThreeWire> Rtc(myWire);
char datestring[20];

int buzzer = 8;

int red = 9;
int green = 10;
int blue = 11;

int echo = 12;
int trig = 13;

int Min = 0;
int inparkMin = 0;
int deparkMin = 0;
int totalMin = 0;
int price = 0;

int count = 1;

void setup() {
  Serial.begin(9600);
  btSerial.begin(9600);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  digitalWrite(green, HIGH); // 기본셋팅 초록불

  pinMode(buzzer, OUTPUT);

  Rtc.Begin();
}

void loop() {
  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);
  char val = btSerial.read();

  if(val == '3'){ //호출신호가 오면 호출음
    callTone();
    Serial.println("호출");
  }
   
  digitalWrite(trig, LOW);
  digitalWrite(echo, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
    
  unsigned long duration = pulseIn(echo, HIGH); 
  float distance = ((float)(340 * duration) / 10000) / 2;

  //Serial.println(distance);

  if (count == 2) {
    if (distance < 7) {
      digitalWrite(red, HIGH); // 차가 들어오면 빨간불
      digitalWrite(green, LOW);
      inparkMin = Min; // 주차시간 등록
      inparkTone();
      count--;

      Serial.println("주차");
      btSerial.write("1");
    } 
  }

  if (count == 1) {
    if (distance > 7) {
      digitalWrite(red, LOW); // 차가 나가면 초록불
      digitalWrite(green, HIGH);
            
      deparkMin = Min; // 출차시간 등록
      totalMin = deparkMin - inparkMin; // 총 주차시간 계산
      Serial.print(totalMin); // 총 주차시간 표시
      Serial.println("Min Parking");
      deparkTone();
      count++;

      Serial.println("출차"); 
      btSerial.write("2");
      //delay(300);
      //btSerial.print(totalMin);
    }
  }
  delay(1000);
}


// RTC 모듈 관련 함수
#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt) // 현재 날짜 및 시간
{
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%04u/%02u/%02u %02u:%02u:%02u"),
            dt.Year(),
            dt.Month(),
            dt.Day(),          
            dt.Hour(),
            dt.Minute(),
            dt.Second() );

            Min = dt.Minute();
}

void inparkTone() {
  tone(buzzer, 262, 500); // 주차시 부저음
  delay(500);
  tone(buzzer, 330, 500);
}

void deparkTone() {
  tone(buzzer, 330, 500); // 출차시 부저음
  delay(500);
  tone(buzzer, 262, 500);
}

void callTone() {
  tone(buzzer, 987, 500);
  delay(500);
  tone(buzzer, 830, 500);
}
