

//-----------------------------------------------------------------------

#define  DEBUG




#include <avr/pgmspace.h>
#include "VirtualRTC.h"         // a virtual Unix Time RTC to simulate a DS32331 or else



// Set the Virtual RTC
//VirtualRTC vRTC;                // SummerTime disable
//VirtualRTC vRTC(EU_SUMMERTIME); // NO_SUMMERTIME:disable, EU_SUMMERTIME~US_SUMMERTIME:Summertime with EU~US rules
//VirtualRTC vRTC(US_SUMMERTIME); // NO_SUMMERTIME:disable, EU_SUMMERTIME~US_SUMMERTIME:Summertime with EU~US rules
//VirtualRTC vRTC(NO_SUMMERTIME); // NO_SUMMERTIME:disable, EU_SUMMERTIME~US_SUMMERTIME:Summertime with EU~US rules



//-----------------------------------------------------------------------



//--- RTC management
byte   SummerTime[]        = {NO_SUMMERTIME, EU_SUMMERTIME, US_SUMMERTIME};


//                           {Year,Month,Day,DayOfWeek, h, min, s}
struct  DateTime  Tnow     = {2000,  3, 26,     0,      1, 59,  10};    // just before summertime



//-----------------------------------------------------------------------



void setup()
{
#ifdef DEBUG
  Serial.begin(9600);
#endif
  Tnow     = {2019,  1,   1,     0,      1, 34,  0};
  showDates();
}

void loop()
{
  byte idx;
  
  setSummertime(SummerTime[idx]);
  testSummer(idx);
  
  while (SummerTime == 2); // stop
}

void testSummer(byte idx)
{
  if (idx == 0 || idx == 1)
    Serial.println("========================");
    Serial.println(" ");
    Serial.println("in range");
    Tnow     = {2020,  2,  1,     0,      1, 34,  0};  // wintertime
    showSummmer();
    Tnow     = {2022,  6,  3,     0,      1, 34,  0};  // summertime
    showSummmer();
    Tnow     = {2022,  7, 28,     0,      2, 34,  0};  // summertime
    showSummmer();
    Tnow     = {2022,  7, 28,     0,      0, 34,  0};  // summertime
    showSummmer();
    Tnow     = {2022,  7, 28,     0,     23, 34,  0};  // summertime
    showSummmer();
    Tnow     = {2022,  7, 28,     0,     24, 34,  0};  // summertime
    showSummmer();
  }
  
  if      (idx == 1)
  {
    Serial.println("========================");
    Serial.println(" ");
    Serial.println("Dday march EU");
    Tnow     = {2019,  3, 31,     0,      1, 59,  10};  // summertime march Dday
    showSummmer();
    Tnow     = {2019,  3, 31,     0,      2,  0,  10};  // summertime march
    showSummmer();
    Tnow     = {2019,  3, 31,     0,      6, 59,  10};  // summertime march
    showSummmer();
    Serial.println("========================");
    Serial.println(" ");
    Serial.println("Dday october EU");
    Tnow     = {2019, 10, 27,     0,      2, 59,  10};  // wintertime october Dday
    showSummmer();
    Tnow     = {2019, 10, 27,     0,      3,  0,  10};  // wintertime october
    showSummmer();
    Tnow     = {2019, 10, 28,     0,      1, 59,  10};  // wintertime october
    showSummmer();
  }
  else if (idx == 2)
  {
    Serial.println("========================");
    Serial.println(" ");
    Serial.println("Dday march US");
    Tnow     = {2019,  3, 10,     0,      1, 59,  10};  // summertime march Dday
    showSummmer();
    Tnow     = {2019,  3, 10,     0,      2,  0,  10};  // summertime march
    showSummmer();
    Tnow     = {2019,  3, 16,     0,      1, 59,  10};  // summertime march
    showSummmer();
    Serial.println("========================");
    Serial.println(" ");
    Serial.println("Dday october US");
    Tnow     = {2019, 10,  6,     0,      1, 59,  10};  // wintertime october Dday
    showSummmer();
    Tnow     = {2019, 10,  6,     0,      2,  0,  10};  // wintertime october
    showSummmer();
    Tnow     = {2019, 10, 16,     0,      1, 59,  10};  // wintertime october
    showSummmer();
    Serial.println("========================");
  }
}

void showSummmer()
{
  vRTC.setTime(& Tnow);          // set time for Virtual RTC, Unix Time ref, 1970-01-01 1h:00m00s, Thursday
  Serial.println("========================");
  Serial.println("===org time");
  Serial.print(Tnow.d); Serial.print(" - "); Serial.print(Tnow.m); Serial.print(" - "); Serial.print(Tnow.y);
  Serial.print("  ///  ");
  Serial.print(Tnow.hh); Serial.print(" : "); Serial.print(Tnow.mm); Serial.print(" : "); Serial.print(Tnow.ss);
  Serial.print("  ///  ");
  Serial.print(vRTC.getSummerTime(& Tnow, false));
  Serial.println(" ");
  
  //*
  vRTC.getTime(& Tnow);          // get time from Virtual RTC
  Serial.println("===RTC time");
  Serial.print(Tnow.d); Serial.print(" - "); Serial.print(Tnow.m); Serial.print(" - "); Serial.print(Tnow.y);
  Serial.print("  ///  ");
  Serial.print(Tnow.hh); Serial.print(" : "); Serial.print(Tnow.mm); Serial.print(" : "); Serial.print(Tnow.ss);
  Serial.print("  ///  ");
  Serial.print(vRTC.getSummerTime(& Tnow, false));
  Serial.println(" ");
  //*/
  
  vRTC.UnixTime += 60;
  
  vRTC.getTime(& Tnow);          // get time from Virtual RTC
  Serial.println("===time + 1min");
  Serial.print(Tnow.d); Serial.print(" - "); Serial.print(Tnow.m); Serial.print(" - "); Serial.print(Tnow.y);
  Serial.print("  ///  ");
  Serial.print(Tnow.hh); Serial.print(" : "); Serial.print(Tnow.mm); Serial.print(" : "); Serial.print(Tnow.ss);
  Serial.print("  ///  ");
  Serial.print(vRTC.getSummerTime(& Tnow, false));
  Serial.println(" ");
}

void showDates()
{
  vRTC.setTime(& Tnow);          // set time for Virtual RTC, Unix Time ref, 1970-01-01 1h:00m00s, Thursday
  Serial.println("========================");
  Serial.print(Tnow.d); Serial.print(" - "); Serial.print(Tnow.m); Serial.print(" - "); Serial.print(Tnow.y);
  Serial.print("  ///  ");
  Serial.print(Tnow.hh); Serial.print(" : "); Serial.print(Tnow.mm); Serial.print(" : "); Serial.print(Tnow.ss);
  Serial.print("  ///  ");
  Serial.print(vRTC.UnixTime);
  Serial.println(" ");
  
  vRTC.getTime(& Tnow);          // get time from Virtual RTC
  //Serial.println(vRTC.UnixTime);
  Serial.print(Tnow.d); Serial.print(" - "); Serial.print(Tnow.m); Serial.print(" - "); Serial.print(Tnow.y);
  Serial.print("  ///  ");
  Serial.print(Tnow.hh); Serial.print(" : "); Serial.print(Tnow.mm); Serial.print(" : "); Serial.print(Tnow.ss);
  Serial.print("  ///  ");
  Serial.print(vRTC.UnixTime);
  Serial.println(" ");
}
