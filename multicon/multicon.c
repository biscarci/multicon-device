#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <signal.h>
#include "json.h"
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>


#include "device_settings.h"
#include "device_mqtt.h"
#include "device_utils.h"
#include "system_logger.h"
#include "http_server.h"

#define VERSION "1.5"

// System functions 
void handle_user_interrupt(int);

// Module functions
int multicon_init(void);
void multicon_run(void);

int main(int argc, char *argv[]) 
{
    printf("\n");
    printf("   __  ___     ____  _                 ___                \n");
    printf("  /  |/  /_ __/ / /_(_)______  ___    / _ | ___  ___      \n");
    printf(" / /|_/ / // / / __/ / __/ _ \\/ _ \\  / __ |/ _ \\/ _ \\ \n");
    printf("/_/  /_/\\_,_/_/\\__/_/\\__/\\___/_//_/ /_/ |_/ .__/ .__/ \n");
    printf("                                         /_/  /_/         \n");
    printf("----------------------------------------------------------\n");  
    printf("\t\tApplication version: %s \n", VERSION);
    printf("----------------------------------------------------------\n\n"); 
    
    
    signal(SIGINT, handle_user_interrupt);
    pid_t cpid;
    int status;
    cpid = fork();
    int started = 0;
    for (;;)
    {
      switch (cpid)
      {
        case -1 : //Fork failure
          exit(EXIT_FAILURE);
        break;

        case 0 : //Child Process
          if(!started)
          {
              system_logger(LOGGER_INFO, "PROCMON", "Multicon process monitor is started");
              multicon_run();
              started = 1;
              return 0; //exit child process
          }
        break;

        default : //Parent process
          if (waitpid(-1, &status, WNOHANG) > 0) 
          {
              system_logger(LOGGER_ERROR, "PROCMON", "Multicon process was aborted abnormally, now it will restarted");
              cpid = fork();
          }
        break;
      }
      sleep(20);
    }
  return 0;
}


int multicon_init(void)
{
  static int init = 0;

  if(!init)
  {

    device_mqtt_init();
    device_settings_init();
    http_server_init();
    logger_init();
    init = 1;
  }


  system_logger(LOGGER_INFO, "MAIN", "Multicon modules initialized");

  return init;
}


void multicon_run(void)
{
  
  system_logger(LOGGER_INFO, "MAIN", "Start Multicon device daemon");
  multicon_init();

  for (;;)
  { 
    /* struct timeval stop, start;
    gettimeofday(&start, NULL); */

    device_settings_run();
    device_mqtt_run();
    http_server_run();
    usleep(500);
    
    /* gettimeofday(&stop, NULL);
    printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);  */
  } 
}


void multicon_stop(void)
{
  device_mqtt_stop();
}


void  handle_user_interrupt(int sig)
{
  char  c;

  signal(sig, SIG_IGN);
  system_logger(LOGGER_WARN, "MAIN", "Did you hit Ctrl-C? Do you really want to quit? [y/n] ");
  signal(SIGINT, handle_user_interrupt);
  exit(0);
  c = getchar();
  if (c == 'y' || c == 'Y')
  {
    multicon_stop();
    exit(0);
  }
  else
  {
    signal(SIGINT, handle_user_interrupt);
  }
  getchar(); // Get new line character
}