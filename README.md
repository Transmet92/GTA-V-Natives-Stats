# GTA-V-Natives-Stats
[rage::scrProgram] Dll to calculate the percentage of use of each natives in-game ( YSC )


Recommanded to use with fixed base ( not random ) PE ( edit your PE Header GTA5.exe ).
Actual version GTA V supported : 1290 Social Club ( not steam )
You can update, two macros to edit in main.cpp.

Just inject the DLL and, when your game loaded, press F9 to apply ( only once ) the trampoline and F11 to print the stats in console ( recommanded to up your height memory console ).

Trampoline native Op Code in scrProgram interpreter of YSC and count the number natives called + calculate the percentage that each native use.


The result format is :
"FunctionPtr : Percentage"
FunctionPtr can translate to natives names/hashs with a Native Translation Table correspond to your game version.
