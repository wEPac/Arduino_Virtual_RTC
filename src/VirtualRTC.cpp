/*  ============================================================================
 *  Written by Eric Paquot, 03/2019
 *  https://github.com/wEPac
 *  ============================================================================
 *  
 *  A virtual Real Time Clock to manage Unix Time as reference.
 *  It will provide dat of the week, leap years, moon phases...
 *  
 *  ============================================================================
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 *
 *  ============================================================================
 */
 


#include <arduino.h>
#include "VirtualRTC.h"




/*/
VirtualRTC::VirtualRTC(byte summertimeRules)
{
  UnixTime      = 0;                // 1970/01/01, 0h00:00 Thursday
  wTemperature  = 0xFFFF;           // flag is null for 1st read
  SummerTime    = summertimeRules;  // false:NO SummerTime,   0x01:EU rules,   0x02:US rules
}

VirtualRTC::VirtualRTC()
{
  UnixTime      = 0;                // 1970/01/01, 0h00:00 Thursday
  wTemperature  = 0xFFFF;           // flag is null for 1st read
  SummerTime    = false;            // false:NO SummerTime,   0x01:EU rules,   0x02:US rules
}
//*/
/*/
VirtualRTC::VirtualRTC(byte summertimeRules):
  UnixTime(0),                // 1970/01/01, 0h00:00 Thursday
  wTemperature(0xFFFF),       // flag is null for 1st read
  SummerTime(summertimeRules) // false:NO SummerTime,   0x01:EU rules,   0x02:US rules
{}

VirtualRTC::VirtualRTC():
  UnixTime(0),                // 1970/01/01, 0h00:00 Thursday
  wTemperature(0xFFFF),       // flag is null for 1st read
  SummerTime(false)           // false:NO SummerTime,   0x01:EU rules,   0x02:US rules
{}
//*/
//*
VirtualRTC::VirtualRTC(byte summertimeRules):
  SummerTime(summertimeRules) // false:NO SummerTime,   0x01:EU rules,   0x02:US rules
{}

VirtualRTC::VirtualRTC():
  VirtualRTC(false)           // false:NO SummerTime,   0x01:EU rules,   0x02:US rules
{}

VirtualRTC::~VirtualRTC()
{
  UnixTime      = 0;          // 1970/01/01, 0h00:00 Thursday
  wTemperature  = 0xFFFF;     // flag is null for 1st read
}
//*/


//-----------------------------------------------------------------------
// Times functions - virtual RTC, UnixTime
//-----------------------------------------------------------------------

// ========================== Private ===========================
bool Integrity = 0;

double VirtualRTC::normalize(double value)
{
  // normalize value between 0~1
  
  value -= floor(value);
  if (value < 0) value += 1;
  return value;
}

long VirtualRTC::getJulianDate(struct DateTime *pTbuff)
{
  // calculate julian date at 12h UT
  //
  // algorithm adapted from Stephen R. Schmitt's "Lunar Phase Computation program"
  //      http://home.att.net/~srschmitt/lunarphasecalc.html
  
  long   JY, JM, JD;                  // Julian Year, Month, Date
  long   K1, K2, K3;                  // temporary var
  
  JY  = pTbuff->y - floor((12 - pTbuff->m) / 10); // 12h UT
  
  JM  = pTbuff->m + 9;                // date in Julian calendar
  if (JM >= 12)      JM -= 12;        // date in Gregorian calendar
  
  K1  = floor(365.25 * (JY + 4712));
  K2  = floor(30.60 * JM + 0.50);
  K3  = floor(floor((JY / 100) + 49) * 0.75) - 38;

  JD  = K1 + K2 + pTbuff->d + 59;     // date in Julian calendar
  if (JD >  2299160) JD -= K3;        // date in Gregorian calendar

  return JD;
}



// ========================== Public ===========================

// ========================== Moon

double VirtualRTC::getMoonPhase(struct DateTime *pTbuff)
{
  // calculate the current phase of the moon based on the current date
  // return moon phase:
  //
  //    phase                       phase max
  //                          29 days         30 days  
  //    -------------------------------------------------------
  //    new moon               0.00    0.00    0.00        0
  //      ]~evening crescent   3.57    3.62    3.69        
  //    1st quarter            7.13    7.25    7.38        7
  //      ]~waxing gibbous    10,70   10.88   11.07        
  //    full moon             14.27   14.50   14.77       14~15
  //      ]~waning gibbous    17.83   18.13   18.46       
  //    last quarter          21.40   21.75   22.15       21~22
  //      ]~morning crescent  24.96   25.38   25.84       
  //    new moon >            28.53   29.00   29.53       28~29
  //
  //    xx.00 >= AG >  xx.53, moon phase from [0, 28]
  //    xx.00 <  AG <= xx.53, moon phase from [0, 29]
  //
  // algorithm adapted from Stephen R. Schmitt's "Lunar Phase Computation program"
  //      http://home.att.net/~srschmitt/lunarphasecalc.html
  
  long   JD = getJulianDate(pTbuff);        // Julian Date at 12h UT
  double AG, IP;                            // Moon's Age, Moon's Phase
  
  // calculate moon's age in days (start at zero)
  IP  = normalize((JD - 2451550.1) / 29.530588853);
  AG  = IP * 29.53;

  //* make this a comment to return floating Moon's age
  // calculate the moon phase
  double DIV    = AG - int(AG);
  byte   offset = ((DIV > 0.53) ? 0 : 1);   // a 30 day long moon phase
  DIV           = (28.0 + offset + DIV) / 4.0;
  AG           += 0.47; 
  if      (AG <           2.0)    AG = 0;   // "New moon"
  else if (AG <     DIV - 1.0)    AG = 1;   // "Evening crescent"
  else if (AG <     DIV + 2.0)    AG = 2;   // "First quarter"
  else if (AG < 2 * DIV      )    AG = 3;   // "Waxing gibbous"
  else if (AG < 2 * DIV + 3.0)    AG = 4;   // "Full moon"
  else if (AG < 3 * DIV - 1.0)    AG = 5;   // "Waning gibbous"
  else if (AG < 3 * DIV + 2.0)    AG = 6;   // "Last quarter"
  else if (AG < 4 * DIV - offset) AG = 7;   // "Morning crescent"
  else                            AG = 0;   // "New moon"
  AG += offset / 10.0;                      // add 0.1 for a 30 day long phase
  //*/

  return AG;
};

byte VirtualRTC::getMoonPosition(struct DateTime *pTbuff)
{
  // calculate moon's position
  // Moon's distance, Moon's ecliptic latitude, Moon's ecliptic longitude
  //
  // algorithm adapted from Stephen R. Schmitt's "Lunar Phase Computation program"
  //      http://home.att.net/~srschmitt/lunarphasecalc.html
  
  long   JD = getJulianDate(pTbuff);    // Julian Date at 12h UT
  double IP;                            // Moon's Age, Moon's Phase
  long   DP, NP, RP;                    // 
  long   DI, LA, LO;                    // Moon's distance, Moon's ecliptic latitude, Moon's ecliptic longitude
  
  IP  = normalize((JD - 2451550.1) / 29.530588853);
  IP *= 2 * PI;                         // Convert phase to radians

  DP  = normalize((JD - 2451562.2) / 27.554549880) * 2 * PI;
  DI  = 60.4 - 3.3 * cos(DP) - 0.6 * cos(2 * IP - DP) - 0.5 * cos(2 * IP);

  NP  = normalize((JD - 2451565.2) / 27.212220817) * 2 * PI;
  LA  = 5.1 * sin(NP);

  RP  = normalize((JD - 2451555.8) / 27.321582241);
  LO  = 360 * RP + 6.3 * sin(DP) + 1.3 * sin(2 * IP - DP) + 0.7 * sin(2 * IP);
  if (LO > 360)      LO -= 360;         // correcting if longitude is not greater than 360
  
  struct MoonPos Moon = {DI, LA, LO};
  return & Moon;
}



// ========================== Date / Time

void VirtualRTC::setSummertime(byte summertimeRules)
{
  // set ON / OFF Summertime usage and rules
  SummerTime    = summertimeRules;  // false:NO SummerTime,   0x01:EU rules,   0x02:US rules
}

bool VirtualRTC::getSummerTime(struct DateTime *pTbuff, bool isUTC)
{
  // return 1 when in SummerTime (means UTC + 1h)
  // isUTC:true,  pTbuff compare with UTC time as reference
  //       false, pTbuff compare with local time as reference 
  // SummerTime:
  //    Europe:   last Sunday from March at 2 AM  -  last Sunday from October at 3 AM
  //    USA:      2nd  Sunday from March at 2 AM  -  1st  Sunday from October at 2 AM

  // boundaries
  if      (!SummerTime || pTbuff->m < 3 || pTbuff->m > 10) return 0;  // WinterTime || NO SummerTime usage
  else if (               pTbuff->m > 3 && pTbuff->m < 10) return 1;  // SummerTime

  // check for march and october
  byte          day_ref;
  bool          US_rules = ((SummerTime & 0x02) ? 1 : 0);
  unsigned int  year_ref = pTbuff->y;
  year_ref               = year_ref + int(year_ref / 4) - int(year_ref / 100) + int(year_ref / 400);
  
  if      (pTbuff->m == 3)    // for march
  {
    day_ref  = ((US_rules) ?  2 : 5);
    day_ref  = (day_ref + year_ref) % 7;
    day_ref  = ((US_rules) ? 14 : 31) - day_ref;    // day in march for summerTime
    //if ((pTbuff->d > day_ref) || (pTbuff->d == day_ref && pTbuff->hh >= 2 - isUTC))             // day to add 1h
    if ((pTbuff->d > day_ref) || (pTbuff->d == day_ref && pTbuff->hh >= 2))                     // day to add 1h
      return 1;    // at 2h AM
  }
  else                        // for october
  {
    day_ref  = ((US_rules) ?  6 : 2);
    day_ref  = (day_ref + year_ref) % 7;
    day_ref  = ((US_rules) ?  7 : 31) - day_ref;    // day in october for winterTime
    if ((pTbuff->d < day_ref) || (pTbuff->d == day_ref && pTbuff->hh <  2 - isUTC + !US_rules)) // day to keep UTC
      return 1;    // at 2h AM for USA, 3h AM for Europe
  }

  return 0;
}

bool VirtualRTC::isLeapYear(int year)
{
  // if year (1970~xxxx) is a leap year, return true
  
  return !((year % 4) && (!(year % 100) || (year % 400)));
}

byte VirtualRTC::getDaysInMonth(byte this_month, bool leap_year)
{
  byte days = 31;
  if      (this_month == 2) days = ((leap_year) ? 29 : 28);
  else if (this_month == 4 || this_month == 6 || this_month == 9 || this_month == 11) days = 30;
  return days;
}

byte VirtualRTC::getDayOfWeek(struct DateTime *pTbuff)
{
  // Return current Day of the Week from Unix time

  bool          isSummerTime = getSummerTime(pTbuff, false);
  unsigned long time = (UnixTime + ((isSummerTime) ? 3600L : 0)) / 86400;
  return ((time + 4) % 7) + 1;  // 1~7, Unix Time 1970/01/[01], 0h00:00 [Thursday](+4), 1st day is Sunday => +1
}

void VirtualRTC::setTime(struct DateTime *pTbuff)
{ 
  // Convert the content from Tbuff into UnixTime

  Integrity = true;
  
  unsigned long SECS_PER_DAY  = 86400;
  unsigned long SECS_PER_HOUR = 3600;
  unsigned long SECS_PER_MIN  = 60;
  int i;
  UnixTime = 0;
  
  i = pTbuff->y;                                      // 1970~xxxx, Unix Time [1970]/01/01, 0h00:00 Thursday
  while (i-- > 1970) UnixTime += ((isLeapYear(i) ? 366 : 365) * SECS_PER_DAY);
  
  i = pTbuff->m;                                      // 1~12,      Unix Time 1970/[01]/01, 0h00:00 Thursday
  while (--i)        UnixTime += getDaysInMonth(i, isLeapYear(pTbuff->y)) * SECS_PER_DAY;
  
  UnixTime += (pTbuff->d  - 1) * SECS_PER_DAY;        // 1~31,      Unix Time 1970/01/[01], 0h00:00 Thursday
  
  UnixTime += (pTbuff->hh - 1) * SECS_PER_HOUR;       // 1~24,      Unix Time 1970/01/01, [0h]00:00 Thursday
  UnixTime +=  pTbuff->mm      * SECS_PER_MIN;
  UnixTime +=  pTbuff->ss;

  UnixTime -= getSummerTime(pTbuff, false) * SECS_PER_HOUR;  // if using Summertime, -1h to keep UTC for UnixTime
}

void VirtualRTC::getTime(struct DateTime *pTbuff)
{
  // Convert UnixTime into Tbuff (Date & Time)
  
  unsigned long time = UnixTime;

  pTbuff->ss   = time % 60;
  time /= 60;
  pTbuff->mm   = time % 60;
  time /= 60;
  time += 1;
  pTbuff->hh   = time % 24;             // 1~24, Unix Time 1970/01/01, [0h]00:00 Thursday
  time /= 24;
  
  pTbuff->dow  = ((time +  4) % 7) + 1; // 1~7,  Unix Time 1970/01/[01], 0h00:00 [Thursday]=> +4, 1st day is Sunday => +1
  
  unsigned long days  = 0;
  unsigned int  temp  = 1970;           // 1970~xxxx, Unix Time [1970]/01/01, 0h00:00 Thursday
  while (time >= (days += (unsigned long)((isLeapYear(temp) ? 366 : 365))))
  {
    ++temp;
  }
  pTbuff->y = temp;
  bool leap_year = isLeapYear(temp);
  
  time -= days - (unsigned long)((leap_year) ? 366 : 365); // days in the current year
  days  = 0;
  temp  = 0;                            // 1~12, Unix Time 1970/[01]/01, 0h00:00 Thursday, month starts at 1 => start with inc
  while (time >= days)
  {
    time -= days;                            
    days  = getDaysInMonth(++temp, leap_year);
  }
  pTbuff->m = temp;
  
  pTbuff->d = time + 1;                 // 1~31, Unix Time 1970/01/[01], 0h00:00 Thursday, month starts at day 1 => +1
  
  if (getSummerTime(pTbuff, true))      // if using Summertime, +1h from UTC UnixTime
  {
    pTbuff->hh++;
    if (pTbuff->hh > 23)
    {
      pTbuff->hh = 0;
      pTbuff->d++;
      pTbuff->dow = (pTbuff->dow % 7) + 1;
      if (pTbuff->d > getDaysInMonth(pTbuff->m, leap_year))
      {
        pTbuff->d = 1;
        pTbuff->m++;    // stop here, SummerTime is between march and october
      }
    }
  }
}



// ========================== Temperature

float VirtualRTC::getTemp(bool is_celsius)
{
  // Return the internal temperature in C or F
  
  // The internal temperature has to be used with the internal reference of 1.1V
  // set the internal reference and mux
#if defined(__AVR__)
#  if defined(__AVR_ATmega32U4__)
  ADMUX   = (_BV(REFS1) | _BV(REFS0) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0));
  ADCSRB  = (_BV(MUX5);                 // the MUX5 bit is in the ADCSRB register
#  elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
  ADMUX   = (_BV(REFS1) | _BV(MUX5) | _BV(MUX1));
#  elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
  ADMUX   = (_BV(REFS1) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1) | _BV(MUX0));
#  else
  ADMUX   = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));
#  endif
#else
  return 0;
#endif
  
  ADCSRA |=  _BV(ADEN);                 // enable the ADC (seems to lock sensor here :S )
  delay(20);                            // wait for voltages to become stable
  ADCSRA |=  _BV(ADSC);                 // start the ADC
  while (bit_is_set(ADCSRA, ADSC));     // detect end-of-conversion

  // read register "ADCW", take care for 1st read
  if (wTemperature == 0xFFFF) wTemperature = ADCW;
  else                        wTemperature = (2 * wTemperature + ADCW) / 3;

  // The offset of 324.31 could be wrong. It is just an indication
  float temp = (wTemperature - 324.31) / 1.22;  // in °C
  if (is_celsius) return temp;
  return (temp * 9) / 5.0 + 32;                 // in °F
}

bool VirtualRTC::getIntegrity()
{
  return Integrity;
}

