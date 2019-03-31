/*  ============================================================================
 *  Written by Eric Paquot, 03/2019
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
 
 

#ifndef VirtualRTC_h
#define VirtualRTC_h



#include "main.h"

#define  NO_SUMMERTIME    0x00  // NO Summertime
#define  EU_SUMMERTIME    0x01  // EU rules
#define  US_SUMMERTIME    0x02  // US rules

class VirtualRTC
{
  public:
    VirtualRTC(byte summertimeRules);
    VirtualRTC();

    ~VirtualRTC();

    VirtualRTC(const VirtualRTC& other)             = delete; // don't copy the object
    VirtualRTC(VirtualRTC&& other)                  = delete;
    VirtualRTC& operator=(const VirtualRTC& other)  = delete;
    
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


#endif  // VirtualRTC_h
