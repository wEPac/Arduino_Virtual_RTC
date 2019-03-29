
 

#ifndef _VIRTUAL_RTC
#define _VIRTUAL_RTC



#define  NO_SUMMERTIME    0x00  // NO Summertime
#define  EU_SUMMERTIME    0x01  // EU rules
#define  US_SUMMERTIME    0x02  // US rules

struct  DateTime
{ //                        range   format    example
  int  y;   // Year:        1970~n  ####      2019
  byte m;   // Month:       1~12    #~##      1 is January
  byte d;   // Day:         1~31    #~##      27
  byte dow; // DayOfWeek:   1~7     #~##      1 is Sunday
  byte hh;  // hour:        0~23    #~##      8
  byte mm;  // min:         0~60    #~##      0
  byte ss;  // s:           0~60    #~##      12
};



class VirtualRTC
{
  public:
    VirtualRTC(byte summertimeRules);
    VirtualRTC();
    
    struct  MoonPos
    {
      long di;  // Moon's distance in earth radian
      long la;  // Moon's ecliptic latitude
      long lo;  // Moon's ecliptic longitude
    };
    unsigned long UnixTime;

    double getMoonPhase(struct DateTime *pTbuff);
    byte   getMoonPosition(struct DateTime *pTbuff);
    void   setSummertime(byte summertimeRules);
    bool   getSummerTime(struct DateTime *pTbuff, bool isUTC);
    bool   isLeapYear(int year);
    byte   getDaysInMonth(byte this_month, bool leap_year);
    byte   getDayOfWeek(struct DateTime *pTbuff);
    void   setTime(struct DateTime *pTbuff);
    void   getTime(struct DateTime *pTbuff);
    float  getTemp(bool is_celsius);
    bool   getIntegrity();

  private:
    unsigned int wTemperature;
    byte         Integrity;
    byte         SummerTime;
    
    double normalize(double value);
    long   getJulianDate(struct DateTime *pTbuff);
    
};


#endif
