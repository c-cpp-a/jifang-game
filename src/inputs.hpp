#ifndef _INPUTS_HEADER_
#define _INPUTS_HEADER_
#include<conio.h>
#undef getch
int getch(){
	return _getch();
}
#endif
