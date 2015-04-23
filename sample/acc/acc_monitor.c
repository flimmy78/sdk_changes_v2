#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>
#include <time.h>
#include <pthread.h>
#include <dirent.h>

#include "aute_gpio.h"
#ifdef HI_UNF_GPIO
#include "hi_unf_gpio.h"
#include "hi_adp_boardcfg.h"
#endif

#define RD_SIZE   1024
#define STR_SIZE  256
#define STR_SHORT 32

#define ACC_OPEN  0
#define ACC_CLOSE 1

#define ACC_MONITOR "acc_mon"
#define OFF_DELAY   "offdelay"
#define GET_VOLTAGE_RAW "voltage_show | awk '{if(NR==2){print $3}}'"

int off_time = 600;
unsigned int acc_monitor = 0;
pthread_mutex_t mutex;
static FILE *logfp = NULL;
const char *accpath = "/tmp/.acc";

static struct vcc_struct {
        unsigned int level;
        unsigned int interval;
		double voltage;
        char Timeaccoff[STR_SHORT];
}vcc = {0, 10, 0, };

int PopenFile(char *cmd_str, char *str, int len)
{
	FILE *fp=NULL;

	if (cmd_str == NULL||str == NULL) {
		return -1;
	}
	
	memset(str, 0, len);		   
	fp = popen(cmd_str, "r");  
	//	printf("%s\n", cmd_str);
	if (fp) {	   
		fgets(str, len, fp);	   
		if (str[strlen(str)-1] == '\n')	{		   
			str[strlen(str)-1] = '\0';	   
		}	   
		pclose(fp); 	   
		return 0;    
	} else {	   
		perror("popen");	   
		str = NULL;
		return -1;   
	}
	
	return 0;
}

#ifdef HI_UNF_GPIO
int set_gpio_bit(int gpio, int value)
{
	int Ret = 0;

	pthread_mutex_lock(&mutex);
	Ret = HI_UNF_GPIO_SetDirBit(gpio, HI_FALSE);
	Ret = HI_UNF_GPIO_WriteBit(gpio, value);
	pthread_mutex_unlock(&mutex);	

	return Ret;
}
int get_gpio_bit(int gpio)
{
	int Ret = 0;
	HI_BOOL ReadBitVal;
	
	pthread_mutex_lock(&mutex);
	Ret = HI_UNF_GPIO_SetDirBit(gpio, HI_TRUE);
	Ret = HI_UNF_GPIO_ReadBit(gpio, &ReadBitVal);
	pthread_mutex_unlock(&mutex);

	return ReadBitVal;
}
#endif

void set_offdelay_shutdown(void)
{
	struct gpio *gpio1;

	gpio1 = getgpio_byname(OFF_DELAY);
	if (NULL != gpio1) {
		set_gpio_bit(gpio1->number, HI_FALSE);
	}
	
	return ;
}

void report_acc(int status, int duration)
{
	char temp_str[STR_SIZE] = {0};

	sprintf(temp_str, "echo %d > %s/status", status, accpath);
	system(temp_str);

	memset(temp_str, 0, sizeof(temp_str));
	sprintf(temp_str, "echo %d > %s/duration", duration, accpath);
	system(temp_str);

	return ;
}

void monitor_acc(int intval)
{
	struct gpio *gpio;
	int value = 0;
	int duration = 0;

#ifdef HI_UNF_GPIO
	gpio = getgpio_byname(ACC_MONITOR);
	if (NULL != gpio) {
		value = get_gpio_bit(gpio->number);

		if (ACC_OPEN == value) {
			acc_monitor = 0;
		} else if (ACC_CLOSE == value) {
			acc_monitor ++;
		} else {
			printf("warning: %s, str:%s, gpio:%d, value:%d\n", __func__, ACC_MONITOR, gpio->number, value);
		}
	}
	duration = acc_monitor * intval;
	if (duration >= off_time) {
		set_offdelay_shutdown();
	}
	report_acc(value, duration);
#endif

	return ;
}

void report_vcc(void)
{
	char temp_str[STR_SIZE] = {0};
	char Timereport[STR_SHORT] = {0};
	struct tm *t;
	time_t tt;

	time(&tt);
	t = localtime(&tt);
	sprintf(Timereport, "%4d-%02d-%02d-%02d:%02d:%02d",
		(t->tm_year + 1900), (t->tm_mon + 1), t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	//printf("time report: %s\n", Timereport);
	if (0 == strlen(vcc.Timeaccoff)) {
		memcpy(vcc.Timeaccoff, Timereport, sizeof(Timereport));
	}
	//printf("voltage:%f level:%d\n", voltage, vcc.level);

	sprintf(temp_str, "{\"recordtime\":\"%s\",\"accofftime\":\"%s\",\"currentvolt\":\"%f\",\"rated\":\"%d\"}",
		Timereport, vcc.Timeaccoff, vcc.voltage, vcc.level);
	//printf("%s\n", temp_str);
	if (logfp != (FILE*)0) {
		(void)fprintf(logfp, "%s\n", temp_str);
		(void)fflush(logfp);
	}

	return ;
}

void *monitor_vcc(void *arg)
{
	char str[STR_SIZE] = {0};

	while (1)
	{
		if (acc_monitor > 0) {
			PopenFile(GET_VOLTAGE_RAW, str, sizeof(str));
			if (0 != strlen(str)) {
				vcc.voltage = atof(str);
				//printf("voltage is %s, %f(v)\n", str, vcc.voltage);

				if (0 == vcc.level) {
					if (vcc.voltage < 17) vcc.level = 10;
					if (vcc.voltage >= 17) vcc.level = 22;
				}
				if (vcc.voltage < vcc.level) {
					report_vcc();
					printf("Warning: lowlevel:%d, voltage:%f, ShutDown Device Immediately.\n", vcc.level, vcc.voltage);
					set_offdelay_shutdown();
				}
				report_vcc();
			} else {
				printf("Warning: %s, %s, %s, cannot get voltage\n", __func__, GET_VOLTAGE_RAW, str);
			}
		}
		sleep(vcc.interval);
	}

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	int opt = 0;
	int Ret = 0;
	int interval = 5, intval = 0;
	char temp_str[STR_SIZE] = {0};
	char *logfile = (char*)0;
	pthread_t vcc_tid;

	/* Analytical parameters. */
	while ((opt = getopt(argc,argv,"t:l:p:v:d:h")) != -1) {
		switch (opt) {
			case 'h':
				printf("acc_main [-t interval] [-p accpath] [-d offdelay time]\n");
				printf("          -h (get help)\n");
				printf("          -t 5 (check interval 1~60 sec)\n");
				printf("          -v 10 (vcc report interval 1~60 sec)\n");
				printf("          -d 600 (offdelay time 1~1200 sec)\n");
				printf("          -l /tmp/vcc.log (log file for vcc)\n");
				printf("          -p /tmp/.acc/ (acc'status path)\n");
				return 0;				
			case 'l':
				if (optarg != NULL)
					logfile = optarg;
				break;
			case 'p':
				if (optarg != NULL)
					accpath = optarg;
				break;
			case 't':
				intval = strtoul(optarg, NULL, 10);
				if ((intval >= 1)&&(intval <= 60)) {
					interval = intval;
				}
				break;
			case 'v':
				intval = strtoul(optarg, NULL, 10);
				if ((intval >= 1)&&(intval <= 60)) {
					vcc.interval = intval;
				}
				break;
			case 'd':
				intval = strtoul(optarg, NULL, 10);
				if ((intval >= 1)&&(intval <= 1200)) {
					off_time = intval;
				}
				break;
			default:
				break;
		}
	}

	/* Log file. */
	if (logfile != (char*)0) {
		logfp = fopen(logfile, "a");
		if (logfp == (FILE*)0) {
			syslog(LOG_CRIT, "%s - %m", logfile);
			perror(logfile);
			exit(1);
		}
		if (logfile[0] != '/') {
			syslog(LOG_WARNING, "logfile is not an absolute path, you may not be able to re-open it");
			(void)fprintf(stderr, "%s: logfile is not an absolute path, you may not be able to re-open it\n", argv[0]);
		}
	}

	sprintf(temp_str, "mkdir -p %s", accpath);
	system(temp_str);
	
	/* Init mutex */
	pthread_mutex_init(&mutex, NULL);

#ifdef HI_UNF_GPIO
	//(HI_VOID)HI_SYS_Init();

	/* HI_UNF_GPIO Init. */
	Ret = HI_UNF_GPIO_Init();
	if (HI_SUCCESS != Ret)
	{
		printf("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, Ret);
		exit(-1);
	}
#endif

	
	Ret = pthread_create(&vcc_tid, NULL, monitor_vcc, NULL);
	if (Ret != 0)
                printf("can't create thread: %s\n", strerror(Ret));

	while (1) {
		monitor_acc(interval);
		sleep(interval);
	}
	
	//printf("led_main, led:%s status:%s interval:%d\n", led_main.name, led_main.status, led_main.interval);


	if (logfp != (FILE*)0)
	fclose(logfp);

#ifdef HI_UNF_GPIO
	Ret = HI_UNF_GPIO_DeInit();
	if (HI_SUCCESS != Ret)
	{
		printf("%s: %d ErrorCode=0x%x\n", __FILE__, __LINE__, Ret);
	}

#endif
	return Ret;
}

