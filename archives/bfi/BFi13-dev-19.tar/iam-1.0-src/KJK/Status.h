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

#ifndef KJK_STATUS_H_INCLUDED_
#define KJK_STATUS_H_INCLUDED_

#include <tchar.h>

namespace KJK
{
#define KJK_ID_(X__) X__

 class Error
 {
  public:
   virtual LPCTSTR Message() = 0;
 };

 class MsgError: public Error
 {
  private:
   LPCTSTR m_pszMessage;

  public:
   MsgError(IN LPCTSTR pszMessage): m_pszMessage(pszMessage) { }
   MsgError(IN const MsgError& y): m_pszMessage(y.m_pszMessage) { }
   virtual LPCTSTR Message() { return m_pszMessage; }
 };

 void ThrowError(IN DWORD);
 void ThrowError(IN DWORD, IN LPCTSTR);
 void ThrowLastError();
 void ThrowLastError(IN LPCTSTR);

 struct BOOLCheck_
 {
  LPCTSTR m_pszFuncName;

  BOOLCheck_(IN LPCTSTR pszFuncName): m_pszFuncName(pszFuncName) { }

  BOOL operator=(IN BOOL bRet)
  {
   if(!bRet) ThrowLastError(m_pszFuncName);
   return bRet;
  }
 };

 struct DWORDCheck_
 {
  LPCTSTR m_pszFuncName;

  DWORDCheck_(IN LPCTSTR pszFuncName): m_pszFuncName(pszFuncName) { }

  DWORD operator=(IN DWORD dwErrCode)
  {
   if(dwErrCode != NO_ERROR) ThrowLastError(m_pszFuncName);
   return NO_ERROR;
  }
 };

 struct PVOIDCheck_
 {
  LPCTSTR m_pszFuncName;

  PVOIDCheck_(IN LPCTSTR pszFuncName): m_pszFuncName(pszFuncName) { }

  template<class T_> T_ operator=(IN T_ hRet)
  {
   if(hRet == NULL) ThrowLastError(m_pszFuncName);
   return hRet;
  }
 };

 struct SIZECheck_
 {
  LPCTSTR m_pszFuncName;

  SIZECheck_(IN LPCTSTR pszFuncName): m_pszFuncName(pszFuncName) { }

  template<class T_> T_ operator=(IN T_ nSize)
  {
   if(nSize == 0) ThrowLastError(m_pszFuncName);
   return hRet;
  }
 };

#define TEST_BOOL_(F_) BOOLCheck_(TEXT(KJK_ID_(#F_))) = F_
#define TEST_DWORD_(F_) DWORDCheck_(TEXT(KJK_ID_(#F_))) = F_
#define TEST_HANDLE_(F_) PVOIDCheck_(TEXT(KJK_ID_(#F_))) = F_
#define TEST_PVOID_(F_) PVOIDCheck_(TEXT(KJK_ID_(#F_))) = F_
#define TEST_SIZE_(F_) SIZECheck_(TEXT(KJK_ID_(#F_))) = F_
} // namespace KJK

#include <KJK/LocalMemory.h>

namespace KJK
{

 class SystemError: public Error
 {
  private:
   DWORD m_dwErrCode;
   LocalMemory<TCHAR> m_apszMessage;

  public:
   SystemError(IN DWORD dwErrCode): m_dwErrCode(dwErrCode) { }
   SystemError(IN const SystemError& y): m_dwErrCode(y.m_dwErrCode) { }

   virtual LPCTSTR Message()
   {
    if(m_apszMessage.Get() == NULL)
    {
     LPTSTR pMessage = NULL;

     DWORD ccMessageLength = FormatMessage
     (
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL,
      m_dwErrCode,
      0,
      reinterpret_cast<LPTSTR>(&pMessage),
      0,
      NULL
     );

     if(ccMessageLength)
     {
      assert(pMessage);
      m_apszMessage.Reset(pMessage);

      for
      (
       LPTSTR p = pMessage + ccMessageLength - 1;
       p && p != pMessage;
       p = _tcsdec(pMessage, p)
      )
       if(std::_istspace(_tcsnextc(p)))
        *p = 0;
       else
        break;
     }
    }

    return m_apszMessage.Get();
   }
 };

 class SystemFuncError: public Error
 {
  protected:
   DWORD m_dwErrCode;
   LPCTSTR m_pszFuncName;
   LocalMemory<TCHAR> m_apszMessage;

  public:
   SystemFuncError(IN DWORD dwErrCode, IN LPCTSTR pszFuncName):
    m_dwErrCode(dwErrCode), m_pszFuncName(pszFuncName)
   { }

   SystemFuncError(IN const SystemFuncError& y):
    m_dwErrCode(y.m_dwErrCode), m_pszFuncName(y.m_pszFuncName)
   { }

   virtual LPCTSTR Message()
   {
    if(m_apszMessage.Get() == NULL)
    {
     LPTSTR pMessage = NULL;

     DWORD ccMessageLength = FormatMessage
     (
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
      NULL,
      m_dwErrCode,
      0,
      reinterpret_cast<LPTSTR>(&pMessage),
      0,
      NULL
     );

     if(ccMessageLength)
     {
      LPTSTR pFullMessage = NULL;
      assert(pMessage);

      for
      (
       LPTSTR p = pMessage + ccMessageLength - 1;
       p && p != pMessage;
       p = _tcsdec(pMessage, p)
      )
       if(std::_istspace(_tcsnextc(p)))
        *p = 0;
       else
        break;

      {
       LocalMemory<void> ap(pMessage);

       ULONG_PTR a_Arguments[] =
       {
        reinterpret_cast<ULONG_PTR>(m_pszFuncName),
        static_cast<ULONG_PTR>(m_dwErrCode),
        reinterpret_cast<ULONG_PTR>(pMessage)
       };

       ccMessageLength = FormatMessage
       (
         FORMAT_MESSAGE_ALLOCATE_BUFFER |
         FORMAT_MESSAGE_FROM_STRING |
         FORMAT_MESSAGE_ARGUMENT_ARRAY,
        TEXT("%1!s!(): error 0x%2!X!: %3!s!"),
        0,
        0,
        reinterpret_cast<LPTSTR>(&pFullMessage),
        0,
        (va_list *)a_Arguments
       );
      }

      if(ccMessageLength)
      {
       assert(pFullMessage);
       m_apszMessage.Reset(pFullMessage);
      }
     }
    }

    return m_apszMessage.Get();
   }
 };

 void ThrowError(IN DWORD dwErrCode) { throw SystemError(dwErrCode); }

 void ThrowError(IN DWORD dwErrCode, IN LPCTSTR pszFuncName)
 { throw SystemFuncError(dwErrCode, pszFuncName); }

 void ThrowLastError() { ThrowError(::GetLastError()); }

 void ThrowLastError(IN LPCTSTR pszFuncName)
 { ThrowError(::GetLastError(), pszFuncName); }

} // namespace KJK

#endif // KJK_STATUS_H_INCLUDED_

// EOF
