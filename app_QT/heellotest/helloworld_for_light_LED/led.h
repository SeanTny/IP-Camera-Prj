#ifndef LED_H
#define LED_H


class led
{

public:
    int fd;
    int val;


public:
     led(int val_org);
    ~led();


public:
   int led_on(void);
   int led_off(void);


};

#endif // LED_H
