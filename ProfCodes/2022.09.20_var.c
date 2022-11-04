#include <stdio.h>
#include <stdarg.h>


double sum(int,...);

int main(){
	printf("%f %f %f", sum(2,7.0,3.0),sum (7, 1.0,2.0,3.0,(double)4,5.0,6.0,7.0), sum(0));
}

double sum (int n, ...){
	double sum=0;
	va_list args;

	va_start(args,n);
	
	for (int i=0;i<n;i++)
		sum+=va_arg(args,double);	
	
	va_end(args);

	return sum;

}


