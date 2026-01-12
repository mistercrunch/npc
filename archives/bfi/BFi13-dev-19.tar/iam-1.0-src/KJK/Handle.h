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

#ifndef KJK_HANDLE_H_INCLUDED_
#define KJK_HANDLE_H_INCLUDED_

#include <KJK/Status.h>

namespace KJK
{
 class Handle
 {
  private:
   HANDLE m_Handle;

  public:
   Handle(): m_Handle(NULL) { }
   Handle(IN HANDLE h_): m_Handle(h_) { }
   Handle(Handle& Y_): m_Handle(Y_.Release()) { }
   ~Handle() { if(m_Handle) TEST_BOOL_(CloseHandle)(m_Handle); }

   HANDLE& Ref() { return m_Handle; }
   const HANDLE& Ref() const { return m_Handle; }

   HANDLE Get() const { return m_Handle; }

   HANDLE Release()
   {
    HANDLE hOld = Get();
    m_Handle = NULL;
    return hOld;
   }

   void Reset(IN HANDLE h_ = NULL)
   {
    HANDLE hOld = Get();
    m_Handle = h_;
    if(hOld) TEST_BOOL_(CloseHandle)(hOld);
   }

   Handle& operator=(IN HANDLE h_) { Reset(h_); return *this; }
 };
};

#endif // KJK_HANDLE_H_INCLUDED_

// EOF
