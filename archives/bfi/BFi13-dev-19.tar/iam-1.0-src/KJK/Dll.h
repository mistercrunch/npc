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

#ifndef KJK_DLL_H_INCLUDED_
#define KJK_DLL_H_INCLUDED_

#include <cassert>

#include <KJK/Status.h>
#include <KJK/LocalMemory.h>

namespace KJK
{
 class DllRef
 {
  private:
   HMODULE m_hModule;

   static HMODULE AddRefByFileName(IN HMODULE hModule)
   {
    assert(hModule);

    DWORD nSize = KJK::TEST_SIZE_(GetModuleFileName)(hModule, NULL, 0);
    LocalMemory<TCHAR> apszFileName(((nSize + 1) * sizeof(TCHAR)));
    KJK::TEST_SIZE_(GetModuleFileName)(hModule, apszFileName.Get(), nSize);

    HMODULE hModule2 = LoadLibrary(apszFileName.Get());

    if(hModule2 == NULL)
     return NULL;

    if(hModule2 != hModule)
    {
     KJK::TEST_BOOL_(FreeLibrary)(hModule2);
     KJK::ThrowError(E_FAIL);
    }

    return hModule;
   }

#if (_WIN32_WINNT >= 0x0500) && defined(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS)
   static BOOL (WINAPI * GetModuleHandleEx)
   (
    IN DWORD dwFlags,
    IN LPCTSTR lpModuleName,
    OUT HMODULE * phModule
   );

   static HMODULE AddRefByHandle(IN HMODULE hModule)
   {
    assert(hModule);
    assert(s_pfnGetModuleHandleEx);

    HMODULE hModule2;

    KJK::TEST_BOOL_(GetModuleHandleEx)
    (
     GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
     const_cast<LPCTSTR>(reinterpret_cast<LPTSTR>(hModule)),
     &hModule2
    );

    assert(hModule2 == hModule);
    return hModule;
   }

   static HMODULE (* s_pfnAddRef)(IN HMODULE);

   static void GetAddRefFunction()
   {
    HANDLE hKernel32 = LoadLibrary(TEXT("kernel32"));

    if(hKernel32)
    {
     PVOID pfnGetModuleHandleEx =
      GetProcAddress
      (
       hKernel32,
       "GetModuleHandleEx"
#ifdef UNICODE
       "W"
#else
       "A"
#endif
      );

     if(pfnGetModuleHandleEx)
     {
      GetModuleHandleEx =
       reinterpret_cast<BOOL (WINAPI *)(IN DWORD, IN LPCTSTR, OUT HMODULE *)>
       (
        GetModuleHandleEx
       );

      s_pfnAddRef = AddRefByHandle;

      return;
     }
    }

    s_pfnAddRef = AddRefByFileName;
   }

   static HMODULE AddRef(IN HMODULE hModule)
   { return hModule ? s_pfnAddRef(hModule) : NULL; }
#else
   static HMODULE AddRef(IN HMODULE hModule)
   { return hModule ? AddRefByFileName(hModule) : NULL; }
#endif

  public:
   DllRef(): m_hModule(NULL) { }
   DllRef(const DllRef& Other): m_hModule(AddRef(Other.m_hModule)) { }
   DllRef(IN HMODULE hModule): m_hModule(hModule) { }

   DllRef(IN LPCTSTR lpFileName):
    m_hModule(KJK::TEST_HANDLE_(LoadLibrary)(lpFileName)) { }

   ~DllRef() { if(m_hModule) KJK::TEST_BOOL_(FreeLibrary)(m_hModule); }

   PVOID ProcAddress(IN LPCSTR lpProcName)
   { return GetProcAddress(m_hModule, lpProcName); }

   template<class T_> T_ ProcAddress(IN LPCSTR lpProcName, IN const T_&)
   { return reinterpret_cast<T_>(ProcAddress(lpProcName)); }
 
   HMODULE Get() const { return m_hModule; }

   HMODULE Release()
   {
    HMODULE hOld = Get();
    m_hModule = NULL;
    return hOld;
   }

   void Reset(IN HMODULE hModule = NULL)
   {
    HMODULE hOld = Get();
    m_hModule = hModule;
    if(hOld) KJK::TEST_BOOL_(FreeLibrary)(hOld);
   }
 };
};

#endif // KJK_DLL_H_INCLUDED_

// EOF
