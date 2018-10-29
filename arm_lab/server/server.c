#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "record_start_thread.h"
#include "play_start_thread.h"
#include "chat_start_thread.h"
int  client;

int main(){
    int ts_x;
    int ts_y;
    pthread_t record_thread_server, play_thread_server, chat_thread_server;
    show_fullbmp("/1.bmp");
    touch_open();
    connect_client();
    while(1){
        get_x_y(&ts_x,&ts_y);
        if(ts_x>336&&ts_x<460&&ts_y>381&&ts_y<467)
            pthread_create(&record_thread_server, NULL, record_start_thread, NULL);
        
        if(ts_x>45&&ts_x<191&&ts_y>62&&ts_y<186)
            pthread_create(&play_thread_server, NULL, play_start_thread, NULL);
        
        if(ts_x>607&&ts_x<737&&ts_y>53&&ts_y<213)
           pthread_create(&chat_thread_server, NULL, chat_start_thread, NULL);
    }
}
