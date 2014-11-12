/* 
 * File:   main.cpp
 * Author: lizheng
 *
 * Created on 2014年7月26日, 上午11:03
 */
#include <iostream>
#include <cstdlib>
#include "TriggerTimer.h"
using namespace std;

void func(TriggerTimer * timer, void *data)
{
    printf("hi,%d\n",*((int*)(data)));
}

int main(int argc, char** argv) 
{
    TriggerTimerManager::instance()->Start();
    int i = 1, j=2,m=3,n=4;
    TriggerTimer a(1000,func,(void *)&i,TriggerTimer::TIMER_CIRCLE);
    TriggerTimer a1(2000,func,(void *)&j,TriggerTimer::TIMER_ONCE);
    TriggerTimer a2(3000,func,(void *)&m,TriggerTimer::TIMER_ONCE);
    TriggerTimer a3(1000,func,(void *)&n,TriggerTimer::TIMER_CIRCLE);
    
    a.Start();
    a1.Start();
    a2.Start();
    a3.Start();
    //a.Start();
    //a1.Start();
    //a2.Start();
    //a3.Start();
    
    a.Reset(2000);
    a1.Stop();
    a3.Stop();
    //while(1);
    sleep(20);
    a.Stop();
    printf("stop timer over\n");
    
    exit(0);
    return 0;
}

