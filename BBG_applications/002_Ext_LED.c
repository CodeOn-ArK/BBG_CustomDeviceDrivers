/*
 ============================================================================
 Name        : Test.c
 Author      : ArK
 Version     : 1.0
 Copyright   : GPL
 Description : Accessing GPIOs in BBG, Ansi-style
 ============================================================================
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/* full gpio path  */
#define GPIO_PATH "/sys/class/gpio/gpio44/"

void print_usage(void){  
  printf("\nTo use:- \n\
      1. First argument should be the GPIO direction (in/out)\n\
      2. Second argument must be the cmd (0/1) if direction is out\n\
      3. Third argument is optional, (invert) used to get/send inverted values at the GPIO pins\n");

}

FILE *set_direction( const char perm){
  char *buf;
  FILE *gpio_dir;

  buf = (char *)malloc(strlen(GPIO_PATH) + strlen("direction"));
  sprintf(buf, GPIO_PATH "direction");
  if(perm == 'w')
    gpio_dir = fopen(buf, "w");
  else if(perm == 'r')
    gpio_dir = fopen(buf, "r");
  else
    print_usage();

  if(gpio_dir == NULL)
    return NULL;

  return  gpio_dir;
}

FILE *set_value( const char perm){
  char *buf;
  FILE *gpio_dir;

  buf = (char *)malloc(strlen(GPIO_PATH) + strlen("value"));
  sprintf(buf, GPIO_PATH "value");
  if(perm == 'w')
    gpio_dir = fopen(buf, "w");
  else if(perm == 'r')
    gpio_dir = fopen(buf, "r");
  else
    print_usage();

  if(gpio_dir == NULL)
    return NULL;

  return  gpio_dir;
}

int main(int argc, char *argv[]){
  FILE *gpio_dir;

  if(argc == 2) {
    if(!strcmp(argv[1], "in")) {
      int val;

      if((gpio_dir = set_direction('w')) == NULL)
        perror("Couldn't open file "); 
      fprintf(gpio_dir, "in");
      fclose(gpio_dir);

      if((gpio_dir = set_value('r')) == NULL)
        perror("Couldn't open file "); 
      fscanf(gpio_dir, "%d", &val);
      printf("The input value is %d\n", val);
      goto free;
    }

  }else if(argc == 3) {
    if(!strcmp(argv[1], "out")) {

      if((gpio_dir = set_direction('w')) == NULL)
        perror("Couldn't open file "); 
      fprintf(gpio_dir, "out");
      fclose(gpio_dir);

      if((gpio_dir = set_value('w')) == NULL)
        perror("Couldn't open file "); 
      fprintf(gpio_dir, "%s", argv[2]);
      goto free;
    }

  }else
    print_usage();

free:
  fclose(gpio_dir);

  return EXIT_SUCCESS;
}
