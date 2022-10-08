/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#include "kernel.h"
#include <vector>
#include "synchconsole.h"
#include "time.h"

#define ENTER ((char)10)
#define TAB ((char)9)
#define SPACE ((char)32)
char checkTabKey(char c) { 
  return c == ENTER || c == TAB || c == SPACE; 
}

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2) { return op1 + op2; }
int SysSub(int op1, int op2) { return op1 - op2; }

bool checkSameNum(char* buffer, int num,int length, bool checkNo){
  int check_no = num;
  int index_check = length- 1;
  if(checkNo != (num < 0)){
    return false;
  }
  while(check_no != 0 && ((checkNo) ? (index_check >= 1) : (index_check >= 0))){
    if((int(buffer[index_check]) - 48) != abs(check_no% 10)){
      return false;
    }
    check_no /= 10;
    index_check--;
  }
  return true;
}
int SysReadNum(){
  // get input
  int MAX_BUFFER = 11;
  char buffer[MAX_BUFFER + 2];
  char c = kernel->synchConsoleIn->GetChar();
  if (c == EOF){
    DEBUG(dbgSys, "Unexpected end of file - number expected");
    return 0;
  }
  if (checkTabKey(c)){
    DEBUG(dbgSys, "Unexpected white-space - number expected");
    return 0;
  }
  int n = 0;
  bool checkNoValid = false;
  while(1) {
    if(c == EOF || c == ENTER) break;
    if(c == TAB || c == SPACE){
      checkNoValid = true;
    }
    buffer[n++] = c;
    c = kernel->synchConsoleIn->GetChar();
  }
  buffer[n]='\0';
  if( n > 11) {
    DEBUG(dbgSys, "Number is too long \n");
    return 0;
  }
  if(checkNoValid){
    DEBUG(dbgSys, "Invalid Number\n");
    return 0;
  }
  bool checkNo = (buffer[0] == '-'); // true la so am
  bool check_dot = false;
  int first = (checkNo) ? 1 : 0;
  int result = 0;
  for (int i = first; i < n; i++){
    char c = buffer[i];
    if (c == '.'){
      check_dot = true;
    }
    else{
      if (c < '0' || c > '9'){
        DEBUG(dbgSys, "Invalid Number\n");
        return 0;
      }
      else{
        if (check_dot){
          if (c != '0') {
            DEBUG(dbgSys, "Invalid number\n");
            return 0;
          };
        }
        else {
          result = result * 10 + (int)(c - 48);
        }
      }
    }
  }
  if(!checkSameNum(buffer, abs(result), n, checkNo)){
    DEBUG(dbgSys,"Number is outside.\n");
    return 0;
  }
  if (checkNo){
    result = -result;
  }
  return result;
}

char SysReadChar(){
  char c = kernel->synchConsoleIn->GetChar();
  char c_test = kernel->synchConsoleIn->GetChar();
  if(int(c_test) !=  ENTER && c != EOF){
    DEBUG(dbgSys, "You can only enter one character.\n");
    return ' ';
  }
  // << " " << char(13) << " " << char(9) << "\n";
  if(c ==  ' '){
    DEBUG(dbgSys, "Character is null\n");
  }
  return c;
}
int SysRandomNum() {
  srand(time(NULL));
  return rand();
}
void SysReadString(char*& buffer, int length){
  buffer = new char[length + 1];
  for(int  i = 0; i < length; i++){
    buffer[i] = kernel->synchConsoleIn->GetChar();
  }
  buffer[length] = '\0';
  return;
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
