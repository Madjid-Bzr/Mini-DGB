#include <stdio.h>

void boo();
void foo();


void boo(){

	foo();
}

void foo(){
	
	boo();
}

int main()
{
	foo();	
	return 0;
}
