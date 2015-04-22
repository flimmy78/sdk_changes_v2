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

#define GET_STATUS "cat %s/%s/status 2> /dev/null"
#define GET_INTVAL "cat %s/%s/interval 2> /dev/null" 
#define CHECK_PATH "ls %s 2> /dev/null"
#define OPEN_LED "gpio %s=0" 
#define CLOSE_LED "gpio %s=1" 
#define OPENLED 1
#define CLOSELED 0

#define DOWNMS 500
/* min time interval 100ms */
unsigned int mintval = 100;

pthread_mutex_t mutex;
static FILE *logfp = NULL;
const char  *ledpath = "/tmp/leds";

static struct led_struct {
	char name[STR_SHORT];
	char status[STR_SHORT]; //up,down,blink
	int  interval;  		//user set interval in seconds
	int  updefault; 	//set up times about min time interval
	int  downdefault;	//set down times about min time interval
	int  uptimes; 		//now up times about min time interval
	int  downtimes;	//now down times about min time interval
}led_main;

typedef struct led_struct elemType ;
/* 定义单链表结点类型 */
typedef struct Node{	
	elemType element;
	pthread_t id;
	struct Node *next;
}Node;
Node *ListHead = NULL;

void initList(Node **pNode)
{
	*pNode = NULL;
	printf("initList ok\n");

	return ;
}

void printList(Node *pHead)
{
	if (NULL == pHead) {
		printf("PrintList, list null\n");
	} else {
		while (NULL != pHead) {
			printf("%s, %s, %s, %d, %d, %d, %d\n", __func__, pHead->element.name,  pHead->element.status,  
				pHead->element.updefault,  pHead->element.downdefault, 
				pHead->element.uptimes, pHead->element.downtimes);
			pHead = pHead->next;
		}
		printf("\n");
	}
	
	return ;
}

int searchList(Node *pHead, char *name)
{
	if (NULL != pHead) {
        while (NULL != pHead) {
			if (0 == strcmp(pHead->element.name, name)) {
				printf("searchlist, led:%s status:%s interval:%d\n", pHead->element.name, pHead->element.status, pHead->element.interval);
				printf("\n");

				return 1;
			} else {
				pHead = pHead->next;
			}
        }
    }

	return 0;
}

void copy_elemType(elemType *oldElem, elemType *newElem)
{
	
	//printf("copy_elemType, old led:%s status:%s interval:%d\n", oldElem->name, oldElem->status, oldElem->interval);
	//printf("checklist, new led:%s status:%s interval:%d\n", newElem->name, newElem->status, newElem->interval);

	//memset(oldElem, 0, sizeof(elemType));
	memcpy(oldElem->name, newElem->name, sizeof(newElem->name));
	memcpy(oldElem->status, newElem->status, sizeof(newElem->status));
	oldElem->interval = newElem->interval;
	oldElem->updefault = newElem->updefault;
	oldElem->downdefault = newElem->downdefault;

	return ;
}

int updateList(Node *pHead, elemType *led)
{
	if (NULL != pHead) {
        while (NULL != pHead) {
			if (0 == strcmp(pHead->element.name, led->name)) {
				copy_elemType(&(pHead->element), led);				
				return 1;
			} else {
				pHead = pHead->next;
			}
        }
    }
	return 0;
}

void clearList(Node *pHead)
{
	Node *pNext;            //定义一个与pHead相邻节点

	if (pHead == NULL) {
		printf("clearList, list null\n");
		return;
	}
	while (pHead->next != NULL) {
		pNext = pHead->next;//保存下一结点的指针
		free(pHead);
		pHead = pNext;      //表头下移
	}
	printf("clearList, list clear\n");

	return ;
}

int sizeList(Node *pHead)
{
	int size = 0;

	while(pHead != NULL)
	{
		size++;         //遍历链表size大小比链表的实际长度小1
		pHead = pHead->next;
	}
	printf("sizeList, list length %d \n",size);
	return size;    //链表的实际长度
}

int isEmptyList(Node *pHead)
{
	if (pHead == NULL) {
		printf("isEmptyList, list is empty\n");
		return 1;
	}
	printf("isEmptyList, list is not empty\n");

	return 0;
}

int insertHeadList(Node **pNode, elemType insertElem)
{
	Node *pInsert;

	pInsert = (Node *)malloc(sizeof(Node));
	memset(pInsert, 0, sizeof(Node));
	copy_elemType(&(pInsert->element), &insertElem);

	pInsert->next = *pNode;
	*pNode = pInsert;

	return 0;
}

int insertLastList(Node **pNode,elemType insertElem)
{
	Node *pInsert;
	Node *pHead;
	Node *pTmp; //定义一个临时链表用来存放第一个节点

	pHead = *pNode;
	pTmp = pHead;
	pInsert = (Node *)malloc(sizeof(Node)); //申请一个新节点
	memset(pInsert, 0, sizeof(Node));
	pInsert->element = insertElem;

	while(pHead->next != NULL)
	{
		pHead = pHead->next;
	}
	pHead->next = pInsert;   //将链表末尾节点的下一结点指向新添加的节点
	*pNode = pTmp;
	printf("insertLastList, insert to tail\n");

	return 1;
}

/*
int main()
{
    Node *pList=NULL;
    int length = 0;
 
    elemType posElem;
 
    initList(&pList);       //链表初始化
    printList(pList);       //遍历链表，打印链表
 
    pList=creatList(pList); //创建链表
    printList(pList);
     
    sizeList(pList);        //链表的长度
    printList(pList);
 
    isEmptyList(pList);     //判断链表是否为空链表
     
    posElem = getElement(pList,3);  //获取第三个元素，如果元素不足3个，则返回0
    printf("getElement, 3 element is %d\n",posElem);   
    printList(pList);
 
    getElemAddr(pList,5);   //获得元素5的地址
 
    modifyElem(pList,4,1);  //将链表中位置4上的元素修改为1
    printList(pList);
 
    insertHeadList(&pList,5);   //表头插入元素12
    printList(pList);
 
    insertLastList(&pList,10);  //表尾插入元素10
    printList(pList);
 
    clearList(pList);       //清空链表
     
}
*/
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

void get_content(struct dirent *ptr, elemType *led)
{
	char buf[RD_SIZE];
	char buf1[RD_SIZE];
	
	memset(led, 0, sizeof(elemType));

	/* Get led blink interval */
	memset(buf, 0, strlen(buf));
	memset(buf1, 0, strlen(buf1));
	sprintf(buf, GET_INTVAL, ledpath, ptr->d_name);
	PopenFile(buf, buf1, sizeof(buf1));
	if (0 == atoi(buf1)) {
		led->interval = 2;
	} else {
		led->interval = atoi(buf1);
	}

	/* Get led status: include up,down,blink,NA */
	memset(buf, 0, strlen(buf));
	memset(buf1, 0, strlen(buf1));
	sprintf(buf, GET_STATUS, ledpath, ptr->d_name);
	PopenFile(buf, buf1, sizeof(buf1));
	if (0 == strcmp(buf1, "up")) {
		memcpy(led->status, buf1, strlen(buf1));
		led->updefault = (led->interval * 1000)/mintval;
	} else if (0 == strcmp(buf1, "down")) {
		memcpy(led->status, buf1, strlen(buf1));
		led->downdefault = (led->interval * 1000)/mintval;
	} else if (0 == strcmp(buf1, "blink")) {
		memcpy(led->status, buf1, strlen(buf1));
		led->updefault = (led->interval * 1000 - DOWNMS)/mintval;
		led->downdefault = DOWNMS/mintval;	
	} else {
		memcpy(led->status, "NA", strlen("NA"));		
	}
	
	/* Get led name */
	memcpy(led->name, ptr->d_name, strlen(ptr->d_name));

	return ;
}

int check_content(Node **pNode, struct dirent *ptr) 
{
	int Ret = -1;
	elemType *led = NULL;
	
	led = (elemType *)malloc(sizeof(elemType));
	if (led == NULL) {
		printf("mallocerror!\n");
		exit(-1);
	}
	
	get_content(ptr, led);
	Ret = updateList(*pNode, led);
	if (0 == Ret) {
		insertHeadList(pNode, *led);
	}
	
	/* set main led struct */
	if (0 == strcmp(led->name, "main")) {
		copy_elemType(&(led_main), led);		
	}
	
	free(led);
	led = NULL;
	return Ret;
}

void dir_pthread(void *arg)
{
	int intval = *(int *)arg;
	DIR *dir;
	struct dirent *ptr;

	while (1) {
		dir = opendir(ledpath);
		while ((ptr = readdir(dir)) != NULL) {
			printf("d_name: %s\n", ptr->d_name);
			if (0 == strcmp(ptr->d_name, "lock")||0 == strcmp(ptr->d_name, ".")||0 == strcmp(ptr->d_name, ".."))
				continue ;

			check_content(&ListHead, ptr);
		}
		closedir(dir);
		sleep(intval);
	}
	pthread_exit(NULL);
}

int dir_dopthread(int *interval)
{
	pthread_t id;
	int *test = interval;
	int Ret = 0;
	
	Ret = pthread_create(&id, NULL, (void *)dir_pthread, test);
	if (Ret != 0) {
		printf ("Create pthread error!\n");
		return Ret;
	}

	//pthread_join(id, NULL);
	return Ret;
}


#ifdef HI_UNF_GPIO
int open_led(int gpio)
{
	int Ret = 0;
	
	pthread_mutex_lock(&mutex);
	printf("gpio %d, open\n", gpio);
	Ret = HI_UNF_GPIO_SetDirBit(gpio, HI_FALSE);
	Ret = HI_UNF_GPIO_WriteBit(gpio, HI_FALSE);
	pthread_mutex_unlock(&mutex);

	return Ret;
}
int close_led(int gpio)
{
	int Ret = 0;
	
	pthread_mutex_lock(&mutex);
	printf("gpio %d, close\n", gpio);
	Ret = HI_UNF_GPIO_SetDirBit(gpio, HI_FALSE);
	Ret = HI_UNF_GPIO_WriteBit(gpio, HI_TRUE);
	pthread_mutex_unlock(&mutex);

	return Ret;
}
#else
int open_led(char *name)
{
	int Ret = 0;
	char buf[STR_SHORT] = {0};
	char buf1[STR_SHORT] = {0};
	
	pthread_mutex_lock(&mutex);
	sprintf(buf, OPEN_LED, name);
	printf("%s\n", buf);
	PopenFile(buf, buf1, sizeof(buf1));
	pthread_mutex_unlock(&mutex);

	return Ret;
}
int close_led(char *name)
{
	int Ret = 0;
	char buf[STR_SHORT] = {0};
	char buf1[STR_SHORT] = {0};
	
	pthread_mutex_lock(&mutex);
	sprintf(buf, CLOSE_LED, name);
	printf("%s\n", buf);
	PopenFile(buf, buf1, sizeof(buf1));
	pthread_mutex_unlock(&mutex);

	return Ret;
}
#endif

void ctrl_led(char *ledname, int status)
{
	struct gpio *gpio;

#ifdef HI_UNF_GPIO
	gpio = getgpio_byname(ledname);
	if (NULL != gpio) {
		if (OPENLED == status) {
			open_led(gpio->number);
		} else if (CLOSELED == status) {
			close_led(gpio->number);
		} else {
			printf("warning: %s, led:%s, status:%d\n", __func__, ledname, status);
		}
	}
#else
	gpio = getgpio_byname(ledname);
	if (NULL != gpio) {
		if (OPENLED == status) {
			open_led(ledname);
		} else if (CLOSELED == status) {
			close_led(ledname);
		} else {
			printf("warning: %s, led:%s, status:%d\n", __func__, ledname, status);
		}
	}
#endif
	return ;
}

void control_led(elemType *led)
{ 
	elemType *led_tmp = NULL;

	if (0 != strcmp(led_main.status, "NA")) {
		led_tmp = &led_main;
	} else {
		led_tmp = led;
	}

	if (led->status == NULL || led_tmp->status == NULL) {
		return ;
	}
	/*
	printf("This is %s,  (%s, %s, %d, %d, %d, %d, %d)\n", __func__, 
		led->name, led->status, led->interval, led->updefault, led->downdefault, led->uptimes, led->downtimes);
	*/
	
	if (0 == strcmp(led_tmp->status, "blink")) {
		if ((led->uptimes <= 0)&&(led->downtimes <= 0)) {
			led->uptimes = led_tmp->updefault;
			led->downtimes = led_tmp->downdefault;

			if (led_tmp->updefault > 0) {
				ctrl_led(led->name, OPENLED);
			} else if (led->downdefault > 0) {
				ctrl_led(led->name, CLOSELED);
			}
		} else if (led->uptimes > 0) {
			led->uptimes --;
			if (led->uptimes == 0) {
				ctrl_led(led->name, CLOSELED);
				//close_led(led->name);
			}
		} else if ((led->uptimes == 0)&&(led->downtimes > 0)) {
			led->downtimes --;
		} else {
			printf("Warning: %s, uptimes:%d, downtimes:%d\n", __func__, led->uptimes, led->downtimes);
		}
	} else if (0 == strcmp(led_tmp->status, "up")) {
		if ((led_tmp->downdefault > 0)||(led->downdefault > 0)||(led->downtimes > 0)) {
			led_tmp->downdefault = 0;
			led->downdefault = 0;
			led->downtimes = 0;
			led->uptimes = 0;
		}
		if ((led->uptimes <= 0)) {
			led->uptimes = led_tmp->updefault;
			ctrl_led(led->name, OPENLED);
		}
	} else if (0 == strcmp(led_tmp->status, "down")) {
		if ((led_tmp->updefault > 0)||(led->updefault > 0)||(led->uptimes > 0)) {
			led_tmp->updefault = 0;
			led->updefault = 0;
			led->uptimes = 0;
			led->downtimes = 0;
		}
		if ((led->downtimes <= 0)) {
			led->downtimes = led_tmp->downdefault;
			ctrl_led(led->name, CLOSELED);
		}
	} else if (0 != strcmp(led_tmp->status, "NA")) {
		printf("Warning: %s, led: %s, status:%s, uptimes:%d, downtimes:%d\n", 
			__func__, led_tmp->name, led_tmp->status, led->updefault, led->downdefault);
	}

	led_tmp = NULL;
	return ;
}

void monitor_led(Node *pHead)
{
	if (NULL != pHead) {
		while (NULL != pHead) {
			control_led(&(pHead->element));
			/*
			printf("%s, %s, %d, %d, %d, %d\n", __func__, pHead->element.name,  
				pHead->element.updefault,  pHead->element.downdefault, 
				pHead->element.uptimes, pHead->element.downtimes);
			*/
			pHead = pHead->next;
		}
	}
	return ;
}

int check_ledpath(void)
{
	char buf[STR_SHORT] = {0};
	char buf1[STR_SHORT] = {0};

	sprintf(buf, CHECK_PATH, ledpath);
	PopenFile(buf, buf1, sizeof(buf1));
	//printf("buf:%s, buf1:%s\n", buf, buf1);
	
	if (0 == strlen(buf1)) {	
		printf("warning: cannot file in ledpath=%s, please use -p [ledpath]\n", ledpath);
		exit(-1);
	}
	
	return 0;
}

int main(int argc, char *argv[])
{
	int opt, count = 0;
	int Ret = 0;
	int intval = 5;
	char *logfile = (char*)0;

	/* Analytical parameters. */
	while ((opt = getopt(argc,argv,"t:l:p:h")) != -1) {
		switch (opt) {
			case 'h':
				printf("led_main [-t interval] [-p ledpath]\n");
				printf("          -h (get help)\n");
				printf("          -t 5 (check interval 1~60 sec)\n");
				printf("          -l /tmp/led.log (log file for led)\n");
				printf("          -p /tmp/leds/ (leds'status path)\n");
				return 0;				
			case 'l':
				logfile = optarg;
				break;
			case 'p':
				if (optarg != NULL)
				ledpath = optarg;
				break;
			case 't':
				intval = strtoul(optarg, NULL, 10);
				if ((intval < 1)||(intval > 60)) {
					printf("		  -t 5 (check interval 1~60 sec)\n");
					intval = 5;
				}
				break;
			default:
				break;
		}
	}
	/* check ledpath */
	check_ledpath();

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

	/* Init mutex */
	pthread_mutex_init(&mutex,NULL);

	/* Init List. */
	initList(&ListHead);

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

	dir_dopthread(&intval);

	/* Get dir list. */
	//printList(ListHead);

	while (1) {
		monitor_led(ListHead);
		if (count == 10) {
			printList(ListHead);
			count = 0;
		}
		count ++;
		usleep(mintval*1000);
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

