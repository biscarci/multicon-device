#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <signal.h>
#include "json.h"

#include "device_settings.h"
#include "device_mqtt.h"
#include "device_utils.h"
#include "system_logger.h"


#define VERSION "0.0.4"

int init_modules(void);
void handle_user_interrupt(int);
void print_start_header();

int main(int argc, char *argv[]) 
{
  signal(SIGINT, handle_user_interrupt);
  print_start_header();
  
  system_logger(LOGGER_INFO, "MAIN", "Start Multicon device daemon");

  init_modules();
  device_mqtt_run();

  for (;;){}
  
  return 0;

}

void  handle_user_interrupt(int sig)
{
  char  c;

  signal(sig, SIG_IGN);
  printf("\n");
  system_logger(LOGGER_WARN, "MAIN", "Did you hit Ctrl-C? Do you really want to quit? [y/n] ");
  signal(SIGINT, handle_user_interrupt);
  exit(0);
  c = getchar();
  if (c == 'y' || c == 'Y')
    exit(0);
  else
    signal(SIGINT, handle_user_interrupt);
  getchar(); // Get new line character
}


void print_start_header()
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
}

int init_modules(void)
{

  static int init = 0;

  if(!init)
  {
    system_logger(LOGGER_INFO, "MAIN", "Initializing Multicon modules");

    device_mqtt_init();
    init = 1;
  }
  return init;
}