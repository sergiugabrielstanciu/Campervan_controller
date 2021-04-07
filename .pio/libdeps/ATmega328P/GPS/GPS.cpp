/* mbed EM-406 GPS Module Library
 * Copyright (c) 2008-2010, sford
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "GPS.h"

GPS::GPS(PinName tx, PinName rx) : _gps(tx, rx) {
    //_gps.baud(4800);
    longitude = 0.0;
    latitude = 0.0;
}

int GPS::sample() {
    float time;
    char ns, ew;
    int lock;

    //return 1; //testing by Jigar
    while (1) {
        getline();
    //printf("\n\rentered the GPS.sample while loop \n\r %s\n\r", msg);
        
        
        // Check if it is a GPGGA msg (matches both locked and non-locked msg)
        // $GPGGA,000116.031,,,,,0,00,,,M,0.0,M,,0000*52
        /*
        eg3. $GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
                        1       2     3  4       5 6  7  8   9  10 11 12 13  14   15
            1    = UTC of Position
            2    = Latitude
            3    = N or S
            4    = Longitude
            5    = E or W
            6    = GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix)
            7    = Number of satellites in use [not those in view]
            8    = Horizontal dilution of position
            9    = Antenna altitude above/below mean sea level (geoid)
            10   = Meters  (Antenna height unit)
            11   = Geoidal separation (Diff. between WGS-84 earth ellipsoid and
                   mean sea level.  -=geoid is below WGS-84 ellipsoid)
            12   = Meters  (Units of geoidal separation)
            13   = Age in seconds since last update from diff. reference station
            14   = Diff. reference station ID#
            15   = Checksum

        */
       // if (sscanf(msg, "GPGGA,%f,%f,%c,%f,%c,%*d,%*d, %*f,%*f,%*f,%*f,%*f,%*f,%d ", &time, &latitude, &ns, &longitude, &ew, &lock) >= 1) {
        if(sscanf(msg, "GPGGA,%f,%f,%c,%f,%c,%d", &time, &latitude, &ns, &longitude, &ew, &lock) >= 1) { 

            if (!lock) 
            {
                longitude = 0.0;
                latitude = 0.0;
                utc=0.0;
                return 0;
            } 
            else 
            {
                if (ns == 'S') {
                    latitude  *= -1.0;
                }
                if (ew == 'W') {
                    longitude *= -1.0;
                }
                float degrees = trunc(latitude / 100.0f);
                float minutes = latitude - (degrees * 100.0f);
                latitude = degrees + minutes / 60.0f;
                 degrees = trunc(longitude / 100.0f ); //degrees = trunc(longitude / 100.0f * 0.01f);
                minutes = longitude - (degrees * 100.0f);
                longitude = degrees + minutes / 60.0f;
                utc=time;
                return 1;
            }
        }  
        return 0;
    }
}

float GPS::trunc(float v) {
    if (v < 0.0) {
        v*= -1.0;
        v = floor(v);
        v*=-1.0;
    } else {
        v = floor(v);
    }
    return v;
}

void GPS::getline() {
    char c;
    while (c = _gps.getc() != '$');   // wait for the start of a line
    // printf("entered the getline loop\n\r");
    for (int i=0; i<256; i++) {
        msg[i] = _gps.getc();
        if (msg[i] == '\r') {
            msg[i] = 0;
            return;
        }
    }
    error("Overflowed message limit");
}

/*
$GPRMC,000115.039,V,,,,,,,291006,,*2C
$GPGGA,000116.031,,,,,0,00,,,M,0.0,M,,0000*52
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPGSV,3,1,12,20,00,000,,10,00,000,,31,00,000,,27,00,000,*7C
$GPGSV,3,2,12,19,00,000,,07,00,000,,04,00,000,,24,00,000,*76
$GPGSV,3,3,12,16,00,000,,28,00,000,,26,00,000,,29,00,000,*78
$GPRMC,000116.031,V,,,,,,,291006,,*27
$GPGGA,000117.035,,,,,0,00,,,M,0.0,M,,0000*57
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPRMC,000117.035,V,,,,,,,291006,,*22
$GPGGA,000118.039,,,,,0,00,,,M,0.0,M,,0000*54
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPRMC,000118.039,V,,,,,,,291006,,*21
$GPGGA,000119.035,,,,,0,00,,,M,0.0,M,,0000*59
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPRMC,000119.035,V,,,,,,,291006,,*2C
$GPGGA,000120.037,,,,,0,00,,,M,0.0,M,,0000*51
$GPGSA,A,1,,,,,,,,,,,,,,,*1E
$GPRMC,000120.037,V,,,,,,,291006,,*24

*/