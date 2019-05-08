#include "BVNetworkListManager.h"
#include <iostream>
#define show( str ) printf( #str##"\n" );

#define CRTLOOP(str, secs)                   \
    {                                        \
        int sec = 0;                         \
        while (1)                            \
        {                                    \
            Sleep(1000);                     \
            sec += 1;                        \
            printf("%s : %d s\n", str, sec); \
            if (sec == secs)                 \
            {                                \
                break;                       \
            }                                \
        }                                    \
    }

void callback1()
{
	printf("%lf\n", GetTickCount() / 1000.0);
	show(【Connection Interrupted !!);
}

void callback2()
{
	printf("%lf\n", GetTickCount() / 1000.0);
	show(【Connection LOST !!);
}

int main()
{
	auto b = BV_NLM::GetInstance();

	b->onInterrupted(callback1);
	b->onLost(callback2);
	b->Begin();
	CRTLOOP("主线程", 30);
	b->Stop();

	BV_NLM::Destroy();
	_CrtDumpMemoryLeaks();
	return(0);
}
