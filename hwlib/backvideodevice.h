#ifndef VIDEODEVICE_H
#define VIDEODEVICE_H
 
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
 
//#include <asm/types.h>
#include <linux/videodev2.h>
 

//#define DEBUG_SWITCH
#if defined(DEBUG_SWITCH)
	#define DEBUG(fmt,arg...) printf(fmt,##arg)
#else
	#define DEBUG(...)
#endif

#define RetValueIf(c, value)	if (c) return (value)
#define RetIf(c)				if (c) return
#define DoIf(c, act)			if (c) { act; }

#define JumpIf(c, lable)		if (c) goto lable
#define BreakIf(c)				if (c) break
#define ContinueIf(c)			if (c) continue


#define  IMG_WIDTH    320
#define  IMG_HEIGHT   240

#define CLEAR(x) memset(&(x), 0, sizeof(x))

class VideoDevice
{
public:
    VideoDevice(const char* );
    int open_device();
    int close_device();
    int init_device();
    int start_capturing();
    int stop_capturing();
    int uninit_device();
    int get_frame(void **, unsigned int*);
    int release_frame();
 
private:
    int init_mmap();
    struct buffer
    {
        void * start;
        unsigned int length;
    };
    const char* dev_name;
    int fd;
    buffer* buffers;
    unsigned int n_buffers;
    int index;
	int m_init; /*Determine video device whether the initialization*/
 
};
 
#endif // VIDEODEVICE_H
