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

#ifndef IAM_ACLEDIT_H_INCLUDED_
#define IAM_ACLEDIT_H_INCLUDED_

namespace IAM
{
 /*

  ACL EDITING

 */
 struct AclOperations
 {
  bool fSetNull;
  bool fSetEmpty;
  std::basic_string<EXPLICIT_ACCESS> strOperations;

  AclOperations(): fSetNull(false), fSetEmpty(false) { }

  void SetSpecific(IN Transition eType, IN LPCTSTR pszArgument);
  void SetAll(IN Transition eType);

  void SetSpecific
  (
   IN Transition eType,
   IN const EXPLICIT_ACCESS& eaOperation
  )
  {
   assert(!(fSetEmpty && fSetNull));
   strOperations += eaOperation;
   fSetNull = false;
   assert(!(fSetEmpty && fSetNull));
  }

  bool WorkToDo() const
  { return !strOperations.empty() && !fSetEmpty && !fSetNull; }

  bool GetACL(OUT PACL * ppAcl, OUT bool * pfHasAcl) const
  {
   // null ACL
   if(fSetNull)
   {
    assert(!fSetEmpty);
    assert(strOperations.empty());
    *ppAcl = NULL;
   }
   // no operation
   else if(strOperations.empty())
   {
    // set an empty ACL
    if(fSetEmpty)
    {
     assert(!fSetNull);

     KJK::LocalMemory<ACL> apEmptyAcl(sizeof(ACL));

     apEmptyAcl->AclRevision = ACL_REVISION;
     apEmptyAcl->Sbz1 = 0;
     apEmptyAcl->AclSize = sizeof(ACL);
     apEmptyAcl->AceCount = 0;
     apEmptyAcl->Sbz2 = 0;

     *ppAcl = apEmptyAcl.Release();
    }
    // set no ACL
    else
    {
     *pfHasAcl = false;
     return true;
    }
   }
   // create a new ACL
   else if(fSetEmpty)
    KJK::TEST_DWORD_(SetEntriesInAcl)
    (
     strOperations.size(),
     const_cast<PEXPLICIT_ACCESS>(strOperations.data()),
     NULL,
     ppAcl
    );
   // we need an existing ACL to continue
   else
    return false;

   *pfHasAcl = true;
   return true;
  } // bool GetACL(OUT PACL *, OUT bool *)

  void GetACL(OUT PACL * ppNewAcl, IN PACL pOldAcl) const
  {
   assert(!strOperations.empty());
   assert(!fSetEmpty);
   assert(pOldAcl);

   KJK::TEST_DWORD_(SetEntriesInAcl)
   (
    strOperations.size(),
    const_cast<PEXPLICIT_ACCESS>(strOperations.data()),
    pOldAcl,
    ppNewAcl
   );
  } // void GetACL(OUT PACL **, IN PACL)
 }; // struct AclOperations
} // namespace IAM

#endif // IAM_ACLEDIT_H_INCLUDED_

// EOF

