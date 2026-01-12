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

#ifndef IAM_PRIV_H_INCLUDED_
#define IAM_PRIV_H_INCLUDED_

#include "Items.h"

namespace IAM
{
 namespace Priv
 {
  enum Region
  {
   Most,
   Others,
   RegionCount
  };

  struct Item: public LUID
  {
   Item() { }

   Item(IN const LUID& Other)
   {
    LowPart = Other.LowPart;
    HighPart = Other.HighPart;
   }

   Item(IN LPCTSTR pszPrivName)
   {
    static const TCHAR pszPrivPrefix[] = TEXT("Se");
    static const TCHAR pszPrivSuffix[] = TEXT("Privilege");

    static const std::size_t c_ccPrefixLength =
     sizeof(pszPrivPrefix) / sizeof(pszPrivPrefix[0]) - 1;

    static const std::size_t c_ccSuffixLength =
     sizeof(pszPrivSuffix) / sizeof(pszPrivSuffix[0]) - 1;

    static const std::size_t c_ccPreSufLength =
     c_ccPrefixLength + c_ccSuffixLength;

    static const std::size_t c_ccMaxLength = (std::size_t)(-1);

    LPTSTR pszPrivFullName;
    KJK::Memory<TCHAR> apszFullNameBuf;
    KJK::Memory<TCHAR>::Allocator_t Allocator;
    std::size_t ccNameLength = std::_tcslen(pszPrivName);

    assert(c_ccMaxLength / sizeof(TCHAR) - 1 >= c_ccPreSufLength);

    if
    (
     // name long enough to contain "Se", "Privilege" and a name
     ccNameLength > c_ccPreSufLength &&
     // name begins with "Se"
     std::_tcsnicmp(pszPrivName, pszPrivPrefix, c_ccPrefixLength) == 0 &&
     // name ends with "Privilege"
     std::_tcsnicmp
     (
      pszPrivName + ccNameLength - c_ccSuffixLength,
      pszPrivSuffix,
      c_ccSuffixLength
     ) == 0
    )
     pszPrivFullName = const_cast<LPTSTR>(pszPrivName);
    // full name too long
    else if
    (
     ccNameLength > (c_ccMaxLength / sizeof(TCHAR) - (c_ccPreSufLength + 1))
    )
     KJK::ThrowError(ERROR_BUFFER_OVERFLOW);
    // build the full name
    else
    {
     // allocate the buffer
     apszFullNameBuf.Reset
     (
      Allocator.Allocate
      (
       (ccNameLength + c_ccPreSufLength + 1) * sizeof(TCHAR)
      )
     );

     pszPrivFullName = apszFullNameBuf.Get();

     LPTSTR p = pszPrivFullName;

     std::memcpy(p, pszPrivPrefix, c_ccPrefixLength * sizeof(TCHAR));
     p += c_ccPrefixLength;

     std::memcpy(p, pszPrivName, ccNameLength * sizeof(TCHAR));
     p += ccNameLength;

     std::memcpy(p, pszPrivSuffix, c_ccSuffixLength * sizeof(TCHAR));
     p += c_ccSuffixLength;

     *p = 0;
    }

    KJK::TEST_BOOL_(LookupPrivilegeValue)(NULL, pszPrivFullName, this);
   }

   bool operator<(IN const Item& Other) const
   {
    if(LowPart < Other.LowPart) return true;
    else if(HighPart < Other.HighPart) return true;
    else return false;
   }

   bool operator==(IN const Item& Other) const
   {
    return LowPart == Other.LowPart && HighPart == Other.HighPart;
   }
  };

  typedef ItemStateTransition StateTransition;
  typedef ItemState<> State;

  typedef ItemStates<Item, State, StateTransition> RegionStates;
 } // namespace Priv
} // namespace IAM

#endif // IAM_PRIV_H_INCLUDED_

// EOF
