/* tool to read power production data for SMA solar power convertors 
   Copyright Wim Hofman 2010 
   Copyright Stephen Collier 2010 

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */

/* compile gcc -lbluetooth -lcurl -lmysqlclient -g -o smatool smatool.c */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include <curl/curl.h>
#include "mysql.c"

/*
 * u16 represents an unsigned 16-bit number.  Adjust the typedef for
 * your hardware.
 */
typedef u_int16_t u16;

#define PPPINITFCS16 0xffff /* Initial FCS value    */
#define PPPGOODFCS16 0xf0b8 /* Good final FCS value */
#define ASSERT(x) assert(x)

typedef struct {
    char Inverter[20];
    char BTAddress[20];
    char Password[20];
    char Config[80];
    char File[80];
    float latitude_f;
    float longitude_f;
    char MySqlHost[20];
    char MySqlDatabase[20];
    char MySqlUser[80];
    char MySqlPwd[80];
    char PVOutputURL[80];
    char PVOutputKey[80];
    char PVOutputSid[20];
} ConfType;

char *accepted_strings[] = {
"$END",
"$ADDR",
"$TIME",
"$SER",
"$CRC",
"$POW",
"$DTOT",
"$ADD2",
"$CHAN",
"$ITIME",
"$TMMI",
"$TMPL",
"$TIMESTRING",
"$TIMEFROM1",
"$TIMETO1",
"$TIMEFROM2",
"$TIMETO2",
"$TESTDATA",
"$ARCHIVEDATA1",
"$PASSWORD",
"$SIGNAL",
"$UNKNOWN"
};

int cc,verbose = 0;
unsigned char fl[1024] = { 0 };



static u16 fcstab[256] = {
   0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
   0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
   0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
   0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
   0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
   0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
   0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
   0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
   0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
   0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
   0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
   0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
   0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
   0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
   0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
   0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
   0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
   0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
   0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
   0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
   0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
   0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
   0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
   0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
   0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
   0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
   0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
   0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
   0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
   0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
   0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
   0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

/*
 * Calculate a new fcs given the current fcs and the new data.
 */
u16 pppfcs16(u16 fcs, void *_cp, int len)
{
    register unsigned char *cp = (unsigned char *)_cp;
    /* don't worry about the efficiency of these asserts here.  gcc will
     * recognise that the asserted expressions are constant and remove them.
     * Whether they are usefull is another question. 
     */

    ASSERT(sizeof (u16) == 2);
    ASSERT(((u16) -1) > 0);
    while (len--)
        fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];
    return (fcs);
}

/*
 * Strip escapes (7D) as they aren't includes in fcs
 */
strip_escapes(unsigned char *cp, int *len)
{
    int i,j;

    for( i=0; i<(*len); i++ ) {
      if( cp[i] == 0x7d ) { //Found escape character. Need to convert
        cp[i] = cp[i+1]^0x20;
        for( j=i+1; j<(*len)-1; j++ ) cp[j] = cp[j+1];
        (*len)--;
      }
   }
}

/*
 * Add escapes (7D) as they are required
 */
add_escapes(unsigned char *cp, int *len)
{
    int i,j;

    for( i=19; i<(*len); i++ ) {
      switch( cp[i] ) {
        case 0x7d :
        case 0x7e :
        case 0x11 :
        case 0x12 :
        case 0x13 :
          for( j=(*len); j>i; j-- ) cp[j] = cp[j-1];
          cp[i+1] = cp[i]^0x20;
          cp[i]=0x7d;
          (*len)++;
      }
   }
}

/*
 * Recalculate and update length to correct for escapes
 */
fix_length_send(unsigned char *cp, int *len)
{
    int	    delta=0;

    if(( cp[1] != (*len)+1 ))
    {
      delta = (*len)+1 - cp[1];
      if( verbose == 1 ) {
          printf( "sum=%x", cp[1]+cp[3] );
          printf( "  length change from %x to %x \n", cp[1],(*len)+1 );
      }
      cp[1] =(*len)+1;
      switch( cp[1] ) {
        case 0x3a: cp[3]=0x44; break;
        case 0x3e: cp[3]=0x40; break;
        case 0x40: cp[3]=0x3e; break;
        case 0x41: cp[3]=0x3f; break;
        case 0x42: cp[3]=0x3c; break;
        case 0x53: cp[3]=0x2d; break;
        case 0x54: cp[3]=0x2a; break;
        case 0x55: cp[3]=0x2b; break;
        case 0x5c: cp[3]=0x22; break;
        case 0x5d: cp[3]=0x23; break;
        case 0x5e: cp[3]=0x20; break;
        case 0x5f: cp[3]=0x21; break;
        case 0x60: cp[3]=0x1e; break;
        case 0x61: cp[3]=0x1f; break;
        case 0x62: cp[3]=0x1e; break;
        default: printf( "NO CONVERSION!" );getchar();break;
      }
    }
}
            
/*
 * Recalculate and update length to correct for escapes
 */
fix_length_received(unsigned char *received, int *len)
{
    int	    delta=0;
    int	    sum;

    if( received[1] != (*len) )
    {
      sum = received[1]+received[3];
      if (verbose == 1) printf( "sum=%x", sum );
      delta = (*len) - received[1];
      if (verbose == 1) printf( "length change from %x to %x\n", received[1], (*len) );
      if(( received[3] != 0x13 )&&( received[3] != 0x14 )) { 
        received[1] = (*len);
        switch( received[1] ) {
          case 0x52: received[3]=0x2c; break;
          case 0x6a: received[3]=0x14; break;
          default:  received[3]=sum-received[1]; break;
        }
      }
    }
}

/*
 * How to use the fcs
 */
tryfcs16(unsigned char *cp, int len)
{
    u16 trialfcs;
    unsigned
    int i;	 
    unsigned char stripped[1024] = { 0 };

    memcpy( stripped, cp, len );
    /* add on output */
    if (verbose ==2){
 	printf("String to calculate FCS\n");	 
        	for (i=0;i<len;i++) printf("%02x ",cp[i]);
	 	printf("\n\n");
    }	
    //strip_escapes( stripped, &len );
    trialfcs = pppfcs16( PPPINITFCS16, stripped, len );
    trialfcs ^= 0xffff;               /* complement */
    fl[cc] = (trialfcs & 0x00ff);    /* least significant byte first */
    fl[cc+1] = ((trialfcs >> 8) & 0x00ff);
    cc+=2;
    if (verbose == 2 ){ 
	printf("FCS = %x%x %x\n",(trialfcs & 0x00ff),((trialfcs >> 8) & 0x00ff), trialfcs); 
    }
}


unsigned char conv(char *nn){
	unsigned char tt=0,res=0;
	int i;   
	
	for(i=0;i<2;i++){
		switch(nn[i]){

		case 65: //A
		case 97: //a
		tt = 10;
		break;

		case 66: //B
		case 98: //b
		tt = 11;
		break;

		case 67: //C
		case 99: //c
		tt = 12;
		break;

		case 68: //D
		case 100: //d
		tt = 13;
		break;

		case 69: //E
		case 101: //e
		tt = 14;
		break;

		case 70: //F
		case 102: //f
		tt = 15;
		break;


		default:
		tt = nn[i] - 48;
		}
		res = res + (tt * pow(16,1-i));
		}
		return res;
}

int select_str(char *s)
{
    int i;
    for (i=0; i < sizeof(accepted_strings)/sizeof(*accepted_strings);i++)
    {
       //printf( "\ni=%d accepted=%s string=%s", i, accepted_strings[i], s );
       if (!strcmp(s, accepted_strings[i])) return i;
    }
    return -1;
}

char *  sunrise( float latitude, float longitude )
{
   //adapted from http://williams.best.vwh.net/sunrise_sunset_algorithm.htm
   time_t curtime;
   struct tm *loctime;
   int day,month,year,hour,minute,second,datapoint;
   char *returntime;

   double lnghour,t,M,L,T,RA,Lquadrant,RAquadrant,sinDec,cosDec;
   double cosH, H, UT, localT,lngHour;
   float localOffset=11.0,zenith=91;
   double pi=M_PI;

   returntime = (char *)malloc(6*sizeof(char));
   curtime = time(NULL);  //get time in seconds since epoch (1/1/1970)	
   loctime = localtime(&curtime);
   day = loctime->tm_mday;
   month = loctime->tm_mon +1;
   year = loctime->tm_year + 1900;
   hour = loctime->tm_hour;
   minute = loctime->tm_min; 

   lngHour = longitude / 15;
   t = loctime->tm_yday + ((6 - lngHour) / 24);
   //Calculate the Sun's mean anomaly
   M = (0.9856 * t) - 3.289;
   //Calculate the Sun's tru longitude
   L = M + (1.916 * sin((pi/180)*M)) + (0.020 * sin(2 * (pi/180)*M)) + 282.634;
   if( L > 360 ) L=L-360;
   if( L < 0 ) L=L+360;
   //calculate the Sun's right ascension
   RA = (180/pi)*atan(0.91764 * tan((pi/180)*L));
   //right ascension value needs to be in the same quadrant as L
   Lquadrant  = (floor( L/90)) * 90;
   RAquadrant = (floor(RA/90)) * 90;
    
   RA = RA + (Lquadrant - RAquadrant);
   //right ascension value needs to be converted into hours
   RA = RA / 15;
   //calculate the Sun's declination
   sinDec = 0.39782 * sin((pi/180)*L);
   cosDec = cos(asin(sinDec));
   //calculate the Sun's local hour angle
   cosH = (cos((pi/180)*zenith) - (sinDec * sin((pi/180)*latitude))) / (cosDec * cos((pi/180)*latitude));
	
   if (cosH >  1); 
	  //the sun never rises on this location (on the specified date)
   if (cosH < -1);
	  //the sun never sets on this location (on the specified date)
   //finish calculating H and convert into hours
   H = 360 -(180/pi)*acos(cosH);
   H = H/15;
   //calculate local mean time of rising/setting
   T = H + RA - (0.06571 * t) - 6.622;
   //adjust back to UTC
   UT = T - lngHour;
   //convert UT value to local time zone of latitude/longitude
   localT = UT + localOffset;
   sprintf( returntime, "%02.0f:%02.0f",floor(localT),(localT-floor(localT))*60 ); 
   return returntime;
}

char * sunset( float latitude, float longitude )
{
   //adapted from http://williams.best.vwh.net/sunrise_sunset_algorithm.htm
   time_t curtime;
   struct tm *loctime;
   int day,month,year,hour,minute,second,datapoint;
   char *returntime;

   returntime = (char *)malloc(6*sizeof(char));
   double lnghour,t,M,L,T,RA,Lquadrant,RAquadrant,sinDec,cosDec;
   double cosH, H, UT, localT,lngHour;
   float localOffset=11.0,zenith=91;
   double pi=M_PI;

   curtime = time(NULL);  //get time in seconds since epoch (1/1/1970)	
   loctime = localtime(&curtime);
   day = loctime->tm_mday;
   month = loctime->tm_mon +1;
   year = loctime->tm_year + 1900;
   hour = loctime->tm_hour;
   minute = loctime->tm_min; 

   lngHour = longitude / 15;
   t = loctime->tm_yday + ((18 - lngHour) / 24);
   //Calculate the Sun's mean anomaly
   M = (0.9856 * t) - 3.289;
   //Calculate the Sun's tru longitude
   L = M + (1.916 * sin((pi/180)*M)) + (0.020 * sin(2 * (pi/180)*M)) + 282.634;
   if( L > 360 ) L=L-360;
   if( L < 0 ) L=L+360;
   //calculate the Sun's right ascension
   RA = (180/pi)*atan(0.91764 * tan((pi/180)*L));
   //right ascension value needs to be in the same quadrant as L
   Lquadrant  = (floor( L/90)) * 90;
   RAquadrant = (floor(RA/90)) * 90;
    
   RA = RA + (Lquadrant - RAquadrant);
   //right ascension value needs to be converted into hours
   RA = RA / 15;
   //calculate the Sun's declination
   sinDec = 0.39782 * sin((pi/180)*L);
   cosDec = cos(asin(sinDec));
   //calculate the Sun's local hour angle
   cosH = (cos((pi/180)*zenith) - (sinDec * sin((pi/180)*latitude))) / (cosDec * cos((pi/180)*latitude));
	
   if (cosH >  1); 
	  //the sun never rises on this location (on the specified date)
   if (cosH < -1);
	  //the sun never sets on this location (on the specified date)
   //finish calculating H and convert into hours
   H = (180/pi)*acos(cosH);
   H = H/15;
   //calculate local mean time of rising/setting
   T = H + RA - (0.06571 * t) - 6.622;
   //adjust back to UTC
   UT = T - lngHour;
   if( UT > 24 ) UT=UT-24;
   if( UT < 0 ) UT=UT+24;
   //convert UT value to local time zone of latitude/longitude
   localT = UT + localOffset;
   sprintf( returntime, "%02.0f:%02.0f",floor(localT),(localT-floor(localT))*60 ); 
   return returntime;
}

int todays_almanac( ConfType *conf )
/*  Check if sunset and sunrise have been set today */
{
    int	        found=0;
    MYSQL_ROW 	row;
    char 	SQLQUERY[200];

    OpenMySqlDatabase( conf->MySqlHost, conf->MySqlUser, conf->MySqlPwd, conf->MySqlDatabase);
    //Get Start of day value
    sprintf(SQLQUERY,"SELECT sunrise FROM Almanac WHERE date=DATE_FORMAT( NOW(), \"%%Y%%m%%d\" ) " );
    if (verbose == 1) printf("%s\n",SQLQUERY);
    DoQuery(SQLQUERY);
    if (row = mysql_fetch_row(res))  //if there is a result, update the row
    {
       found=1;
    }
    mysql_close(conn);
    return found;
}

void update_almanac( ConfType *conf, char * sunrise, char * sunset )
{
    MYSQL_ROW 	row;
    char 	SQLQUERY[200];

    OpenMySqlDatabase( conf->MySqlHost, conf->MySqlUser, conf->MySqlPwd, conf->MySqlDatabase);
    //Get Start of day value
    sprintf(SQLQUERY,"INSERT INTO Almanac SET sunrise=CONCAT(DATE_FORMAT( NOW(), \"%%Y-%%m-%%d \"),\"%s\"), sunset=CONCAT(DATE_FORMAT( NOW(), \"%%Y-%%m-%%d \"),\"%s\" ), date=NOW() ", sunrise, sunset );
    if (verbose == 1) printf("%s\n",SQLQUERY);
    DoQuery(SQLQUERY);
    mysql_close(conn);
}

int is_light( ConfType * conf )
/*  Check if all data done and past sunset or before sunrise */
{
    int	        light=1;
    MYSQL_ROW 	row;
    char 	SQLQUERY[200];

    OpenMySqlDatabase( conf->MySqlHost, conf->MySqlUser, conf->MySqlPwd, conf->MySqlDatabase);
    //Get Start of day value
    sprintf(SQLQUERY,"SELECT if(sunrise < NOW(),1,0) FROM Almanac WHERE date= DATE_FORMAT( NOW(), \"%%Y-%%m-%%d\" ) " );
    if (verbose == 1) printf("%s\n",SQLQUERY);
    DoQuery(SQLQUERY);
    if (row = mysql_fetch_row(res))  //if there is a result, update the row
    {
       if( atoi( (char *)row[0] ) == 0 ) light=0;
    }
    if( light ) {
       sprintf(SQLQUERY,"SELECT if( dd.datetime > al.sunset,1,0) FROM DayData as dd left join Almanac as al on al.date=DATE(dd.datetime) and al.date=DATE(NOW()) WHERE 1 ORDER BY dd.datetime DESC LIMIT 1" );
       if (verbose == 1) printf("%s\n",SQLQUERY);
       DoQuery(SQLQUERY);
       if (row = mysql_fetch_row(res))  //if there is a result, update the row
       {
          if( atoi( (char *)row[0] ) == 1 ) light=0;
       }
    }
    
    mysql_close(conn);
    return light;
}

//Set a value depending on inverter
int  SetInverterType( ConfType *conf, char *InverterCode )  
{
    if( strcmp(conf->Inverter, "3000TL") == 0 ) {
        InverterCode[0] = 0x12;
        InverterCode[1] = 0x1a;
        InverterCode[2] = 0xd9;
        InverterCode[3] = 0x38;
    }
    if( strcmp(conf->Inverter, "5000TL") == 0 ) {
        InverterCode[0] = 0x3f;
        InverterCode[1] = 0x10;
        InverterCode[2] = 0xfb;
        InverterCode[3] = 0x39;
    }
}

// Set switches to save lots of strcmps
int  SetSwitches( ConfType *conf, int *location, int *mysql, int *post, int *file )  
{
    //Check if all location variables are set
    if(( conf->latitude_f <= 180 )&&( conf->longitude_f <= 180 ))
        (*location)=1;
    else
        (*location)=0;
    //Check if all Mysql variables are set
    if(( strlen(conf->MySqlUser) > 0 )
	 &&( strlen(conf->MySqlPwd) > 0 )
	 &&( strlen(conf->MySqlHost) > 0 )
	 &&( strlen(conf->MySqlDatabase) > 0 ))
        (*mysql)=1;
    else
        (*mysql)=0;
    //Check if all File variables are set
    if( strlen(conf->File) > 0 )
        (*file)=1;
    else
        (*file)=0;
    //Check if all PVOutput variables are set
    if(( strlen(conf->PVOutputURL) > 0 )
	 &&( strlen(conf->PVOutputKey) > 0 )
	 &&( strlen(conf->PVOutputSid) > 0 ))
        (*post)=1;
    else
        (*post)=0;
}

/* read Config from file */
int GetConfig( ConfType *conf )
{
    FILE 	*fp;
    char	line[400];
    char	variable[400];
    char	value[400];

    if (strlen(conf->Config) > 0 )
        fp=fopen(conf->Config,"r");
    else
        fp=fopen("./smatool.conf","r");
    while (!feof(fp)){	
	if (fgets(line,400,fp) != NULL){				//read line from smatool.conf
            if( line[0] != '#' ) 
            {
                strcpy( value, "" ); //Null out value
                sscanf( line, "%s %s", variable, value );
                if( verbose == 1 ) printf( "variable=%s value=%s\n", variable, value );
                if( strcmp( variable, "Inverter" ) == 0 )
                   strcpy( conf->Inverter, value );  
                if( strcmp( variable, "BTAddress" ) == 0 )
                   strcpy( conf->BTAddress, value );  
                if( strcmp( variable, "Password" ) == 0 )
                   strcpy( conf->Password, value );  
                if( strcmp( variable, "File" ) == 0 )
                   strcpy( conf->File, value );  
                if( strcmp( variable, "Latitude" ) == 0 )
                   conf->latitude_f = atof(value) ;  
                if( strcmp( variable, "Longitude" ) == 0 )
                   conf->longitude_f = atof(value) ;  
                if( strcmp( variable, "MySqlHost" ) == 0 )
                   strcpy( conf->MySqlHost, value );  
                if( strcmp( variable, "MySqlDatabase" ) == 0 )
                   strcpy( conf->MySqlDatabase, value );  
                if( strcmp( variable, "MySqlUser" ) == 0 )
                   strcpy( conf->MySqlUser, value );  
                if( strcmp( variable, "MySqlPwd" ) == 0 )
                   strcpy( conf->MySqlPwd, value );  
                if( strcmp( variable, "PVOutputURL" ) == 0 )
                   strcpy( conf->PVOutputURL, value );  
                if( strcmp( variable, "PVOutputKey" ) == 0 )
                   strcpy( conf->PVOutputKey, value );  
                if( strcmp( variable, "PVOutputSid" ) == 0 )
                   strcpy( conf->PVOutputSid, value );  
            }
        }
    }
    fclose( fp );
}

int main(int argc, char **argv)
{
	FILE *fp;
        ConfType conf;
	struct sockaddr_rc addr = { 0 };
	unsigned char received[1024];
	unsigned char datarecord[1024];
        int archdatalen=0;
        int failedbluetooth=0;
	int s,i,j,status,mysql=0,post=0,file=0,from=0,to=0;
        int location=0;
	int ret,found,crc_at_end,last, finished=0;
        int togo=0;
        int  initstarted=0,setupstarted=0,rangedatastarted=0;
        long returnpos;
        int returnline;
        char compurl[200];
	char datefrom[100];
	char dateto[100];
        int  pass_i;
	char config[100];
	char line[400];
	char address[6] = { 0 };
	char address2[6] = { 0 };
	char timestr[25] = { 0 };
	char serial[4] = { 0 };
	char unknown[4] = { 0 };
	char *lineread;
	time_t curtime;
	time_t reporttime;
	time_t fromtime;
	time_t totime;
	time_t idate;
	struct tm *loctime;
	struct tm tm;
	int day,month,year,hour,minute,second,datapoint;
	char tt[10] = {48,48,48,48,48,48,48,48,48,48}; 
	char ti[3];	
	char pac[2];
	char tot[2];
	char chan[1];
	float currentpower;
        int   rr;
	int linenum = 0;
	float dtotal;
	float gtotal;
	float ptotal;
	float strength;
	MYSQL_ROW row;
	char SQLQUERY[200];
	char rowres[10];
   struct archdata_type
   {
      time_t date;
      float  accum_value;
      float  current_value;
   } *archdatalist;

   char sunrise_time[6],sunset_time[6];
   
   CURL *curl;
   CURLcode result;

   memset(received,0,1024);
   /* get the report time - used in various places */
   reporttime = time(NULL);  //get time in seconds since epoch (1/1/1970)	
   //reporttime = 1292823628;
   
    for (i=1;i<argc;i++)			//Read through passed arguments
    {
        if (strcmp(argv[i],"-address")==0){
            i++;
            if (i<argc){
	        strcpy(conf.BTAddress,argv[i]);
            }
	}
	if (strcmp(argv[i],"-u")==0){
            i++;
            if (i<argc){
		strcpy(conf.MySqlUser,argv[i]);
            }
        }
	if (strcmp(argv[i],"-p")==0){
            i++;
            if (i<argc){
		strcpy(conf.MySqlPwd,argv[i]);
            }
	}				
	if (strcmp(argv[i],"-v")==0) verbose = 1;
	if (strcmp(argv[i],"-mysql")==0) mysql=1;
	if (strcmp(argv[i],"-post")==0){
	    i++;
	    if(i<argc){
		strcpy(conf.PVOutputURL,argv[i]);
	    }
	}
	if (strcmp(argv[i],"-pass")==0){
	    i++;
	    if(i<argc){
	        strcpy(conf.Password,argv[i]);
	    }
	}
	if (strcmp(argv[i],"-lat")==0){
	    i++;
	    if(i<argc){
		conf.latitude_f=atof(argv[i]);
	    }
	}
	if (strcmp(argv[i],"-long")==0){
	    i++;
	    if(i<argc){
		conf.longitude_f=atof(argv[i]);
	    }
	}
	if (strcmp(argv[i],"-sid")==0){
	    i++;
	    if(i<argc){
		strcpy(conf.PVOutputSid,argv[i]);
	    }
	}
	if (strcmp(argv[i],"-key")==0){
	    i++;
	    if(i<argc){
		strcpy(conf.PVOutputKey,argv[i]);
	    }
	}
	if (strcmp(argv[i],"-file")==0){
	    i++;
	    if(i<argc){
		strcpy(conf.File,argv[i]);
	    }
	}
	if (strcmp(argv[i],"-conf")==0){
	    i++;
	    if(i<argc){
		strncpy(conf.Config,argv[i], sizeof(conf.Config));
	    }
	}
	if (strcmp(argv[i],"-from")==0){
	    i++;
	    if(i<argc){
		strcpy(datefrom,argv[i]);
                from = 1;
	    }
	}
	if (strcmp(argv[i],"-to")==0){
	    i++;
	    if(i<argc){
		strcpy(dateto,argv[i]);
		to  = 1;
	    }
	}
    }
    // read Config file
    GetConfig( &conf );
    // set switches used through the program
    SetSwitches( &conf, &location, &mysql, &post, &file );  
    // Set value for inverter type
    SetInverterType( &conf, unknown );
    // Location based information to avoid quering Inverter in the dark
    if((location==1)&&(mysql==1)) {
       if( ! todays_almanac( &conf ) ) {
           sprintf( sunrise_time, "%s", sunrise(conf.latitude_f,conf.longitude_f ));
           sprintf( sunset_time, "%s", sunset(conf.latitude_f, conf.longitude_f ));
           printf( "sunrise=%s sunset=%s\n", sunrise_time, sunset_time );
           update_almanac(  &conf, sunrise_time, sunset_time );
        }
    }
    if((location=0)||(mysql==0)||is_light( &conf ))
        {


	if (verbose ==1) printf("Address %s\n",conf.BTAddress);

        if (file ==1)
	  fp=fopen(conf.File,"r");
        else
	  fp=fopen("/etc/sma.in","r");
	// allocate a socket
   s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

   // set the connection parameters (who to connect to)
   addr.rc_family = AF_BLUETOOTH;
   addr.rc_channel = (uint8_t) 1;
   str2ba( conf.BTAddress, &addr.rc_bdaddr );

   // connect to server
   for( i=1; i<5; i++ ){
      status = connect(s, (struct sockaddr *)&addr, sizeof(addr));
	if (status <0){
		printf("Error connecting to %s\n",conf.BTAddress);
	}
        else
           break;
   }
   if (status < 0 )
   {
	return( -1 );
   }

	// convert address
	address[5] = conv(strtok(conf.BTAddress,":"));
	address[4] = conv(strtok(NULL,":"));
	address[3] = conv(strtok(NULL,":"));
	address[2] = conv(strtok(NULL,":"));
	address[1] = conv(strtok(NULL,":"));
	address[0] = conv(strtok(NULL,":"));
	
while (!feof(fp)){	
        start:
	if (fgets(line,400,fp) != NULL){				//read line from sma.in
		linenum++;
		lineread = strtok(line," ;");
		if(!strcmp(lineread,"R")){		//See if line is something we need to receive
			if (verbose	== 1) printf("[%d] Waiting for string\n",linenum);
			cc = 0;
			do{
				lineread = strtok(NULL," ;");
				switch(select_str(lineread)) {
	
				case 0: // $END
				//do nothing
				break;			

				case 1: // $ADDR
				for (i=0;i<6;i++){
					fl[cc] = address[i];
					cc++;
				}
				break;	

				case 3: // $SER
				for (i=0;i<4;i++){
					fl[cc] = serial[i];
					cc++;
				}
				break;	
				
				case 7: // $ADD2
				for (i=0;i<6;i++){
					fl[cc] = address2[i];
					cc++;
				}
				break;	

				case 8: // $CHAN
				fl[cc] = chan[0];
				cc++;
				break;

				default :
				fl[cc] = conv(lineread);
				cc++;
				}

			} while (strcmp(lineread,"$END"));
			if (verbose == 1){ 
				printf("[%d]   waiting for: ");
				for (i=0;i<cc;i++) printf("%02x ",fl[i]);
			   printf("\n\n");
			}
			if (verbose == 1) printf("[%d] Waiting for data on rfcomm\n",linenum);
			found = 0;
			do {
                            rr=0;
                            if( read_bluetooth( &s, &rr, received ) != 0 )
                            {
                               fseek( fp, returnpos, 0 );
                               linenum = returnline;
                               found=0;
                               if( archdatalen > 0 )
                                  free( archdatalist );
                               archdatalen=0;
                               strcpy( lineread, "" );
                               sleep(10);
                               failedbluetooth++;
                               if( failedbluetooth > 3 )
                                 exit(-1);
                               goto start;
                            }
                            else {
			      if (verbose == 1){ 
                                printf( "  [%d] looking for: ",linenum);
				for (i=0;i<cc;i++) printf("%02x ",fl[i]);
                                printf( "\n" );
                                printf( "  [%d] received:    ",linenum);
				for (i=0;i<rr;i++) printf("%02x ",received[i]);
			        printf("\n\n");
			      }
                           
			      if (memcmp(fl,received,cc) == 0){
				  found = 1;
				  if (verbose == 1) printf("[%d] Found string we are waiting for\n",linenum); 
			      } else {
				  if (verbose == 1) printf("[%d] Did not find string\n",linenum); 
			      }
                            }
			} while (found == 0);
			if (verbose == 1){ 
				for (i=0;i<cc;i++) printf("%02x ",fl[i]);
			   printf("\n\n");
			}
		}
		if(!strcmp(lineread,"S")){		//See if line is something we need to send
			if (verbose	== 1) printf("[%d] Sending\n",linenum);
			cc = 0;
			do{
				lineread = strtok(NULL," ;");
				switch(select_str(lineread)) {
	
				case 0: // $END
				//do nothing
				break;			

				case 1: // $ADDR
				for (i=0;i<6;i++){
					fl[cc] = address[i];
					cc++;
				}
				break;

				case 3: // $SER
				for (i=0;i<4;i++){
					fl[cc] = serial[i];
					cc++;
				}
				break;	
				

				case 7: // $ADD2
				for (i=0;i<6;i++){
					fl[cc] = address2[i];
					cc++;
				}
				break;

				case 2: // $TIME	
				// get report time and convert
				sprintf(tt,"%x",(int)reporttime); //convert to a hex in a string
				for (i=7;i>0;i=i-2){ //change order and convert to integer
					ti[1] = tt[i];
					ti[0] = tt[i-1];	
                                        ti[2] = '\0';
					fl[cc] = conv(ti);
					cc++;		
				}
				break;

				case 11: // $TMPLUS	
				// get report time and convert
				sprintf(tt,"%x",(int)reporttime+1); //convert to a hex in a string
				for (i=7;i>0;i=i-2){ //change order and convert to integer
					ti[1] = tt[i];
					ti[0] = tt[i-1];	
                                        ti[2] = '\0';
					fl[cc] = conv(ti);
					cc++;		
				}
				break;


				case 10: // $TMMINUS
				// get report time and convert
				sprintf(tt,"%x",(int)reporttime-1); //convert to a hex in a string
				for (i=7;i>0;i=i-2){ //change order and convert to integer
					ti[1] = tt[i];
					ti[0] = tt[i-1];	
                                        ti[2] = '\0';
					fl[cc] = conv(ti);
					cc++;		
				}
				break;

				case 4: //$crc
				tryfcs16(fl+19, cc -19);
                                add_escapes(fl,&cc);
                                fix_length_send(fl,&cc);
				break;

				case 8: // $CHAN
				fl[cc] = chan[0];
				cc++;
				break;

				case 12: // $TIMESTRING
				for (i=0;i<25;i++){
					fl[cc] = timestr[i];
					cc++;
				}
				break;

				case 13: // $TIMEFROM1	
				// get report time and convert
                                if( from == 1 ) {
                                    strptime( datefrom, "%Y-%m-%d %H:%M:%S", &tm);
                                    fromtime=mktime(&tm);
                                    if( fromtime == -1 ) {
                                    // Error we need to do something about it
                                        printf( "%03x",(int)fromtime ); getchar();
                                        printf( "\n%03x", fromtime ); getchar();
                                        fromtime=0;
                                        printf( "bad from" ); getchar();
                                    }
                                }
                                else
                                {
                                  printf( "no from" ); getchar();
                                  fromtime=0;
                                }
				sprintf(tt,"%03x",(int)fromtime-300); //convert to a hex in a string and start 5 mins before for dummy read.
				for (i=7;i>0;i=i-2){ //change order and convert to integer
					ti[1] = tt[i];
					ti[0] = tt[i-1];	
                                        ti[2] = '\0';
					fl[cc] = conv(ti);
					cc++;		
				}
				break;

				case 14: // $TIMETO1	
                                if( to == 1 ) {
                                    strptime( dateto, "%Y-%m-%d %H:%M:%S", &tm);
                                    totime=mktime(&tm);
                                    if( totime == -1 ) {
                                    // Error we need to do something about it
                                        printf( "%03x",(int)totime ); getchar();
                                        printf( "\n%03x", totime ); getchar();
                                        fromtime=0;
                                        printf( "bad from" ); getchar();
                                    }
                                }
                                else
                                  totime=0;
				sprintf(tt,"%03x",(int)totime); //convert to a hex in a string
				// get report time and convert
				for (i=7;i>0;i=i-2){ //change order and convert to integer
					ti[1] = tt[i];
					ti[0] = tt[i-1];	
                                        ti[2] = '\0';
					fl[cc] = conv(ti);
					cc++;		
				}
				break;

				case 15: // $TIMEFROM2	
                                if( from == 1 ) {
                                    strptime( datefrom, "%Y-%m-%d %H:%M:%S", &tm);
                                    fromtime=mktime(&tm)-86400;
                                    if( fromtime == -1 ) {
                                    // Error we need to do something about it
                                        printf( "%03x",(int)fromtime ); getchar();
                                        printf( "\n%03x", fromtime ); getchar();
                                        fromtime=0;
                                        printf( "bad from" ); getchar();
                                    }
                                }
                                else
                                {
                                  printf( "no from" ); getchar();
                                  fromtime=0;
                                }
				sprintf(tt,"%03x",(int)fromtime); //convert to a hex in a string
				for (i=7;i>0;i=i-2){ //change order and convert to integer
					ti[1] = tt[i];
					ti[0] = tt[i-1];	
                                        ti[2] = '\0';
					fl[cc] = conv(ti);
					cc++;		
				}
				break;

				case 16: // $TIMETO2	
                                if( to == 1 ) {
                                    strptime( dateto, "%Y-%m-%d %H:%M:%S", &tm);

                                    totime=mktime(&tm)-86400;
                                    if( totime == -1 ) {
                                    // Error we need to do something about it
                                        printf( "%03x",(int)totime ); getchar();
                                        printf( "\n%03x", totime ); getchar();
                                        fromtime=0;
                                        printf( "bad from" ); getchar();
                                    }
                                }
                                else
                                  totime=0;
				sprintf(tt,"%03x",(int)totime); //convert to a hex in a string
				for (i=7;i>0;i=i-2){ //change order and convert to integer
					ti[1] = tt[i];
					ti[0] = tt[i-1];	
                                        ti[2] = '\0';
					fl[cc] = conv(ti);
					cc++;		
				}
				break;
				
				case 19: // $PASSWORD
                              
				for(i=0;i<12;i++){
				    if( conf.Password[i] == '\0' )
                                  	fl[cc] = 0x88;
                                    else {
                                        pass_i = conf.Password[i];
                                        fl[cc] = (( pass_i+0x88 )%0xff);
                                    }
                                    cc++;
				}
				break;	

				case 21: // $UNKNOWN
				for (i=0;i<4;i++){
			            fl[cc] = unknown[i];
				    cc++;
				}
                                break;

				default :
				fl[cc] = conv(lineread);
				cc++;
				}

			} while (strcmp(lineread,"$END"));
			if (verbose == 1){ 
				printf( "  [%d] sending:\n",linenum);
                                printf( "    %08x: .. .. .. .. .. .. .. .. .. .. .. .. ", 0 );
                                j=12;
				for (i=0;i<cc;i++) {
                                   if( j%16== 0 )
                                      printf( "\n    %08x: ",j);
                                   printf("%02x ",fl[i]);
                                   j++;
                                }
                           printf(" cc=%d",cc);
			   printf("\n\n");
			}
			write(s,fl,cc);
		}


		if(!strcmp(lineread,"E")){		//See if line is something we need to extract
			if (verbose	== 1) printf("[%d] Extracting\n",linenum);
			cc = 0;
			do{
				lineread = strtok(NULL," ;");
				//printf( "\nselect=%d", select_str(lineread)); 
				switch(select_str(lineread)) {
                              
                                case 3: // Extract Serial of Inverter
                                serial[3]=received[78];
                                serial[2]=received[77];
                                serial[1]=received[76];
                                serial[0]=received[75];
			        if (verbose	== 1) printf( "serial=%02x:%02x:%02x:%02x\n",serial[3]&0xff,serial[2]&0xff,serial[1]&0xff,serial[0]&0xff ); 
                                break;
                                
				case 9: // extract Time from Inverter
				idate = (received[66] * 16777216 ) + (received[65] *65536 )+ (received[64] * 256) + received[63];
                                loctime = localtime(&idate);
                                day = loctime->tm_mday;
                                month = loctime->tm_mon +1;
                                year = loctime->tm_year + 1900;
                                hour = loctime->tm_hour;
                                minute = loctime->tm_min; 
                                second = loctime->tm_sec; 
				printf("Date power = %d/%d/%4d %02d:%02d:%02d\n",day, month, year, hour, minute,second);
				//currentpower = (received[72] * 256) + received[71];
				//printf("Current power = %i Watt\n",currentpower);
				break;
				case 5: // extract current power
				idate = (received[66] * 16777216 ) + (received[65] *65536 )+ (received[64] * 256) + received[63];
                                loctime = localtime(&idate);
                                day = loctime->tm_mday;
                                month = loctime->tm_mon +1;
                                year = loctime->tm_year + 1900;
                                hour = loctime->tm_hour;
                                minute = loctime->tm_min; 
                                second = loctime->tm_sec; 
				currentpower = (received[69] * 65536) + (received[68] * 256) + received[67];
				printf("%d-%02d-%02d %02d:%02d:%02d Current power = %.0f Watt\n", year, month, day, hour, minute, second, currentpower);
				break;

				case 6: // extract total energy collected today
                          
				gtotal = (received[69] * 65536) + (received[68] * 256) + received[67];
				gtotal = gtotal / 1000;
            printf("G total so far = %.2f Kwh\n",gtotal);
				dtotal = (received[84] * 256) + received[83];
				dtotal = dtotal / 1000;
            printf("E total today = %.2f Kwh\n",dtotal);
            break;		

				case 7: // extract 2nd address
				memcpy(address2,received+26,6);
				if (verbose == 1) printf("address 2 \n");
				break;
				
				case 8: // extract bluetooth channel
				memcpy(chan,received+22,1);
				if (verbose == 1) printf("Bluetooth channel = %i\n",chan[0]);
				break;

				case 12: // extract time strings $TIMESTRING
				memcpy(timestr,received+63,24);
				if (verbose == 1) printf("extracting timestring\n");
                                
				break;

				case 17: // Test data
                                i=19;
                                printf( "\n" );
                                for( j=0; j<8; j++ ) {
				    idate = (received[i+3] * 16777216 ) + (received[i+2] *65536 )+ (received[i+1] * 256) + received[i];
                                    loctime = localtime(&idate);
                                    day = loctime->tm_mday;
                                    month = loctime->tm_mon +1;
                                    year = loctime->tm_year + 1900;
                                    hour = loctime->tm_hour;
                                    minute = loctime->tm_min; 
                                    second = loctime->tm_sec; 
				    gtotal = (received[i+6] * 65536) + (received[i+5] * 256) + received[i+4];
				    gtotal = gtotal / 1000;
	                            if (verbose == 1) printf("%d/%d/%4d %02d:%02d:%02d  total=%.3f Kwh", day, month, year, hour, minute,second, gtotal);
                                    i+=12;
                                    printf( "\n" );
                                }
                                printf( "\n" );
                          
				gtotal = (received[69] * 65536) + (received[68] * 256) + received[67];
				gtotal = gtotal / 1000;
            printf("G total so far = %.2f Kwh\n",gtotal);
				dtotal = (received[84] * 256) + received[83];
				dtotal = dtotal / 1000;
				break;
				
				case 18: // $ARCHIVEDATA1
                                i=59;
                                togo=received[43]+256*received[44];
                                last=0;
                                finished=0;
                                crc_at_end=0;
                                j=0;
                                ptotal=0;
                                printf( "\n" );
                                while( finished != 1 ) {
                                    if( i> 500 ) break;
                                    //printf( "\ngetting data i=%d j=%d rr=%d", i, j, rr );
                                    if(( i < rr )&&(( crc_at_end != 1)||(i+3 < rr ))) {
                                       
                                       datarecord[j]=received[i];
                                       j++;
                                       if( j > 11 ) {
				         idate = (datarecord[3] * 16777216 ) + (datarecord[2] *65536 )+ (datarecord[1] * 256) + datarecord[0];
                                         loctime = localtime(&idate);
                                         day = loctime->tm_mday;
                                         month = loctime->tm_mon +1;
                                         year = loctime->tm_year + 1900;
                                         hour = loctime->tm_hour;
                                         minute = loctime->tm_min; 
                                         second = loctime->tm_sec; 
				         gtotal = (datarecord[6] * 65536) + (datarecord[5] * 256) + datarecord[4];
                                         if( ptotal == 0 )
                                            ptotal = gtotal;
	                                 if (verbose == 1) printf("\n%d/%d/%4d %02d:%02d:%02d  total=%.3f Kwh current=%.0f Watts togo=%d", day, month, year, hour, minute,second, gtotal/1000, (gtotal-ptotal)*12, togo);
                                         if( archdatalen == 0 )
                                            archdatalist = (struct archdata_type *)malloc( sizeof( struct archdata_type ) );
                                         else
                                            archdatalist = (struct archdata_type *)realloc( archdatalist, sizeof( struct archdata_type )*(archdatalen+1));
                                         (archdatalist+archdatalen)->date=idate;
                                         (archdatalist+archdatalen)->accum_value=gtotal/1000;
                                         (archdatalist+archdatalen)->current_value=(gtotal-ptotal)*12;
                                         archdatalen++;
                                         ptotal=gtotal;
                                         j=0; //get ready for another record
                                      }
                                      i++;
                                    }
                                    else
			            { 
                                        if( last == 1 ) {
                                           last = 0;
                                           break;
                                        }
                                        if( read_bluetooth( &s, &rr, received ) != 0 )
                                        {
                                           fseek( fp, returnpos, 0 );
                                           linenum = returnline;
                                           found=0;
                                           if( archdatalen > 0 )
                                              free( archdatalist );
                                           archdatalen=0;
                                           strcpy( lineread, "" );
                                           sleep(10);
                                           failedbluetooth++;
                                           if( failedbluetooth > 3 )
                                             exit(-1);
                                           goto start;
                                        }
                                    
					switch ( received[3] ) {
                                        
                                        case 0x13 :
                                        case 0x15 :
                                           if( crc_at_end == 1 ) {
                                              i=59;
                                              crc_at_end = 0;
                                              j=0;
                                              togo=received[43]+256*received[44];
                                           }
                                           else
                                             i=18;
                                           break;
                                             
                                        default:
                                           if( crc_at_end == 1 ) {
                                              i=59;
                                              crc_at_end = 1;
                                              j=0;
                                           }
                                           else
                                             i=18;
                                           crc_at_end=1;
                                           if( togo == 0 )
                                             last=1;
                                           break;
                                        }
			            }	
                                }
                                printf( "\n" );
                          
				break;
				case 20: // SIGNAL signal strength
                          
				strength  = (received[22] * 100.0)/0xff;
	                        if (verbose == 1) {
                                    printf("bluetooth signal  = %.0f%\n",strength);
                                }
                                break;		
				}				
                            }
				
			while (strcmp(lineread,"$END"));
		} 
		if(!strcmp(lineread,":init")){		//See if line is something we need to extract
                   initstarted=1;
                   returnpos=ftell(fp);
		   returnline = linenum;
                }
		if(!strcmp(lineread,":setup")){		//See if line is something we need to extract
                   setupstarted=1;
                   returnpos=ftell(fp);
		   returnline = linenum;
                }
		if(!strcmp(lineread,":getrangedata")){		//See if line is something we need to extract
                   rangedatastarted=1;
                   returnpos=ftell(fp);
		   returnline = linenum;
                }
	}
}

if (mysql ==1){
	/* Connect to database */
    OpenMySqlDatabase( conf.MySqlHost, conf.MySqlUser, conf.MySqlPwd, conf.MySqlDatabase );
    for( i=1; i<archdatalen; i++ ) //Start at 1 as the first record is a dummy
    {
	sprintf(SQLQUERY,"INSERT INTO DayData ( DateTime, CurrentPower, EtotalToday ) VALUES ( FROM_UNIXTIME(%ld), %0.f, %.3f ) ON DUPLICATE KEY UPDATE DateTime=Datetime, CurrentPower=VALUES(CurrentPower), EtotalToday=VALUES(EtotalToday)",(archdatalist+i)->date, (archdatalist+i)->current_value, (archdatalist+i)->accum_value );
	if (verbose == 1) printf("%s\n",SQLQUERY);
	DoQuery(SQLQUERY);
    }
    mysql_close(conn);
}

loctime = localtime(&curtime);
day = loctime->tm_mday;
month = loctime->tm_mon +1;
year = loctime->tm_year + 1900;
hour = loctime->tm_hour;
minute = loctime->tm_min; 
datapoint = (int)(((hour * 60) + minute)) / 5; 

if (post ==1){
    char *stopstring;

    float dtotal, starttotal;
    
    /* Connect to database */
    OpenMySqlDatabase( conf.MySqlHost, conf.MySqlUser, conf.MySqlPwd, conf.MySqlDatabase );
    //Get Start of day value
    sprintf(SQLQUERY,"SELECT EtotalToday FROM DayData WHERE DateTime=DATE_FORMAT( NOW(), \"%%Y%%m%%d000000\" ) " );
    if (verbose == 1) printf("%s\n",SQLQUERY);
    DoQuery(SQLQUERY);
    if (row = mysql_fetch_row(res))  //if there is a result, update the row
    {
        starttotal = atof( (char *)row[0] );

        for( i=1; i<archdatalen; i++ ) { //Start at 1 as the first record is a dummy
           if((archdatalist+i)->current_value > 0 )
           {
	      dtotal = (archdatalist+i)->accum_value*1000 - (starttotal*1000);
              idate = (archdatalist+i)->date;
	      loctime = localtime(&(archdatalist+i)->date);
              day = loctime->tm_mday;
              month = loctime->tm_mon +1;
              year = loctime->tm_year + 1900;
              hour = loctime->tm_hour;
              minute = loctime->tm_min; 
              second = loctime->tm_sec; 
	      ret=sprintf(compurl,"%s?d=%04i%02i%02i&t=%02i:%02i&v1=%f&v2=%f&key=%s&sid=%s",conf.PVOutputURL,year,month,day,hour,minute,dtotal,(archdatalist+i)->current_value,conf.PVOutputKey,conf.PVOutputSid);
              sprintf(SQLQUERY,"SELECT PVOutput FROM DayData WHERE DateTime=\"%i%02i%02i%02i%02i%02i\"  and PVOutput IS NOT NULL", year, month, day, hour, minute, second );
              if (verbose == 1) printf("%s\n",SQLQUERY);
              DoQuery(SQLQUERY);
	      if (verbose == 1) printf("url = %s\n",compurl); 
              if (row = mysql_fetch_row(res))  //if there is a result, already done
              {
	         if (verbose == 1) printf("Already Updated\n");
              }
              else
              {
                
	        curl = curl_easy_init();
	        if (curl){
		     curl_easy_setopt(curl, CURLOPT_URL, compurl);
		     curl_easy_setopt(curl, CURLOPT_FAILONERROR, compurl);
		     result = curl_easy_perform(curl);
	             if (verbose == 1) printf("result = %d\n",result);
		     curl_easy_cleanup(curl);
                     if( result==0 ) 
                     {
                        sprintf(SQLQUERY,"UPDATE DayData  set PVOutput=NOW() WHERE DateTime=\"%i%02i%02i%02i%02i%02i\"  ", year, month, day, hour, minute, second );
                        if (verbose == 1) printf("%s\n",SQLQUERY);
                        DoQuery(SQLQUERY);
                     }
                     else
                        break;
		  
	        }
             }
           }
        }
    }
    mysql_close(conn);
}

close(s);
free(archdatalist);
return 0;
}
}

int
read_bluetooth( int *s, int *rr, unsigned char *received )
{
    int bytes_read,i,j;
    unsigned char buf[1024]; //read buffer
    unsigned char header[3]; //read buffer
    struct timeval tv;
    fd_set readfds;

    tv.tv_sec = 10; // set timeout of reading to 5 seconds
    tv.tv_usec = 0;
    memset(buf,0,1024);

    FD_ZERO(&readfds);
    FD_SET((*s), &readfds);
				
    select((*s)+1, &readfds, NULL, NULL, &tv);
				
    // first read the header to get the record length
    if (FD_ISSET((*s), &readfds)){	// did we receive anything within 5 seconds
        bytes_read = recv((*s), header, sizeof(header), 0); //Get length of string
	(*rr) = 0;
        for( i=0; i<sizeof(header); i++ ) {
            received[(*rr)] = header[i];
	    if (verbose == 1) printf("%02x ", received[(*rr)]);
            (*rr)++;
        }
    }
    else
    {
       printf("Timeout reading bluetooth socket\n");
       (*rr) = 0;
       memset(received,0,1024);
       return -1;
    }
    if (FD_ISSET((*s), &readfds)){	// did we receive anything within 5 seconds
        bytes_read = recv((*s), buf, header[1]-3, 0); //Read the length specified by header
    }
    else
    {
       printf("Timeout reading bluetooth socket\n");
       (*rr) = 0;
       memset(received,0,1024);
       return -1;
    }
    if ( bytes_read > 0){
        if (verbose == 1) printf("\nReceiving\n");
	if (verbose == 1){ 
           printf( "    %08x: .. .. .. .. .. .. .. .. .. .. .. .. ", 0 );
           j=12;
           for( i=0; i<sizeof(header); i++ ) {
              if( j%16== 0 )
                 printf( "\n    %08x: ",j);
              printf("%02x ",header[i]);
              j++;
           }
	   for (i=0;i<bytes_read;i++) {
              if( j%16== 0 )
                 printf( "\n    %08x: ",j);
              printf("%02x ",buf[i]);
              j++;
           }
           printf(" rr=%d",(bytes_read+(*rr)));
	   printf("\n\n");
   }
        for (i=0;i<bytes_read;i++){ //start copy the rec buffer in to received
            if (buf[i] == 0x7d){ //did we receive the escape char
	        switch (buf[i+1]){   // act depending on the char after the escape char
					
		    case 0x5e :
	                received[(*rr)] = 0x7e;
		        break;
							   
		    case 0x5d :
		        received[(*rr)] = 0x7d;
		        break;
							
		    default :
		        received[(*rr)] = buf[i+1] ^ 0x20;
		        break;
	        }
		    i++;
	    }
	    else { 
               received[(*rr)] = buf[i];
            }
	    if (verbose == 1) printf("%02x ", received[(*rr)]);
	    (*rr)++;
	}
        fix_length_received( received, rr );
	if (verbose == 1) {
	    printf("\n");
            for( i=0;i<(*rr); i++ ) printf("%02x ", received[(i)]);
        }
	if (verbose == 1) printf("\n\n");
    }	
    return 0;
}
