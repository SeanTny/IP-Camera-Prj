#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/videodev2.h>
#include <linux/vmalloc.h>
#include <linux/wait.h>
#include <asm/atomic.h>
#include <asm/unaligned.h>

#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>

/* 参考 drivers/media/video/uvc */

struct frame_desc {
    int width;
    int height;
};

/* 参考uvc_video_queue定义一些结构体 */
struct myuvc_buffer {
    struct v4l2_buffer buf;
    int state;
    int vma_use_count; /* 表示是否已经被mmap */
    wait_queue_head_t wait;  /* APP要读某个缓冲区,如果无数据,在此休眠 */
	struct list_head stream;
	struct list_head irq;    
};

struct myuvc_queue {
    void *mem;
    int count;
    int buf_size;    
    struct myuvc_buffer buffer[32];
	struct list_head mainqueue;   /* 供APP消费用 */
	struct list_head irqqueue;    /* 供底层驱动生产用 */
};

static struct myuvc_queue myuvc_queue;

static struct video_device *myuvc_vdev;
static struct v4l2_format myuvc_format;

static struct frame_desc frames[] = {{640, 480}, {352, 288}, {320, 240}, {176, 144}, {160, 120}};
static int frame_idx = 1;
static int bBitsPerPixel = 16; /* lsusb -v -d 0x1e4e:  "bBitsPerPixel" */

/* A2 参考 uvc_v4l2_do_ioctl */
static int myuvc_vidioc_querycap(struct file *file, void  *priv,
					struct v4l2_capability *cap)
{    
    memset(cap, 0, sizeof *cap);
    strcpy(cap->driver, "myuvc");
    strcpy(cap->card, "myuvc");
    cap->version = 1;
    
    cap->capabilities = V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING;
    break;

	return 0;
}

/* A3 列举支持哪种格式
 * 参考: uvc_fmts 数组
 */
static int myuvc_vidioc_enum_fmt_vid_cap(struct file *file, void  *priv,
					struct v4l2_fmtdesc *f)
{
    /* 人工查看描述符可知我们用的摄像头只支持1种格式 */
	if (f->index >= 1)
		return -EINVAL;

    /* 支持什么格式呢?
     * 查看VideoStreaming Interface的描述符,
     * 得到GUID为"59 55 59 32 00 00 10 00 80 00 00 aa 00 38 9b 71"
     */
	strcpy(f->description, "4:2:2, packed, YUYV");
	f->pixelformat = V4L2_PIX_FMT_YUYV;    
    
	return 0;
}

/* A4 返回当前所使用的格式 */
static int myuvc_vidioc_g_fmt_vid_cap(struct file *file, void *priv,
					struct v4l2_format *f)
{
    memcpy(f, &myuvc_format, sizeof(myuvc_format));
	return (0);
}

/* A5 测试驱动程序是否支持某种格式, 强制设置该格式 
 * 参考: uvc_v4l2_try_format
 *       myvivi_vidioc_try_fmt_vid_cap
 */
static int myuvc_vidioc_try_fmt_vid_cap(struct file *file, void *priv,
			struct v4l2_format *f)
{
    if (f->type != V4L2_BUF_TYPE_VIDEO_CAPTURE)
    {
        return -EINVAL;
    }

    if (f->fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV)
        return -EINVAL;
    
    /* 调整format的width, height, 
     * 计算bytesperline, sizeimage
     */

    /* 人工查看描述符, 确定支持哪几种分辨率 */
    f->fmt.pix.width  = frames[frame_idx].width;
    f->fmt.pix.height = frames[frame_idx].height;
    
	f->fmt.pix.bytesperline =
		(f->fmt.pix.width * bBitsPerPixel) >> 3;
	f->fmt.pix.sizeimage =
		f->fmt.pix.height * f->fmt.pix.bytesperline;
    
    return 0;
}

/* A6 参考 myvivi_vidioc_s_fmt_vid_cap */
static int myuvc_vidioc_s_fmt_vid_cap(struct file *file, void *priv,
					struct v4l2_format *f)
{
	int ret = myuvc_vidioc_try_fmt_vid_cap(file, NULL, f);
	if (ret < 0)
		return ret;

    memcpy(&myuvc_format, f, sizeof(myuvc_format));
    
    return 0;
}

static int myuvc_free_buffers(void)
{
    kfree(myuvc_queue.mem);
    memset(&myuvc_queue, 0, sizeof(myuvc_queue));
    return 0;
}

/* A7 APP调用该ioctl让驱动程序分配若干个缓存, APP将从这些缓存中读到视频数据 
 * 参考: uvc_alloc_buffers
 */
static int myuvc_vidioc_reqbufs(struct file *file, void *priv,
			  struct v4l2_requestbuffers *p)
{
    int nbuffers = p->count;
    int bufsize  = PAGE_ALIGN(myuvc_format.fmt.pix.sizeimage);
    unsigned int i;
    void *mem = NULL;
    int ret;

    if ((ret = myuvc_free_buffers()) < 0)
        goto done;

    /* Bail out if no buffers should be allocated. */
    if (nbuffers == 0)
        goto done;

    /* Decrement the number of buffers until allocation succeeds. */
    for (; nbuffers > 0; --nbuffers) {
        mem = vmalloc_32(nbuffers * bufsize);
        if (mem != NULL)
            break;
    }

    if (mem == NULL) {
        ret = -ENOMEM;
        goto done;
    }

    /* 这些缓存是一次性作为一个整体来分配的 */
    memset(&myuvc_queue, 0, sizeof(myuvc_queue));

	INIT_LIST_HEAD(&myuvc_queue.mainqueue);
	INIT_LIST_HEAD(&myuvc_queue.irqqueue);

    for (i = 0; i < nbuffers; ++i) {
        myuvc_queue.buffer[i].buf.index = i;
        myuvc_queue.buffer[i].buf.m.offset = i * bufsize;
        myuvc_queue.buffer[i].buf.length = myuvc_format.fmt.pix.sizeimage;
        myuvc_queue.buffer[i].buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        myuvc_queue.buffer[i].buf.sequence = 0;
        myuvc_queue.buffer[i].buf.field = V4L2_FIELD_NONE;
        myuvc_queue.buffer[i].buf.memory = V4L2_MEMORY_MMAP;
        myuvc_queue.buffer[i].buf.flags = 0;
        myuvc_queue.buffer[i].state     = VIDEOBUF_IDLE;
        init_waitqueue_head(&myuvc_queue.buffer[i].wait);
    }

    myuvc_queue.mem = mem;
    myuvc_queue.count = nbuffers;
    myuvc_queue.buf_size = bufsize;
    ret = nbuffers;

done:
    return ret;
}

/* A8 查询缓存状态, 比如地址信息(APP可以用mmap进行映射) 
 * 参考 uvc_query_buffer
 */
static int myuvc_vidioc_querybuf(struct file *file, void *priv, struct v4l2_buffer *v4l2_buf)
{
    int ret = 0;
    
	if (v4l2_buf->index >= myuvc_queue.count) {
		ret = -EINVAL;
		goto done;
	}

    memcpy(v4l2_buf, myuvc_queue.buffer[v4l2_buf->index].buf, sizeof(*v4l2_buf));

    /* 更新flags */
	if (myuvc_queue.buffer[v4l2_buf->index].vma_use_count)
		v4l2_buf->flags |= V4L2_BUF_FLAG_MAPPED;


	switch (myuvc_queue.buffer[v4l2_buf->index].state) {
    	case VIDEOBUF_ERROR:
    	case VIDEOBUF_DONE:
    		v4l2_buf->flags |= V4L2_BUF_FLAG_DONE;
    		break;
    	case VIDEOBUF_QUEUED:
    	case VIDEOBUF_ACTIVE:
    		v4l2_buf->flags |= V4L2_BUF_FLAG_QUEUED;
    		break;
    	case VIDEOBUF_IDLE:
    	default:
    		break;
	}

done:    
	return ret;
}

/* A10 把缓冲区放入队列, 底层的硬件操作函数将会把数据放入这个队列的缓存 
 * 参考: uvc_queue_buffer
 */
static int myuvc_vidioc_qbuf(struct file *file, void *priv, struct v4l2_buffer *v4l2_buf)
{
    struct myuvc_buffer *buf = &myuvc_queue.buffer[v4l2_buf->index];
    /* 1. 修改状态 */
	buf->state = VIDEOBUF_QUEUED;
	v4l2_buf->bytesused = 0;

    /* 2. 放入2个队列 */
    /* 队列1: 供APP使用 
     * 当缓冲区没有数据时,放入mainqueue队列
     * 当缓冲区有数据时, APP从mainqueue队列中取出
     */
	list_add_tail(&buf->stream, &myuvc_queue.mainqueue);

    /* 队列2: 供产生数据的函数使用
     * 当采集到数据时,从irqqueue队列中取出第1个缓冲区,存入数据
     */
	list_add_tail(&buf->queue, &myuvc_queue.irqqueue);
    
	return 0;
}

/* A11 启动传输 
 * 参考: uvc_video_enable(video, 1)
 */
static int myuvc_vidioc_streamon(struct file *file, void *priv, enum v4l2_buf_type i)
{
    /* 1. 向USB摄像头设置参数 */

    /* 2. 分配设置URB */

    /* 3. 提交URB以接收数据 */
    
	return 0;
}

/* A13 APP通过poll/select确定有数据后, 把缓存从队列中取出来
 * 参考: uvc_dequeue_buffer
 */
static int myuvc_vidioc_dqbuf(struct file *file, void *priv, struct v4l2_buffer *v4l2_buf)
{
    /* APP发现数据就绪后, 从mainqueue里取出这个buffer */

    struct myuvc_buffer *buf = &myuvc_queue.buffer[v4l2_buf->index];
    
	list_del(&buf->stream);
    
	return 0;
}

/*
 * A14 之前已经通过mmap映射了缓存, APP可以直接读数据
 * A15 再次调用myuvc_vidioc_qbuf把缓存放入队列
 * A16 poll...
 */

/* A17 停止 */
static int myuvc_vidioc_streamoff(struct file *file, void *priv, enum v4l2_buf_type i)
{
    return 0;
}



static const struct v4l2_ioctl_ops myuvc_ioctl_ops = {
        // 表示它是一个摄像头设备
        .vidioc_querycap      = myuvc_vidioc_querycap,

        /* 用于列举、获得、测试、设置摄像头的数据的格式 */
        .vidioc_enum_fmt_vid_cap  = myuvc_vidioc_enum_fmt_vid_cap,
        .vidioc_g_fmt_vid_cap     = myuvc_vidioc_g_fmt_vid_cap,
        .vidioc_try_fmt_vid_cap   = myuvc_vidioc_try_fmt_vid_cap,
        .vidioc_s_fmt_vid_cap     = myuvc_vidioc_s_fmt_vid_cap,
        
        /* 缓冲区操作: 申请/查询/放入队列/取出队列 */
        .vidioc_reqbufs       = myuvc_vidioc_reqbufs,
        .vidioc_querybuf      = myuvc_vidioc_querybuf,
        .vidioc_qbuf          = myuvc_vidioc_qbuf,
        .vidioc_dqbuf         = myuvc_vidioc_dqbuf,
        
        // 启动/停止
        .vidioc_streamon      = myuvc_vidioc_streamon,
        .vidioc_streamoff     = myuvc_vidioc_streamoff,   
};

/* A1 */
static int myuvc_open(struct file *file)
{
	return 0;
}


/* A9 把缓存映射到APP的空间,以后APP就可以直接操作这块缓存 */
static int myuvc_mmap(struct file *file, struct vm_area_struct *vma)
{
	return 0;
}

/* A12 APP调用POLL/select来确定缓存是否就绪(有数据) */
static unsigned int myuvc_poll(struct file *file, struct poll_table_struct *wait)
{
	return 0;
}

/* A18 关闭 */
static int myuvc_close(struct file *file)
{
    
	return 0;
}

static const struct v4l2_file_operations myuvc_fops = {
	.owner		= THIS_MODULE,
    .open       = myuvc_open,
    .release    = myuvc_close,
    .mmap       = myuvc_mmap,
    .ioctl      = video_ioctl2, /* V4L2 ioctl handler */
    .poll       = myuvc_poll,
};

static void myuvc_release(struct video_device *vdev)
{
}


static int myuvc_probe(struct usb_interface *intf,
		     const struct usb_device_id *id)
{
    static int cnt = 0;
	struct usb_device *dev = interface_to_usbdev(intf);
    struct usb_device_descriptor *descriptor = &dev->descriptor;
    struct usb_host_config *hostconfig;
    struct usb_config_descriptor *config;
	struct usb_interface_assoc_descriptor *assoc_desc;
    struct usb_interface_descriptor	*interface;
    struct usb_endpoint_descriptor  *endpoint;
    int i, j, k, l, m;
	unsigned char *buffer;
	int buflen;
    int desc_len;
    int desc_cnt;

    printk("myuvc_probe : cnt = %d\n", cnt++);

    if (cnt == 2)
    {
        /* 1. 分配一个video_device结构体 */
        myuvc_vdev = video_device_alloc();

        /* 2. 设置 */
        /* 2.1 */
        myuvc_vdev->release = myuvc_release;
        
        /* 2.2 */
        myuvc_vdev->fops    = &myuvc_fops;
        
        /* 2.3 */
        myuvc_vdev->ioctl_ops = &myuvc_ioctl_ops;

        /* 3. 注册 */
        video_register_device(myuvc_vdev, VFL_TYPE_GRABBER, -1);
    }
    
    
    return 0;
}

static void myuvc_disconnect(struct usb_interface *intf)
{
    static int cnt = 0;
    printk("myuvc_disconnect : cnt = %d\n", cnt++);

    if (cnt == 2)
    {
        video_unregister_device(myuvc_vdev);
        video_device_release(myuvc_vdev);
    }
    
}

static struct usb_device_id myuvc_ids[] = {
	/* Generic USB Video Class */
	{ USB_INTERFACE_INFO(USB_CLASS_VIDEO, 1, 0) },  /* VideoControl Interface */
    { USB_INTERFACE_INFO(USB_CLASS_VIDEO, 2, 0) },  /* VideoStreaming Interface */
	{}
};

/* 1. 分配usb_driver */
/* 2. 设置 */
static struct usb_driver myuvc_driver = {
    .name       = "myuvc",
    .probe      = myuvc_probe,
    .disconnect = myuvc_disconnect,
    .id_table   = myuvc_ids,
};

static int myuvc_init(void)
{
    /* 3. 注册 */
    usb_register(&myuvc_driver);
    return 0;
}

static void myuvc_exit(void)
{
    usb_deregister(&myuvc_driver);
}

module_init(myuvc_init);
module_exit(myuvc_exit);

MODULE_LICENSE("GPL");



