#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
extern int client;
void *record_start_thread(void *arg){
    system("arecord -d3 -c1 -r16000 -twav -fS16_LE 1.wav");
    int wav_fd;
    if(-1 == (wav_fd = open("1.wav", O_RDWR))){
        printf("open wav error");
        return NULL;
    }
    int wav_size = lseek(wav_fd, 0, SEEK_END);
    lseek(wav_fd, 0, SEEK_SET);
    char wav_buf[wav_size];
    if(-1 == read(wav_fd, wav_buf, wav_size)){
        printf("read wav error");
        return NULL;
    }
    if(-1 == write(client, &wav_size, sizeof(wav_size))){
        printf("write wav_size error");
        return NULL;
    }
    if(-1 == write(client, wav_buf, wav_size)){
        printf("client write error");
        return NULL;
    }
    close(wav_fd);
}
