#include "BVNetworkListManager.h"
#include <iostream>
#define show(str) printf(#str##"\n");

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

void callback1(PVOID para)
{
    printf("%lf %s\n", GetTickCount() / 1000.0, static_cast<const char *>(para));
}

void callback2(PVOID para)
{
    printf("%lf %s\n", GetTickCount() / 1000.0, static_cast<const char *>(para));
}

int main()
{
    auto b = BV_NLM::GetInstance();
    const char str1[] = "【Connection Interrupted !!",
               str2[] = "【Connection LOST !!";
    b->onInterrupted(callback1, (PVOID)str1);
    b->onLost(callback2, (PVOID)str2);
    b->Begin();
    CRTLOOP("主线程", 30);
    b->Stop();

    BV_NLM::Destroy();
    _CrtDumpMemoryLeaks();
    return 0;
}
