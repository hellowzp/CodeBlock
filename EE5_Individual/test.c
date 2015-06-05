#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#ifndef MAX_FRAME_SIZE
#define MAX_FRAME_SIZE 100
#endif

#define PARAMETER_ERROR(fun) do{ printf("%s\n","fun() error: invalid parameter..."); \
                                 exit(EXIT_FAILURE); \
                               }while(0)

#define FREE_STRING(string)  do{ free(string); string=NULL; } while(0)

#define FREE_STRING_ARRAY(sarray) do{ int i=0;  \
	while(*(sarray+i)) {                        \
		free(*(sarray+i));                      \
		*(sarray+i) = NULL;                     \
		i++;                                    \
	}                                           \
	free(sarray);                               \
	sarray = NULL;                              \
} while(0)
	
#define ALLOW_EMPTY_FIELD
#define QUEUE_INITIAL_CAPACITY 20
#define BUSY 1
#define IDLE 0

typedef char* String;
typedef unsigned char Byte;
typedef struct {
	String name;
	String value;
} Map, *map_ptr_t;

typedef struct {
	char* address;
	map_ptr_t payload;
} Data, *data_ptr_t;

typedef struct{
	int head;
	int tail;
	data_ptr_t queue;
} Queue, *queue_ptr_t;

int int_to_str(long num,char** str) {
	int bits = 1;
	long temp = num;
	while(num = num/10) {
		bits++;
		//num = num/10;
	}
	*str = malloc(bits+1);
	int i;
	for(i=0; i<bits; i++) {
		*(*str+bits-1-i) = temp%10 + 48;
		temp = temp/10;
	}
	*(*str+bits) = 0;
	return bits;
}

 /* convert integers to string of length specified by len
  * if integer bits is larger than len, truncate the leading bits
  * else padding zeros in the leading bits
  */
void int_to_nstr(long num,int len,char** retStr) {
	char* str = (char*)malloc(len+1);
	int bits = 0;  //valid number bits (no leading 0)
	long res = num;
	while (res) {
		bits++;
		if(bits>len) break;
		*(str+len-bits) = res%10 + 48;
		res = res/10;
	}
	if(bits<len) {
		int i;
		for(i=0;i<len-bits;i++)
			*(str+i) = 48;
	}
	*(str+len) = 0;
	//printf("%ld %d\n",str_length(str),sizeof(str)); 10 8 why?
	*retStr = str;
}

int str_length( char* str) {
	if(!str) return -1;
	int len = 0;
	while(*(str+len)) len++;
	return len;
}

long power(char a, unsigned char b) {
	unsigned char i;
	long pow = 1;
	if(a>=48) a-=48;  //like if a='1' instead of a=1
	for(i=0; i<b; i++) {
		pow *= a;
	}
	return pow;
}

/*
void FREE_STRING_ARRAY(char*** sarray) {
    if(!sarray) {
        PARAMETER_ERROR(FREE_STRING_ARRAY);
    }
    if(*sarray) {
        int i = 0;
        while(**(sarray+i)) {
            printf("%s %s\n",**(sarray+i+1),"ok");
            free(**(sarray+i));
            printf("%d\n",i);
            **(sarray+i) = NULL;
            i++;
        }
        free(*sarray);
        *sarray = NULL;
    }
}
*/

long str_to_int( char* str) {
	if(!str) {
		printf("%s\n","str_to_int(): invalid parameter...");
		return -1;
	}
	unsigned char bits = (unsigned char)str_length(str);
	printf("%d bits.\n",(int)bits);
	if(bits>10) {
		printf("%s\n","str_to_int(): number string too long...");
		exit(-2);
	}
	long ret = 0;
	unsigned char i;
	for(i=0; i<bits; i++) {
		char c;
		if(*(str+i)>47) c = *(str+i) - 48;  //*(str+i)='1' ==>49
		else c = *(str+i);
		ret += c * power(10,bits-1-i);
		printf("%d %d\n",(int)c,(int)(bits-1-i));
	}
	return ret;
}

//concatenate two strings, reject NULL but accept empty string
int str_cat(char* strl, char* strr,  char del, char** str) {
	if(!str || !strl || !strr) PARAMETER_ERROR(str_cat);
	int ll = str_length(strl);
	int lr = str_length(strr);
	char* ret = (char*)malloc(ll+lr+3);
	int i;
	for(i=0;i<ll;i++)
		*(ret+i) = *(strl+i);
	*(ret+ll) = del;
	for(i=0;i<lr;i++)
		*(ret+ll+1+i) = *(strr+i);
	*(ret+ll+lr+1)=del;
	*(ret+ll+lr+2)=0;
	*str = ret;
	return (ll+lr+2);
}

int str_append(char* des,char* src,int n) {
	if(!des || !src) PARAMETER_ERROR(str_append);
	int dlen = str_length(des);
	int slen = str_length(src);
	if(n>slen) n = slen;
	des = (char*)realloc(des,dlen+n+1);
	int i;
	for(i=0; i<n; i++) {
		*(des+dlen+i) = *(src+i);
	}
	*(des+dlen+n) = 0;
	return (dlen+n);
}

void str_free(char* str) {
	printf("%p\n",str);
	free(str);
	str = NULL;  //actually this doesn't change the actual argument
}

//concatenate variable length of strings(sensor field)
//if allow empty field, treat NULL field as empty and also add it
//else exit with error
char* str_multi_cat(int num,  char del, ...) {
	char* str = (char*)malloc(MAX_FRAME_SIZE);
    va_list argmt;
    va_start(argmt,del);
    int i,j,index=0;
    for(i=0; i<num; i++) {
        char* sf = (char *)va_arg(argmt,char*);
        int len = str_length(sf);
        if(len<=0) {  //empty(0) or NULL(-1)
            //first complete the current loop then point to the new position
            // cuz each time index points to the beginning position of the loop
            #ifdef ALLOW_EMPTY_FIELD
            *(str+index) = del;
            index++;
            #else
            printf("%s\n","str_multi_cat(): empty sensor field allowed.");
            exit(EXIT_FAILURE);
            #endif
        } else {
			for(j=0; j<len; j++) {
				*(str+index+j) = *(sf+j);
			}
			*(str+index+len) = del;
			index += (len+1);
        }
	}
    va_end(argmt);
    *(str+index) = 0;
    return str;
}

//get sub-string including the chars both at beginning and the ending index
char* str_get_between_index( char* str, int bn, int en) {
	if(!str || bn<0 || bn>en) {
		printf("%s\n","str_get_between_index(): invalid parameter...");
		return NULL;
	}
	int len = str_length(str);
	if(en>=len) return NULL;
	char* data = (char*)malloc(en-bn+2);
	int i;
	for(i=bn; i<=en; i++) {
		*(data+i-bn) = *(str+i);
	}
	*(data+en-bn+1) = 0;
	return data;
}

char* tty_get_data_at_del( char* pkt, int n,  char del) {
	if(!pkt || n<=0) {
		printf("%s\n","tty_get_data_index_at_del(): invalid parameter...");
		return NULL;
	}
	int i,j,cnt = 0;
	int len = str_length(pkt);
	for(j=0; j<len; j++) {
		if(*(pkt+j)==del) {
			cnt++;
			if(cnt==n-1) i=j;
			if(cnt==n) break;
		}
	}
	if(n==1) return str_get_between_index(pkt, 0, j-1); //char at j is the del, remove it
	if(i+1<j-1) return str_get_between_index(pkt, i+1, j-1);
	else if(i+1==j-1) {    //between the 2 dels contains only one char
		char* c = (char*)malloc(2);
		*c = *(pkt+i+1);
		*(c+1) = 0;
		return c;
	} else {               //the 2 dels are next to each other
		char* c = (char*)malloc(1);
		*c = 0;
		return c;
	}
}

//the returned string still contains the delimeter in order to be further unpacked
//if bn=0, get all the string before the del specified by en
char* tty_get_data_between_del( char* pkt, int bn, int en,  char del) {
	if(!pkt || bn<0 || en<=bn+1) {
		printf("%s\n","tty_get_data_between_del(): invalid parameter...");
		return NULL;
	}
	int i,j,cnt = 0;
	int len = str_length(pkt);
	for(j=0; j<len; j++) {
		if(*(pkt+j)==del) {
			cnt++;
			if(cnt==bn) i=j;
			if(cnt==en) break;
		}
	}
	if(bn==0) return str_get_between_index(pkt, 0, j);  //get before the second
	return str_get_between_index(pkt, i+1, j);
}

int str_split( char* str, char del,char*** retArray) {
	if(!str) {
		printf("%s\n","str_split(): invalid parameter...");
		return -1;
	}
	int len = str_length(str);
	int i,j=0;
	int index[len];  //save the index of delimiters
	for(i=0; i<len; i++) {
		if(*(str+i)==del) {
			index[j]=i;
			j++;
		}
	}

	char** str_array = NULL;
	int size = (j+1)*sizeof(*str_array);  //one extra space for indicating the end in order to free later
	str_array = (char**)malloc(size);
	//printf("%d %d\n",j,size);
	for(i=0; i<j; i++) {  //fill the array row by row
		int k;
		if(i==0) {
			*(str_array+i) = (char*)malloc(index[i]+1);
			for(k=0; k<index[i]; k++) {
				*(*(str_array+i)+k) = *(str+k);
			}
			*(*(str_array+i)+k) = 0; //terminator \0
			//printf("%s\n",*(str_array+i));
		} else {
			*(str_array+i) = (char*)malloc(index[i]-index[i-1]);
			for(k=0; k<index[i]-index[i-1]-1; k++) {
				*(*(str_array+i)+k) = *(str + index[i-1] + k +1);
			}
			*(*(str_array+i)+k) = 0; //terminator \0
			//printf("%s\n",*(str_array+i));
		}
	}
	*(str_array+j) = 0;
	*retArray = str_array;
	return j;
}

char** str_nsplit( char* str,int num, char del) {
    if(!str || num<=0) {
		printf("%s\n","str_split(): invalid parameter...");
		return NULL;
	}
	int len = str_length(str);
	int i,j=0;
	int index[num+1];  //save the index of delimiters
	for(i=0; i<len; i++) {
		if(*(str+i)==del) {
			index[j]=i;
			j++;
		}
	}
	if(j!=num) {
        printf("%s\n","str_split(): parameters does not match...");
		return NULL;
	}

	char** str_array = NULL;
	str_array = (char**)malloc(j*sizeof(*str_array));
	for(i=0; i<j; i++) {  //fill the array row by row
		int k;
		if(i==0) {
			*(str_array+i) = (char*)malloc(index[i]+1);
			for(k=0; k<index[i]; k++) {
				*(*(str_array+i)+k) = *(str+k);
			}
			*(*(str_array+i)+k) = 0; //terminator \0
		} else {
			*(str_array+i) = (char*)malloc(index[i]-index[i-1]);
			for(k=0; k<index[i]-index[i-1]-1; k++) {
				*(*(str_array+i)+k) = *(str + index[i-1] + k +1);
			}
			*(*(str_array+i)+k) = 0; //terminator \0
		}
	}
	*(str_array+j) = 0;
	return str_array;
}

map_ptr_t str_to_map( char* str, char fdel, char sdel) {
    char** payload = NULL;
    int fields = str_split(str,fdel,&payload);  //frame delimiter
    //printf("%p %s\n",*(payload),*(payload+1));
    if(fields<=0) {
        printf("%s\n","str_to_map(): invalid parameter...");
		return NULL;
    }
    map_ptr_t map = (map_ptr_t)malloc((fields+1)* sizeof(Map));
    int i=0;
    int j=0;
    for(i=0; i<fields; i++) {
        //printf("%d %p %s\n",i,*(payload),*(payload+i));
        int len = str_length(*(payload+i));
        for(j=0; j<len; j++) {
			if(*(*(payload+i)+j) == sdel) {
				(map+i)->name = str_get_between_index(*(payload+i),0,j-1);
				(map+i)->value = str_get_between_index(*(payload+i),j+1,len-1);
				break;
			}
		}
	}
    FREE_STRING_ARRAY(payload);
    (map+fields)->name = NULL;  //in oder to calculate the size
    return map;
}

int map_to_str(map_ptr_t map,  char del, char** str) {
	if(!map || !str) PARAMETER_ERROR(map_to_str);
	int nlen = str_length(map->name);
	int vlen = str_length(map->value);
	*str = (char*)malloc(nlen+vlen+2);
	int i;
	for(i=0; i<nlen; i++) {
		*(*str+i) = *(map->name+i);
	}
	*(*str+nlen) = del;
	for(i=0; i<vlen; i++) {
		*(*str+nlen+1+i) = *(map->value+i);
	}
	*(*str+nlen+vlen+1) = 0;
	return (nlen+vlen+1);
}

int map_cat(map_ptr_t map,  char fdel,  char sdel, char** str) {
	if(!map) PARAMETER_ERROR(map_cat);
	/*
	int fields = map_get_size(map);
	char** sensors = (char**)malloc((fields+1)*sizeof(char*));
	int i,len=0;
	for(i=0; i<fields; i++) {
		len += map_to_str(map+i,sdel,&(*(sensor+i)));
	}
	*(sensors+fields) = NULL;
	*str = (char*)str_multi_cat(fields,);
	*/
	char* str_temp = (char*)calloc(MAX_FRAME_SIZE,1); //initialize the memory block so str_length() returns 0
	int i = 0;
	int len = 0;
    while(((map+i)->name)) {
        char* sensor = NULL;
        //len += str_cat((map+i)->name,(map+i)->value,sdel,&sensor); str_cat() will add sdel in the end!!
        len += map_to_str(&map[i],sdel,&sensor);
        strncat(str_temp,sensor,len);   //will not change len 
        *(str_temp+len+i) = fdel;
        str_free(sensor);
        i++;
    }
    if(len+i != str_length(str_temp)) {
		printf("%d %d %s %s\n",len+i,str_length(str_temp),str_temp,"algorithm error in map_cat()...");
		exit(EXIT_FAILURE);
	}
	printf("%s\n",str_temp);
    *str = str_get_between_index(str_temp,0,len+i-1);
    str_free(str_temp);
	return (len+i);
}

/*
void map_init(map_ptr_t* mptr,int fields) {
    if(!mptr) {
        printf("%s\n","map_init(): invalid parameter...");
		return NULL;
    }
    *mptr = (map_ptr_t)malloc((fields+1)* sizeof(Map));
    int i;
    for(i=0; i<fields; i++) {
        (mptr+i)->name = NULL;
        (mptr+i)->value = NULL;
    }
    (mptr+fields)->name = NULL;   //in oder to calculate the size
}
*/	

int map_get_size(map_ptr_t map) {
    if(!map) PARAMETER_ERROR(map_get_size);
    int i = 0;
    while(!((map+i)->name)) {
        i++;
    }
    return i;
}

char* map_get_value(map_ptr_t map,char* name) {
	if(!map || !name) PARAMETER_ERROR(map_get_value);
    int i = 0;
    while(!((map+i)->name)) {
        if(strcmp((map+i)->name,name)==0)
			return (map+i)->value;
        i++;
    } 
    printf("%s\n","the map does not contain the iven name..");
    return NULL;
}

void map_destroy(map_ptr_t map) {
	if(!map) PARAMETER_ERROR(map_destroy);
    int i = 0;
    while(!((map+i)->name)) {
        free((map+i)->name);
        (map+i)->name = NULL;
        free((map+i)->value);
        (map+i)->value = NULL;
        i++;
    } 
    free(map);
    map = NULL;
}

void data_destroy(data_ptr_t dptr) {
	if(!dptr) PARAMETER_ERROR(data_destroy);
	free(dptr->address);
	dptr->address = NULL;
	free(dptr->payload);
	dptr->payload = NULL;
	free(dptr);
	dptr = NULL;
}

int tty_serial_read(int fds, char* buf) {
	int bytes = read(fds,buf,MAX_FRAME_SIZE);
	if(bytes<0) {
		
	}
	return bytes;
}

void tty_pkt_get_data(char * pkt, data_ptr_t* data) {
	char* address = tty_get_data_at_del(pkt, 2, '#');
	
	//FIRST GET THE PAYLOAD STRING and then convert it to map format
	char* payload_str = NULL;
	map_ptr_t payload_map = NULL;
	char fields = *(pkt+4); //the fifth char, expressed as 0x03(char with asc value 3), so no need to convert (from asc) to number
	payload_str = tty_get_data_between_del(pkt,4,(int)fields+4,'#');
	payload_map = str_to_map(payload_str,'#',':');
	*data = (data_ptr_t)malloc(sizeof(Data));
	(*data)->address = address;
	(*data)->payload = payload_map;
	free(payload_str);
}	

int tty_pkt_assemble(Byte frame_type, Byte fields, char* serial_id,Byte seq,map_ptr_t payload,char** pkt) {
	if(!payload || !pkt) PARAMETER_ERROR(tty_pkt_assemble);
	char* seq_str = NULL;
	char* pld_str = NULL;
	int_to_str((long)seq,&seq_str);
	map_cat(payload,'#',':',&pld_str);
	char* start = malloc(6); //first 5 bytes
	*start = 60;
	*(start+1) = 61;
	*(start+2) = 62;
	*(start+3) = (char)frame_type;
	*(start+4) = (char)fields;
	*(start+5) = 0;
	printf("%s\n",start);
	
	*pkt = str_multi_cat(5,'#',start,serial_id,"",seq_str,pld_str);  //will end with double fdels
	int len = str_length(*pkt);
	*(*pkt+len-1) = 0;	
	str_free(start);
	str_free(seq_str);
	str_free(pld_str);
	return str_length(*pkt);
}	

int main() {

	char** sarray;
	char* st = "123K456K78K";
	str_split(st,'K',&sarray);
	FREE_STRING_ARRAY(sarray);
	
	char* str = "Temp:35#GPS:31.200;42.100#DATE:12-01-01#";
	map_ptr_t map = str_to_map(str,'#',':');
	printf("%d %d\n",str_length(st),str_length(str));
	
	char* pkt = NULL;
	int len = tty_pkt_assemble(126,3,"0012345678",10,map,&pkt);
	printf("%d %d %s\n",str_length(str),len,pkt);
	
	data_ptr_t dptr = NULL;
	tty_pkt_get_data(pkt,&dptr);
	printf("%s\n",dptr->address);
	
	free(pkt);	
	map_destroy(map);
	data_destroy(dptr);
	
	return 0;
}
