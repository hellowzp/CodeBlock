#include <stdio.h>
#include <stdlib.h>

typedef struct {
short day, month;
unsigned year;
} Date;
void DateStruct( int day, int month, int year, Date *date) {
Date dummy;
dummy.day = (short)day;
dummy.month = (short)month;
dummy.year = (unsigned)year;
date = &dummy;
}
int main( void ) {
int day, month, year;
Date d;
printf("\nGive day, month, year:");
scanf("%d %d %d", &day, &month, &year);
DateStruct( day, month, year, &d);  //first assign the address of d to date
printf("\ndate struct values: %d-%d-%d", d.day, d.month, d.year);
//d is not initialized ever since, random number will be printed
return 0;
}
