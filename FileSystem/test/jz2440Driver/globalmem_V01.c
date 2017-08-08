/*
 * globalmem.c
 *
 *  Created on: 2017年5月29日
 *      Author: tusen
 */
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/input.h>
#include <linux/platform_device.h>


#define GLOBALMEM_SIZE   0x1000
#define MEM_CLEAR        0x01
#define GLOBALMEM_MAJOR  230


static int  globalmem_major=GLOBALMEM_MAJOR;
//module_param(globalmem_major,int ,S_IRUGO);



struct globalmem_dev{
	struct cdev cdev;
	unsigned char mem[GLOBALMEM_SIZE];
	struct mutex mutex;
};
struct globalmem_dev *globalmem_devp;


static ssize_t globalmem_read (struct file *filp,char __user *buf,size_t size,
		loff_t * ppos)
{
	unsigned long p = *ppos; //读取文件的位置 偏移
	unsigned int  count=size;//读取文件内的字节数
	int ret =0;
	struct globalmem_dev  *dev=filp->private_data;

	if(p>=GLOBALMEM_SIZE)
		return 0;

	if(count > GLOBALMEM_SIZE-p)
		count=GLOBALMEM_SIZE-p;

	if( copy_to_user(buf,dev->mem+p,count) ){
		ret=-EFAULT;
	} else {
		*ppos+=count;
		ret=count;

		printk(KERN_INFO "read %u byte(s) from %lu \n",count ,p);
	}

	return ret;
}


static ssize_t globalmem_write(struct file *filp, const char __user *buf,
		size_t size,loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret =0;
	struct globalmem_dev *dev=filp->private_data;


	if(p >= GLOBALMEM_SIZE )
		return 0;

	if(count >GLOBALMEM_SIZE-p) //p 是文件偏移量
		count = GLOBALMEM_SIZE-p;

	if(copy_from_user(dev->mem+p,buf,count))
		ret=-EFAULT;
	else {
		*ppos+=count;
		ret = count;

		printk(KERN_INFO "written %u byte(s) from %lu \n",count,p);
	}
	return ret;
}



static loff_t globalmem_llseek(struct file * filp,loff_t offest,int orig)
{
	loff_t ret=0;
	switch (orig){
	case 0:/* 从文件开头位置seek */
		if(offest<0){
			ret=-EINVAL;
			break;
		}
		if((unsigned int )offest > GLOBALMEM_SIZE){
			ret=-EINVAL;
			break;
		}
		filp->f_pos=(unsigned int) offest;
		ret=filp->f_pos;
		break;
	case 1:
		if ( (filp->f_pos + offest) > GLOBALMEM_SIZE  ){
			ret =-EINVAL;
			break;
		}
		if( filp->f_pos + offest < 0 ){
			ret =-EINVAL;
			break;
		}
		filp->f_pos+=offest;
		ret =filp->f_pos;
		break;
	default :
		ret = -EINVAL;
		break;
	}
	return ret;
}


static long globalmem_ioctl (struct file *filp,unsigned int cmd,
		unsigned long arg)
{
	struct globalmem_dev *dev = filp->private_data;

	switch (cmd){
	case MEM_CLEAR:
		memset(dev->mem,0,GLOBALMEM_SIZE);
		printk( KERN_INFO "globalmem is set to zero \n" );
		break;

	default:
		return -EINVAL;
	}
	return 0;
}



static int globalmem_open( struct inode *inode ,struct file *filp )
{
	filp->private_data=globalmem_devp;
	return 0;
}


static int globalmem_release(struct inode *inode , struct file *filp)
{

	return 0;
}



static const struct file_operations globalmem_fops = {
		.owner = THIS_MODULE,
		.llseek = globalmem_llseek,
		.read = globalmem_read,
		.write = globalmem_write,
		.unlocked_ioctl = globalmem_ioctl,
		.open = globalmem_open,
		.release = globalmem_release,
};


static void  globalmem_setup_cdev(struct globalmem_dev *dev,int index)
{
	int err ,devno=MKDEV(globalmem_major,index);

	cdev_init(&dev->cdev,&globalmem_fops);

	dev->cdev.owner=THIS_MODULE;
	err=cdev_add(&dev->cdev,devno,1);
	if(err)
		printk(KERN_NOTICE "Error %d adding globalmem%d",err,index);

}


static int __init globalmem_init (void)
{
	int ret;
	dev_t devno=MKDEV(globalmem_major,0);

	if(globalmem_major)
		//分配 (globalmem_major：0)  开始的1个设备号出来
		//name=globalmem：这个的设备的名字; 它会出现在 /proc/devices 和 sysfs 中.
		ret=register_chrdev_region(devno,1,"globalmem_dev");
	else{
		//系统自动分配以 从设备号为0开始的一个设备号出来，然后将完整的设备号放入 devno中
		ret=alloc_chrdev_region(&devno,0,1,"globalmem_dev");
		globalmem_major=MAJOR(devno);
	}
	if(ret<0)
		return ret;

	/* 分配设备结构体的空间 */
	globalmem_devp =kzalloc(sizeof(struct globalmem_dev),GFP_KERNEL);
	if(!globalmem_devp) {
		ret = -ENOMEM;
		goto faill_malloc;
	}

	globalmem_setup_cdev(globalmem_devp,0);
	return 0;

	faill_malloc:
	unregister_chrdev_region(devno,1);
	return ret;
}




static int __exit globalmem_exit(void)
{
	cdev_del(&globalmem_devp->cdev);
	kfree(globalmem_devp);
	unregister_chrdev_region(MKDEV(globalmem_major,0),1);
	return 0;
}


module_init(globalmem_init);
module_exit(globalmem_exit);



MODULE_AUTHOR("Tu_sen");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(" Use the meme to Globalmem driver ");
MODULE_ALIAS(" a simple globalmem device ");



