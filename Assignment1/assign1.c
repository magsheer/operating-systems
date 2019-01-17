#define _GNU_SOURCE

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/syscall.h> 
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*********************************************************************
 *
 * These C functions use patterns and functionality often found in
 * operating system code. Your job is to implement them. Of course you
 * should write test cases. However, do not hand in your test cases
 * and (especially) do not hand in a main() function since it will
 * interfere with our tester.
 *
 * Additional requirements on all functions you write:
 *
 * - you may not refer to any global variables
 *
 * - you may not call any functions except those specifically
 *   permitted in the specification
 *
 * - your code must compile successfully on CADE lab Linux
 *   machines when using:
 *
 * /usr/bin/gcc -O2 -fmessage-length=0 -pedantic-errors -std=c99 -Werror -Wall -Wextra -Wwrite-strings -Winit-self -Wcast-align -Wcast-qual -Wpointer-arith -Wstrict-aliasing -Wformat=2 -Wmissing-include-dirs -Wno-unused-parameter -Wshadow -Wuninitialized -Wold-style-definition -c assign1.c x
 *
 * NOTE 1: Some of the specifications below are specific to 64-bit
 * machines, such as those found in the CADE lab.  If you choose to
 * develop on 32-bit machines, some quantities (the size of an
 * unsigned long and the size of a pointer) will change. Since we will
 * be grading on 64-bit machines, you must make sure your code works
 * there.
 *
 * NOTE 2: You should not need to include any additional header files,
 * but you may do so if you feel that it helps.
 *
 * HANDIN: submit your finished file, still called assign1.c, in Canvas
 *
 *
 *********************************************************************/

/*********************************************************************
 *
 * byte_sort()
 *
 * specification: byte_sort() treats its argument as a sequence of
 * 8 bytes, and returns a new unsigned long integer containing the
 * same bytes, sorted numerically, with the smaller-valued bytes in
 * the lower-order byte positions of the return value
 * 
 * EXAMPLE: byte_sort (0x0403deadbeef0201) returns 0xefdebead04030201
 *
 *********************************************************************/

unsigned long byte_sort (unsigned long arg)
{
  unsigned long to_sort[16]={0};
  unsigned long temp;
  int i = 0, j, k;
  
  //store the input in a char array after splitting into 8 bytes
  while(arg != 0){
    to_sort[i] = arg & 0xFF;
    i++;
    arg >>= 8;
  }
  
  //bubble sort
  for(k=0;k<(i-1);k++){
    for(j=0;j<i-k-1;j++){
      if(to_sort[j]<to_sort[j+1]){
	temp = to_sort[j];
	to_sort[j] = to_sort[j+1];
	to_sort[j+1]=temp;
      }
    }
  }

  //leading zeroes
   temp = 0;
   for(k = 0; k <8; k++){
     temp = to_sort[k] | temp;
     if(k<7){
       temp<<=8;
     }
   }
  
  return temp;
}

/*********************************************************************
 *
 * nibble_sort()
 *
 * specification: nibble_sort() treats its argument as a sequence of 16 4-bit
 * numbers, and returns a new unsigned long integer containing the same nibbles,
 * sorted numerically, with smaller-valued nibbles towards the "small end" of
 * the unsigned long value that you return
 *
 * the fact that nibbles and hex digits correspond should make it easy to
 * verify that your code is working correctly
 * 
 * EXAMPLE: nibble_sort (0x0403deadbeef0201) returns 0xfeeeddba43210000
 *
 *********************************************************************/

unsigned long nibble_sort (unsigned long arg)
{
  unsigned long to_sort[16]={0};
  unsigned long temp;
  int i = 0, j, k;
  
  //store the input in a char array after splitting into 8 bytes
  while(arg != 0){
    to_sort[i] = arg & 0xF;
    i++;
    arg >>= 4;
  }
  
  //bubble sort
  for(k=0;k<(i-1);k++){
    for(j=0;j<i-k-1;j++){
      if(to_sort[j]<to_sort[j+1]){
	temp = to_sort[j];
	to_sort[j] = to_sort[j+1];
	to_sort[j+1]=temp;
      }
    }
  }

  //leading zeroes
   temp = 0;
   for(k = 0; k <16; k++){
     temp = to_sort[k] | temp;
     if(k<15){
       temp<<=4;
     }
   }
  
  return temp;
}


/*********************************************************************
 *
 * name_list()
 *
 * specification: allocate and return a pointer to a linked list of
 * struct elts
 *
 * - the first element in the list should contain in its "val" field the first
 *   letter of your first name; the second element the second letter, etc.;
 *
 * - the last element of the linked list should contain in its "val" field
 *   the last letter of your first name and its "link" field should be a null
 *   pointer
 *
 * - each element must be dynamically allocated using a malloc() call
 *
 * - if any call to malloc() fails, your function must return NULL and must also
 *   free any heap memory that has been allocated so far; that is, it must not
 *   leak memory when allocation fails
 *  
 *********************************************************************/

struct elt {
  char val;
  struct elt *link;
};

struct elt *name_list (void)
{
  char name[] = "greeshma";
  int name_len = strlen(name);
  int i;

  struct elt* previous = NULL;
  struct elt* head = NULL;
  struct elt* temp;
 
  if(name_len == 0)
    return NULL;
   
  for(i = 0; i< name_len; i++){
    struct elt* new_node = (struct elt*)malloc(sizeof(struct elt));

    //test for malloc failure
    if(new_node == NULL){
      
    //free previously allocated memory
    while(head!=NULL){
	temp = head;
	head = head->link;
	free(temp);
      }
    return NULL;
    }

    //create new node
    new_node->val = name[i];
    new_node->link = NULL;
    
    //add to list 
    if(previous != NULL)
      previous->link = new_node;
    else
      head = new_node;    
    previous = new_node;        
  }
  
  return head;
}

/*********************************************************************
 *
 * convert()
 *
 * specification: depending on the value of "mode", print "value" as
 * octal, binary, or hexidecimal to stdout, followed by a newline
 * character
 *
 * extra requirement 1: output must be via putc()
 *
 * extra requirement 2: print nothing if "mode" is not one of OCT,
 * BIN, or HEX
 *
 * extra requirement 3: all leading/trailing zeros should be printed
 *
 * EXAMPLE: convert (HEX, 0xdeadbeef) should print
 * "00000000deadbeef\n" (including the newline character but not
 * including the quotes)
 *
 *********************************************************************/

enum format_t {
  OCT = 66, BIN, HEX
};

void convert (enum format_t mode, unsigned long value)
{
  int remainder;
  unsigned long binary, i;
  char answer[100];

  //OCTAL
  if(mode == 66){
    binary = 0;
    i = 1;

    while (value != 0){
      remainder = value % 8;
      value = value / 8; 
      binary = binary + (remainder * i);
      i = i * 10;
    }
    sprintf(answer, "%ld", binary);

    for (i = 0; i < (32)-strlen(answer);i++)
      putc('0',stdout);

    for (i = 0; i < strlen(answer); i++)
      putc(answer[i], stdout);

    putc('\n', stdout);
  }

  //BINARY
  if(mode == 67){
    binary = 0;
    i = 1;
    
    while (value != 0){
      remainder = value % 2;
      value = value / 2; 
      binary = binary + (remainder * i);
      i = i * 10;
    }
    sprintf(answer, "%lu", binary);

    for (i = 0; i < (64)-strlen(answer);i++)
      putc('0',stdout);

    for (i = 0; i < strlen(answer); i++)
      putc(answer[i], stdout);

    putc('\n', stdout);
  
  }
  
  //HEXADECIMAL
  if(mode == 68){
    sprintf(answer, "%lx", value);

    for (i = 0; i < (16)-strlen(answer);i++)
      putc('0',stdout);

    for (i = 0; i < strlen(answer); i++)
      putc(answer[i], stdout);

    putc('\n', stdout);
  } 
  
  else
    return;

}

/*********************************************************************
 *
 * draw_me()
 *
 * this function creates a file called me.txt which contains an ASCII-art
 * picture of you (it does not need to be very big). the file must (pointlessly,
 * since it does not contain executable content) be marked as executable.
 * 
 * extra requirement 1: you may only call the function syscall() (type "man
 * syscall" to see what this does)
 *
 * extra requirement 2: you must ensure that every system call succeeds; if any
 * fails, you must clean up the system state (closing any open files, deleting
 * any files created in the file system, etc.) such that no trash is left
 * sitting around
 *
 * you might be wondering how to learn what system calls to use and what
 * arguments they expect. one way is to look at a web page like this one:
 * http://blog.rchapman.org/post/36801038863/linux-system-call-table-for-x86-64
 * another thing you can do is write some C code that uses standard I/O
 * functions to draw the picture and mark it as executable, compile the program
 * statically (e.g. "gcc foo.c -O -static -o foo"), and then disassemble it
 * ("objdump -d foo") and look at how the system calls are invoked, then do
 * the same thing manually using syscall()
 *
 *********************************************************************/

void draw_me (void)
{
   char my_image[] = "\n       ,O, \n      ,OOO, \n'oooooOOOOOooooo' \n  `OOOOOOOOOOO` \n    `OOOOOOO` \n    OOOO'OOOO \n   OOO'   'OOO \n  O'         'O \n \n \n";
   int fd_create, fd_write, fd_close, fd_check, fd_del;

   //check if file already exists
   fd_check = syscall(SYS_access, "me.txt", F_OK);
   if(fd_check == 0){
     fd_del = syscall(SYS_unlink,"me.txt");
     if(fd_del == -1){
       printf("File already exists.\n File deletion failed.\n"); 
       return;     
     }
   }
  
   //create the file
   umask(0000);
   fd_create = syscall(SYS_open, "me.txt", O_CREAT | O_WRONLY, S_IRWXU | S_IRWXG | S_IRWXO);
     
   //in case of syscall failure
   if(fd_create == -1){
      printf("File creation unsuccessful.\n"); 
      fd_del = syscall(SYS_unlink,"me.txt");
      if(fd_del == -1){
       printf("Clean up failed.\n"); 
       return; 
      }
   }
   
   
   //write to file
   fd_write = syscall(SYS_write, fd_create , my_image, strlen(my_image)-1);
   if(fd_write == -1){
     printf("Writing to file unsuccessful.\n"); 

     //delete the file and clean up
      fd_del = syscall(SYS_unlink,"me.txt");
      if(fd_del == -1){
       printf("File deletion failed."); 
       return;
      }
   }
   
   //close the file
   fd_close = syscall(SYS_close, fd_create);   
   if(fd_close == -1){
     printf("Could not close file.\n"); 
      fd_del = syscall(SYS_unlink,"me.txt");
      if(fd_del == -1){
       printf("Clean up failed.\n"); 
       return;
      }
   }
}
