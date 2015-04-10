
//Lecture March30
//UDP SERver

#include <sys/socket.h>
#include <stdint.h>

#define OPCODE_OFFSET 0
#define BLOCK_OFFSET 2
#define MESSAGE_OFFSET 6
uin16_t
uin8_t
uin32_t

int main()
struct wro_packet
{
   unint16_t opcode;
   unint32_t block;
   char msg[100];

}

struct hostent he=gethostbyname("localhost");


struct  sockaddr_in destaddr;
	bzero(&dest,sizeof(dest));
	dest.sin_family=PF_INET;
	dest.sin_addr.s_addr=
	dest.sin_port=htons(10000):
//
//CLIENT
///

// socket
	int sockfd=socket(PF_INET,SOCK_DGRAM,0);

	
// sendto
	char msg[]="HELLO UDP WORLD\n";
	char recmsg[100];
	char packet[100];
	sendto(sockfd,msg,sizeof(msg),0,(const struct sockaddr*) &dest,sizeof(dest));	

	packet[0]=htons[3];/// wrong because 2 byte value chqanged to a 1 byte value
        //     BYTE 1    BYTE2		   
	//3 ->00000000 00000011
	// Packet structure
	// ONLY SINGLE byte vbalues so byte 1 of 3 is gone and byte2 is placed in packet[0]
	// ^ BADDD     ^

	// using short packet[100] then packet[0]= all 16 bits
	// short at least 16 bits



 
// recvfrom
	recvfrm(sockfd, recmsg,sizeof(recmsg),0,(const struct sockadr*) dest,sizeof(dest));




//printf


//--------------------APRIL 2---------------------//
// Puts things into a packet

// pack function
// then sendto
uint_t packet[100];


struct wro_packet p;
	p.opcode=htons(3);
	p.block=htonl(3000000000);
	strcopy(p.msg,p);




void pack_wro_packet(uint8_t* packet,short opcode)
{
	*((uin16_t *)packet+OPCODE_OFFSET)*=htons(opcode);
	*((uin32_t *)(packet+BLOCK_OFFSET))*=htonl(300000);
	strcopy( *((char *)(packet +MESSAGE_OFFSET)), msg);

/*
*((char *)(packet +MESSAGE_OFFSET)) *=msg[0];
*((char *)(packet +MESSAGE_OFFSET)) *=msg[1];
*((char *)(packet +MESSAGE_OFFSET)) *=msg[2];


*/
}

