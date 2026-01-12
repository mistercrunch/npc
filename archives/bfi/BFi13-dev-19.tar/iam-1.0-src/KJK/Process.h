/*
 Copyright (c) 2004, KJK::Hyperion
 All rights reserved.

 Redistribution and use in source and binary forms, with or without 
 modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this 
    list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, 
    this list of conditions and the following disclaimer in the documentation 
    and/or other materials provided with the distribution.
  * Neither the name of the KJK::Namespace nor the names of its contributors 
    may be used to endorse or promote products derived from this software 
    without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE 
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef KJK_PROCESS_H_INCLUDED_
#define KJK_PROCESS_H_INCLUDED_

#include <cassert>
#include <cstddef>
#include <KJK/Handle.h>

namespace KJK
{
 class Thread: public Handle
 {
  public:
   // CONSTRUCTORS
   Thread(): Handle() { }
   Thread(IN HANDLE h_): Handle(h_) { }
   Thread(Handle& Y_): Handle(Y_) { }
   
   Handle& operator=(IN HANDLE h_) { Reset(h_); return *this; }

   // METHODS
   DWORD Resume()
   {
    DWORD dwRet = ResumeThread(this->Get());
    if(dwRet == (DWORD)-1) ThrowLastError(TEXT("ResumeThread"));
    return dwRet;
   }

   HANDLE OpenToken(IN DWORD DesiredAccess, IN BOOL OpenAsSelf = TRUE)
   {
    HANDLE hToken;
    TEST_BOOL_(OpenThreadToken)(Get(), DesiredAccess, OpenAsSelf, &hToken);
    return hToken;
   } // OpenToken()
 }; // class Thread

 class Process: public Handle
 {
  public:
   // CONSTRUCTORS
   Process(): Handle() { }
   Process(IN HANDLE h_): Handle(h_) { }
   Process(Handle& Y_): Handle(Y_) { }

   // METHODS
   HANDLE OpenToken(IN DWORD DesiredAccess) const
   {
    HANDLE hToken;
    TEST_BOOL_(OpenProcessToken)(Get(), DesiredAccess, &hToken);
    return hToken;
   } // OpenToken()

   void Terminate(IN DWORD dwExitCode) const
   { TEST_BOOL_(TerminateProcess)(Get(), dwExitCode); }

   HANDLE Create
   (
    LPCTSTR lpApplicationName,
    LPTSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCTSTR lpCurrentDirectory,
    LPSTARTUPINFO lpStartupInfo,
    HANDLE hToken = NULL
   )
   {
    if(lpStartupInfo->dwFlags & STARTF_USESTDHANDLES)
     bInheritHandles = TRUE;

    PROCESS_INFORMATION pi;

    if(hToken)
     TEST_BOOL_(CreateProcessAsUser)
     (
      hToken,
      lpApplicationName,
      lpCommandLine,
      lpProcessAttributes,
      lpThreadAttributes,
      bInheritHandles,
      dwCreationFlags,
      lpEnvironment,
      lpCurrentDirectory,
      lpStartupInfo,
      &pi
     );
    else
     TEST_BOOL_(CreateProcess)
     (
      lpApplicationName,
      lpCommandLine,
      lpProcessAttributes,
      lpThreadAttributes,
      bInheritHandles,
      dwCreationFlags,
      lpEnvironment,
      lpCurrentDirectory,
      lpStartupInfo,
      &pi
     );

    this->Reset(pi.hProcess);
    return pi.hThread;
   } // Create()

  public:
   // Wraps STARTUPINFO to hide the ugliness
   class StartupInfo: public STARTUPINFO
   {
    public:
     // just a little "namespace" class to bury these constants deep enough
     class Flags
     {
      public:
       enum Flags_
       {
        ForceOnFeedback = STARTF_FORCEONFEEDBACK,
        ForceOffFeedback = STARTF_FORCEOFFFEEDBACK,
        ConsoleIsFullScreen = STARTF_RUNFULLSCREEN,
        ConsoleSize = STARTF_USECOUNTCHARS,
        ConsoleFill = STARTF_USEFILLATTRIBUTE,
        WindowPos = STARTF_USEPOSITION,
        WindowShowState = STARTF_USESHOWWINDOW,
        WindowSize = STARTF_USESIZE,
        StdHandles = STARTF_USESTDHANDLES,

        MiscFlags = ForceOnFeedback | ForceOffFeedback,
        ConsoleFlags = ConsoleIsFullScreen,
        ConsoleAttribs = ConsoleSize | ConsoleFill | ConsoleFlags,
        WindowAttribs = WindowPos | WindowShowState | WindowSize,

        Everything =
         MiscFlags | ConsoleFlags | ConsoleAttribs | WindowAttribs | StdHandles,

        Nothing = 0
       }; // enum Flags_
     }; // class Flags

    private:
     static void CloseHandleIf(IN HANDLE h)
     { if(h) TEST_BOOL_(CloseHandle)(h); }

     void SetStdHandle(IN OUT HANDLE& hStd, IN HANDLE hStdNew)
     {
      // one-time initialization
      if(!(this->dwFlags & Flags::StdHandles))
      {
       this->dwFlags |= Flags::StdHandles;
       this->hStdInput = NULL;
       this->hStdOutput = NULL;
       this->hStdError = NULL;
      }

      // use existing standard handle
      switch((UINT_PTR)hStdNew)
      {
       case (UINT_PTR)(HANDLE)STD_INPUT_HANDLE:
       case (UINT_PTR)(HANDLE)STD_OUTPUT_HANDLE:
       case (UINT_PTR)(HANDLE)STD_ERROR_HANDLE:
        hStdNew = GetStdHandle((DWORD)(UINT_PTR)hStdNew);
      }

      // duplicate the provided handle into an inheritable handle
      if(hStdNew)
       TEST_BOOL_(DuplicateHandle)
       (
        GetCurrentProcess(),
        hStdNew,
        GetCurrentProcess(),
        &hStdNew,
        0,
        TRUE,
        DUPLICATE_SAME_ACCESS
       );

      // close the old handle
      CloseHandleIf(hStd);

      // set the new handle
      hStd = hStdNew;
     } // SetStdHandle

     void Inherit(IN const STARTUPINFO * psiOther, IN DWORD dwFlags)
     {
      if(dwFlags & Flags::StdHandles)
      {
       SetStdHandle(this->hStdInput, psiOther->hStdInput);
       SetStdHandle(this->hStdOutput, psiOther->hStdOutput);
       SetStdHandle(this->hStdError, psiOther->hStdError);
      }

      if(dwFlags == Flags::StdHandles)
       return;

      DWORD dwNewFlags = psiOther->dwFlags & dwFlags;

      if(dwNewFlags & Flags::ConsoleSize)
      {
       this->dwXCountChars = psiOther->dwXCountChars;
       this->dwYCountChars = psiOther->dwYCountChars;
      }

      if(dwNewFlags & Flags::ConsoleFill)
       this->dwFillAttribute = psiOther->dwFillAttribute;

      if(dwNewFlags & Flags::WindowPos)
      {
       this->dwX = psiOther->dwX;
       this->dwY = psiOther->dwY;
      }

      if(dwNewFlags & Flags::WindowShowState)
       this->wShowWindow = psiOther->wShowWindow;

      if(dwNewFlags & Flags::WindowSize)
      {
       this->dwXSize = psiOther->dwXSize;
       this->dwYSize = psiOther->dwYSize;
      }

      this->dwFlags |= dwNewFlags;
     } // Inherit(IN const STARTUPINFO *, IN DWORD)

     void Inherit(IN const STARTUPINFO * psiOther)
     {
      Inherit(psiOther, Flags::Everything);
     } // Inherit(IN const STARTUPINFO *)

     unsigned ValidateStdHandle(IN OUT HANDLE& hStd)
     {
      if((GetFileType(hStd) == FILE_TYPE_UNKNOWN) && GetLastError())
      {
       hStd = NULL;
       return 0;
      }
      else
       return 1;
     }

     void Inherit(IN DWORD dwFlags)
     {
      STARTUPINFO siCurrent;
      GetStartupInfo(&siCurrent);

      // The initial values aren't necessarily valid...
      siCurrent.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
      siCurrent.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
      siCurrent.hStdError = GetStdHandle(STD_ERROR_HANDLE);

      // ... nor the current values, for that matter
      unsigned nValidHandles = 0;

      nValidHandles += ValidateStdHandle(siCurrent.hStdInput);
      nValidHandles += ValidateStdHandle(siCurrent.hStdOutput);
      nValidHandles += ValidateStdHandle(siCurrent.hStdError);

      if(nValidHandles > 0)
       siCurrent.dwFlags |= Flags::StdHandles;

      Inherit(&siCurrent, dwFlags);
     } // Inherit(IN DWORD)

     void Initialize()
     {
      this->cb = sizeof(STARTUPINFO);
      this->lpReserved = NULL;
      this->cbReserved2 = 0;
      this->lpReserved2 = NULL;

      this->lpDesktop = NULL;
      this->lpTitle = NULL;
      this->dwFlags = 0;
     }

    public:
     // CONSTRUCTORS
     // Default constructor
     StartupInfo() { Initialize(); Inherit(Flags::Everything); }

     // Copy constructors
     StartupInfo(IN const StartupInfo& Other) { Initialize(); Inherit(&Other); }
     StartupInfo(IN const STARTUPINFO& Other) { Initialize(); Inherit(&Other); }

     // Parametrized constructors
     explicit StartupInfo(IN DWORD dwFlags)
     { Initialize(); Inherit(dwFlags); }

     explicit StartupInfo(IN const StartupInfo& Other, IN DWORD dwFlags)
     { Initialize(); Inherit(&Other, dwFlags); }

     explicit StartupInfo(IN const STARTUPINFO& Other, IN DWORD dwFlags)
     { Initialize(); Inherit(&Other, dwFlags); }

     // DESTRUCTOR
     ~StartupInfo() { ClearStdHandles(); }

     // METHODS
     // Console attributes
     void SetConsoleFlags(IN DWORD dwConsoleFlags)
     { this->dwFlags |= dwConsoleFlags & Flags::ConsoleFlags; }

     void SetConsoleSize(IN DWORD dwXCountChars, IN DWORD dwYCountChars)
     {
      this->dwFlags |= Flags::ConsoleSize;
      this->dwXCountChars = dwXCountChars;
      this->dwYCountChars = dwYCountChars;
     }

     void SetConsoleFill(IN DWORD dwFillAttribute)
     {
      this->dwFlags |= Flags::ConsoleFill;
      this->dwFillAttribute = dwFillAttribute;
     }

     void ClearConsoleFlags() { this->dwFlags &= ~Flags::ConsoleFlags; }
     void ClearConsoleSize() { this->dwFlags &= ~Flags::ConsoleSize; }
     void ClearConsoleFill() { this->dwFlags &= ~Flags::ConsoleFill; }
     void ClearConsoleAttribs() { this->dwFlags &= ~Flags::ConsoleAttribs; }

     void InheritConsoleFlags() { Inherit(Flags::ConsoleFlags); }
     void InheritConsoleSize() { Inherit(Flags::ConsoleSize); }
     void InheritConsoleFill() { Inherit(Flags::ConsoleFill); }
     void InheritConsoleAttribs() { Inherit(Flags::ConsoleAttribs); }

     // Window attributes
     void SetWindowPosition()
     {
      this->dwFlags |= Flags::WindowPos;
      this->dwX = dwX;
      this->dwY = dwY;
     }

     void SetWindowShowState()
     {
      this->dwFlags |= Flags::WindowShowState;
      this->wShowWindow = wShowWindow;
     }

     void SetWindowSize()
     {
      this->dwFlags |= Flags::WindowSize;
      this->dwXSize = dwXSize;
      this->dwYSize = dwYSize;
     }

     void ClearWindowPosition() { this->dwFlags &= ~Flags::WindowPos; }
     void ClearWindowShowState() { this->dwFlags &= ~Flags::WindowShowState; }
     void ClearWindowSize() { this->dwFlags &= ~Flags::WindowSize; }
     void ClearWindowAttribs() { this->dwFlags &= ~Flags::WindowAttribs; }

     void InheritWindowPosition() { Inherit(Flags::WindowPos); }
     void InheritWindowShowState() { Inherit(Flags::WindowShowState); }
     void InheritWindowSize() { Inherit(Flags::WindowSize); }
     void InheritWindowAttribs() { Inherit(Flags::WindowAttribs); }

     // Standard I/O handles
     void SetStdInput(IN HANDLE hStdInput = (HANDLE)STD_INPUT_HANDLE)
     { SetStdHandle(this->hStdInput, hStdInput); }

     void SetStdOutput(IN HANDLE hStdOutput = (HANDLE)STD_OUTPUT_HANDLE)
     { SetStdHandle(this->hStdOutput, hStdOutput); }

     void SetStdError(IN HANDLE hStdError = (HANDLE)STD_ERROR_HANDLE)
     { SetStdHandle(this->hStdError, hStdError); }

     void ClearStdHandles()
     {
      if(this->dwFlags & Flags::StdHandles)
      {
       this->dwFlags &= ~Flags::StdHandles;

       CloseHandleIf(this->hStdInput);
       CloseHandleIf(this->hStdOutput);
       CloseHandleIf(this->hStdError);
      }
     }

     void InheritStdHandles() { Inherit(Flags::StdHandles); }

     // Inheritance from the caller's startup information
     void InheritEverything() { Inherit(Flags::Everything); }
   }; // class StartupInfo

   class Killer
   {
    private:
     const Process * m_pps;

    public:
     Killer(): m_pps(NULL) { }
     Killer(IN const Process& ps): m_pps(&ps) { }
     ~Killer() { if(m_pps) m_pps->Terminate(ERROR_SUCCESS); }

     void Detach() { m_pps = NULL; }
     void Attach(IN const Process& ps) { m_pps = &ps; }
   }; // class Killer
 }; // class Process
};

#endif // KJK_PROCESS_H_INCLUDED_

// EOF
