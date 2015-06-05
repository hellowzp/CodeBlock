#include <stdio.h>
#include<ctype.h>
int main()
{
    int a='1',i;  
 for(i=0;i<40;i++)    
{  
printf("%c:%s\n",a,isprint(a)?"yes":"no");
/*a=0x7f;
printf("%c:%s\n",a,isprint(a)?"yes":"no");*/
//getchar();   
 printf("%c: hex %x,dec %d,oct %o\n",a,a,a,a);   
     a++;    }
return 1;
}
