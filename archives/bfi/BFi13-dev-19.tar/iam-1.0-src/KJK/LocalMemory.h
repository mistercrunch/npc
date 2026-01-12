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

#ifndef KJK_LOCALMEMORY_H_INCLUDED_
#define KJK_LOCALMEMORY_H_INCLUDED_

#include <KJK/Status.h>
#include <KJK/Memory.h>

namespace KJK
{
 template<class T_> struct LocalMemoryAllocatorImpl
 {
  typedef SIZE_T Size_t;
  typedef T_ Value_t;

  static Value_t * GetMem(IN Size_t nSize)
  {
   return reinterpret_cast<Value_t *>
   (
    TEST_PVOID_(LocalAlloc)(LMEM_FIXED, nSize)
   );
  }

  static void FreeMem(IN Value_t * pMem) { LocalFree(pMem); }
 };

 template<class T_> class LocalMemoryAllocator:
  public TAllocator<LocalMemoryAllocatorImpl<T_> > { };

 template<class T_> class LocalMemory:
  public Memory<T_, LocalMemoryAllocator<T_> >
 {
  private:
   typedef Memory<T_, LocalMemoryAllocator<T_> > Super_;

  public:
   LocalMemory(): Super_() { }
   LocalMemory(T_ * p_): Super_(p_) { }
   LocalMemory(LocalMemoryAllocator<T_>::Size_t n_): Super_(n_) { }
 };
};

#endif // KJK_LOCALMEMORY_H_INCLUDED_

// EOF
