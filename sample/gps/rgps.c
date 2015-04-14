#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>
#include<netdb.h>
#include<sys/socket.h>
#include<stdarg.h>
#include<syslog.h>
#include<time.h>

#define RD_SIZE 1024
#define STR_SIZE 256
static struct gps_struct {
	char str_host[20];
	char east_west;
	char north_south;
	char gps_Lng[20];
	char gps_Lat[20];
	char gps_Date[30];
	char gps_Time[30];
	char gps_Velocity[10];
	char gps_Orientation[10];
	char gps_Elevation[10];
	int gps_satellite;
}rgps;
static FILE* logfp;
int flag_GPGGA;
int flag_GPRMC;
char Timereport[50] = {0};
const char *logpath = "/tmp/.gps";

int print_timestr(char *str)
{
	char *sep = ":";
	char *token;
	unsigned long tm_int[3]; 
	int i = 1;

	if (NULL == str) {
		return -1;
	}

	token = strtok(str, sep);
	if (NULL != token) {
		tm_int[0] = strtoul(token, NULL, 10);
	}

	while ((token != NULL)&&(i < 3)) {
		token = strtok(NULL, sep);
		if (NULL != token) {
			tm_int[i] = strtoul(token, NULL, 10);
		}
		i++;
	}

	tm_int[0] = tm_int[0] + 8;
	if (tm_int[0] > 24) {
		tm_int[0] = tm_int[0]- 24;
	}

	//printf("Time      %02d:%02d:%02d \n", tm_int[0], tm_int[1], tm_int[2]);
	sprintf(rgps.gps_Time, "%02d:%02d:%02d", tm_int[0], tm_int[1], tm_int[2]);

	return 0;
}
int PopenFile (char *cmd_str, char *str, int len)
{
	FILE *fp=NULL; 

	if (cmd_str == NULL||str == NULL)
		return -1;

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
void print_gpsinfo(char *buf)
{
	char ReceivingF = 0;
	char EndF = 0;
	char NewByteF = '0';

	unsigned char GPS_time[9] = {0};			//UTCê±??
	unsigned char GPS_wd[20] = {0};				//?3?è
	unsigned char GPS_jd[12] = {0};				//?-?è
	unsigned char GPS_status = 0;				// ?¨??×′ì?
	unsigned char GPS_alt[8] = {0};				//o￡°?
	unsigned char GPS_sv[3] = {0};				//ê1ó??àD?
	unsigned char GPS_precision[6] = {0};		//???????è
	unsigned char GPS_speed[6] = {0};			//?ù?è
	unsigned char GPS_direction[7] = {0};		//·??ò??
	unsigned char GPS_date[9] = {0};			//UTCè??ú	
	unsigned char GPS_warn = 0;					//?¨???ˉ??
	unsigned char Segment = 0;					//?oo???êy
	unsigned char null_flag = 0;
	unsigned char Bytes_counter = 0;
	unsigned char Command = 0;

	if (buf == NULL) {
		return;
	}
	if (*buf == '\n') {
		*buf ++;
	}
	if (*buf == '$') {				//?eê?±ê??
		ReceivingF = 1;
		Bytes_counter = 0;
		Segment = 0;   				//????ó???????êy?÷
		*buf ++;
	}
	while (ReceivingF)
	{                
		if (buf == NULL) {
			return;
		}
		if (*buf == ',') {
			++Segment;
			*buf ++;
			Bytes_counter = 0;		//??????×??ú??êy?÷
		}
		if (*buf == '*') {			//ê?μ??áê?±ê??
			if ((Command == 1)&&(flag_GPGGA == 0)) {
				print_timestr(GPS_time);
				flag_GPGGA=1;

				rgps.gps_satellite = atoi(GPS_sv);
				memset(rgps.gps_Lng, 0, sizeof(rgps.gps_Lng));
				memcpy(rgps.gps_Lng, GPS_jd, strlen(GPS_jd));
				memset(rgps.gps_Lat, 0, sizeof(rgps.gps_Lat));
				memcpy(rgps.gps_Lat, GPS_wd, strlen(GPS_wd));
				memset(rgps.gps_Elevation, 0, sizeof(rgps.gps_Elevation));
				memcpy(rgps.gps_Elevation, GPS_alt, strlen(GPS_alt));
			}
			if (Command == 2&&(flag_GPRMC == 0)) {
				flag_GPRMC=1;

				memset(rgps.gps_Velocity, 0, sizeof(rgps.gps_Velocity));
				memcpy(rgps.gps_Velocity, GPS_speed, strlen(GPS_speed));
				memset(rgps.gps_Date, 0, sizeof(rgps.gps_Date));
				sprintf(rgps.gps_Date, "20%s", GPS_date);
				//memcpy(rgps.gps_Date, GPS_date, strlen(GPS_date));
				memset(rgps.gps_Orientation, 0, sizeof(rgps.gps_Orientation));
				memcpy(rgps.gps_Orientation, GPS_direction, strlen(GPS_direction));
			}
			return;
		}
		switch (Segment) {					//分段处理
			case 0:
				if (Bytes_counter == 3) 	//段0，语句类型判断
				switch (*buf) {
					case 'G':
						if (1 == flag_GPGGA) {	
							Command = 0;
							ReceivingF = 0;
						} else
							Command = 1;	//语句类型 $GPGGA
						break;
					case 'M':
						if (1 == flag_GPRMC) {
							Command = 0;
							ReceivingF = 0;
						} else						
							Command = 2;	//语句类型 $GPRMC
						break;
#if 0
					case 'T':
						Command = 3;		//this is GPVTG
						break;	
#endif
					default:
						Command = 0; 		//非有效数据类型，终止当前数据接收
						ReceivingF = 0;
					break;
				}
				break;
			
			case 1:
				switch (Command) {
					case 1:        
						if ((Bytes_counter == 2)||(Bytes_counter == 5)) {        //$GPGGA段1，UTC时间，hhmmss（时分秒）格式,取前6位 转换为HH:MM:SS格式
				        	GPS_time[Bytes_counter] = *buf;
				        	GPS_time[Bytes_counter] = ':';
							Bytes_counter++;
						}
						if (Bytes_counter < 8) {
							GPS_time[Bytes_counter] = *buf;
						}
						if (Bytes_counter == 8) {
							GPS_time[Bytes_counter]='\0';
						}
				    	break;

					case 2:
						break;						//$GPRMC第1段处理 忽略
					case 3:
						if (Bytes_counter < 6) {	//$GPVTG第1段处理 地面航向
							GPS_direction[Bytes_counter] = *buf;
							GPS_direction[Bytes_counter+1] = '\0';
						}
						break;
				}
				break;

			case 2:
				switch (Command) {
					case 1:        
				        if (Bytes_counter == 10) {
							GPS_wd[Bytes_counter] = '\0';
							break;
				        }
						if (Bytes_counter == 2) {               //$GPGGA 段2处理 纬度ddmm.mmmm（度分）格式
							GPS_wd[Bytes_counter] = '.';        //接收第二个字节后插入'.'
			                ++ Bytes_counter;
				        }
						if (Bytes_counter == 5) {				//去掉分mm.mmmm之间的"."
							*buf ++;
							GPS_wd[Bytes_counter] = *buf;
						}
						else
							GPS_wd[Bytes_counter] = *buf;   
						break;
					case 2:        
						GPS_warn = *buf;						//$GPRMC第二段处理 定位状态，A=有效定位，V=无效定位
 						if ('A' != GPS_warn) {
							Command = 0;
							ReceivingF = 0;
						}
						break;
				}
				break;
			
			case 3:
				switch (Command) {
					case 1:        
						rgps.north_south = *buf;	//$GPGGA第3段处理 纬度半球N（北半球）或S（南半球）
						break;
					case 2:							//$GPRMC第3段处理 忽略
					    break;
				}
				break;
				
			case 4:
				switch (Command) {
					case 1:        
				        if (Bytes_counter == 11) {
							GPS_jd[Bytes_counter] = '\0';
							break;
						}
						if (Bytes_counter == 3) {               //$GPGGA 段4处理 经度dddmm.mmmm（度分）格式
			                GPS_jd[Bytes_counter] = '.';        //接收第3个字节后插入'.'
			                ++ Bytes_counter;
				        }
						if (Bytes_counter == 6) {
							*buf ++;
							GPS_jd[Bytes_counter] = *buf;
							break;
						}
						else
							GPS_jd[Bytes_counter] = *buf;          
						break;
					case 2:										//$GPRMC第4段 忽略
						break;
					case 3:
						if (Bytes_counter < 5) {				//$GPVTG 第4段处理 地面速率（000.0~999.9节，前面的0也将被传输）
							GPS_speed[Bytes_counter] = *buf;
							GPS_speed[Bytes_counter + 1] = '\0';
						}
						break;
				}
				break;
				
			case 5:        
				switch (Command) {
					case 1:        
						rgps.east_west = *buf;		//$GPGGA第5段处理 经度半球E（东经）或W（西经）
						break;
					case 2:                         //$GPRMC第5段处理 忽略
						break;
				}
				break;
			
			case 6:        
				switch (Command) {
					case 1:        
						GPS_status = *buf;         //$GPGGA第6段处理 GPS状态：0=未定位，1=非差分定位，2=差分定位，6=正在估算
						if ('0' == GPS_status) {
							Command = 0;
							ReceivingF = 0;
						}
					    break;
					case 2:							//$GPRMC第6段处理 忽略
						break;
				}
				break;  
			
			case 7:        
				switch (Command) {			                                                          
					case 1:        
						if (Bytes_counter < 2)		//$GPGGA第7段处理  正在使用解算位置的卫星数量（00~12）（前面的0也将被传输）
							GPS_sv[Bytes_counter] = *buf;
						GPS_sv[2] = '\0';         
						break;
					case 2:       	
	 					if (Bytes_counter < 5) {	//$GPRMC第7段处理 地面速率（000.0~999.9节，前面的0也将被传输）
							GPS_speed[Bytes_counter] = *buf;
							GPS_speed[Bytes_counter + 1] = '\0';
						}
						break;
				}
				break;   
			case 8:        
				switch (Command) {                                                          
					case 1:        
						/*if(Bytes_counter<5) {			//$GPGGA第8段处理  水平精度
							//GPS_precision[Bytes_counter] = *buf;
							//GPS_precision[Bytes_counter+1] = '\0';         
						}*/
						break;
					case 2:  
						if (*buf == ',') {
							null_flag = 1;
						} else {   
	 						if (Bytes_counter < 6) {	//$GPRMC第8段处理 地面航向
							
								GPS_direction[Bytes_counter] = *buf;
								GPS_direction[Bytes_counter + 1] = '\0';
							}
						}
						break;
				}
				break;  	
			case 9:        
				switch (Command)
				{
					case 1:        
						if (Bytes_counter < 7) {		//$GPGGA第9段处理 海拔高度（-9999.9~99999.9）
							GPS_alt[Bytes_counter] = *buf;
							GPS_alt[Bytes_counter + 1] = '\0';
				        } 
						break;
					case 2:        
						if (Bytes_counter < 2) {		//$GPRMC第9段处理 UTC日期，ddmmyy（日月年）格式转换为yy.mm.dd
							GPS_date[6 + Bytes_counter] = *buf;
				        }
				        if ((Bytes_counter > 1)&&(Bytes_counter < 4)) {	//月
							GPS_date[1 + Bytes_counter] = *buf;
							GPS_date[5] = '-';
				        }
				        if ((Bytes_counter > 3)&&(Bytes_counter < 6)) {	//年
							GPS_date[Bytes_counter-4] = *buf;
							GPS_date[2] = '-';
							GPS_date[8] = '\0';
				        }
				        break;
				}
				break;
				
			default:
				break;
		}
		if (!null_flag) {
			++Bytes_counter;
			*buf ++;
		} else {
			++Segment;
			*buf ++;
		}
		null_flag = 0;
	}
		
	NewByteF = 0;
}

int gps_report(int status)
{
	char temp_str[STR_SIZE] = {0};
	char temp_str1[STR_SIZE] = {0};
	
	struct tm *t;
	time_t tt;

	if (((rgps.east_west != 'E')&&(rgps.east_west != 'W'))||((rgps.north_south != 'N')&&(rgps.north_south != 'S'))) {
		status = 0;
	}

	if (0 == status) {
		//sprintf(temp_str, "{\"status\":\"%d\",\"latitude\":\"%s\",\"east_west\":\"%c\",\"date\":\"%s-%s\",\"speed\":\"%s\",\"north_south\":\"%c\",\"longitude\":\"%s\",\"height\":\"%s\"}", status, rgps.gps_Lat, rgps.east_west, rgps.gps_Date, rgps.gps_Time, rgps.gps_Velocity, rgps.north_south, rgps.gps_Lng, rgps.gps_Elevation);

		
		time(&tt);
		t = localtime(&tt);
		
		memset(Timereport, 0, sizeof(Timereport));
		if (10 == (strlen(rgps.gps_Date))&&(8 == strlen(rgps.gps_Time))) {
			sprintf(Timereport, "%s-%s", rgps.gps_Date, rgps.gps_Time);
		} else if (8 == strlen(rgps.gps_Time)) {
			sprintf(temp_str1, "%4d-%02d-%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday);
			sprintf(Timereport, "%s-%s", temp_str1, rgps.gps_Time);
		} else {
			sprintf(temp_str1, "%4d-%02d-%02d-%02d:%02d:%02d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
			sprintf(Timereport, "%s", temp_str1);
		}

		sprintf(temp_str, "{\"status\":\"%d\",\"latitude\":\"\",\"east_west\":\"\",\"date\":\"%s\",\"speed\":\"\",\"north_south\":\"\",\"longitude\":\"\",\"height\":\"\"}", status, Timereport);
		printf("%s\n", temp_str);
		system("/usr/sbin/gps_invalid.sh");
	} else {
		sprintf(temp_str, "{\"status\":\"%d\",\"latitude\":\"%s\",\"east_west\":\"%c\",\"date\":\"%s-%s\",\"speed\":\"%s\",\"north_south\":\"%c\",\"longitude\":\"%s\",\"height\":\"%s\"}",
		status, rgps.gps_Lat, rgps.east_west, rgps.gps_Date, rgps.gps_Time, rgps.gps_Velocity, rgps.north_south, rgps.gps_Lng, rgps.gps_Elevation);
		sprintf(Timereport, "%s-%s", rgps.gps_Date, rgps.gps_Time);
		printf("%s\n", temp_str);
		system("/usr/sbin/gps_valid.sh");
	}
	if (logfp != (FILE*)0) {	
		(void)fprintf(logfp, "%s\n", temp_str);
		(void)fflush(logfp);
	}

	return status;
}

void gps_debug(int status)
{
	char temp_str[STR_SIZE] = {0};

	sprintf(temp_str, "echo %d > %s/status", status, logpath);
	system(temp_str);

	memset(temp_str, 0, sizeof(temp_str));
	sprintf(temp_str, "echo %c > %s/east_west", rgps.east_west, logpath);
	system(temp_str);

	memset(temp_str, 0, sizeof(temp_str));
	sprintf(temp_str, "echo %c > %s/north_south", rgps.north_south, logpath);
	system(temp_str);

	memset(temp_str, 0, sizeof(temp_str));
	sprintf(temp_str, "echo %s > %s/gps_Lat", rgps.gps_Lat, logpath);
	system(temp_str);

	memset(temp_str, 0, sizeof(temp_str));
	sprintf(temp_str, "echo %s > %s/gps_Lng", rgps.gps_Lng, logpath);
	system(temp_str);

	memset(temp_str, 0, sizeof(temp_str));
	sprintf(temp_str, "echo %s > %s/gps_Velocity", rgps.gps_Velocity, logpath);
	system(temp_str);

	memset(temp_str, 0, sizeof(temp_str));
	sprintf(temp_str, "echo %s > %s/gps_Elevation", rgps.gps_Elevation, logpath);
	system(temp_str);

	memset(temp_str, 0, sizeof(temp_str));
	sprintf(temp_str, "echo %s > %s/gps_time", Timereport, logpath);
	system(temp_str);

	memset(temp_str, 0, sizeof(temp_str));
	sprintf(temp_str, "echo %d > %s/gps_satellite", rgps.gps_satellite, logpath); 	
	system(temp_str);

	return;
}

int main(int argc, char *argv[])
{
	int opt, intval = 15;
	char temp_str[STR_SIZE] = {0};
	int fd, nread, i = 0, j = 0, status = 0;
	char buf[RD_SIZE];
	char buf1[RD_SIZE] = {0};
	char *dev_path = (char*)0;
	char *dev_data = "/tmp/.gps.log";
	char *logfile = (char*)0;
	struct timeval tv_begin, tv_end;
	int tv_intval;
	
	while ((opt = getopt(argc,argv,"t:d:l:p:u:h")) != -1) {
		switch (opt) {
			case 'h':
				printf("rgps [-d /dev/ttyAMA2] [-t interval] [-l logpath]\n");
				printf("          -h (get help)\n");
				printf("          -d [gps_device or gps_log] (tty device or nmea file)\n");
				printf("          -t 15 (report interval 5~300 sec)\n");
				printf("          -l /tmp/gps.log (log file for gps)\n");
				printf("          -p /tmp/.gps/ (log path for gps)\n");
				return 0;				
			case 'd':
				dev_path = optarg;
				break;
			case 'l':
				logfile = optarg;
				break;
			case 'p':
				if (optarg != NULL)
					logpath = optarg;
				break;
			case 't':
				intval = strtoul(optarg, NULL, 10);
				if ((intval < 5)||(intval > 300)) {
					printf("		  -t 15 (report interval 5~300 sec)\n");
					intval = 15;
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
			syslog( LOG_WARNING, "logfile is not an absolute path, you may not be able to re-open it" );
			(void) fprintf( stderr, "%s: logfile is not an absolute path, you may not be able to re-open it\n", argv[0] );
		}
	}

	sprintf(temp_str, "mkdir -p %s", logpath);
	system(temp_str);
	printf("\n GPS positioning ...\n");
	memset(temp_str, 0, sizeof(temp_str));
	sprintf(temp_str, "head -n 30 %s > %s ; cp %s %s", dev_path, dev_data, dev_data, logpath);
	while(1) {
		printf("\n GPS waiting ...\n");
		flag_GPGGA = 0;
		flag_GPRMC = 0;
		gettimeofday(&tv_begin, NULL);

		do {
			printf("\n GPS reading ...\n");
			system(temp_str);
			fd = open(dev_data, O_RDONLY);
			if (fd == -1) {
				printf("open dev %s fail !!\n", dev_data);
				exit(1);
			}
			memset(buf, 0, RD_SIZE);
			memset(buf1, 0, RD_SIZE);
			nread = read(fd, buf, RD_SIZE-1);
			close(fd);

			if (nread > 0) {
				printf("\n GPS DATALen=%d\n", nread);
				buf[nread] = '\0';
				i = 0;
				j = 0;
				//printf("%s           %d\n\n", buf, nread);			
				while (nread > 1) {
					buf1[j] = buf[i];
					i++;
					j++;
					if ('\n' == buf[i]) {
						buf1[j] = '\0';
						//printf( "\n\n\n-----%d----------buf1 %s\n", nread, buf1);
						print_gpsinfo(buf1);
						memset(buf1, 0, RD_SIZE);
						j = 0;
						i ++;
					}
					nread --;
				}
				if ((1 == flag_GPGGA)&&(1 == flag_GPRMC)) {
					status = gps_report(1);
					gps_debug(status);
					break;
				}				
			}
			else
				printf("\n read fail %d\n", nread);

			gettimeofday(&tv_end, NULL);
			tv_intval = tv_end.tv_sec - tv_begin.tv_sec;
			printf("begin:%d, end:%d, interval:%d\n", tv_begin.tv_sec, tv_end.tv_sec, tv_intval);
			if (tv_intval > 3) {
				status = gps_report(0);				
				gps_debug(status);
				break;
			}
		} while ((1 != flag_GPGGA)||(1 != flag_GPRMC));
		printf("\n GPS closeing ...\n");
		sleep(intval);
	}

	if (logfp != (FILE*)0)
		fclose(logfp);

	return 0;
}

