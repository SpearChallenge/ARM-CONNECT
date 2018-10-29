#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>           
#include <fcntl.h>            
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>        
#include <linux/videodev2.h>

#define CAMERA_DEVICE "/dev/video7"
#define CAPTURE_FILE "frame.jpg"
#include "lcdjpg.h"
#define VIDEO_WIDTH 640
#define VIDEO_HEIGHT 480
#define VIDEO_FORMAT V4L2_PIX_FMT_YUYV
#define BUFFER_COUNT 4
                         // x轴坐标      y轴坐标      图片名       默认NULL    默认0         默认大小0，缩小1
extern int lcd_draw_jpg(unsigned int ,unsigned int ,const char *,  char *,    unsigned int ,unsigned int );


typedef struct VideoBuffer {
    void   *start;
    size_t  length;
} VideoBuffer;
VideoBuffer framebuf[BUFFER_COUNT];   

void *chat_start_thread(void *arg)
{
    int i, ret;

    
    int fd;
    fd = open(CAMERA_DEVICE, O_RDWR, 0);
    if (fd < 0) {
        printf("Open %s failed\n", CAMERA_DEVICE);
        return NULL;
    }

    
    struct v4l2_capability cap;
    if(-1 == ioctl(fd, VIDIOC_QUERYCAP, &cap)){
        printf("VIDIOC_QUERYCAP failed\n");
        return NULL;
    }
    
    printf("Capability Informations:\n");
    printf(" driver: %s\n", cap.driver);
    printf(" card: %s\n", cap.card);
    printf(" bus_info: %s\n", cap.bus_info);
    printf(" version: %08X\n", cap.version);
    printf(" capabilities: %08X\n", cap.capabilities);


    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width       = VIDEO_WIDTH;
    fmt.fmt.pix.height      = VIDEO_HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
    if(-1 == ioctl(fd, VIDIOC_S_FMT, &fmt)){
        printf("VIDIOC_S_FMT failed \n");
        return NULL;
    }

    
    if(-1 == ioctl(fd, VIDIOC_G_FMT, &fmt)){
        printf("VIDIOC_G_FMT failed (%d)\n", ret);
        return NULL;
    }
    printf("Stream Format Informations:\n");
    printf(" type: %d\n", fmt.type);
    printf(" width: %d\n", fmt.fmt.pix.width);
    printf(" height: %d\n", fmt.fmt.pix.height);
    char fmtstr[8];
    memset(fmtstr, 0, 8);
    memcpy(fmtstr, &fmt.fmt.pix.pixelformat, 4);
    printf(" pixelformat: %s\n", fmtstr);
    printf(" field: %d\n", fmt.fmt.pix.field);
    printf(" bytesperline: %d\n", fmt.fmt.pix.bytesperline);
    printf(" sizeimage: %d\n", fmt.fmt.pix.sizeimage);
    printf(" colorspace: %d\n", fmt.fmt.pix.colorspace);
    printf(" priv: %d\n", fmt.fmt.pix.priv);
    printf(" raw_date: %s\n", fmt.fmt.raw_data);

    
    struct v4l2_requestbuffers reqbuf;
    
    reqbuf.count = BUFFER_COUNT;
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_MMAP;
    
    
    
    
    if(-1 == ioctl(fd , VIDIOC_REQBUFS, &reqbuf)){
        printf("VIDIOC_REQBUFS failed \n");
        return NULL;
    }

    
    VideoBuffer*  buffers = calloc( reqbuf.count, sizeof(*buffers) );
    struct v4l2_buffer buf;

    for (i = 0; i < reqbuf.count; i++) 
    {
        buf.index = i;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        if(-1 == ioctl(fd , VIDIOC_QUERYBUF, &buf)){
            printf("VIDIOC_QUERYBUF failed \n");
            return NULL;
        }

        // mmap buffer
        framebuf[i].length = buf.length;
        framebuf[i].start = (char *) mmap(0, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (framebuf[i].start == MAP_FAILED) {
            printf("mmap (%d) failed: %s\n", i, strerror(errno));
            return NULL;
        }
    
        // Queen buffer
        if(-1 == ioctl(fd , VIDIOC_QBUF, &buf)) {
            printf("VIDIOC_QBUF failed \n");
            return NULL;
        }

        printf("Frame buffer %d: address=0x%x, length=%d\n", i, (unsigned int)framebuf[i].start, framebuf[i].length);
    }

    // 开始录制
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(-1 == ioctl(fd, VIDIOC_STREAMON, &type)){
        printf("VIDIOC_STREAMON failed \n");
        return NULL;
    }

    // Get frame
    if(-1 == ioctl(fd, VIDIOC_DQBUF, &buf)){
        printf("VIDIOC_DQBUF failed \n");
        return NULL;
    }

    // Process the frame
    FILE *fp = fopen(CAPTURE_FILE, "wb");
    if (fp < 0) {
        printf("open frame data file failed\n");
        return NULL;
    }
    fwrite(framebuf[buf.index].start, 1, buf.length, fp);
    fclose(fp);
    printf("Capture one frame saved in %s\n", CAPTURE_FILE);

    // Re-queen buffer
    if(-1 == ioctl(fd, VIDIOC_QBUF, &buf)){
        printf("VIDIOC_QBUF failed \n");
        return NULL;
    }

    // Release the resource
    for (i=0; i< 4; i++) 
    {
        munmap(framebuf[i].start, framebuf[i].length);
    }

    close(fd);
    printf("Camera test Done.\n");
	
	lcd_draw_jpg(0,0,CAPTURE_FILE,NULL,0,0);
	
    return 0;
}
