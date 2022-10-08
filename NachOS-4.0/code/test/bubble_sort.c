#include "syscall.h"

int main() {
    int n,i, option, j, temp;
    int arr[100 + 1];
    int swapped;
    while(1){
        PrintString("Input length of array (1 <= n <= 100) n:");
        n = ReadNum();
        if( n < 1 || n >  100){
            PrintString("Input again\n");
        }
        else
            break;
    }
    for( i = 0; i < n; i++){
        PrintString("Input Array[");
        PrintNum(i);
        PrintString("]:");
        arr[i] = ReadNum();
    }
    // for(i = 0; i< n; i++){
    //     PrintNum(arr[i]);
    // }
    PrintString("Choose Option\n");
    PrintString("1:Sort up ascending\n");
    PrintString("2:Sort Descending\n");
    PrintString("*Input 1 or 2 to choose option\n");
    do{
        PrintString("Input Option:");
        option = ReadNum();
    }while(option != 1  && option != 2);
    // if(option == 1){
        swapped = 0;
        for(i = 0; i< n-1; i++){
            swapped = 0;
            for(j = 0; j < n - i - 1; j++){
                if((option == 1)? arr[j] > arr[j+1] : arr[j] < arr[j+1]){
                    temp = arr[j];
                    arr[j] = arr[j+1];
                    arr[j+1] = temp;
                    swapped = 1;
                }else {}
            }
            if(swapped == 0){
                break;
            }
        }
    // }
    // else{

    // }
    PrintString("Array after sort: ");
    for( i = 0; i< n ; i++){
        PrintNum(arr[i]);
        PrintChar(' ');
    }
    Halt();
}