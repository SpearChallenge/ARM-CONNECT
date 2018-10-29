#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>

int show_fullbmp(char *bmppath){
    int bmp_fd,lcd_fd;
    int i, x, y;
    char bmp_buf[800*480*3];
    int lcd_buf[800*480];
    int temp_buf[800*480];

    if(-1 == (bmp_fd = open(bmppath, O_RDWR))){
        printf("open bmp error");
        return -1;
    }
    if(-1 == (lcd_fd = open("/dev/fb0", O_RDWR))){
        printf("open dev error");
        return -1;
    }
    lseek(bmp_fd, 52, SEEK_SET);
    if(-1 == read(bmp_fd, bmp_buf ,800*480*3)){
        printf("read bmp error");
        return -1;
    }
    for(i = 0; i < 800*480; i++)
        lcd_buf[i] = bmp_buf[3*i]|bmp_buf[3*i+1] << 8|bmp_buf[3*i+2] << 16|0x00 << 24;
    for(x = 0; x < 800; x++){
        for(y = 0; y < 480; y++){
            temp_buf[(479-y)*800+x] = lcd_buf[y*800+x];
        }
    }
    if(-1 == write(lcd_fd, temp_buf, 800*480*4)){
        printf("write bmp error");
        return -1;
    }
    close(lcd_fd);
    close(bmp_fd);
    return 0;
}
