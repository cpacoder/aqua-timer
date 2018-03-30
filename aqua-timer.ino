#include <Wire.h>
#include "RTClib.h"
#include <EEPROM.h>

// #define     RelayPin1   6 // air        
// #define     RelayPin2   7 // carbon       
// #define     RelayPin3   8 // light

#define     RelayPin1   49 // air        
#define     RelayPin2   51 // carbon       
#define     RelayPin3   53 // light

int ifInRange(int start[], int stop[], int now[]) {
    if (start[0] <= stop[0]) {
        if (now[0] >= start[0] && now[0] <= stop[0]) {
            if (now[0] == start[0] && now[1] < start[1] || now[0] == stop[0] && now[1] >= stop[1]) {
                return 0;
            } else {
                return 1;
            }
        } else {
            return 0;
        }
    } else {
        if (now[0] >= start[0] && now[0] <= 23 || now[0] <= stop[0] && now[0] >= 0) {
            if (now[0] == start[0] && now[1] < start[1] || now[0] == stop[0] && now[1] >= stop[1]) {
                return 0;
            } else {
                return 1;
            }
        } else {
            return 0;
        }
    }
}

RTC_DS1307 rtc;

int relay1_on[] = {23, 0};
int relay1_off[] = {14, 0};

int relay2_on[] = {14, 0};
int relay2_off[] = {23, 0};

int relay3_on[] = {15, 0};
int relay3_off[] = {23, 0};

int now_time[2];

int address = 0;
int dst;
int year;
int mon;
int day;
int dow;
int h;
int m;
int s;

void setup () {
  while (!Serial); // for Leonardo/Micro/Zero

  Wire.begin();

  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  // EEPROM.write(address, 0);
  // rtc.adjust(DateTime(2016, 11, 6, 22, 59, 45));
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  dst = EEPROM.read(address);
  pinMode(RelayPin1, OUTPUT);
  pinMode(RelayPin2, OUTPUT);
  pinMode(RelayPin3, OUTPUT);
  digitalWrite(RelayPin1, LOW);
  digitalWrite(RelayPin2, LOW);
  digitalWrite(RelayPin3, LOW);
}

void loop () {
    DateTime now = rtc.now();
    
    year = now.year();
    mon = now.month();
    day = now.day();
    dow = now.dayOfTheWeek();
    h = now.hour();
    m = now.minute();
    s = now.second();
    now_time[0] = now.hour();
    now_time[1] = now.minute();
    
    if (dow == 0 && mon == 3 && day >= 25 && day <=31 && h == 3 && dst == 0){
      // set time
      rtc.adjust(DateTime(year, mon, day, h + 1, m, s));
      dst = 1;
      EEPROM.write(address, dst);
    };
    
    if (dow == 0 && mon == 10 && day >= 25 && day <=31 && h == 4 && dst == 1){
      // set time
      rtc.adjust(DateTime(year, mon, day, h - 1, m, s));
      dst = 0;
      EEPROM.write(address, dst);
    };

    if (ifInRange(relay1_on, relay1_off, now_time) == 1) {
        digitalWrite(RelayPin1, HIGH);        
    } else {
        digitalWrite(RelayPin1, LOW);
    }

    if (ifInRange(relay2_on, relay2_off, now_time) == 1) {
        digitalWrite(RelayPin2, HIGH);        
    } else {
        digitalWrite(RelayPin2, LOW);
    }

    if (ifInRange(relay3_on, relay3_off, now_time) == 1) {
        digitalWrite(RelayPin3, HIGH);        
    } else {
        digitalWrite(RelayPin3, LOW);
    }
        
    Serial.print("DST = ");
    Serial.print(dst);
    Serial.println();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(now.dayOfTheWeek());
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    Serial.println();
    
    delay(1000);
}
