#include "pch.h"


int main(int argc, char* argv)
{
	if (SERVER->Initialize())
	{
		SERVER->StartServer();
	}
	std::cout << std::endl;

	/*printf("Hello Graduation_Work\n");

	printf("yesterday %ws\n", CLOCK->yesterday().c_str());
	printf("today %ws\n", CLOCK->today().c_str());
	printf("tomorrow %ws\n", CLOCK->tomorrow().c_str());
	printf("today is [%d]week of the day\n", CLOCK->todayOfTheWeek());	
	printf("%ws\n", CLOCK->nowTimeWithMilliSecond().c_str());*/

	return 0;
}

 