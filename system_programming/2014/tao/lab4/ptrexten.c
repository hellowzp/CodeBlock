#include<stdio.h>
#include<stdlib.h>
#include<malloc.h>
#include<string.h>
#include<ctype.h>
	
		
	    int main(int argc,char *argv[]){
		const int a=10;
		typedef unsigned char Mem;
		int i,j=0,Num,FullRow;
		int flag1=0,flag2=0;
		//Mem *Q=(Mem*)malloc(2*sizeof(Q));
        Mem *Q;
 	    //begin
 	    printf("FIY the address of the main function is %p\n", &main);
	    printf("FIY the address of the first address in main() is %p\n", &a);
 	    if(argc>1){
			
			Q=(Mem*)argv[1];
			Num=atoi(argv[2]);
		}
		else if(argc==1){
				 
		//waiting for input
		printf("Enter the start address<hex> of dump:\n");
	    scanf("%p",&Q);
		
		//waiting for input
		printf("Enter the number of bytes to dump: \n");
	    scanf("%d",&Num);
      	
        }

        flag1= Num/abs(Num);
	    FullRow = abs(Num)/10;
	    flag2 = abs(Num)%10;

    	printf("Address          Bytes                            Chars\n");
	    printf("--------         ----------------------------     -------\n");
        
        
        //begin to process
	for (i = 0; i < FullRow; i++)
		{
			
			printf("%p\t",Q);
			for (j = 0; j < a; j++)
			{
				if (*(Q + flag1*j) <= 15)
				printf("0");
				printf("%x ", *(Q + flag1*j));
			}				
			printf("\t");
			for (j = 0; j < a; j++)
			{
				if (isprint(*(Q + flag1*j)))
				{
					printf("%c", *(Q + flag1*j));
				}
				else
				{
					printf(".");
				}
			}
			printf("\n");
			Q+=flag1*10;
		}
	if (flag2 != 0)
	{

		printf("%p\t",Q);
		for(i = 0; i < a; i++)
		{
			if (i < flag2)
			{
				if (*(Q + flag1*j) <= 15)  //if Q+flag1*j<=F
				printf("0");
				printf("%x ", *(Q + flag2*j));
 			}
			else 
			{
				printf("   ");
			}
		}
		printf("\t");
		for(i = 0; i < a; i++)
		{
			if (i < flag2)
			{
				if (isprint(*(Q + flag1*j)))
				{
					printf("%c", *(Q + flag1*j));
				}
				else
				{
					printf(".");
				}
			}
			else
			{
				printf(" ");
			}
		}
		printf("\n");
	}
	//free(Q);
	return 0;
}
