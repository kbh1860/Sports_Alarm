#include <core_build_options.h>
#include <swRTC.h>
#include <LiquidCrystal_I2C.h> //LCD용 라이브러리
#include <SoftwareSerial.h> 
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27,20,4); //I2C 주소 0x27, 20열 4행
SoftwareSerial bt(3,2); //rxPin = 블루투스 tx bt : bluetooth
swRTC rtc;
int piezo = 5; //부저 디지털 핀
int piezo_on_off = 1; //부저 끌 데이터 1일때 꺼짐 0일때 켜짐

String lcdString = ""; //스크린 초기화 값
char c_time; // 데이터 받을때 1일때, 2일때 값 받기

int Current_time = 24;//현재 시간 초기값
//1~24

int Current_minute = 59;//현재 분 초기값
//0~59

int Current_seconed = 50;//현재 초 초기값
//0~59

//String CIf_AM_PM; //1이면 오전 2이면 오후
String Current_AM_PM = "AM";//현재 오전/오후 값
//만약 시간 값이 1~12이면 오전
//만약 시간 값이 13~24이면 오후

int Current_Month=0;//현재 달
int Current_Day=0;//현재 년도

int Alarm_time=10; //알람 시간 초기값
int Alarm_minute=38; //알람 분 초기값
//String AIf_AM_PM;
String Alarm_AM_PM = "PM"; //알람 오전/오후 값

void Set_lowThanTen(int time) { //시간이 만약 1의 자리일 경우 앞에 0을 넣어주는 함수
  if(time < 10){
    lcd.print("0");
    lcd.print(time%10);  
  }
  else {
    lcd.print(time);
  }
}

void Set_AMPM(int hour, String AM_PM) { //오전인지 오후인지 설정하는 함수  
  if(hour == 24) {//만약 24시 일 경우 오전 12시 AM 12
    lcd.print("PM ");
    lcd.print(12);
    AM_PM = "PM";
  }
  else if(hour > 12) {//만약 24가 아닌 12보다 높은 값 오후 12시~오후 11시 까지
    lcd.print("PM "); 
    if(hour%12 < 10) {//만약 hour가 1~9일 경우 앞에 0이 없음
      lcd.print("0");
      lcd.print(hour%12, DEC);
    }
    else if(hour%12 >= 10){//만약 hour가 10~11일 경우 그냥 출력
      lcd.print(hour%12, DEC); 
    }
    AM_PM = "PM";
  }
  else if(hour <= 12){
    lcd.print("AM ");
    if(hour < 10) {
      lcd.print("0");
      lcd.print(hour, DEC);
    }
    else {
      lcd.print(hour, DEC); 
    }
    AM_PM = "AM";
  }
}
//오전 12시 = 24시간 시간 표기법으로 12:00
//오후 12시 = 24시간 시간 표기법으로 24:00

void setup() {
  pinMode(7, INPUT);
  lcd.init(); //lcd 초기화
  lcd.backlight(); //백라이트 열기
  lcd.begin(20,4); //LCD 크기 지정 20칸 4줄 
  lcd.clear();//화면 초기화
  rtc.stopRTC();//시간 or 날짜 변경하기 전에 중지
  
  //rtc.setTime(Current_time,Current_minute,Current_seconed);//초기 시간 설정 시 분 초
  //rtc.setDate(15,2,2020);
  rtc.startRTC();//변경 완료후 다시 시각
  pinMode(piezo, OUTPUT);
  Serial.begin(9600); //시리얼 초기화
  Serial.begin(9600);
  bt.begin(9600); //소프트웨어 시리얼 통신
}

void loop() {
  int readValue = digitalRead(7); //버튼 값 일기
  if(readValue == HIGH) {//켜져 있다면
    piezo_on_off = 0;//부저 끄기
    //digitalWrite(piezo,LOW);
  }
  if(piezo_on_off == 0) {//만약 0일때
     noTone(5);
  }
  //int day;
  c_time = bt.read(); //c_time에 블루투스 통신으로 인한 데이터 읽어오기
//  if(bt.available()) {
//    Serial.write(bt.read());  //시리얼 모니터에 수신한 데이터를 출력함  
//  }
  if(c_time == '1') {//만약 c_time이 1일때 현재시간과 관련된 값들을 받아옴
      Current_time = bt.parseInt();
      Current_minute = bt.parseInt();
      Current_seconed = bt.parseInt();  
      Current_Month = bt.parseInt();
      Current_Day = bt.parseInt();
  }
  else if(c_time == '2') {//만약 c_time이 2일때 알람시간과 관련된 값들을 받아옴
      Alarm_time = bt.parseInt();
      Alarm_minute = bt.parseInt();
  }
  lcd.setCursor(0,0);//원하는 위치로 이동
  Set_AMPM(Current_time,Current_AM_PM); //현재시간 오전 오후값 확인
  lcd.print(":"); //lcd화면에 나타내기(ASCII 문자만 지원함)
  //다른 특수문자나 기호를 표시하려면 문자 생성기 사용해야함 1픽셀당 => 8행 5열로 구성되어 있음
  Set_lowThanTen(Current_minute); //현재 분을 얻는 함수 rtc.getMinutes()
  lcd.print(":");
  Set_lowThanTen(Current_seconed); // 현재 초를 얻는 함수 rtc.getSeconds()
  lcd.print("[");
  Set_lowThanTen(Current_Month); // 현재 달을 얻는 함수 rtc.getMonth()
  lcd.print("/");
  Set_lowThanTen(Current_Day); // 현재 일을 얻는 함수 rtc.getDay()
  lcd.print("]");
  //시계 기능=========================================================
  Current_seconed++;
  if(Current_seconed>59) {
    Current_seconed=0;
    Current_minute++;
    if(Current_minute>59) {
      Current_minute=0;
      Current_time++;
      if(Current_time == 25) {
        Current_time = 1;
        Current_Day++;
        Current_AM_PM = "AM";
      }
      else if(Current_time>12) {
        Current_AM_PM = "PM";
      }
    }
  }
  //=========================================================





//세팅된 알람시간을 LCD에 출력
  lcd.setCursor(0,1);
  lcd.print("Alarm ");
  Set_AMPM(Alarm_time,Alarm_AM_PM);//알람 시
  
  lcd.print(":");
  Set_lowThanTen(Alarm_minute);//알람 초 temp%100

//1초마다 LCD갱신
  String lcdString = ""; //문자열 초기화
  lcd.print(" "); //전 글씨 삭제
  delay(1000); //1초 마다

//알람이 울릴 시간인지 체크
    if(Alarm_time == Current_time && Alarm_minute == Current_minute) {
        piezo_on_off = 1;
        analogWrite(piezo,128);  
    }
}

//2월 28일
//1 3 5 7 8 10 12 31일
//4 6 9 11 30일

//알람이 끄기 1분전까지는 버튼을 눌러도 계속 부저의 소리가 나옴
// 그 이후 버튼을 눌러야 소리가 꺼짐
