
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	short day, month;
	unsigned year;
} Date;

void f( void ) {
	double x = 1, y=12, z=3;
	printf("%e %e %e\n", x, y, z );
}

Date* DateStruct( int day, int month, int year ) {
	Date dummy;
	dummy.day = (short)day;
	dummy.month = (short)month;
	dummy.year = (unsigned)year;
	return &dummy;
}

int main( void ) {
	int day, month, year;
	Date *d;
        //printf("%p",d);
	printf("\nGive day, month, year:");
	scanf("%d %d %d", &day, &month, &year);
	d = DateStruct( day, month, year);  
        f();
	printf("\ndate struct values: %d-%d-%d ", d->day, d->month, d->year);

	return 0;
}
