 #ifndef QUEUE_H_
 #define QUEUE_H_


enum errCode { ERR_NONE = 0, ERR_EMPTY, ERR_FULL, ERR_MEM, ERR_INIT, ERR_UNDEFINED };

typedef enum errCode ErrCode;

typedef long int Element;


ErrCode Init( void );
ErrCode Enqueue( Element element );
ErrCode Dequeue(void) ;
int QueueTop(void);
int QueueSize(void);
void QueueDestroy(void);

#endif /*QUEUE_H_*/
