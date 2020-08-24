#include<stdio.h>
#include<iostream>
#include<tchar.h>
#include<stdlib.h>
#include<windows.h>
#include<conio.h>

HANDLE serial_open();
HANDLE serial_initialaize(HANDLE);
HANDLE serial_Config(HANDLE);
//void serial_Write(HANDLE, const char*);
void serial_Write(HANDLE, double, int);

void serial_close(HANDLE);

