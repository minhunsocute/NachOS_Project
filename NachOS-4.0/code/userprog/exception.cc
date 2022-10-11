// HUNG NACHOS
// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include "synchconsole.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

void counter() // hàm thực hiện program counter để tránh lặp chương trình
{
	/* set previous programm counter (debugging only)*/
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(NextPCReg));

	/* set next programm counter for brach execution */
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(NextPCReg) + 4);
}

void handle_SC_Add()
{
	DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
					/* int op2 */ (int)kernel->machine->ReadRegister(5));

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
	return counter();
}

void handle_SC_Sub()
{
	DEBUG(dbgSys, "Sub " << kernel->machine->ReadRegister(4) << " - " << kernel->machine->ReadRegister(5) << "\n");
	/* Process SysAdd Systemcall*/
	int result;
	result = SysSub(/* int op1 */ (int)kernel->machine->ReadRegister(4),
					/* int op2 */ (int)kernel->machine->ReadRegister(5));
	DEBUG(dbgSys, "Sub returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);

	return counter();
}

void handle_SC_halt()
{
	DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
	SysHalt();
	ASSERTNOTREACHED();
}


void handle_SC_ReadNum() { // hàm để thực hiện system call read num đầu tiên nó sẽ goirjddeesn hàm Sysreadnum để đọc số
	int num_read = SysReadNum();
	kernel->machine->WriteRegister(2, num_read); 
	return counter();
}

void handle_SC_ReadChar() { // hàm để thực hiện system call read char đầu tiên nó sẽ goirjddeesn hàm SysrreadChar để đọc char
	char result = SysReadChar();
	cout << result << "\n";
	kernel->machine->WriteRegister(2, (int)result);
	return counter();
}

void handle_SC_RandomNum(){ // hàm để thực hiện system call  đầu tiên nó sẽ goị đến SysRandomNum để tiến hành random
	int result = SysRandomNum();
	cout<< result<< "\n";
	kernel->machine->WriteRegister(2, result);
	return counter();
}
int MAX_STRING = 255;
void sendDataToUserSpace(){

}
void handle_SC_ReadString() { 
	int add = kernel->machine->ReadRegister(4); //Đọc địa chỉ của string
	int length = kernel->machine->ReadRegister(5);// Đọc độ dài của chuỗi
	// cout << add << endl; 
	// cout << length << "\n";
	char* result = new char[256]; // Khởi tạo mảng char để chứa chuỗi
	SysReadString(result, length); // Gọi hàm SysReadString để tiến hành đọc chuỗi
// 	cout << result <<"-" << sizeof(result) << " ["<< result[length]<<"]" << endl ;
	for(int i = 0; i<= length ; i++) { // Ứng với từng phần tử trong chuỗi tiến hành gửi dữ liệu của phần tử chuỗi sang userspace
		kernel->machine->WriteMem(add + i, 1,result[i]);
	}
	delete[] result;
	return counter();
}
void handle_SC_PrintChar(){ // in char
	int result = kernel->machine->ReadRegister(4); // đọc phần tử char đầu vào tại thanh ghi thứ 4
	kernel->synchConsoleOut->PutChar(char(result)); // TIến hành in char bằng cách gọi hành PutChar
	return counter();
}
void handle_SC_PrintString(){ // in chuỗi
	int add = kernel->machine->ReadRegister(4); // đọc độ dài chuỗi
	char* result = new char[MAX_STRING+1];// khởi tạo mảng chứa chuỗi
	int len = 0; 
	while(true){ // tiến hafnnh lấy chuỗi nhập vào bõ vào mảng chuỗi
		int c;
		kernel->machine->ReadMem(add + len,1, &c); // Đọc chuỗi
		if(c == '\0') break; 
		len++;
	}
	for(int i = 0; i< len ; i++) {
		int c;
		kernel->machine->ReadMem(add + i, 1, &c);
		result[i] = (char)c;
	}
	for(int i = 0; i< len; i++){ // dùng hàm putchar để in ra từng phần tử chuỗi
		kernel->synchConsoleOut->PutChar(result[i]);
	}
	delete[] result; // xóa mảng đã tạo
	return counter();
}

void handle_SC_PrintNum() {
	int num = kernel->machine->ReadRegister(4);// đọc số nhập vào ở thanh ghi thứ 4
	if(num == 0) { // nếu như bằng 0 thì cho nó out bằng 0 luôn
		kernel->synchConsoleOut->PutChar('0');
		return counter();
	} 
	if(num + 1 == -2147483647){ // nếu như nó vượt giới hạn dưới
		kernel->synchConsoleOut->PutChar('-'); // cho in số âm trc
		string num_arr = "2147483648";	 // r in từng phần tử trong chuỗi này sau
		for(int i =  0; i < 10; i++) {
			kernel->synchConsoleOut->PutChar(num_arr[i]);
		}
		return counter();
	}
	if(num < 0) { // nếu như số amm cho in dấu - ra trc
		kernel->synchConsoleOut->PutChar('-');
		num = -num;
	}
	char* buffer = new char[13]; // sao chép số đã đọc đc vào mãng chuỗi
	int n = 0;
	while(num != 0){
		buffer[n++] = (char)(num % 10 + 48);
		num /= 10;
	}
	for(int i = n-1;i >= 0; i--) { /// cho in ra các phần tử trong mảng bằng hàm putchar
		kernel->synchConsoleOut->PutChar(buffer[i]);
	}
	delete[] buffer;
	return counter();
}
void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case NoException:
		kernel->interrupt->setStatus(SystemMode);
		DEBUG(dbgSys, "Switch to system mode \n");
		break;
	case PageFaultException: // No valid translation found
		DEBUG(dbgSys, "No valid translation found\n");
		kernel->interrupt->Halt();
		break;
	case ReadOnlyException: // Write attempted to page marked // "read-only"
		DEBUG(dbgSys, "Weite attempted to page marked ready only\n");
		kernel->interrupt->Halt();
		break;
	case BusErrorException: // Translation resulted in an // invalid physical address
		DEBUG(dbgSys, "Translation resulted in a invalid physical address\n");
		kernel->interrupt->Halt();
		break;

	case AddressErrorException: // Unaligned reference or one that// was beyond the end of the // address space
		DEBUG(dbgSys, "Unaligned reference or one that was beyond the end of the address space\n");
		kernel->interrupt->Halt();
		break;

	case OverflowException: // Integer overflow in add or sub.
		DEBUG(dbgSys, "No valid translation found\n");
		kernel->interrupt->Halt();
		break;

	case IllegalInstrException: // Unimplemented or reserved instr.
		DEBUG(dbgSys, "No valid translation found\n");
		kernel->interrupt->Halt();
		break;
	case NumExceptionTypes:
		DEBUG(dbgSys, "No valid translation found\n");
		kernel->interrupt->Halt();
		break;
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			return handle_SC_halt();
		case SC_Add:
			return handle_SC_Add();
		case SC_Sub:
			return handle_SC_Sub();
		case SC_ReadNum:
			return handle_SC_ReadNum();
		case SC_ReadChar:
			return handle_SC_ReadChar();
		case SC_RandomNum:
			return handle_SC_RandomNum();
		case SC_ReadString:
			return handle_SC_ReadString();
		case SC_PrintString:
			return handle_SC_PrintString();
		case SC_PrintChar:
			return handle_SC_PrintChar();
		case SC_PrintNum:
			return handle_SC_PrintNum();
		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
