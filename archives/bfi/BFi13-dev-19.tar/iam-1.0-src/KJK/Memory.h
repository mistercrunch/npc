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

#ifndef KJK_MEMORY_H_INCLUDED_
#define KJK_MEMORY_H_INCLUDED_

#include <cstddef>
#include <cstdlib>
#include <new>

namespace KJK
{
 template<class TAllocatorImpl> class TAllocator
 {
  private:
   TAllocatorImpl m_Allocator;

  public:
   typedef TAllocatorImpl::Size_t Size_t;
   typedef TAllocatorImpl::Value_t Value_t;

   Value_t * Allocate(IN Size_t cbSize)
   {
    Value_t * p = m_Allocator.GetMem(cbSize);
    if(p == NULL) throw std::bad_alloc();
    else return p;
   }

   Value_t * Allocate() { return Allocate(sizeof(T_)); }

   void Free(IN Value_t * pMem) { m_Allocator.FreeMem(pMem); }
 };

 template<class T_> struct CPRTAllocatorImpl
 {
  typedef std::size_t Size_t;
  typedef T_ Value_t;

  static Value_t * GetMem(IN Size_t cbSize)
  { return reinterpret_cast<Value_t *>(operator new(cbSize, std::nothrow)); }

  static void FreeMem(IN Value_t * pMem)
  { operator delete(pMem, std::nothrow); }
 };

 template<class T_> class CPRTAllocator:
  public TAllocator<CPRTAllocatorImpl<T_> >
 {
  public:
   typedef TAllocator<CPRTAllocatorImpl<T_> >::Value_t Value_t;
   typedef TAllocator<CPRTAllocatorImpl<T_> >::Size_t Size_t;
 };

 template<class T_> struct CRTAllocatorImpl
 {
  typedef std::size_t Size_t;
  typedef T_ Value_t;

  static Value_t * GetMem(IN Size_t cbSize)
  { return reinterpret_cast<Value_t *>(std::malloc(cbSize)); }

  static void FreeMem(IN T_ * pMem) { std::free(pMem); }
 };

 template<class T_> class CRTAllocator:
  public TAllocator<CRTAllocatorImpl<T_> > { };

 template<class T_, class Allocator_ = CRTAllocator<T_> > class Memory
 {
  private:
   T_ * m_Ptr;
   Allocator_ m_Alloc;

  public:
   typedef Allocator_ Allocator_t;

   Memory(): m_Ptr(NULL) { }
   Memory(T_ * p_): m_Ptr(p_) { }
   template<class T2_> Memory(T2_ * p_): m_Ptr(reinterpret_cast<T_>(p_)) { }
   Memory(Allocator_t::Size_t n_): m_Alloc(), m_Ptr(m_Alloc.Allocate(n_)) { }

   Memory(const Memory<T_, Allocator_>& m_):
    m_Ptr(const_cast<Memory<T_, Allocator_> >(m_).Release())
    { }

   ~Memory() { if(m_Ptr) m_Alloc.Free(m_Ptr); }

   T_ * Get() const { return m_Ptr; }
   T_ * operator->() const { return Get(); }

   T_ * Release()
   {
    T_ * pOld = Get();
    m_Ptr = NULL;
    return pOld;
   }

   void Reset(T_ * p_ = NULL)
   {
    T_ * pOld = Get();
    m_Ptr = p_;

    if(pOld)
     m_Alloc.Free(pOld);
   }

   void Reset(Allocator_t::Size_t n_) { Reset(m_Alloc.Allocate(n_)); }

 };
 
};

#endif // KJK_MEMORY_H_INCLUDED_

// EOF
