struct packet 
{
	unsigned char id_b1:8, id_b2:4,bits6_b1:6,value_b1:8,value_b2:5,check_b1:1; 
};
typedef struct packet outpacket;
extern unsigned char parity(outpacket *ino);
