#include "led.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>




led::led(int val_org)
{
    val=val_org;

    fd = open("/dev/xyz", O_RDWR);
    if (fd < 0)
    {
        perror("can't open!\n");
    }

}




led::~led( )
{
   close(fd);
}




int  led::led_on(void)
{
    val=1;
    write(fd, &val, 4);

    return 0;
}





int  led::led_off(void)
{
    val=0;
    write(fd, &val, 4);

    return 0;
}











