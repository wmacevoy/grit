//##########################################################
//##                      R O B O T I S                   ##
//##          ReadWrite Example code for Dynamixel.       ##
//##                                           2009.11.10 ##
//##########################################################
#include <stdio.h>
#include <termio.h>
#include <unistd.h>
#include <dynamixel.h>

// Defulat setting
#define ID_1		1
#define ID_2            2
#define DXL_CCW_ANGLE 8
#define DXL_CW_ANGLE 6
#define DXL_MOVING_SPEED 32
#define DXL_PRESENT_POSITION 36
#define DXL_PRESENT_LOAD 40

void PrintCommStatus(int CommStatus);
void PrintErrorCode(void);

int main()
{
	int baudnum = 34;
	int deviceIndex = 0;
	int PresentPos;
	int CommStatus;

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
        dxl_write_word(ID_1,DXL_CCW_ANGLE,0);
        dxl_write_word(ID_1,DXL_CW_ANGLE,0);
        dxl_write_word(ID_1,DXL_MOVING_SPEED,512);
        dxl_write_word(ID_2,DXL_CCW_ANGLE,0);
        dxl_write_word(ID_2,DXL_CW_ANGLE,0);
        dxl_write_word(ID_2,DXL_MOVING_SPEED,512+1024);
	while(1)
	{
		printf( "Press Enter key to continue!(press ESC and Enter to quit)\n" );
		if(getchar() == 0x1b)
			break;
                int i;
                for (i=0;i<100;i++) {
                  int p=dxl_read_word(ID_1,DXL_PRESENT_POSITION);
                  int l=dxl_read_word(ID_2,DXL_PRESENT_LOAD);
                  printf("S=%4i,L=%4i\n",p,l);
                }
                
	}

	// Close device
        dxl_write_word(ID_1,DXL_MOVING_SPEED,0);
        dxl_write_word(ID_2,DXL_MOVING_SPEED,0);
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
