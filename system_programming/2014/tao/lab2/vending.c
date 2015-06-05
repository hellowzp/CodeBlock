#include<stdio.h>
#include<stdlib.h>
#include<string.h>
int main(){
    int Coin;
    int Mon=0;
    char Slt;
    char *Sell;
    int flag=0;
    int backm=0;
    char Value;
        printf("Waiting for input...");
        while(Mon<300){
           scanf("%d %c",&Coin,&Value);
           switch(Value){
           	case 'C':Coin=1*Coin;printf("Transfering...\n");break;
           	case 'c':Coin=1*Coin;printf("Transfering...\n");break;
           	case 'E':Coin=100*Coin;printf("Transfering...\n");break;
           	case 'e':Coin=100*Coin;printf("Transfering...\n");break;
           	default:break;
           }
           if(Coin==200){
           	printf("The coin is refused!\n");
           }
           else {
            Mon+=Coin;
            printf("Current Money is %d C\n",Mon);
            while(Mon<80){
            printf("waiting for more...");
            scanf("%d %c",&Coin,&Value);
            Mon+=Coin;
            printf("Current Money is %d C\n",Mon);
            }
            if(Mon>=80){
            printf("Please select your choice:\n");
            while(Mon>=80&&Mon<100){
                  scanf("%c",&Slt);
                  Sell=&Slt;
                    switch(*Sell){
                     case 'c':printf("You have choosen coca-cola!\n");
					          backm=Mon-80;flag=1;
			    	          printf("Return money is %d C\n",backm);
                             if(flag==1){
                                       exit(0);
                                      }break;
                     case 'a':printf("You don't have enough money!\n");
                              backm=Mon;flag=1;
        	                  printf("Return money is %d C\n",backm);
                             if(flag==1){
                                       exit(0);
                                      }break;
                     case 'o':printf("You don't have enough money!\n");
                              backm=Mon;flag=1;
						      printf("Return money is %d C\n",backm);
                             if(flag==1){
                                       exit(0);
                                      }break;
                     case 'w':printf("You have choosen water!\n");
                              backm=Mon-65;flag=1;
						      printf("Return money is %d C\n",backm);
                             if(flag==1){
                                       exit(0);
                                      }break;
                     case 'l':printf("You have choosen lemonade!\n");
                              backm=Mon-80;flag=1;
						      printf("Return money is %d C\n",backm);
                             if(flag==1){
                                       exit(0);
                                      }break;
                      default:   break;
                      getchar();
                    }
                }
             while(Mon>=100){
                  scanf("%c",&Slt);
                  Sell=&Slt;
                    switch(*Sell){
                     case 'c':printf("You have choosen coca-cola!\n");
                              backm=Mon-80;flag=1;
					          printf("Return money is %d C\n",backm);
                             if(flag==1){
                                       exit(0);
                                      }break;
                     case 'a':printf("You have choosen aquarius!\n");
                             backm=Mon-100;flag=1;
							 printf("Return money is %d C\n",backm);
                             if(flag==1){
                                       exit(0);
                                      }break;
                     case 'o':printf("You have choosen orange-juice!\n");
                              backm=Mon-100;flag=1;
					           printf("Return money is %d C\n",backm);
                             if(flag==1){
                                       exit(0);
                                      }break;
                     case 'w':printf("You have choosen water!\n");
                              backm=Mon-65;flag=1;
					          printf("Return money is %d C\n",backm);
                             if(flag==1){
                                       exit(0);
                                      }break;
                     case 'l':printf("You have choosen lemonade!\n");
                              backm=Mon-80;flag=1;
					          printf("Return money is %d C\n",backm);
                             if(flag==1){
                                       exit(0);
                                      }break;
                      default:   break;
                      getchar();
                           
                                 }      
                          }
            }
           }
                  }
         return 0; 
      }

