#include<stdio.h>
#include<linux/input.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

int touch_fd;

int touch_open(){
    if(-1 == ((touch_fd = open("/dev/input/event0", O_RDWR)))){ 
        printf("open event error");
        return -1;
    }
}

int get_x_y(int *x, int *y){
    int count = 0;
    struct input_event touch_event;
    while(1){
        if(-1 == read(touch_fd, &touch_event, sizeof(touch_event))){
            printf("read event error");
            return -1;
        }
        if(touch_event.type == EV_ABS){
            if(touch_event.code == ABS_X){
                *x = touch_event.value;
                count ++;
            }
            if(touch_event.code == ABS_Y){
                *y = touch_event.value;
                count ++;
            }
            if(2 == count)
                break;
        }
    }
}
int touch_close(){
    close(touch_fd);
    return 0;
}
