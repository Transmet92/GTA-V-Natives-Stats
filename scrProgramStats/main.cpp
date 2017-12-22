/*
	IMPORTANT OF NOT CHANGE THE CONFIGURATION PROJECT
		RECOMMANDED TO COMPILE FROM VS2017


	IF YOU CHANGE THE COMPILER OPTIONS ( NOT RELEASE OPTIMISED ECT... )
	AND GTA5 CRASH, YOU NEED TO REFRESH THE PUSH, POP REGISTERS ( SAVE/RESTORE ) IN
	TRAMPOLINE.ASM ( YOU CAN CHECK FROM SECOND ADDRESS IN FIRST PRINTF CONSOLE )



	ACTUAL UPDATE : 1290 ( NEED TO UPDATE FOR OTHERS UPDATES )


	Transmet 2017
*/

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <Psapi.h>

using namespace std;

bool inline get_key_pressed(int nVirtKey) { return (GetAsyncKeyState(nVirtKey) & 0x8000) != 0; }
typedef unsigned int long long QWORD;


// LARGE NOPS TO SET QWORD PTR TO TRAMPOLINE
#define OFFSET_LARGE_NOPS_GTA5EXE 0x7C0BF2
#define NOPS_AND_REL_ADDR_OF_LNOPS 0x9090FF29F4BA25FF;

// OFFSET OF "CALL R8" INSTRUCTION IN NATIVE OP Code CASE ( IN SWITCH INTERPRETER rage::scrProgram )
#define OFFSET_NATIVE_OPC_CALL_R8 0x1521746



static unsigned short NativesInArray = 0;
struct {
	QWORD ptrFunc;
	DWORD iterator;
} Natives[6000];

static QWORD NativesIterator = 0; // INCREMENT WHEN A ANY NATIVE IS CALLED

static QWORD baseImage = 0;

extern "C" QWORD callptr = 0;
extern "C" void __fastcall StatNative(QWORD thisNative)
{
	NativesIterator++;

	QWORD corrected = (thisNative - baseImage) + 0x140000000;
	for (unsigned short i = 0; i < NativesInArray; i++)
		if (Natives[i].ptrFunc == corrected)
		{
			Natives[i].iterator++;
			return;
		}

	Natives[NativesInArray].ptrFunc = corrected;
	Natives[NativesInArray].iterator = 0;
	NativesInArray++;
}

extern "C" void TrampolineFunc(void);

int ThreadStat(void* none)
{
	// LOAD CONSOLE
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	HWND hConsole = GetConsoleWindow();
	RECT rect;
	GetWindowRect(hConsole, &rect);
	SetWindowPos(hConsole, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, 0);
	freopen("CON", "w", stdout);
	freopen("CONIN$", "r", stdin);


	MODULEINFO ModInfo = {};
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &ModInfo, sizeof(ModInfo));
	baseImage = (QWORD)ModInfo.lpBaseOfDll;

	void(*trampolinePtr)(void);
	trampolinePtr = TrampolineFunc;

	void(*stNatPtr)(QWORD);
	stNatPtr = StatNative;

	bool monoPatch = true;
	for (;;)
	{
		if (get_key_pressed(VK_F9) && monoPatch)
		{
			printf("Trampoline ptr : %p\nStat Native ptr : %p\n\n", trampolinePtr, stNatPtr);

			// JMP SHORT TO ESCAPE THE QWORD PTR
			short* ptrToJmp = (short*)(baseImage + OFFSET_LARGE_NOPS_GTA5EXE); // ADDRESS OF LARGE NOPS SECTION IN GTA5.exe
			*ptrToJmp = 0x14EB;
			ptrToJmp += 6;

			// ADD CODE ASM SECTION
			// char* buffer_exec = new char[128];
			// ZeroMemory(buffer_exec, 128);

			char* addrCallR8 = (char*)(baseImage + OFFSET_NATIVE_OPC_CALL_R8);
			callptr = (QWORD)addrCallR8;
			*(QWORD*)ptrToJmp = (QWORD)trampolinePtr;


			// const char func[] = {
			// 	// OVERWRITED MOVED INSTRUCTIONS
			// 	0x48, 0x89, 0x4D, 0xA7, // mov   qword ptr [rbp-59h],rcx
			// 	0x48, 0x8D, 0x4D, 0x97, // lea   rcx,[rbp - 69h]
			// 
			// 	// SAVE REGISTERS
			// 	0x54, // push rsp
			// 	0x51, // push rcx
			// 	0x50, // push rax
			// 	0x52, // push rdx
			// 
			// 	// TRAMPOLINE INSTRUCTION
			// 	0xFF, 0x15, 0x36, 0x00, 0x00, 0x00,  // call StatNative
			// 
			// 	// RESTORE REGISTERS
			// 	0x5A, // pop rdx
			// 	0x58, // pop rax
			// 	0x59, // pop rcx
			// 	0x5C, // pop rsp
			// 
			// 	// RE JUMP ORIGINAL
			// 	0xFF, 0x25, 0x38, 0x00, 0x00, 0x00, // jmp qword ptr  --- JMP TO CALL R8  NATIVES
			// };


			// *(QWORD*)(buffer_exec + 32) = (QWORD)*StatNative; // POINTER TO StatNative Function
			// *(QWORD*)(buffer_exec + 40) = (QWORD)addrCallR8; // POINTER TO GTA5.exe CALL R8 ( NATIVE OPC )
			// 
			// memcpy(buffer_exec, func, sizeof(func) / sizeof(func[0]));
			// DWORD nullfilled = 0;
			// VirtualProtect(buffer_exec, 128, PAGE_EXECUTE_READWRITE, &nullfilled);


			// PATCH MY CODE  --- StatNative, PATCH REGISTER
			// MOV RAX, 0x8888FFFFFFFF8888
			// const char* pattern = "\x48\xB8\x88\x88\xFF\xFF\xFF\xFF\x88\x88";
			// char* curInstr = (char*)*StatNative;
			// for (; curInstr < curInstr + 4096; curInstr++)
			// {
			// 	bool bypass = true;
			// 	for (unsigned char i = 0; i < 10; i++)
			// 		if (*(curInstr + i) != pattern[i])
			// 		{
			// 			bypass = false;
			// 			break;
			// 		}
			// 
			// 	if (bypass)
			// 	{
			// 		// MOV RAX, R8
			// 		const char* thisPatch = "\x4C\x89\xC0\x90\x90\x90\x90\x90\x90\x90";
			// 
			// 		memcpy(curInstr, thisPatch, 10);
			// 	}
			// }



			// LAST PATCH, JMP BEFORE CALL R8
			addrCallR8 -= 8;
			*(QWORD*)addrCallR8 = 0x9090FF29F4BA25FF; // jmp qword ptr[ptrToJmp]  nop nop


			printf("\n\tDetourned Native OP Code to Stat function\n\n\t");

			monoPatch = false;

			Sleep(1000);
		}


		// IF PRESSED, START THE STATS PRINT
		/*
			WARNINGS :
				EXECUTION INTER-THREADS WITHOUT CriticalSection
		*/
		if (get_key_pressed(VK_F11))
		{
			printf("\n\n");
			for (unsigned short i = 0; i < NativesInArray; i++)
			{
				float percent = ((float)Natives[i].iterator / (float)NativesIterator) * 100.0f;
				printf("0x%p : %.6f %%\n", (void*)Natives[i].ptrFunc, percent);
			}
			printf("\n\n\tNatives Count : %u\n\tNatives Called Count : %u\n\n", NativesInArray, NativesIterator);
		}

		// DO NOT PRESS, JUST TO FORCE THE IMPLEMENTATION WITHOUT :NOREF
		// if (get_key_pressed(VK_F10) && get_key_pressed(VK_F5))
		// 	StatNative();

		Sleep(100);
	}

	return 0;
}

BOOL __stdcall DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH) {
		// if (get_key_pressed(VK_F10) && get_key_pressed(VK_F5))
		// 	StatNative();
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ThreadStat, 0, 0, 0);
	}

	if (dwReason == DLL_PROCESS_DETACH)
	{ }

	return true;
}