//在线程函数里，做个循环接受 

int udpRcv( int inSocket, 
int inMyPort, 
struct timeval *rcvTimeOut, 
) 
{ 
struct sockaddr_in aMySockaddr; 
FD_SET( inSocket, &rfds ); 
aMySockLen = sizeof( aMySockaddr ); 

bzero( &aMySockaddr, aMySockLen ); 

aMySockaddr.sin_family = AF_INET; 
aMySockaddr.sin_port = htons( inMyPort ); 
aMySockaddr.sin_addr.s_addr= htonl( INADDR_ANY ); 
bzero( &(aMySockaddr.sin_zero), 8 ); 
aAddrLen = sizeof( struct sockaddr ); 

aSubRet = select( (inSocket + 1), &rfds, NULL, NULL, rcvTimeOut ); 
if( aSubRet > 0 ) 
{ 
aRecvSize = recvfrom ( inSocket, 接受BUF, 大小, (INT)MSG_PEEK, (struct sockaddr *)&aMySockaddr, (socklen_t*)&aAddrLen ); 

} 
