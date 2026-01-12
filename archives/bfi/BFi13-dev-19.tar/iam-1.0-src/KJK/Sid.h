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

#ifndef KJK_SID_H_INCLUDED_
#define KJK_SID_H_INCLUDED_

#include <cassert>
#include <sddl.h>
#include <KJK/Status.h>
#include <KJK/Handle.h>
#include <KJK/LocalMemory.h>

namespace KJK
{
 class SidBase_: public SID
 {
  private:
   // member-wise compare two low-level SID structures
   static int DeepCompare(IN const SID * pSid1, IN const SID * pSid2)
   {
    // same object: equal
    if(pSid1 == pSid2) return 0;
    // the null object always compares as the smallest
    else if(pSid1 == NULL) return -1;
    else if(pSid2 == NULL) return 1;

    int n;

    // compare the revisions
    n = pSid1->Revision - pSid2->Revision;
    if(n) return n;

    // compare the identifier authorities
    for(unsigned i = 0; i < 6; ++ i)
    {
     n =
      pSid1->IdentifierAuthority.Value[i] - pSid2->IdentifierAuthority.Value[i];

     if(n) return n;
    }

    BYTE l =
     pSid1->SubAuthorityCount < pSid2->SubAuthorityCount ?
      pSid1->SubAuthorityCount :
      pSid2->SubAuthorityCount;

    // compare the subauthorities
    for(BYTE i = 0; i < l; ++ i)
    {
     n = pSid1->SubAuthority[i] - pSid2->SubAuthority[i];
     if(n) return n;
    }

    // compare the number of subauthorities
    n = pSid1->SubAuthorityCount - pSid2->SubAuthorityCount;
    if(n) return n;

    return 0;
   }

  public:
   // MEMBERS
   bool IsValid() const
   { return IsValidSid(const_cast<SID *>(static_cast<const SID *>(this))); }

   DWORD Length() const { return GetSidLengthRequired(SubAuthorityCount); }

   // OPERATORS
   bool operator==(const SidBase_& Y) const
   {
    return DeepCompare
    (
     static_cast<const SID *>(this),
     static_cast<const SID *>(&Y)
    ) == 0;
   }

   bool operator<(const SidBase_& Y) const
   {
    return DeepCompare
    (
     static_cast<const SID *>(this),
     static_cast<const SID *>(&Y)
    ) < 0;
   }

   bool operator>(const SidBase_& Y) const
   {
    return DeepCompare
    (
     static_cast<const SID *>(this),
     static_cast<const SID *>(&Y)
    ) > 0;
   }

   bool operator==(const void * Y) const
   {
    return DeepCompare
    (
     static_cast<const SID *>(this),
     reinterpret_cast<const SID *>(Y)
    ) == 0;
   }

   bool operator<(const void * Y) const
   {
    return DeepCompare
    (
     static_cast<const SID *>(this),
     reinterpret_cast<const SID *>(Y)
    ) < 0;
   }

   bool operator>(const void * Y) const
   {
    return DeepCompare
    (
     static_cast<const SID *>(this),
     reinterpret_cast<const SID *>(Y)
    ) > 0;
   }

   bool operator!=(const SidBase_& Y) const { return !(*this == Y); }
   bool operator>=(const SidBase_& Y) const { return !(*this < Y); }
   bool operator<=(const SidBase_& Y) const { return !(*this > Y); }

   bool operator!=(const void * Y) const { return !(*this == Y); }
   bool operator>=(const void * Y) const { return !(*this < Y); }
   bool operator<=(const void * Y) const { return !(*this > Y); }
 };

 // non-member operators
 bool operator==(IN const void * x, const class SidBase_& y) { return y == x; }
 bool operator!=(IN const void * x, const class SidBase_& y) { return y != x; }
 bool operator<(IN const void * x, const class SidBase_& y) { return y >= x; }
 bool operator>(IN const void * x, const class SidBase_& y) { return y <= x; }
 bool operator<=(IN const void * x, const class SidBase_& y) { return y > x; }
 bool operator>=(IN const void * x, const class SidBase_& y) { return y < x; }

 struct LogonId_t { };
 static const LogonId_t LogonId;

 template<class Ptr_> class SidPtrBase_
 {
  private:
   // construct a SID from a string
   static PVOID FromString
   (
    IN LPCTSTR pszNameOrSidString,
    IN LPCTSTR pszSystemName OPTIONAL
   )
   {
    PSID pSid;

    if(!pszNameOrSidString)
     ThrowError(ERROR_INVALID_PARAMETER);

    // try to parse the group name as a SID string
    if(!pszSystemName && ConvertStringSidToSid(pszNameOrSidString, &pSid))
     return pSid;

    DWORD cbSid = 0;
    DWORD ccDomainName = 0;
    SID_NAME_USE eUse;
    LPCTSTR pszName = pszNameOrSidString;

    // Get the required buffer size
    LookupAccountName
    (
     pszSystemName,
     pszName,
     NULL,
     &cbSid,
     NULL,
     &ccDomainName,
     &eUse
    );

    // failure
    if(cbSid == 0)
     ThrowLastError(TEXT("LookupAccountName"));

    // allocate the buffers
    Ptr_ SidBuf(cbSid);
    LocalMemory< TCHAR > DomainNameBuf(ccDomainName * sizeof(TCHAR));

    // lookup the group name
    TEST_BOOL_(LookupAccountName)
    (
     pszSystemName,
     pszName,
     SidBuf.Get(),
     &cbSid,
     DomainNameBuf.Get(),
     &ccDomainName,
     &eUse
    );

    // success
    return SidBuf.Release();
   }

   // copy a SID from another
   PVOID FromOtherSid(IN PSID pSid)
   { return FromOtherSid(reinterpret_cast<SID *>(pSid)); }

   PVOID FromOtherSid(IN const SID * pSid)
   {
    if(pSid == NULL)
     ThrowError(ERROR_INVALID_PARAMETER);

    if(pSid->Revision != 1)
     ThrowError(ERROR_UNKNOWN_REVISION);

    DWORD nBufSize = GetSidLengthRequired(pSid->SubAuthorityCount);
    PVOID pSidBuf = m_Allocator.Allocate(nBufSize);

    if(pSidBuf)
     std::memcpy(pSidBuf, pSid, static_cast<std::size_t>(nBufSize));

    return pSidBuf;
   }

   // copy a SID from the current process's logon SID
   PVOID FromLogonId()
   {
    Handle hToken;

    // open the primary token
    TEST_BOOL_(OpenProcessToken)
    (
     GetCurrentProcess(),
     TOKEN_QUERY,
     &hToken.Ref()
    );

    DWORD cbGroupsBufSize = 0;

    GetTokenInformation(hToken.Get(), TokenGroups, NULL, 0, &cbGroupsBufSize);

    // Get the required buffer size to query the token's groups
    if(cbGroupsBufSize == 0)
     ThrowLastError(TEXT("GetTokenInformation"));

    // allocate the buffer
    LocalMemory< TOKEN_GROUPS > GroupsBuf(cbGroupsBufSize);

    // query the token's groups
    TEST_BOOL_(GetTokenInformation)
    (
     hToken.Get(),
     TokenGroups,
     GroupsBuf.Get(),
     cbGroupsBufSize,
     &cbGroupsBufSize
    );

    // look for the logon SID
    for(DWORD i = 0; i < GroupsBuf->GroupCount; ++ i)
     if(GroupsBuf->Groups[i].Attributes & SE_GROUP_LOGON_ID)
      return FromOtherSid(reinterpret_cast<SID *>(GroupsBuf->Groups[i].Sid));

    // logon SID not found
    return NULL;
   }

   // FIELDS
   Ptr_ m_Ptr;
   Ptr_::Allocator_t m_Allocator;

  public:
   // CONSTRUCTORS
   // default constructor
   SidPtrBase_() { }

   // copy constructors
   SidPtrBase_(IN const SidPtrBase_& Y):
    m_Allocator(), m_Ptr(FromOtherSid(Y.Get())) { }
    
   SidPtrBase_(IN const SidBase_& Y): m_Allocator(), m_Ptr(FromOtherSid(&Y)) { }
   SidPtrBase_(IN const SID * Y): m_Allocator(), m_Ptr(FromOtherSid(Y)) { }
   SidPtrBase_(IN PSID Y): m_Allocator(), m_Ptr(FromOtherSid(Y)) { }

   // construct a SID from a string and an optional system name
   SidPtrBase_
   (
    IN LPCTSTR pszNameOrSidString,
    IN LPCTSTR pszSystemName = NULL OPTIONAL
   ):
    m_Ptr(FromString(pszNameOrSidString, pszSystemName)) { }

   // construct a SID copied from the current process's logon SID
   SidPtrBase_(IN const LogonId_t&): m_Ptr(FromLogonId()) { }

   // MEMBERS
   const SidBase_ * Get() const
   { return reinterpret_cast< const SidBase_ * >(m_Ptr.Get()); }

   SidBase_ * Get()
   { return reinterpret_cast< SidBase_ * >(m_Ptr.Get()); }

   const SidBase_ * Release()
   { return reinterpret_cast< const SidBase_ * >(m_Ptr.Release()); }

   void Reset(IN SID * p = NULL) { m_Ptr.Reset(FromOtherSid(p)); }

   void Reset
   (
    IN LPCTSTR pszNameOrSidString,
    IN LPCTSTR pszSystemName = NULL OPTIONAL
   )
   { m_Ptr.Reset(FromString(pszNameOrSidString, pszSystemName)); }

   // EXTRACTORS
   operator bool() const { return m_Ptr.Get() != NULL; }
   operator const SidBase_&() const { return *Get(); }

   // OPERATORS
   const SidBase_ * operator->() const { return Get(); }

   bool operator==(const SidBase_& Y) const { return *(Get()) == Y; }
   bool operator<(const SidBase_& Y) const { return *(Get()) < Y; }
   bool operator>(const SidBase_& Y) const { return *(Get()) > Y; }
   bool operator!=(const SidBase_& Y) const { return *(Get()) != Y; }
   bool operator<=(const SidBase_& Y) const { return *(Get()) <= Y; }
   bool operator>=(const SidBase_& Y) const { return *(Get()) >= Y; }
 };

 typedef SidPtrBase_< LocalMemory<VOID> > Sid;

 struct S_;

 // statically allocated SID
 struct SidStatic_: public SidBase_
 {
  friend struct S_;

  private:
   struct Step0;
   DWORD SubAuthorityBuf_[14];

  public:
   // operator- for S-<R>-<I>-<S>-... syntax: append a subauthority to the SID
   const SidStatic_& operator-(IN DWORD dwSubAuthority) const
   {
    SidStatic_ * varthis = const_cast<SidStatic_ *>(this);

    if(SubAuthorityCount < 15)
    {
     varthis->SubAuthority[SubAuthorityCount] = dwSubAuthority;
     ++ varthis->SubAuthorityCount;
    }

    return *this;
   };

   // default constructor: construct an empty SID
   SidStatic_()
   {
    Revision = 1;
    SubAuthorityCount = 0;
    assert(&SubAuthorityBuf_[0] == &SubAuthority[1]);
   }

  protected:
   // construct a static SID 
   SidStatic_(IN BYTE)
   {
    Revision = 1;
    SubAuthorityCount = 0;
    assert(&SubAuthorityBuf_[0] == &SubAuthority[1]);
   }
 };

 struct SidStatic_::Step0: public SidStatic_
 {
  Step0(IN BYTE nRevision): SidStatic_(nRevision) { }

  const SidStatic_& operator-(IN DWORD64 lnIdentifierAuthority) const
  {
   SidStatic_::Step0 * varthis = const_cast<SidStatic_::Step0 *>(this);

   varthis->IdentifierAuthority.Value[0] = (lnIdentifierAuthority >> 40) & 0xFF;
   varthis->IdentifierAuthority.Value[1] = (lnIdentifierAuthority >> 32) & 0xFF;
   varthis->IdentifierAuthority.Value[2] = (lnIdentifierAuthority >> 24) & 0xFF;
   varthis->IdentifierAuthority.Value[3] = (lnIdentifierAuthority >> 16) & 0xFF;
   varthis->IdentifierAuthority.Value[4] = (lnIdentifierAuthority >>  8) & 0xFF;
   varthis->IdentifierAuthority.Value[5] = (lnIdentifierAuthority >>  0) & 0xFF;

   return *this;
  };
 };

 // singleton class for literal SID syntax
 struct S_
 {
  const SidStatic_::Step0 operator-(IN BYTE nRevision) const
  { return nRevision; }
 };

 // "S" singleton for S-R-I-S-S-... syntax
 static const S_ S;
};

#endif // KJK_SID_H_INCLUDED_

// EOF
