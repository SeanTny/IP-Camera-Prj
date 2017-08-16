
/* 仿照vivi.c */
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/pci.h>
#include <linux/random.h>
#include <linux/version.h>
#include <linux/mutex.h>
#include <linux/videodev2.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/highmem.h>
#include <linux/freezer.h>
#include <media/videobuf-vmalloc.h>
#include <media/v4l2-device.h>
#include <media/v4l2-ioctl.h>


static const struct v4l2_file_operations myvivi_fops = {
	.owner		= THIS_MODULE,
};


static struct video_device *myvivi_device;

static void myvivi_release(struct video_device *vdev)
{
}


static int myvivi_init(void)
{
    int error;
    
    /* 1. 分配一个video_device结构体 */
    myvivi_device = video_device_alloc();

    /* 2. 设置 */
    myvivi_device->release = myvivi_release;
    myvivi_device->fops    = &myvivi_fops;

    /* 3. 注册 */
    error = video_register_device(myvivi_device, VFL_TYPE_GRABBER, -1);
    
    return error;
}

static void myvivi_exit(void)
{
    video_unregister_device(myvivi_device);
    video_device_release(myvivi_device);
}

module_init(myvivi_init);
module_exit(myvivi_exit);
MODULE_LICENSE("GPL");

