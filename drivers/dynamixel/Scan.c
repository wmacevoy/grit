//##########################################################
//##                      R O B O T I S                   ##
//##          ReadWrite Example code for Dynamixel.       ##
//##                                           2009.11.10 ##
//##########################################################
#include <stdio.h>
#include <termio.h>
#include <unistd.h>
#include <dynamixel.h>

// Control table address
#define P_CW_ANGLE_LIMIT    6
#define P_CCW_ANGLE_LIMIT   8
#define P_MOVING_SPEED      32
#define P_GOAL_POSITION	    30
#define P_PRESENT_POSITION	36
#define P_MOVING		    46

// number of values that are within the jitter of the servo
#define JITTER 10

// Default setting
#define DEFAULT_BAUDNUM		1 // 1Mbps
#define DEFAULT_ID		1

void PrintCommStatus(int CommStatus);
void PrintErrorCode(void);

//int joint=0;
//int wheel=0;
int result;

void wheelMode(int id,int speed) {
//  if (!wheel) {
    dxl_write_word(id,P_CCW_ANGLE_LIMIT,0);
    int result = dxl_get_result( );
    if( result == COMM_TXSUCCESS ) printf("CCW Good\n");
    dxl_write_word(id,P_CW_ANGLE_LIMIT,0);
    if( result == COMM_TXSUCCESS ) printf("CW Good\n");
//    wheel=1;
//    joint=0;
//  }
  dxl_write_word(id,P_MOVING_SPEED,speed);
  if( result == COMM_TXSUCCESS ) printf("Speed Good\n");
}


void jointMode(int id,int angle) {
  int pos=((180+angle)*4095)/360;
//  if (!joint) {
	dxl_write_word(id,P_CCW_ANGLE_LIMIT,4095);
    int result = dxl_get_result( );
    if( result == COMM_TXSUCCESS ) printf("CCW Good\n");
    dxl_write_word(id,P_CW_ANGLE_LIMIT,0);
    if( result == COMM_TXSUCCESS ) printf("CW Good\n");
//    joint=1;
//    wheel=0;
//  }
  printf("Joint %d is at %d\n",id,pos);
  dxl_write_word(id,P_GOAL_POSITION,pos);
  if( result == COMM_TXSUCCESS ) printf("Position Good\n");
}

void servoGoto(int id,int angle) {
	// Can handle angles > -180 to +180
	int cw=0;
	int ccw=0;
	while (angle>180) { angle-=360; cw++;}
	while (angle<-180) {angle+=360; ccw++;}
	printf("Final angle %d\n",angle);
	printf("Clockwise revolutions %d\n",cw);
	printf("Counterclockwise revolutions %d\n",ccw);
	int oldValue=dxl_read_byte(id,P_PRESENT_POSITION);
	while (cw>0) {
	  wheelMode(id,1023); 
	  int newValue=dxl_read_byte(id,P_PRESENT_POSITION);
      if ((newValue-oldValue)<-2*JITTER) { 
        cw--;
      }
 	}
	while (ccw>0) {
	  wheelMode(id,-1023); 
	  int newValue=dxl_read_byte(id,P_PRESENT_POSITION);
      if ((oldValue-newValue)<-2*JITTER) { 
        ccw--;
      }
	}
	jointMode(id,angle);
}

int main()
{
	int baudnum = 34;
	int deviceIndex = 0;

	printf( "\n\nRead/Write example for Linux\n\n" );
	///////// Open USB2Dynamixel ////////////
	if( dxl_initialize(deviceIndex, baudnum) == 0 )
	{
		printf( "Failed to open USB2Dynamixel!\n" );
		printf( "Press Enter key to terminate...\n" );
		getchar();
		return 0;
	}
	else
		printf( "Succeed to open USB2Dynamixel!\n" );
	while(1)
	{
		printf( "Press Enter key to continue!(press ESC and Enter to quit)\n" );
		if(getchar() == 0x1b)
			break;
		  jointMode(4,180);
		  jointMode(5,180);

/*                int i;
                for (i=4;i<=5;i++)  {
                  dxl_ping(i);
                  printf("%i",i);
                  int result = dxl_get_result( );
                  if( result == COMM_TXSUCCESS )
		  {
                    printf("TX+");
		  }
		  else if( result == COMM_RXSUCCESS )
		  {
                    printf("RX+");
                    int j;
                    for (j=0;j<=8;j++) {
                      int value;
                      result=dxl_get_result( );
                      if (result == COMM_RXSUCCESS) {
                        value=dxl_read_byte(i,j);
                        printf("%i->%i\n",j,value);
                      } else 
                        printf("Error reading byte\n");
                    }
		  }
		  else if( result == COMM_TXFAIL )
		  {
                    printf("TX-");
		  }
		  else if( result == COMM_RXFAIL)
		  {
                    printf("RX-");
		  }
		  else if( result == COMM_TXERROR )
		  {
                    printf("TX-");
		  }
		  else if( result == COMM_RXWAITING )
		  {
                    printf("W?");
		  }
                  printf("\n");
                } */
	}

	// Close device
	dxl_terminate();
	printf( "Press Enter key to terminate...\n" );
	getchar();
	return 0;
}
// Print communication result
void PrintCommStatus(int CommStatus)
{
	switch(CommStatus)
	{
	case COMM_TXFAIL:
		printf("COMM_TXFAIL: Failed transmit instruction packet!\n");
		break;

	case COMM_TXERROR:
		printf("COMM_TXERROR: Incorrect instruction packet!\n");
		break;

	case COMM_RXFAIL:
		printf("COMM_RXFAIL: Failed get status packet from device!\n");
		break;

	case COMM_RXWAITING:
		printf("COMM_RXWAITING: Now recieving status packet!\n");
		break;

	case COMM_RXTIMEOUT:
		printf("COMM_RXTIMEOUT: There is no status packet!\n");
		break;

	case COMM_RXCORRUPT:
		printf("COMM_RXCORRUPT: Incorrect status packet!\n");
		break;

	default:
		printf("This is unknown error code!\n");
		break;
	}
}

// Print error bit of status packet
void PrintErrorCode()
{
	if(dxl_get_rxpacket_error(ERRBIT_VOLTAGE) == 1)
		printf("Input voltage error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_ANGLE) == 1)
		printf("Angle limit error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_OVERHEAT) == 1)
		printf("Overheat error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_RANGE) == 1)
		printf("Out of range error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_CHECKSUM) == 1)
		printf("Checksum error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_OVERLOAD) == 1)
		printf("Overload error!\n");

	if(dxl_get_rxpacket_error(ERRBIT_INSTRUCTION) == 1)
		printf("Instruction code error!\n");
}
