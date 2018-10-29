#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<pthread.h>
extern int client;

void *play_start_thread(void *arg){
    int wav_size;
    if(-1 == read(client, &wav_size, sizeof(wav_size))){
        printf("read wav_size error");
        return NULL;
    }
    char wav_buf[wav_size];
    if(-1 == read(client, wav_buf, wav_size)){
        printf("read wav_buf error ");
        return NULL;
    }
    int new_wav_fd;
    if(-1 == (new_wav_fd = open("new.wav", O_CREAT | O_TRUNC | O_RDWR))){
        printf("open new wav error");
        return NULL;
    }
    if(-1 == write(new_wav_fd, wav_buf, wav_size)){
        printf("write new wav error");
        return NULL;
    }
    close(new_wav_fd);
    system("aplay 1.wav");
}

