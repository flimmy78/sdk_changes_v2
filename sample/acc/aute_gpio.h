#ifndef AUTE_GPIO_H
#define AUTE_GPIO_H

#define HI_UNF_GPIO 1

#define GPIO_NAME_LEN       18
#define GPIO_LINE_LEN       1023

#define GPIO_NUMBER_SATA    58
#define GPIO_NUMBER_SYS     57
#define GPIO_NUMBER_ALARM   54
#define GPIO_NUMBER_GPS_LED   56
#define GPIO_NUMBER_OFFDELAY  42
#define GPIO_NUMBER_3G_LED1   38
#define GPIO_NUMBER_3G_LED2   39
#define GPIO_NUMBER_3G_CTL1   40
#define GPIO_NUMBER_3G_CTL2   41
#define GPIO_NUMBER_FORCE_SHUT      136
#define GPIO_NUMBER_PWR       135
#define GPIO_NUMBER_RB_SHUT    44
#define GPIO_NUMBER_RB_RESET   134
#define GPIO_NUMBER_PSE_CON   64
#define GPIO_NUMBER_RESET   59
#define GPIO_NUMBER_ACC_MON   43
#define GPIO_NUMBER_OCP_STA   108

#define GPIO_NAME_SATA      "sata"
#define GPIO_NAME_SYS       "sys"
#define GPIO_NAME_ALARM     "alarm"
#define GPIO_NAME_GPS_LED   "gps"
#define GPIO_NAME_OFFDELAY  "offdelay"
#define GPIO_NAME_3G_LED1   "3g_led1"
#define GPIO_NAME_3G_LED2   "3g_led2"
#define GPIO_NAME_3G_CTL1   "3g_ctl1"
#define GPIO_NAME_3G_CTL2   "3g_ctl2"
#define GPIO_NAME_FORCE_SHUT      "forceshut"
#define GPIO_NAME_PWR       "pwr"
#define GPIO_NAME_RB_SHUT   "rb_shut"
#define GPIO_NAME_RB_RESET  "rb_reset"
#define GPIO_NAME_PSE_CON   "pse_con"
#define GPIO_NAME_RESET     "reset"
#define GPIO_NAME_ACC_MON   "acc_mon"
#define GPIO_NAME_OCP_STA   "ocp_sta"

#define GPIO_R      0x01
#define GPIO_W      0x02
#define GPIO_RW     (GPIO_R | GPIO_W)

struct gpio {
	char name[1 + GPIO_NAME_LEN];
	int number;
	int flag;
};

#define GPIO_INIT(_name, _number, _flag)  {.name=_name, .number=_number, .flag=_flag}
static struct gpio GPIO[] = {
	GPIO_INIT(GPIO_NAME_SATA,   GPIO_NUMBER_SATA,   GPIO_W),
	GPIO_INIT(GPIO_NAME_SYS,    GPIO_NUMBER_SYS,    GPIO_W),
	GPIO_INIT(GPIO_NAME_ALARM,  GPIO_NUMBER_ALARM,  GPIO_W),
	GPIO_INIT(GPIO_NAME_GPS_LED,  GPIO_NUMBER_GPS_LED,  GPIO_W),
	GPIO_INIT(GPIO_NAME_OFFDELAY,  GPIO_NUMBER_OFFDELAY,  GPIO_W),
	GPIO_INIT(GPIO_NAME_3G_LED1,  GPIO_NUMBER_3G_LED1,  GPIO_W),
	GPIO_INIT(GPIO_NAME_3G_LED2,  GPIO_NUMBER_3G_LED2,  GPIO_W),
	GPIO_INIT(GPIO_NAME_3G_CTL1,  GPIO_NUMBER_3G_CTL1,  GPIO_W),
	GPIO_INIT(GPIO_NAME_3G_CTL2,  GPIO_NUMBER_3G_CTL2,  GPIO_W),
	GPIO_INIT(GPIO_NAME_FORCE_SHUT,  GPIO_NUMBER_FORCE_SHUT,  GPIO_W),
	GPIO_INIT(GPIO_NAME_PWR,  GPIO_NUMBER_PWR,  GPIO_W),
	GPIO_INIT(GPIO_NAME_RB_SHUT,  GPIO_NUMBER_RB_SHUT,  GPIO_W),
	GPIO_INIT(GPIO_NAME_RB_RESET,  GPIO_NUMBER_RB_RESET,  GPIO_W),
	GPIO_INIT(GPIO_NAME_PSE_CON,  GPIO_NUMBER_PSE_CON,  GPIO_W),
	GPIO_INIT(GPIO_NAME_RESET,	GPIO_NUMBER_RESET,	GPIO_R),
	GPIO_INIT(GPIO_NAME_ACC_MON,  GPIO_NUMBER_ACC_MON,	GPIO_R),
	GPIO_INIT(GPIO_NAME_OCP_STA,  GPIO_NUMBER_OCP_STA,	GPIO_R),
};

#define ASIZE(x)    (sizeof(x)/sizeof((x)[0]))
#define COUNT       ASIZE(GPIO)

#define println(fmt, args...)   printf(fmt "\n", ##args)
#define dprintln println

struct gpio *
getgpio_byname(char *name)
{
    int i;

    for (i=0; i<COUNT; i++) {
        if (0 == strcmp(name, GPIO[i].name)) {
            return &GPIO[i];
        }
    }

    return NULL;
}

#endif
