

//-----------------------------------------------------------------------

#define  DEBUG




#include <avr/pgmspace.h>
#include "VirtualRTC.h"         // a virtual Unix Time RTC to simulate a DS32331 or else



// Set the Virtual RTC
//VirtualRTC vRTC;                // SummerTime disable
VirtualRTC vRTC(EU_SUMMERTIME); // NO_SUMMERTIME:disable, EU_SUMMERTIME~US_SUMMERTIME:Summertime with EU~US rules
//VirtualRTC vRTC(US_SUMMERTIME); // NO_SUMMERTIME:disable, EU_SUMMERTIME~US_SUMMERTIME:Summertime with EU~US rules
//VirtualRTC vRTC(NO_SUMMERTIME); // NO_SUMMERTIME:disable, EU_SUMMERTIME~US_SUMMERTIME:Summertime with EU~US rules



//-----------------------------------------------------------------------



//--- RTC management
float  MoonDay        = 0;

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
  testMoon();
}

void testMoon()
{
  int    iAG    = int(vRTC.getMoonPhase(& Tnow));
  
  String phase;
  switch(iAG)
  {
    case 0: phase = "New moon";         break;
    case 1: phase = "Evening crescent"; break;
    case 2: phase = "First quarter";    break;
    case 3: phase = "Waxing gibbous";   break;
    case 4: phase = "Full moon";        break;
    case 5: phase = "Waning gibbous";   break;
    case 6: phase = "Last quarter";     break;
    case 7: phase = "Morning crescent"; break;
  }

  if (iAG == 0 || iAG == 2 || iAG == 4 || iAG == 6)
  {
    Serial.println("========================");
    Serial.print(Tnow.d); Serial.print(" - "); Serial.print(Tnow.m); Serial.print(" - "); Serial.println(Tnow.y);
    Serial.print(AG); Serial.print(" - "); Serial.print(m_Days); Serial.print(" - "); 
    Serial.print("Moon phase: "); Serial.println(phase);
  }

  Tnow.d++;
  vRTC.setTime(& Tnow);
  
  vRTC.getTime(& Tnow);
  delay(500);
}





