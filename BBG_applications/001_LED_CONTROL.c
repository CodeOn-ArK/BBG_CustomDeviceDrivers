/*
 ============================================================================
 Name        : Test.c
 Author      : ArK
 Version     : 1.0
 Copyright   : GPL
 Description : blinkin' LEDs in BBG, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SYSFS_PATH	"/sys/class/leds"
#define USER_LED	"/beaglebone:green:usr" "%d/"

char *led_path = NULL;
void print_usage(void);
void test_cmd_input(const char *str,const char *cmd_val);
void set_brightness(const char *cmd_val);
void set_action_trigger(const char *cmd_val);

int main(int argc, char *argv[])
{
  if((argc == 2) && !strcmp(argv[1],"reset")){
        int i=0;
        led_path = (char *)malloc(strlen(SYSFS_PATH USER_LED) + strlen("trigger"));

        while(i<5){
	  sprintf(led_path, SYSFS_PATH USER_LED "trigger", i);
          switch(i){
            case 0 : set_action_trigger("heartbeat") ;
                     break;
            case 1 : set_action_trigger("mmc0") ;
                     break;
            case 2 : set_action_trigger("cpu") ;
                     break;
            case 3 : set_action_trigger("mmc1") ;
                     break;
          }
          i++;
        }
        free(led_path);
  }else if(argc == 4){
        int led_num = atoi(argv[1]);
        if(led_num > 4 || led_num < 0)
          print_usage();
        led_path = (char *)malloc(strlen(SYSFS_PATH USER_LED) + strlen(argv[2]));
	sprintf(led_path, SYSFS_PATH USER_LED "%s", led_num, argv[2]);
        printf("led_path provided -> %s\n", led_path);

        test_cmd_input(argv[2], argv[3]);

        free(led_path);
  }else
        print_usage();

  return EXIT_SUCCESS;
}

void print_usage(void){  
  printf("\nTo use:- \n\
      1. First argument should be the onboard LED number (0-3)\n\
      2. Second argument must be the cmd (trigger/brightness)\n\
      3. Third argument must be the cmd specific arg (0/1 for brightness)\n");

}

void test_cmd_input(const char *str,const char *cmd_val){
  if(strcmp(str, "trigger") == 0)   
    set_action_trigger(cmd_val);  
  else if(strcmp(str, "brightness") == 0)
    set_brightness(cmd_val);
  else
    print_usage();
}

void set_brightness(const char *cmd_val){
  int brightness_val = atoi(cmd_val) ;

  if(brightness_val == 0 || brightness_val == 1){
    FILE *uled_f = fopen(led_path, "w") ;
    fputs(cmd_val, uled_f);
    fclose(uled_f);
  }else
    print_usage();
}

void set_action_trigger(const char *cmd_val){
  FILE *uled_f;
  uled_f = fopen(led_path, "w");

  if(uled_f == NULL){
    printf("failed to open file\n");
    return;
  }

  if(!strcmp(cmd_val, "none")){
    goto write_to_file; 

  }else if(!strcmp(cmd_val, "heartbeat")){
    goto write_to_file; 

  }else if(!strcmp(cmd_val, "cpu")){
    goto write_to_file; 

  }else if(!strcmp(cmd_val, "timer")){
    goto write_to_file; 

  }else if(!strcmp(cmd_val, "mmc0")){
    goto write_to_file; 

  }else if(!strcmp(cmd_val, "mmc1")){
    goto write_to_file; 

  }else{
    print_usage() ;
    goto close_file;

  }

write_to_file:
  fputs(cmd_val, uled_f);
close_file:
  fclose(uled_f) ;
   
}
