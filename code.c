#include <ctime>

void waitforseconds(int second) //延迟函数
{
	clock_t st;
	st=clock();
	while(clock()-st<second*CLOCKS_PER_SEC)
}