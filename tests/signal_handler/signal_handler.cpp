#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>  // for waitpid

void signal_handler(int signo)
{  
    printf("#%d : get a signal: %d ", getpid(), signo);  
      
    bool do_exit = false;  
    switch(signo)
    {  
        case SIGCHLD:  
            printf("SIGCHLD\n");  
            int status;  
            while(waitpid(0, &status, WNOHANG) > 0);              
            break;  
        case SIGALRM:  
            printf("SIGALRM\n");  
            break;  
        case SIGIO:  
            printf("SIGIO\n");  
            break;  
        case SIGINT:  
            printf("SIGINT\n");  
            do_exit = true;  
            break;  
        case SIGHUP:  
            printf("SIGHUP\n");  
            do_exit = true;  
            break;  
        case SIGTERM:  
            printf("SIGTERM\n");  
            do_exit = true;  
            break;  
        case SIGQUIT:  
            printf("SIGQUIT\n");  
            do_exit = true;  
            break;  
        case SIGUSR1:  
            printf("SIGUSR1\n");  
            break;  
        case SIGUSR2:  
            printf("SIGUSR2\n");  
            break;  
    }  
      
    if(do_exit)
    {  
        exit(0);  
    }  
} 

void register_signal_handler_imp(int signum, void (*handler)(int))
{  
    struct sigaction action;  
      
    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask);  
    //action.sa_flags = 0;  
      
    if(sigaction(signum, &action, NULL) == -1)
    {  
        printf("register signal handler failed!\n");  
    }  
}
  
void register_signal_handler()
{  
    register_signal_handler_imp(SIGCHLD, signal_handler);  
    register_signal_handler_imp(SIGALRM, signal_handler);  
    register_signal_handler_imp(SIGIO,   signal_handler);  
    register_signal_handler_imp(SIGINT,  signal_handler);  
    register_signal_handler_imp(SIGHUP,  signal_handler);  
    register_signal_handler_imp(SIGTERM, signal_handler);  
    register_signal_handler_imp(SIGQUIT, signal_handler);  
    register_signal_handler_imp(SIGUSR1, signal_handler);  
    register_signal_handler_imp(SIGUSR2, signal_handler);  
    printf("register signal handler success!\n");  
}  

int main()
{
    register_signal_handler();
    while(1);
}
