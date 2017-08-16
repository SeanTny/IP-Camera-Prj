
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


static int myvivi_vidioc_querycap(struct file *file, void  *priv,
					struct v4l2_capability *cap)
{
	strcpy(cap->driver, "myvivi");
	strcpy(cap->card, "myvivi");
	cap->version = 0x0001;
	cap->capabilities =	V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
	return 0;
}

static const struct v4l2_ioctl_ops myvivi_ioctl_ops = {
        // 表示它是一个摄像头设备
        .vidioc_querycap      = myvivi_vidioc_querycap,
#if 0    
        /* 用于列举、获得、测试、设置摄像头的数据的格式 */
        .vidioc_enum_fmt_vid_cap  = myvivi_vidioc_enum_fmt_vid_cap,
        .vidioc_g_fmt_vid_cap     = myvivi_vidioc_g_fmt_vid_cap,
        .vidioc_try_fmt_vid_cap   = myvivi_vidioc_try_fmt_vid_cap,
        .vidioc_s_fmt_vid_cap     = myvivi_vidioc_s_fmt_vid_cap,
        
        /* 缓冲区操作: 申请/查询/放入队列/取出队列 */
        .vidioc_reqbufs       = myvivi_vidioc_reqbufs,
        .vidioc_querybuf      = myvivi_vidioc_querybuf,
        .vidioc_qbuf          = myvivi_vidioc_qbuf,
        .vidioc_dqbuf         = myvivi_vidioc_dqbuf,
        
        // 启动/停止
        .vidioc_streamon      = myvivi_vidioc_streamon,
        .vidioc_streamoff     = myvivi_vidioc_streamoff,   
#endif
};

static const struct v4l2_file_operations myvivi_fops = {
	.owner		= THIS_MODULE,
    .ioctl      = video_ioctl2, /* V4L2 ioctl handler */
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
    myvivi_device->ioctl_ops = &myvivi_ioctl_ops;

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

