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

#include <cassert>
#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <new>
#include <set>
#include <map>
#include <string>
#include <functional>

#include <tchar.h>

#include <windows.h>
#include <accctrl.h>
#include <aclapi.h>

#include <KJK/Sid.h>
#include <KJK/Dll.h>
#include <KJK/Handle.h>
#include <KJK/Memory.h>
#include <KJK/Process.h>
#include <KJK/Status.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "iam.h"
#include "ACLEdit.h"
#include "GAcc.h"
#include "Groups.h"
#include "Group.h"
#include "RGroup.h"
#include "Priv.h"

// if we're using a code validator, don't override the allocator
#if !defined(__CODEGUARD__)

#if !defined(__BORLANDC__)
void * operator new(std::size_t n, const std::nothrow_t&)
{ return std::malloc(n); }

void * operator new[](std::size_t n, const std::nothrow_t&)
{ return std::malloc(n); }

void operator delete(void * p, const std::nothrow_t&) { std::free(p); }
void operator delete[](void * p, const std::nothrow_t&) { std::free(p); }
#endif

void * operator new(std::size_t n)
{ void * p = std::malloc(n); if(p) return p; else throw std::bad_alloc(); }

void * operator new[](std::size_t n)
{ void * p = std::malloc(n); if(p) return p; else throw std::bad_alloc(); }

void operator delete(void * p) { std::free(p); }
void operator delete[](void * p) { std::free(p); }

namespace IAM
{
 static HANDLE g_hHeap = NULL;

 HANDLE GetHeap()
 {
  if(g_hHeap == NULL)
  {
   g_hHeap = HeapCreate(HEAP_NO_SERIALIZE | HEAP_GENERATE_EXCEPTIONS, 0, 0);
   if(g_hHeap == NULL) ExitProcess(GetLastError());
  }

  return g_hHeap;
 }

 void DestroyHeap() { HeapDestroy(GetHeap()); }

 extern "C"
 {
  void * __cdecl malloc(std::size_t n)
  { return HeapAlloc(GetHeap(), 0, n ? n : 1); }

  void * __cdecl realloc(void * p, std::size_t n)
  {
   if(n)
   {
    if(p)
     return HeapReAlloc(GetHeap(), 0, p, n);
    else
     return HeapAlloc(GetHeap(), 0, n);
   }
   else if(p)
    HeapFree(GetHeap(), 0, p);

   return NULL;
  }

  void __cdecl free(void * p) { HeapFree(GetHeap(), 0, p); }
 }
} // namespace IAM
#else
namespace IAM { void DestroyHeap() { } }
#endif

namespace IAM
{
 /*

  EXCEPTIONS

 */
 static const KJK::MsgError ENoCommand = TEXT("no command specified");
 static const KJK::MsgError EUnknownOption = TEXT("unknown option");
 static const KJK::MsgError EInvalidArgument = TEXT("invalid argument");
 static const KJK::MsgError EInvalidAccessMask = TEXT("invalid access mask");

 static const KJK::MsgError EInvalidInheritance =
  TEXT("invalid inheritance flags");

 static const KJK::MsgError ECantDisableGroup =
  TEXT("can't disable a mandatory group");

 static const KJK::MsgError ECantEnableGroup =
  TEXT("can't enable a deny-only group");

 /*

  GLOBAL VARIABLES

 */
 static struct Globals_
 {
  bool fDone;
  GroupSet SidPool;
  LPCTSTR pszContext;

  // -nowait
  bool fWait;

  // -priv
  bool fRPriv;
  Priv::RegionStates PrivStates[Priv::RegionCount];

  // -group
  Group::RegionStates GroupsStates[Group::RegionCount];

  // -rtoken
  bool fRToken;

  // -mrgroup
  GroupSet MRGroups;

  // -rgroup
  RGroup::RegionStates RGroupStates[RGroup::RegionCount];

  // -tokacl
  const IntuitiveGenericMapping c_igmToken;
  AclOperations TokAcl;

  // -acl
  const IntuitiveGenericMapping c_igmFile;
  AclOperations DefAcl;
  PACL paclDefault;
  SECURITY_DESCRIPTOR sdDefault;
  KJK::LocalMemory<void> apDefaultACLMem;

  // -owner
  LPCTSTR pszOwnerOpt;
  KJK::Sid psidOwner;

  // -pgroup
  LPCTSTR pszPGroupOpt;
  KJK::Sid psidPGroup;

  Globals_():
   fDone(false),
   pszContext(NULL),

   // -nowait
   fWait(true),

   // -priv
   fRPriv(false),

   // -rtoken
   fRToken(false),

   // -tokacl
   c_igmToken(TOKEN_READ, TOKEN_WRITE, TOKEN_EXECUTE, TOKEN_ALL_ACCESS),

   // -acl
   paclDefault(NULL),
   c_igmFile
   (
    FILE_GENERIC_READ,
    FILE_GENERIC_WRITE,
    FILE_GENERIC_EXECUTE,
    FILE_ALL_ACCESS
   ),

   // -owner
   pszOwnerOpt(NULL),

   // -pgroup
   pszPGroupOpt(NULL)
  {
   InitializeSecurityDescriptor(&sdDefault, SECURITY_DESCRIPTOR_REVISION);
  }

  ~Globals_()
  {
  }
 }
 g_;

#define IAM_WITHCONTEXT_(CTX_) \
 { \
  LPCTSTR pszPrevContext_ = g_.pszContext; \
  g_.pszContext = (CTX_);

#define IAM_ENDCONTEXT_ \
  g_.pszContext = pszPrevContext_; \
 }

 GroupSet::const_iterator GroupItem::FromSid(IN const KJK::Sid& Sid)
 {
  GroupSet::const_iterator p = g_.SidPool.find(Sid);

  if(p == g_.SidPool.end())
  {
   std::pair<GroupSet::const_iterator, bool> ret = g_.SidPool.insert(Sid);

   assert(ret.first != g_.SidPool.end());
   assert(ret.second);

   p = ret.first;
  }

  return p;
 }

 /*

  COMMAND-LINE OPTIONS PARSING

 */
 namespace Opt
 {
  Prefix ParsePrefix(IN OUT const _TCHAR ** ppszStrTail)
  {
   // states of the parser machine
   enum
   {
    StateBegin,
    StateMinus,
    StatePlus,
    StateMinusMinus,
    StatePlusPlus
   }
   eState = StateBegin;

   // input loop
   for(;;)
   {
    switch(eState)
    {
     case StateBegin:
     {
      switch(*(*ppszStrTail))
      {
       case _T('-'): eState = StateMinus; break; // -<option>
       case _T('+'): eState = StatePlus; break;  // +<option>
       default: return NoPrefix;                 // <option> [match]
      }

      break;
     }

     case StateMinus:
     {
      switch(*(*ppszStrTail))
      {
       case _T('-'): eState = StateMinusMinus; break; // --<option>
       default: return Minus;                         // -<option> [match]
      }

      break;
     }

     case StatePlus:
     {
      switch(*(*ppszStrTail))
      {
       case _T('+'): eState = StatePlusPlus; break; // ++<option>
       default: return Plus;                        // +<option> [match]
      }

      break;
     }

     case StateMinusMinus: return MinusMinus; // ++<option> [match]
     case StatePlusPlus: return PlusPlus;     // --<option> [match]
    }

    ++ (*ppszStrTail);
   }
  } // ParsePrefix()

  Modifier ParseModifier(IN OUT const _TCHAR ** ppszStrTail)
  {
   if(*(*ppszStrTail) == _T('#'))
   {
    ++ (*ppszStrTail);
    return Hash;
   }

   return NoModifier;
  } // ParseModifier()

  Type ParseType(IN OUT const _TCHAR ** ppszStrTail)
  {
#define IAM_OPT_TYPE_TEXT(S_) \
 (S_), \
 (sizeof(S_) / sizeof((S_)[0]) - 1)

   static const struct
   {
    const _TCHAR * pszName;
    std::size_t ccNameLength;
    Type eType;
   }
   sa_OptTypeTable[] =
   {
    { IAM_OPT_TYPE_TEXT(_T("help")), Help },
    { IAM_OPT_TYPE_TEXT(_T("about")), About },
    { IAM_OPT_TYPE_TEXT(_T("nowait")), NoWait },
    { IAM_OPT_TYPE_TEXT(_T("priv")), Priv },
    { IAM_OPT_TYPE_TEXT(_T("group")), Group },
    { IAM_OPT_TYPE_TEXT(_T("rtoken")), RToken },
    { IAM_OPT_TYPE_TEXT(_T("mrgroup")), MRGroup },
    { IAM_OPT_TYPE_TEXT(_T("rgroup")), RGroup },
    { IAM_OPT_TYPE_TEXT(_T("tokacl")), TokAcl },
    { IAM_OPT_TYPE_TEXT(_T("acl")), Acl },
    { IAM_OPT_TYPE_TEXT(_T("owner")), Owner },
    { IAM_OPT_TYPE_TEXT(_T("pgroup")), PGroup },
    { IAM_OPT_TYPE_TEXT(_T("typical")), Typical },
    { IAM_OPT_TYPE_TEXT(_T("wincompat")), WinCompat }
   };

#undef IAM_OPT_TYPE_TEXT

   static const std::size_t s_nOptTypeEntries =
    sizeof(sa_OptTypeTable) / sizeof(sa_OptTypeTable[0]);

   // look for a ':' or '=' that separates the option from its argument
   std::size_t ccOptLen = std::_tcscspn(*ppszStrTail, _T(":="));

   // empty option name
   if(ccOptLen == 0) return Unknown;

   const _TCHAR * pszNameBegin = *ppszStrTail;

   // if ':' or '=' follows the option name, skip it
   if((*ppszStrTail)[ccOptLen] != 0)
    ++ (*ppszStrTail);

   // skip the option name
   (*ppszStrTail) += ccOptLen;

   // try to recognize the option name
   for(std::size_t i = 0; i < s_nOptTypeEntries; ++ i)
    if
    (
     ccOptLen == sa_OptTypeTable[i].ccNameLength &&
     std::_tcsncmp(pszNameBegin, sa_OptTypeTable[i].pszName, ccOptLen) == 0
    )
     return sa_OptTypeTable[i].eType;

   // option name not recognized
   return Unknown;
  } // ParseType()

  ArgType ParseArgType(IN OUT const _TCHAR ** ppszStrTail)
  {
   enum
   {
    StateBegin,
    StateStar,
    StateStarStar
   }
   eState = StateBegin;

   for(;;)
   {
    switch(eState)
    {
     case StateBegin:
     {
      switch(*(*ppszStrTail))
      {
       case _T('*'): eState = StateStar; break;
       default: return TextArg;
      }

      break;
     }

     case StateStar:
     {
      switch(*(*ppszStrTail))
      {
       case _T('*'): eState = StateStarStar; break;
       default: return Star;
      }

      break;
     }

     case StateStarStar: default: return StarStar;
    }

    ++ (*ppszStrTail);
   }
  } // ParseArgType()

  void ParseAccess
  (
   IN Transition eType,
   IN LPCTSTR pszArgument,
   OUT EXPLICIT_ACCESS * pOperation
  )
  {
   assert(pszArgument);
   assert(pOperation);

   // set some defaults
   pOperation->grfAccessPermissions = IntuitiveGenericAll;
   pOperation->grfInheritance = 0;
   pOperation->Trustee.pMultipleTrustee = NULL;
   pOperation->Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
   pOperation->Trustee.TrusteeForm = TRUSTEE_IS_SID;
   pOperation->Trustee.TrusteeType = TRUSTEE_IS_UNKNOWN;

   // set the access mode
   switch(eType)
   {
    case DoDelete: pOperation->grfAccessMode = DENY_ACCESS; break;
    case DoNegative: pOperation->grfAccessMode = REVOKE_ACCESS; break;
    case DoPositive: pOperation->grfAccessMode = GRANT_ACCESS; break;
    case DoUndelete: pOperation->grfAccessMode = SET_ACCESS; break;
    default: assert(false);
   }

   // find the
   const _TCHAR * p = std::_tcschr(pszArgument, _T(','));

   // parse the group name
   pOperation->Trustee.ptstrName =
    const_cast<LPTSTR>
    (
     reinterpret_cast<LPCTSTR>
     (
      // insert the SID in the pool
      g_.SidPool.insert
      (
       KJK::Sid
       (
        // did the argument contain a comma?
        p ?
         // yes: use the argument up to the comma
         std::basic_string<_TCHAR>(pszArgument, p - pszArgument).data() :
         // no: use the whole argument
         pszArgument
       )
      ).first->Get()
     )
    );

   // parse the access mask

   // skip the comma
   if(p) p = _tcsinc(p);

   unsigned int c;

   c = p ? _tcsnextc(p) : 0;

   if(c == 0)
    return;

   DWORD dwAccessPermissions = IntuitiveNone;

   for(; c; p = _tcsinc(p), c = _tcsnextc(p))
   {
    // end of access mask
    if(c == _T(','))
    {
     // if no access specified, use F
     if(dwAccessPermissions == IntuitiveNone)
      dwAccessPermissions = IntuitiveGenericAll;

     // skip the comma
     p = _tcsinc(p);
     break;
    }
    // read
    else if(std::_tcsnicmp(p, _T("R"), 1) == 0)
     pOperation->grfAccessPermissions |= IntuitiveGenericRead;
    // write
    else if(std::_tcsnicmp(p, _T("W"), 1) == 0)
     pOperation->grfAccessPermissions |= IntuitiveGenericWrite;
    // execute
    else if(std::_tcsnicmp(p, _T("X"), 1) == 0)
     pOperation->grfAccessPermissions |= IntuitiveGenericExecute;
    // change
    else if(std::_tcsnicmp(p, _T("C"), 1) == 0)
     pOperation->grfAccessPermissions |= IntuitiveGenericChange;
    // full
    else if(std::_tcsnicmp(p, _T("F"), 1) == 0)
     pOperation->grfAccessPermissions |= IntuitiveGenericAll;
    // invalid argument
    else
     throw EInvalidAccessMask;
   } // for(; c; p = _tcsinc(p), c = _tcsnextc(p))

   pOperation->grfAccessPermissions = dwAccessPermissions;

   if(p && _tcsnextc(p))
   {
    pOperation->grfInheritance = 0;

    static const struct
    {
     const _TCHAR * pszChar;
     DWORD grfInheritance;
    }
    c_a_InheritanceFlags[] =
    {
     { _T("O"), OBJECT_INHERIT_ACE },
     { _T("C"), CONTAINER_INHERIT_ACE },
     { _T("I"), INHERIT_ONLY_ACE },
     { _T("1"), NO_PROPAGATE_INHERIT_ACE }
    };

    static const std::size_t c_ccInheritanceFlags =
     sizeof(c_a_InheritanceFlags) / sizeof(c_a_InheritanceFlags[0]);

    // parse the inheritance flags
    for(std::size_t i = 0; i < c_ccInheritanceFlags; ++ i)
    {
     if(std::_tcsnicmp(p, c_a_InheritanceFlags[i].pszChar, 1) == 0)
     {
      pOperation->grfInheritance |= c_a_InheritanceFlags[i].grfInheritance;
      p = _tcsinc(p);
      if(_tcsnextc(p) == 0) break;
     }
    }

    c = _tcsnextc(p);

    // unknown character: invalid argument
    if(c != 0)
     throw EInvalidInheritance;

    // neither O nor C specified
    if(!(pOperation->grfInheritance & SUB_CONTAINERS_AND_OBJECTS_INHERIT))
     // imply both
     pOperation->grfInheritance |= SUB_CONTAINERS_AND_OBJECTS_INHERIT;
   }
  } // ParseAccess()
 } // namespace Opt

 /*

  TOKACL OPTION

 */
 namespace TokACL
 {
  void SetSpecific
  (
   IN Transition eType,
   IN bool,
   IN LPCTSTR pszArgument,
   IN LPCTSTR
  )
  {
   g_.TokAcl.SetSpecific(eType, pszArgument);
  } // SetSpecific()

  void SetAll(IN Transition eType, IN bool, IN LPCTSTR, IN LPCTSTR)
  {
   g_.TokAcl.SetAll(eType);
  } // SetAll()
 } // namespace TokACL

 /*

  DEFACL OPTION

 */
 namespace DefACL
 {
  void SetSpecific
  (
   IN Transition eType,
   IN bool,
   IN LPCTSTR pszArgument,
   IN LPCTSTR
  )
  {
   g_.DefAcl.SetSpecific(eType, pszArgument);
  } // SetSpecific()

  void SetAll(IN Transition eType, IN bool, IN LPCTSTR, IN LPCTSTR)
  {
   g_.DefAcl.SetAll(eType);
  } // SetAll()
 } // namespace DefACL

 /*

  GROUP OPTION

 */
 namespace Group
 {
  Region GetRegion(IN const KJK::Sid& pSid)
  {
   using KJK::Sid;
   using KJK::S;

   static const Sid s_pLogonId(KJK::LogonId);

   if(pSid == S-1-1-0 || pSid == S-1-5-32-545 || pSid == s_pLogonId)
    return Others;
   else
    return Most;
  } // GetRegion()

  void SetSpecific
  (
   IN Transition eType,
   IN bool fCritical,
   IN LPCTSTR pszArgument,
   IN LPCTSTR pszText,
   IN const KJK::Sid& pSid
  )
  {
   g_.GroupsStates[GetRegion(pSid)].State
   (
    Item(pSid),
    StateTransition(eType, fCritical, pszArgument, pszText)
   );

   if(eType == DoDelete || eType == DoUndelete)
    Set::RToken(DoNegative, false, NULL, pszText);
  } // SetSpecific()

  void SetSpecific
  (
   IN Transition eType,
   IN bool fCritical,
   IN LPCTSTR pszArgument,
   IN LPCTSTR pszText
  )
  {
   SetSpecific(eType, fCritical, pszArgument, pszText, KJK::Sid(pszArgument));
  } // SetSpecific()

  void SetMost
  (
   IN Transition eType,
   IN bool fCritical,
   IN LPCTSTR,
   IN LPCTSTR pszText
  )
  {
   g_.GroupsStates[Most].
    State(StateTransition(eType, fCritical, NULL, pszText));

   if(eType == DoDelete || eType == DoUndelete)
    Set::RToken(DoNegative, false, NULL, pszText);
  } // SetMost()

  void SetAll
  (
   IN Transition eType,
   IN bool fCritical,
   IN LPCTSTR,
   IN LPCTSTR pszText
  )
  {
   for(unsigned i = 0; i < RegionCount; ++ i)
    g_.GroupsStates[i].
     State(StateTransition(eType, fCritical, NULL, pszText));

   if(eType == DoDelete || eType == DoUndelete)
    Set::RToken(DoNegative, false, NULL, pszText);
  } // SetAll()
 } // namespace Group

 /*

  RGROUP OPTION

 */
 namespace RGroup
 {
  Region GetRegion(IN const KJK::Sid& pSid)
  {
   using KJK::Sid;
   using KJK::S;

   // FIXME? this shouldn't be needed. But who adds the sandbox group (S-1-5-12)
   // to the ACLs of their window stations, really?
   // static const Sid s_apLogonId(KJK::LogonId);
   static const Sid s_apIAMGroup(TEXT("IAM"));

   if
   (
    // Everyone
    pSid == S-1-1-0 ||
    // BUILTIN\Users
    pSid == S-1-5-32-545 ||
    // RESTRICTIONS
    pSid == S-1-5-12 ||
    // IAM
    pSid == s_apIAMGroup // ||
    //pSid == s_apLogonId
   )
    return Others;
   else if
   (
    // BUILTIN\Administrators
    pSid == S-1-5-32-544 ||
    // BUILTIN\Power Users
    pSid == S-1-5-32-547
   )
    return SuperUsers;
   else
    return Most;
  } // GetRegion()

  void SetSpecific
  (
   IN enum Transition eType,
   IN bool fCritical,
   IN LPCTSTR pszArgument,
   IN LPCTSTR pszText
  )
  {
   switch(eType)
   {
    case DoNegative:
    case DoPositive:
     break;

    case DoDelete:
    case DoUndelete:
     throw EInvalidArgument;

    default:
     assert(false);
   }

   KJK::Sid pSid(pszArgument);

   g_.RGroupStates[GetRegion(pSid)].State
   (
    Item(pSid),
    StateTransition(eType, fCritical, pszArgument, pszText)
   );

   Set::RToken(DoNegative, false, NULL, pszText);
  } // SetSpecific()

  void SetMost
  (
   IN enum Transition eType,
   IN bool fCritical,
   IN LPCTSTR,
   IN LPCTSTR pszText
  )
  {
   switch(eType)
   {
    // -rgroup=*
    case DoNegative:
     g_.RGroupStates[Most].
      State(StateTransition(eType, fCritical, NULL, pszText));

     g_.RGroupStates[SuperUsers].
      State(StateTransition(eType, fCritical, NULL, pszText));

     break;

    // +rgroup=*
    case DoPositive:
     g_.RGroupStates[Others].
      State(StateTransition(eType, fCritical, NULL, pszText));

     break;

    case DoDelete:
    case DoUndelete:
     throw EInvalidArgument;

    default:
     assert(false);
   }

   Set::RToken(DoNegative, false, NULL, pszText);
  } // SetMost()

  void SetAll
  (
   IN enum Transition eType,
   IN bool fCritical,
   IN LPCTSTR,
   IN LPCTSTR pszText
  )
  {
   switch(eType)
   {
    // -rgroup=**
    case DoNegative:
    {
     for(unsigned i = 0; i < RegionCount; ++ i)
      g_.RGroupStates[i].
       State(StateTransition(eType, fCritical, NULL, pszText));

     break;
    }

    // +rgroup=**
    case DoPositive:
    {
     g_.RGroupStates[Most].
      State(StateTransition(eType, fCritical, NULL, pszText));

     break;
    }

    case DoDelete:
    case DoUndelete:
     throw EInvalidArgument;

    default:
     assert(false);
   }

   Set::RToken(DoNegative, false, NULL, pszText);
  } // SetAll()

 } // namespace RGroup

 /*

  PRIV OPTION

 */
 namespace Priv
 {
  Region GetRegion(IN const Item& Priv)
  {
   static Item ChangeNotify(TEXT("SeChangeNotifyPrivilege"));

   if(Priv == ChangeNotify) return Others;
   else return Most;
  }

  void SetSpecific
  (
   IN Transition eType,
   IN bool fCritical,
   IN LPCTSTR pszArgument,
   IN LPCTSTR pszText
  )
  {
   Item Privilege(pszArgument);

   g_.PrivStates[GetRegion(Privilege)].State
   (
    Privilege,
    StateTransition(eType, fCritical, pszArgument, pszText)
   );

   if(eType == DoDelete || eType == DoUndelete)
    g_.fRPriv = true;
  } // SetSpecific()

  void SetMost
  (
   IN Transition eType,
   IN bool fCritical,
   IN LPCTSTR,
   IN LPCTSTR pszText
  )
  {
   g_.PrivStates[Most].State(StateTransition(eType, fCritical, NULL, pszText));

   if(eType == DoDelete || eType == DoUndelete)
    g_.fRPriv = true;
  } // SetMost()

  void SetAll
  (
   IN Transition eType,
   IN bool fCritical,
   IN LPCTSTR,
   IN LPCTSTR pszText
  )
  {
   for(unsigned i = 0; i < RegionCount; ++ i)
    g_.PrivStates[i].State(StateTransition(eType, fCritical, NULL, pszText));

   if(eType == DoDelete || eType == DoUndelete)
    g_.fRPriv = true;
  } // SetAll()
 } // namespace Priv

 /*

  MRGROUP OPTION

 */
 namespace MRGroup
 {
  void SetSpecific
  (
   IN Transition eType,
   IN bool fCritical,
   IN LPCTSTR pszArgument,
   IN LPCTSTR pszText
  )
  {
   switch(eType)
   {
    case DoPositive:
     g_.MRGroups.insert(KJK::Sid(pszArgument));
     break;

    case DoNegative:
     g_.MRGroups.erase(KJK::Sid(pszArgument));
     break;

    case DoDelete:
    case DoUndelete:
     throw EInvalidArgument;

    default:
     assert(false);
   }

   Set::RToken(DoNegative, false, NULL, pszText);
  } // SetSpecific

  void SetMost
  (
   IN Transition eType,
   IN bool fCritical,
   IN LPCTSTR,
   IN LPCTSTR pszText
  )
  {
   switch(eType)
   {
    // +mrgroup=*
    case DoPositive:
    {
     g_.MRGroups.insert(KJK::S-1-5-12);
     g_.MRGroups.insert(KJK::Sid(_T("IAM")));
     break;
    }

    // -mrgroup=*
    case DoNegative:
    {
     KJK::Sid psidIAM(_T("IAM"));

     for
     (
      GroupSet::iterator p = g_.MRGroups.begin();
      p != g_.MRGroups.end();
      ++ p
     )
      if(*p != psidIAM && *p != KJK::S-1-5-12)
       g_.MRGroups.erase(p);

     break;
    }

    case DoDelete:
    case DoUndelete:
     throw EInvalidArgument;

    default:
     assert(false);
   }

   Set::RToken(DoNegative, false, NULL, pszText);
  } // SetMost()

  void SetAll
  (
   IN Transition eType,
   IN bool fCritical,
   IN LPCTSTR,
   IN LPCTSTR pszText
  )
  {
   switch(eType)
   {
    // -mrgroup=**
    case DoNegative:
     g_.MRGroups.clear();
     break;

    case DoPositive:
    case DoDelete:
    case DoUndelete:
     throw EInvalidArgument;

    default:
     assert(false);
   }

   Set::RToken(DoNegative, false, NULL, pszText);
  } // SetAll()
 } // namespace MRGroup

 /*

  MISCELLANEOUS OPTIONS

 */
 namespace Set
 {
  void NoWait(IN Transition, IN bool, IN LPCTSTR, IN LPCTSTR)
  { g_.fWait = false; }

  void Owner(IN Transition, IN bool, IN LPCTSTR pszArgument, IN LPCTSTR pszText)
  {
   g_.psidOwner.Reset(pszArgument);
   g_.pszOwnerOpt = pszText;
  }

  void OwnerUser(IN Transition, IN bool, IN LPCTSTR, IN LPCTSTR pszText)
  {
   KJK::Handle ahToken;

   KJK::TEST_BOOL_(OpenProcessToken)
   (
    GetCurrentProcess(),
    TOKEN_QUERY,
    &ahToken.Ref()
   );

   DWORD cbRequiredSize = 0;

   GetTokenInformation
   (
    ahToken.Get(),
    TokenUser,
    NULL,
    0,
    &cbRequiredSize
   );

   if(cbRequiredSize == 0)
    KJK::ThrowLastError(TEXT("GetTokenInformation"));

   KJK::Memory<TOKEN_USER> aptu(cbRequiredSize);

   KJK::TEST_BOOL_(GetTokenInformation)
   (
    ahToken.Get(),
    TokenUser,
    aptu.Get(),
    cbRequiredSize,
    &cbRequiredSize
   );

   g_.psidOwner.Reset(reinterpret_cast<SID *>(aptu->User.Sid));
   g_.pszOwnerOpt = pszText;
  }

  void PGroup
  (
   IN Transition,
   IN bool,
   IN LPCTSTR pszArgument,
   IN LPCTSTR pszText
  )
  {
   g_.psidPGroup.Reset(pszArgument);
   g_.pszPGroupOpt = pszText;
  }

  void RToken(IN Transition, IN bool fCritical, IN LPCTSTR, IN LPCTSTR pszText)
  {
   if(!g_.fRToken)
   {
    g_.fRToken = true;
    MRGroup::SetMost(DoPositive, fCritical, NULL, pszText);
   }
  }

  void Typical(IN Transition, IN bool fCritical, IN LPCTSTR, IN LPCTSTR pszText)
  {
   // -rtoken
   RToken(DoNegative, fCritical, NULL, pszText);

   // +rgroup=*
   RGroup::SetMost(DoPositive, fCritical, NULL, pszText);

   // -#priv=*
   Priv::SetMost(DoDelete, fCritical, NULL, pszText);

   // -#group=*
   Group::SetMost(DoDelete, fCritical, NULL, pszText);

   // +owner=*
   OwnerUser(DoPositive, fCritical, NULL, pszText);
  }

  void WinCompat
  (
   IN Transition,
   IN bool fCritical,
   IN LPCTSTR,
   IN LPCTSTR pszText
  )
  {
   // -rtoken
   RToken(DoNegative, fCritical, NULL, pszText);

   // +rgroup=**
   RGroup::SetAll(DoPositive, fCritical, NULL, pszText);

   // -#priv=*
   Priv::SetMost(DoDelete, fCritical, NULL, pszText);

   // +owner=*
   OwnerUser(DoPositive, fCritical, NULL, pszText);

   // -#group=BUILTIN\Administrators
   Group::SetSpecific(DoDelete, fCritical, NULL, pszText, KJK::S-1-5-32-544);

   // -#group=BUILTIN\Power Users
   Group::SetSpecific(DoDelete, fCritical, NULL, pszText, KJK::S-1-5-32-547);
  }
 } // namespace Set

 void AclOperations::SetSpecific
 (
  IN Transition eType,
  IN LPCTSTR pszArgument
 )
 {
  EXPLICIT_ACCESS Operation;
  Opt::ParseAccess(eType, pszArgument, &Operation);
  SetSpecific(eType, Operation);
 }

 void AclOperations::SetAll(IN Transition eType)
 {
  assert(!(fSetEmpty && fSetNull));

  strOperations.clear();

  switch(eType)
  {
   case DoDelete:
   case DoNegative:
   {
    fSetEmpty = true;
    fSetNull = false;
    break;
   }

   case DoPositive:
   case DoUndelete:
   {
    fSetEmpty = false;
    fSetNull = true;
    break;
   }

   default: assert(false);
  }

  assert(!(fSetEmpty && fSetNull));
 }

 /*

  RUN MODES

 */
 namespace Run
 {
  void Help(IN Transition, IN bool, IN LPCTSTR, IN LPCTSTR)
  {
   std::_fputts
   (
    _T
    (
"I Am: sanboxing tool for Windows\n"
"\n"
"Usage: IAM [OPTION]... [--] COMMAND [PARAMETERS]...\n"
"\n"
"Runs COMMAND with PARAMETERS in a sandbox constructed according to the\n"
"OPTIONs\n"
"\n"
"   OPTION           DESCRIPTION                                     CRITICAL\n"
"Privileges:\n"
"  -priv=PRIVILEGE   Disable PRIVILEGE in the token                    no\n"
"  -priv=*           Disable all privileges in the token               --\n"
"  -priv=**          ... including ChangeNotify                        --\n"
"  +priv=PRIVILEGE   Enable PRIVILEGE in the token                     no\n"
"  +priv=*           Enable all privileges in the token                --\n"
" -#priv=PRIVILEGE   Delete PRIVILEGE from the token [1]               no\n"
" -#priv=*           Delete all privileges from the token [1]          --\n"
" -#priv=**          ... including ChangeNotify [1]                    --\n"
" +#priv=PRIVILEGE   Undelete PRIVILEGE from the token [1]             --\n"
" +#priv=*           Undelete all privileges from the token [1]        --\n"
"\n"
"Groups:\n"
"  -group=GROUP      Disable GROUP in the token                        [4]\n"
"  -group=*          Disable all groups in the token                   yes\n"
"  -group=**         ... including Everyone, Users and the logon SID   yes\n"
"                    [2]\n"
"  +group=GROUP      Enable GROUP in the token                         no\n"
"  +group=*          Enable all groups in the token                    no\n"
" -#group=GROUP      Set GROUP in the token as deny-only [3]           no\n"
" -#group=*          Set all groups in the token as deny-only [3]      --\n"
" -#group=**         ... including Everyone, Users and the logon SID   --\n"
"                    [2][3]\n"
" +#group=GROUP      Unset GROUP in the token as deny-only [3]         --\n"
" +#group=*          Unset all groups in the token as deny-only [3]    --\n"
"\n"
"Restricting groups: [3]\n"
"  -mrgroup=GROUP    Unset GROUP as a main restricting group           no\n"
"  -mrgroup=*        Set no group as a main restricting group          --\n"
"  -mrgroup=**       ... including IAM and S-1-5-12                    --\n"
"  +mrgroup=GROUP    Set GROUP as a main restricting group. Implies:   no\n"
"                     - +rgroup=GROUP\n"
"                     - +tokacl=GROUP,F\n"
"                     - +acl=GROUP,F\n"
"                    DO NOT SPECIFY A GROUP WITH ANY MEMBERS\n"
"  +mrgroup=*        Equivalent to +mrgroup=IAM +mrgroup=S-1-5-12      no\n"
"  -rgroup=GROUP     Don't add GROUP to the restricting groups of the  no\n"
"                    token. Doesn't remove restricting groups added\n"
"                    with +mrgroup\n"
"  -rgroup=*         Add no group to the restricting groups of the     --\n"
"                    token\n"
"  -rgroup=**        ... including Everyone, Users, S-1-5-12 and IAM   --\n"
"                    [5]\n"
"  +rgroup=GROUP     Add GROUP to the restricting groups of the token  no\n"
"  +rgroup=*         Add Everyone, Users, S-1-5-12 and IAM to the      no\n"
"                    restricting groups of the token [5][6]\n"
"  +rgroup=**        Add all the existing groups of the token to the   no\n"
"                    restricting groups, except BUILTIN\\Administrators\n"
"                    and BUILTIN\\Power Users, plus IAM and S-1-5-12 [5]\n"
"\n"
"Token ACL:\n"
"  -tokacl=ACCESS    Revoke ACCESS                                     --\n"
"  -tokacl=*         Set the token ACL to an empty ACL                 --\n"
"  +tokacl=ACCESS    Grant ACCESS                                      --\n"
"  +tokacl=*         Set the token ACL to a null ACL. NOT RECOMMENDED  --\n"
" -#tokacl=ACCESS    Deny ACCESS                                       --\n"
" -#tokacl=*         Same as -tokacl=*                                 --\n"
" +#tokacl=ACCESS    Set ACCESS                                        --\n"
" +#tokacl=*         Same as +tokacl=*                                 --\n"
"\n"
"Default security descriptor:\n"
"  -acl=ACCESS       Revoke ACCESS in the default ACL [6]              --\n"
"  -acl=*            Set the default ACL to an empty ACL               --\n"
"  +acl=ACCESS       Grant ACCESS in the default ACL [6]               --\n"
"  +acl=*            Set the default ACL to a NULL ACL                 --\n"
" -#acl=ACCESS       Deny ACCESS [6]                                   --\n"
" -#acl=*            Same as -acl=*                                    --\n"
" +#acl=ACCESS       Set ACCESS in the default ACL [6]                 --\n"
" +#acl=*            Same as +acl=*                                    --\n"
"  +owner=GROUP      Set GROUP as the default owner. GROUP must be a   yes\n"
"                    valid owner group contained in the current\n"
"                    token\n"
"  +owner=*          Set the user id as the default owner              --\n"
"  +pgroup=GROUP     Set GROUP as the default primary group. GROUP     yes\n"
"                    must be any group contained in the current token\n"
"\n"
"Miscellaneous:\n"
"  -nowait           Don't wait for the COMMAND to terminate           --\n"
"  -rtoken           Use a restricting token. Requires system support  --\n"
"                    for restricted tokens (Windows 2000 and later).\n"
"                    Initially implies +mrgroup=*\n"
"  -typical          Select typical options for a sandbox. Equivalent  --\n"
"                    to -rtoken +rgroup=* -#priv=* -#group=* +owner=*\n"
"  -wincompat        Select options compatible with the native Windows --\n"
"                    sandbox. Equivalent to -rtoken +rgroup=**\n"
"                    -#priv=* +owner=* -#group=BUILTIN\\Administrators\n"
"                    \"-#group=BUILTIN\\Power Users\"\n"
"  -help             Show this help and exit                           --\n"
"  -about            Show version and copyright information and exit   --\n"
"\n"
"Notes:\n"
"[1] Requires system support for filtered tokens (Windows 2000 and later),\n"
"    for the SE_PRIVILEGE_REMOVED attribute (Windows Server 2003 and later)\n"
"    (unimplemented), or requires the CreateToken and AssignPrimaryToken\n"
"    privileges (unimplemented)\n"
"[2] Disabling or setting to deny-only the Everyone or Users groups is likely\n"
"    to cause problems and, in general, doesn't achieve anything that\n"
"    restricting groups can't do better. Disabling or setting to deny-only\n"
"    the logon SID will, in a typical environment, cause problems in all\n"
"    programs that link to user32.dll\n"
"[3] Implies -rtoken\n"
"[4] Non-critical if GROUP is not found in the token, critical if GROUP is\n"
"    mandatory and can't be disabled\n"
"[5] A group named IAM is required for this feature to work. The IAM group\n"
"    will be deprecated in favor of its standard equivalent S-1-5-12 in a\n"
"    future version\n"
"[6] I Am normalizes the ACL by mapping any specific rights as if they were\n"
"    file access rights. If any specific right remains unmapped, I Am exits\n"
"    with an error. This shouldn't be a problem in most cases\n"
"\n"
"Remarks:\n"
"I Am doesn't require the user to hold any privilege, since it can only\n"
"further limit the user's rights. I Am doesn't break auditing or accounting\n"
"because the altered token still contains the same user and set of groups,\n"
"altough they don't grant the same access to the same objects as before\n"
"\n"
"Options whose failure causes the token to be less restrictive than it was\n"
"meant to are considered critical, and their failure causes I Am to abort\n"
"without running the command. To make non-critical options critical, use the\n"
"-- or ++ prefix instead of - or +. Failure to look up a privilege or group\n"
"name is always considered critical, regardless of the prefix\n"
"\n"
"Options are executed sequentially: the last option wins over the previous,\n"
"compatible ones. For example, +priv=Shutdown -#priv=** has the effect of\n"
"deleting all the privileges - i.e. the enabling of the Shutdown privilege is\n"
"\"forgotten\"\n"
"\n"
"Restricting groups:\n"
"The system allows duplicate restricting groups in tokens, but I Am won't add\n"
"restricting groups that are already in the current primary token\n"
"\n"
"Group names:\n"
"Any valid account name is a valid group name. This means that user names,\n"
"machine names, built-in user names, etc. are all valid group names to I Am.\n"
"All group names are first parsed as SID strings. This allows non-existing\n"
"(or even invalid) SIDs to be used as restricting groups, further adding to\n"
"the flexibility of the mechanism\n"
"\n"
"I Am currently requires a group named \"IAM\" to be created in any domain\n"
"visible from the machine it runs on. This is the cleanest way for I Am to\n"
"provide an useful default that integrates well with Windows domains, but\n"
"extreme care must be exercised: NO USER SHALL BE A MEMBER OF THE \"IAM\"\n"
"GROUP. The group's sole purpose is to be specified in ACLs as a default\n"
"filter for processes sandboxed by I Am: to add members to it would defeat\n"
"this whole purpose\n"
"\n"
"Privilege names:\n"
"I Am recognizes privileges by their base name, for example Shutdown, Audit,\n"
"etc. or their full name, for example SeShutdownPrivilege, SeAuditPrivilege,\n"
"etc.\n"
"\n"
"Access strings:\n"
"Access strings are in the form GROUP,[ACCESS][,INHERITANCE]], where:\n"
" - GROUP is a group name\n"
" - ACCESS can be any combination of:\n"
"    - R: GENERIC_READ\n"
"    - W: GENERIC_WRITE\n"
"    - X: GENERIC_EXECUTE\n"
"    - C: R + W\n"
"    - F: GENERIC_ALL\n"
"   If none is specified, \"F\" is implied\n"
" - INHERITANCE is a string in the form [O][C][I][1], where the letters mean:\n"
"    - O: child objects inherit this access\n"
"    - C: child containers inherit this access\n"
"    - I: inherited access only\n"
"    - 1: inheritance is limited to the first level of children\n"
    ),
    stdout
   );

   g_.fDone = true;
  } // Help()

  void About(IN Transition, IN bool, IN LPCTSTR, IN LPCTSTR)
  {
   std::_fputts
   (
    _T("I Am v1.0 -- copyright (c) 2004 by KJK::Hyperion\n"),
    stdout
   );

   g_.fDone = true;
  } // About()

  static BOOL WINAPI IgnoreHandler(IN DWORD dwCtrlType)
  {
   switch(dwCtrlType)
   {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
     return TRUE;

    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
    default:
     return FALSE;
   }
  }

  bool operator==(const LUID& x, const LUID& y)
  { return x.LowPart == y.LowPart && x.HighPart == y.HighPart; }

  void PerformTokenAdjustments(IN HANDLE hToken)
  {
   DWORD cbRequiredSize = 0;

   // STEP 1: Gather the token's groups and privileges
   // get the required buffer size
   GetTokenInformation
   (
    hToken,
    TokenGroupsAndPrivileges,
    NULL,
    0,
    &cbRequiredSize
   );

   if(cbRequiredSize == 0)
    KJK::ThrowLastError(TEXT("GetTokenInformation"));

   KJK::Memory<TOKEN_GROUPS_AND_PRIVILEGES> apTokenInfo(cbRequiredSize);

   // get the information
   KJK::TEST_BOOL_(GetTokenInformation)
   (
    hToken,
    TokenGroupsAndPrivileges,
    apTokenInfo.Get(),
    cbRequiredSize,
    &cbRequiredSize
   );

   // STEP 2: Perform group operations
   {
    std::size_t ccGroupCount = 0;
    DWORD cbBufferLength =
     offsetof(TOKEN_GROUPS, Groups) +
     sizeof(SID_AND_ATTRIBUTES) * apTokenInfo->SidCount;

    KJK::Memory<TOKEN_GROUPS> apNewState(cbBufferLength);

    for(DWORD i = 0; i < apTokenInfo->SidCount; ++ i)
    {
     Group::State state =
      g_.GroupsStates[Group::GetRegion(apTokenInfo->Sids[i].Sid)].
       ConsumeState(KJK::Sid(apTokenInfo->Sids[i].Sid));

     IAM_WITHCONTEXT_(state.pszText)
     {
      switch(state.eState)
      {
       // disable
       case Negative:
       {
        // can't disable mandatory groups
        if(apTokenInfo->Sids[i].Attributes & SE_GROUP_MANDATORY)
        {
         assert(apTokenInfo->Sids[i].Attributes & SE_GROUP_ENABLED);
         throw ECantDisableGroup;
        }
        // only disable enabled groups
        else if(apTokenInfo->Sids[i].Attributes & SE_GROUP_ENABLED)
        {
         apNewState->Groups[ccGroupCount].Sid = apTokenInfo->Sids[i].Sid;
         apNewState->Groups[ccGroupCount].Attributes =
          apTokenInfo->Sids[i].Attributes & ~SE_GROUP_ENABLED;

         ++ ccGroupCount;
        }
        else
        {
         // TODO? warn about already disabled groups?
        }

        break;
       } // case Negative:

       // enable
       case Positive:
       {
        // can't enable deny-only groups
        if(apTokenInfo->Sids[i].Attributes & SE_GROUP_USE_FOR_DENY_ONLY)
        {
         assert(!(apTokenInfo->Sids[i].Attributes & SE_GROUP_ENABLED));
         throw(ECantEnableGroup);
        }
        // only enable disabled groups
        else if(!(apTokenInfo->Sids[i].Attributes & SE_GROUP_ENABLED))
        {
         apNewState->Groups[ccGroupCount].Sid = apTokenInfo->Sids[i].Sid;
         apNewState->Groups[ccGroupCount].Attributes =
          apTokenInfo->Sids[i].Attributes | SE_GROUP_ENABLED;

         ++ ccGroupCount;
        }
        else
        {
         // TODO? warn about already enabled groups?
        }

        break;
       } // case Positive:

       case Neutral: break;
       case Delete: break; // assert(false);
       default: assert(false); break;
      } // switch(state.eState)
     }
     IAM_ENDCONTEXT_
    } // for(DWORD i = 0; i < apTokenInfo->SidCount; ++ i)

    // TODO: check for any critical specific states still left

    if(ccGroupCount)
    {
     assert(ccGroupCount <= apTokenInfo->SidCount);
     apNewState->GroupCount = ccGroupCount;

     KJK::TEST_BOOL_(AdjustTokenGroups)
     (
      hToken,
      FALSE,
      apNewState.Get(),
      cbBufferLength,
      NULL,
      0
     );
    }
   } // STEP 2

   // STEP 3: perform privilege operations
   {
    std::size_t ccPrivilegeCount = 0;
    DWORD cbBufferLength =
     offsetof(TOKEN_PRIVILEGES, Privileges) +
     sizeof(LUID_AND_ATTRIBUTES) * apTokenInfo->PrivilegeCount;

    KJK::Memory<TOKEN_PRIVILEGES> apNewState(cbBufferLength);

    for(DWORD i = 0; i < apTokenInfo->PrivilegeCount; ++ i)
    {
     Priv::State state =
      g_.PrivStates[Priv::GetRegion(apTokenInfo->Privileges[i].Luid)].
       ConsumeState(apTokenInfo->Privileges[i].Luid);

#ifndef SE_PRIVILEGE_REMOVED
#define SE_PRIVILEGE_REMOVED (0X00000004L)
#endif

     IAM_WITHCONTEXT_(state.pszText)
     {
      switch(state.eState)
      {
       // disable
       case Negative:
       {
        if(apTokenInfo->Privileges[i].Attributes & SE_PRIVILEGE_REMOVED)
        {
         // TODO: act like the privilege wasn't found
        }
        // only disable enabled privileges
        else if(apTokenInfo->Privileges[i].Attributes & SE_PRIVILEGE_ENABLED)
        {
         apNewState->Privileges[ccPrivilegeCount].Luid =
          apTokenInfo->Privileges[i].Luid;

         apNewState->Privileges[ccPrivilegeCount].Attributes =
          apTokenInfo->Privileges[i].Attributes & ~SE_PRIVILEGE_ENABLED;

         ++ ccPrivilegeCount;
        }
        else
        {
         // TODO? warn about already disabled groups?
        }

        break;
       } // case Negative:

       // enable
       case Positive:
       {
        if(apTokenInfo->Privileges[i].Attributes & SE_PRIVILEGE_REMOVED)
        {
         // TODO: act like the privilege wasn't found
        }
        // only enable disabled privileges
        else if(!(apTokenInfo->Privileges[i].Attributes & SE_PRIVILEGE_ENABLED))
        {
         apNewState->Privileges[ccPrivilegeCount].Luid =
          apTokenInfo->Privileges[i].Luid;

         apNewState->Privileges[ccPrivilegeCount].Attributes =
          apTokenInfo->Privileges[i].Attributes | SE_PRIVILEGE_ENABLED;

         ++ ccPrivilegeCount;
        }
        else
        {
         // TODO? warn about already disabled groups?
        }

        break;
       } // case Positive:

       case Neutral: break;
       // TODO: use SE_PRIVILEGE_REMOVED, if available
       case Delete: break;
       // FIXME: implement critical options
       // case Delete: assert(false); break;
       default: assert(false); break;
      } // switch(state.eState)
     }
     IAM_ENDCONTEXT_
    } // for(DWORD i = 0; i < apTokenInfo->PrivilegeCount; ++ i)

    // TODO: check for any critical specific states still left

    if(ccPrivilegeCount)
    {
     assert(ccPrivilegeCount <= apTokenInfo->PrivilegeCount);
     apNewState->PrivilegeCount = ccPrivilegeCount;

     KJK::TEST_BOOL_(AdjustTokenPrivileges)
     (
      hToken,
      FALSE,
      apNewState.Get(),
      cbBufferLength,
      NULL,
      0
     );
    }
   } // STEP 3
  } // PerformTokenAdjustments

  void PerformTokenOperations(IN HANDLE hToken)
  {
   DWORD dwRevision;
   SECURITY_DESCRIPTOR_CONTROL sdc;

   // STEP 1: Set the default security descriptor
   // 1.1 Default owner
   if(g_.psidOwner.Get())
    IAM_WITHCONTEXT_(g_.pszOwnerOpt)
    {
     TOKEN_OWNER to =
      { const_cast<PSID>(static_cast<const VOID *>(g_.psidOwner.Get())) };

     KJK::TEST_BOOL_(SetTokenInformation)(hToken, TokenOwner, &to, sizeof(to));
    }
    IAM_ENDCONTEXT_;

   // 1.2 Default primary group
   if(g_.psidPGroup.Get())
    IAM_WITHCONTEXT_(g_.pszPGroupOpt)
    {
     TOKEN_PRIMARY_GROUP tpg =
      { const_cast<PSID>(static_cast<const VOID *>(g_.psidPGroup.Get())) };

     KJK::TEST_BOOL_(SetTokenInformation)
     (
      hToken,
      TokenOwner,
      &tpg,
      sizeof(tpg)
     );
    }
    IAM_ENDCONTEXT_;

   // 1.3 Default ACL
   if(g_.paclDefault)
   {
    TOKEN_DEFAULT_DACL tdd = { g_.paclDefault };

    KJK::TEST_BOOL_(SetTokenInformation)
    (
     hToken,
     TokenDefaultDacl,
     &tdd,
     sizeof(tdd)
    );
   }

   // STEP 2: Set the token's ACL
   if(g_.TokAcl.WorkToDo())
   {
    PACL pNewAcl;
    KJK::LocalMemory<ACL> apNewAcl;
    bool fHasACL;

    // see if we need an ACL to edit
    if(!g_.TokAcl.GetACL(&pNewAcl, &fHasACL))
    {
     DWORD cbRequiredSize = 0;

     // get the required buffer size
     GetKernelObjectSecurity
     (
      hToken,
      DACL_SECURITY_INFORMATION,
      NULL,
      0,
      &cbRequiredSize
     );

     if(cbRequiredSize == 0)
      KJK::ThrowLastError(TEXT("GetKernelObjectSecurity"));

     // allocate the buffer
     KJK::Memory<SECURITY_DESCRIPTOR> apsdTokenOriginal(cbRequiredSize);

     // get the existing ACL
     KJK::TEST_BOOL_(GetKernelObjectSecurity)
     (
      hToken,
      DACL_SECURITY_INFORMATION,
      apsdTokenOriginal.Get(),
      cbRequiredSize,
      &cbRequiredSize
     );

     BOOL bDaclPresent;
     BOOL bDaclDefaulted;
     PACL pOldAcl;

     GetSecurityDescriptorDacl
     (
      apsdTokenOriginal.Get(),
      &bDaclPresent,
      &pOldAcl,
      &bDaclDefaulted
     );

     assert(bDaclPresent);

     // map the access masks into intuitive generic access rights
     g_.c_igmToken.MapToIntuitive(pOldAcl);

     // edit the ACL
     g_.TokAcl.GetACL(&pNewAcl, pOldAcl);

     // map the access masks back into real access rights
     g_.c_igmToken.MapFromIntuitive(pNewAcl);

     fHasACL = true;
    }

    assert(fHasACL);

    // assign the ACL to the token
    apNewAcl.Reset(pNewAcl);

    SECURITY_DESCRIPTOR sdToken;

    KJK::TEST_BOOL_(InitializeSecurityDescriptor)
    (
     &sdToken,
     SECURITY_DESCRIPTOR_REVISION
    );

    KJK::TEST_BOOL_(SetSecurityDescriptorDacl)(&sdToken, TRUE, pNewAcl, FALSE);

    KJK::TEST_BOOL_(SetKernelObjectSecurity)
    (
     hToken,
     DACL_SECURITY_INFORMATION,
     &sdToken
    );
   } // STEP 2
  }

  void Command(IN int argc, IN const _TCHAR * const argv[])
  {
   KJK::Handle ahToken;

//   const bool fRestrictedTokenReallyNeeded = ;

//   const bool fRestrictedTokenNeeded =
//    fRestrictedTokenReallyNeeded ||
//    ;

   // STEP 1: if we need a restricted token:
   if(g_.fRPriv || g_.fRToken)
   {
    BOOL (WINAPI * CreateRestrictedToken)
    (
     IN HANDLE ExistingTokenHandle,
     IN DWORD Flags,
     IN DWORD DisableSidCount,
     IN PSID_AND_ATTRIBUTES SidsToDisable,
     IN DWORD DeletePrivilegeCount,
     IN PLUID_AND_ATTRIBUTES PrivilegesToDelete,
     IN DWORD RestrictedSidCount,
     IN PSID_AND_ATTRIBUTES SidsToRestrict,
     OUT PHANDLE NewTokenHandle
    );

    // 1.1 lookup advapi32!CreateRestrictedToken
    KJK::DllRef advapi32(TEXT("advapi32"));

    CreateRestrictedToken =
     advapi32.ProcAddress("CreateRestrictedToken", CreateRestrictedToken);

    // if it was found:
    if(CreateRestrictedToken)
    {
     IN HANDLE hExistingTokenHandle;
     IN DWORD dwFlags = 0;
     IN DWORD ccDisableSidCount = 0;
     IN PSID_AND_ATTRIBUTES pSidsToDisable = NULL;
     IN DWORD ccDeletePrivilegeCount = 0;
     IN PLUID_AND_ATTRIBUTES pPrivilegesToDelete = NULL;
     IN DWORD ccRestrictedSidCount = 0;
     IN PSID_AND_ATTRIBUTES pSidsToRestrict = NULL;

     // 1.2 open the current primary token:
     KJK::TEST_BOOL_(OpenProcessToken)
     (
      GetCurrentProcess(),
      TOKEN_ALL_ACCESS,
      &hExistingTokenHandle
     );

     // keep the handle alive until the end of the scope
     KJK::Handle ahExistingTokenHandle(hExistingTokenHandle);

     // 1.2.1 gather its groups, privileges and restricting groups
     DWORD cbRequiredSize = 0;

     // get the required buffer size
     GetTokenInformation
     (
      hExistingTokenHandle,
      TokenGroupsAndPrivileges,
      NULL,
      0,
      &cbRequiredSize
     );

     if(cbRequiredSize == 0)
      KJK::ThrowLastError(TEXT("GetTokenInformation"));

     // allocate the buffer
     KJK::Memory<TOKEN_GROUPS_AND_PRIVILEGES> apTokenInfo(cbRequiredSize);

     // get the information
     KJK::TEST_BOOL_(GetTokenInformation)
     (
      hExistingTokenHandle,
      TokenGroupsAndPrivileges,
      apTokenInfo.Get(),
      cbRequiredSize,
      &cbRequiredSize
     );

     // 1.2.2 build a list of privileges to delete
     std::basic_string<LUID_AND_ATTRIBUTES> strPrivilegesToDelete;

     if
     (
      g_.PrivStates[Priv::Most].SpecificStateCount() == 0 &&
      g_.PrivStates[Priv::Most].State() == Delete &&
      g_.PrivStates[Priv::Others].SpecificStateCount() == 0 &&
      g_.PrivStates[Priv::Others].State() != Delete
     )
      dwFlags |= DISABLE_MAX_PRIVILEGE;
     else
     {
      for(DWORD i = 0; i < apTokenInfo->PrivilegeCount; ++ i)
      {
       // see if this privilege should be deleted
       bool fDelete =
        g_.PrivStates[Priv::GetRegion(apTokenInfo->Privileges[i].Luid)].
         CompareAndConsumeState
         (
          apTokenInfo->Privileges[i].Luid,
          Delete
         );

       // it should
       if(fDelete)
       {
        LUID_AND_ATTRIBUTES Element;

        Element.Luid = apTokenInfo->Privileges[i].Luid;
        Element.Attributes = 0;

        strPrivilegesToDelete += Element;
       }
      }

      // done
      ccDeletePrivilegeCount = strPrivilegesToDelete.size();
      pPrivilegesToDelete =
       const_cast<PLUID_AND_ATTRIBUTES>(strPrivilegesToDelete.c_str());

      // TODO: check for any critical specific states still left
     }

     // 1.2.3 build a list of groups to set to deny-only
     std::basic_string<SID_AND_ATTRIBUTES> strSidsToDisable;

     assert(apTokenInfo->SidCount >= 1);

     for(DWORD i = 1; i < apTokenInfo->SidCount; ++ i)
     {
      // see if this group should be disabled
      bool fDelete =
       g_.GroupsStates[Group::GetRegion(apTokenInfo->Sids[i].Sid)].
        CompareAndConsumeState
        (
         KJK::Sid(apTokenInfo->Sids[i].Sid),
         Delete
        );

      // it should
      if(fDelete)
      {
       SID_AND_ATTRIBUTES Element;

       Element.Sid = apTokenInfo->Sids[i].Sid;
       Element.Attributes = 0;

       strSidsToDisable += Element;
      }
     }

     // done
     ccDisableSidCount = strSidsToDisable.size();
     pSidsToDisable = const_cast<PSID_AND_ATTRIBUTES>(strSidsToDisable.c_str());

     // 1.2.4 build a list of restricting groups to add
     std::basic_string<SID_AND_ATTRIBUTES> strSidsToRestrict;

     {
      using KJK::S;

      std::set<KJK::Sid> setRGroups;

      // add all the current groups, minus Administrators and Power Users, plus
      // IAM and S-1-5-12 (+rgroup=**)
      if(g_.RGroupStates[RGroup::Most].State() == Positive)
      {
       assert(apTokenInfo->SidCount >= 1);

       for(DWORD i = 1; i < apTokenInfo->SidCount; ++ i)
       {
        KJK::Sid psidCur = apTokenInfo->Sids[i].Sid;

        if
        (
         // BUILTIN\Administrators
         psidCur == S-1-5-32-544 ||
         // BUILTIN\Power Users
         psidCur == S-1-5-32-547
        )
         continue;

        RGroup::RegionStates& States =
         g_.RGroupStates[RGroup::GetRegion(psidCur)];

        // group explicitely excluded by the user
        if(States.State() != Neutral && States.State(psidCur) == Neutral)
         continue;

        setRGroups.insert(psidCur);
       }

       setRGroups.insert(_T("IAM"));
       setRGroups.insert(S-1-5-12);
      }

      // add the predefined groups: Everyone, BUILTIN\Users, S-1-5-12 and IAM
      // (+rgroup=*)
      if(g_.fRToken)
      {
       static const KJK::Sid c_a_psidPredef[] =
       {
        S-1-1-0,
        S-1-5-32-545,
        S-1-5-12,
        _T("IAM")
       };

       static const std::size_t c_cPredefSids =
        sizeof(c_a_psidPredef) / sizeof(c_a_psidPredef[0]);

       for(std::size_t i = 0; i < c_cPredefSids; ++ i)
       {
        assert(RGroup::GetRegion(c_a_psidPredef[i]) == RGroup::Others);

        // insert the group unless the user explicitely excluded it
        if(g_.RGroupStates[RGroup::Others].State(c_a_psidPredef[i]) == Positive)
         setRGroups.insert(c_a_psidPredef[i]);
       }
      }

      // add all the groups specified explicitely
      for(unsigned i = 0; i < RGroup::RegionCount; ++ i)
      {
       RGroup::RegionStates& States = g_.RGroupStates[i];

       if(States.State() == Neutral && States.SpecificStateCount())
       {
        RGroup::RegionStates::Iterator p = States.Begin();

        for(; p != States.End(); p = States.Next(p))
        {
         assert(p->second == Positive);
         setRGroups.insert(p->first.Get());
        }
       }
      }

      // add the main restricting groups
      for
      (
       GroupSet::iterator p = g_.MRGroups.begin();
       p != g_.MRGroups.end();
       ++ p
      )
      {
       setRGroups.insert(p->Get());

       EXPLICIT_ACCESS ea;

       ea.grfAccessPermissions = IntuitiveGenericAll;
       ea.grfAccessMode = GRANT_ACCESS;
       ea.grfInheritance = 0;
       ea.Trustee.pMultipleTrustee = NULL;
       ea.Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
       ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
       ea.Trustee.TrusteeType = TRUSTEE_IS_UNKNOWN;
       ea.Trustee.ptstrName =
        const_cast<LPTSTR>(reinterpret_cast<LPCTSTR>(p->Get()));

       g_.TokAcl.SetSpecific(DoPositive, ea);
       g_.DefAcl.SetSpecific(DoPositive, ea);
      }

      // remove the restricting groups already present in the current token, to
      // save kernel memory
      for(DWORD i = 0; i < apTokenInfo->RestrictedSidCount; ++ i)
       setRGroups.erase(apTokenInfo->RestrictedSids[i].Sid);

      // convert the set of restricting groups into an array
      for
      (
       std::set<KJK::Sid>::const_iterator p = setRGroups.begin();
       p != setRGroups.end();
       ++ p
      )
      {
       GroupSet::iterator x = g_.SidPool.insert(*p).first;

       SID_AND_ATTRIBUTES Element;

       // BLEAH: this program is very inefficient, ugly and based on contorted
       // logic. This line is the last drop. Please mr. author, make it better
       Element.Sid = (PSID)(x->Get());
       Element.Attributes = 0;

       strSidsToRestrict += Element;
      }
     }

     // done
     ccRestrictedSidCount = strSidsToRestrict.size();

     pSidsToRestrict =
      const_cast<PSID_AND_ATTRIBUTES>(strSidsToRestrict.data());

     // 1.3 create the restricted token
     KJK::TEST_BOOL_(CreateRestrictedToken)
     (
      hExistingTokenHandle,
      dwFlags,
      ccDisableSidCount,
      pSidsToDisable,
      ccDeletePrivilegeCount,
      pPrivilegesToDelete,
      ccRestrictedSidCount,
      pSidsToRestrict,
      &ahToken.Ref()
     );
    }
    else
    {
    }

#if 0
    // else, if a restricted token is really needed:
    else if(fRestrictedTokenReallyNeeded)
    {
     // TODO
     // failure
    }
    // else:
    else
    {
     // TODO
     // 1.2 lookup ntdll!ZwCreateToken
     // 1.3 enable the CreateToken and AssignPrimaryToken privileges
     // 1.4 open the current primary token and retrieve all of its components
     // 1.5 delete the privileges
     // 1.6 create the token
    }
#endif
   }

   LPTSTR lpCommandLine;

   // STEP 2: default security descriptor
   // 2.1 Owner
   if(g_.psidOwner.Get())
    KJK::TEST_BOOL_(SetSecurityDescriptorOwner)
    (
     &g_.sdDefault,
     g_.psidOwner.Get(),
     TRUE
    );

   // 2.2 Primary group
   if(g_.psidPGroup.Get())
    KJK::TEST_BOOL_(SetSecurityDescriptorGroup)
    (
     &g_.sdDefault,
     g_.psidPGroup.Get(),
     TRUE
    );

   // 2.3 Default ACL
   if(g_.DefAcl.WorkToDo())
   {
    KJK::Handle ahCurToken;

    KJK::TEST_BOOL_(OpenProcessToken)
    (
     GetCurrentProcess(),
     TOKEN_ALL_ACCESS,
     &ahCurToken.Ref()
    );

    bool fHasDefaultACL;

    if(g_.DefAcl.GetACL(&g_.paclDefault, &fHasDefaultACL))
     g_.apDefaultACLMem.Reset(g_.paclDefault);
    else
    {
     fHasDefaultACL = true;

     DWORD cbRequiredSize;

     GetTokenInformation
     (
      ahCurToken.Get(),
      TokenDefaultDacl,
      NULL,
      0,
      &cbRequiredSize
     );

     if(cbRequiredSize == 0)
      KJK::ThrowLastError(TEXT("GetTokenInformation"));

     KJK::LocalMemory<TOKEN_DEFAULT_DACL> aptdd(cbRequiredSize);

     KJK::TEST_BOOL_(GetTokenInformation)
     (
      ahCurToken.Get(),
      TokenDefaultDacl,
      aptdd.Get(),
      cbRequiredSize,
      &cbRequiredSize
     );

     // map the access masks into intuitive generic access rights
     g_.c_igmFile.MapToIntuitive(aptdd->DefaultDacl);

     // edit the ACL
     g_.DefAcl.GetACL(&g_.paclDefault, aptdd->DefaultDacl);

     // map the access masks back into real generic access rights
     g_.c_igmFile.MapGenericFromIntuitive(g_.paclDefault);

     g_.apDefaultACLMem.Reset(aptdd.Release());
    }

    assert(fHasDefaultACL);

    KJK::TEST_BOOL_(SetSecurityDescriptorDacl)
    (
     &g_.sdDefault,
     TRUE,
     g_.paclDefault,
     FALSE
    );
   }

   // STEP 3: build the arguments for CreateProcess
   // 3.1 Command line
   assert(argc >= 1);

   std::basic_string<TCHAR> strCommandLine;

   for(int i = 0; i < argc; ++ i)
   {
    // append a space to the previous argument
    if(i > 0)
     strCommandLine += _T(' ');

    // scan the argument for whitespace and double quotes
    bool fNeedQuotes = false;
    bool fNeedEscape = false;
    const _TCHAR * p = argv[i];
    const _TCHAR * p2;
    unsigned int c = _tcsnextc(p);

    // NOTE: we use _tcsinc and _tcsnextc to support multi-byte encodings
    for(; c != 0; p2 = p, p = _tcsinc(p))
    {
     if(!fNeedQuotes && _istspace(c))
      fNeedQuotes = true;

     // the argument contains at least one quote: it needs to be quoted and
     // escaped. No sense in scanning the rest of the string: we already know
     // the argument needs to be quoted, and we don't need to know the length of
     // the argument anymore
     if(c == _T('"'))
     {
      assert(!fNeedEscape);
      fNeedEscape = true;

      p2 = argv[i];

      strCommandLine += _T('"');

      do
      {
       std::size_t n = p - p2;
       assert(n >= 1);
       -- n;

       strCommandLine.append(p2, n);
       // FIXME: check for trailing backslashes
       strCommandLine += _T('\\');
       strCommandLine += _T('"');

       p2 = p;
       p = _tcschr(p, _T('"')) + 1;
      }
      while(p != ((_TCHAR *)NULL + 1));

      strCommandLine.append(p2);

      // FIXME: check for trailing backslashes
      strCommandLine += _T('"');

      break;
     }

     c = _tcsnextc(p);
    } // for(; c != 0; p2 = p, p = _tcsinc(p))

    if(!fNeedEscape)
    {
     std::size_t cbArgLen;

     assert(p2 >= argv[i]);
     cbArgLen = (p2 - argv[i]) * sizeof(_TCHAR);

     if(cbArgLen == 0)
     {
      assert(!fNeedQuotes);
      assert(!fNeedEscape);
      fNeedQuotes = true;
     }

     assert((cbArgLen % sizeof(_TCHAR)) == 0);

     if(fNeedQuotes)
      strCommandLine += _T('"');

     strCommandLine.append(argv[i], cbArgLen / sizeof(_TCHAR));

     if(fNeedQuotes)
      strCommandLine += _T('"');
    } // if(!fNeedEscape)
   } // for(int i = 0; i < argc; ++ i)

   lpCommandLine = const_cast<LPTSTR>(strCommandLine.data());

   // 3.2 Process and thread security
   SECURITY_ATTRIBUTES ProcessAttributes;
   SECURITY_ATTRIBUTES ThreadAttributes;

   ProcessAttributes.nLength = sizeof(ProcessAttributes);
   ProcessAttributes.lpSecurityDescriptor = &g_.sdDefault;
   ProcessAttributes.bInheritHandle = FALSE;

   ThreadAttributes.nLength = sizeof(ThreadAttributes);
   ThreadAttributes.lpSecurityDescriptor = &g_.sdDefault;
   ThreadAttributes.bInheritHandle = FALSE;

   KJK::Process::Killer kKiller;
   KJK::Process psCommand;
   KJK::Thread thrCommandMain;

   {
    // 3.3 Startup information
    // automatically inherits the current process's startup information
    KJK::Process::StartupInfo StartupInfo;

    // STEP 4: Perform token adjustments
    if(ahToken.Get())
     PerformTokenAdjustments(ahToken.Get());

    // STEP 4: create the process
    thrCommandMain = psCommand.Create
    (
     NULL,
     lpCommandLine,
     &ProcessAttributes,
     &ThreadAttributes,
     TRUE,
     CREATE_SEPARATE_WOW_VDM | CREATE_SUSPENDED,
     NULL,
     NULL,
     &StartupInfo,
     ahToken.Get()
    );
   }

   kKiller.Attach(psCommand);

   // STEP 5: Perform actions on the resulting token
   bool fAdjDone = ahToken.Get();

   ahToken.Reset(psCommand.OpenToken(TOKEN_ALL_ACCESS));

   if(!fAdjDone)
    PerformTokenAdjustments(ahToken.Get());

   PerformTokenOperations(ahToken.Get());

   // STEP 6: resume the process
   thrCommandMain.Resume();
   kKiller.Detach();

   // -nowait
   if(!g_.fWait)
    // we don't really care about cleaning up
    TerminateProcess(GetCurrentProcess(), NO_ERROR);

   thrCommandMain.Reset();

   // STEP 7: free all the resources we can free

   // Dear Santa, for Christmas I wish a Windows implementation of exec()

   // only return when the command returns - ignore console interrupts
   SetConsoleCtrlHandler(Run::IgnoreHandler, TRUE);

   // destroy the globals
   operator delete(&g_, &g_);

   // destroy the malloc heap
   // POINT OF NO RETURN -- TerminateProcess the only safe way to exit
   DestroyHeap();

   // swap out the process
   SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T)-1, (SIZE_T)-1);

   // even exceptions are unsafe: let's use good old Win32 API
   HANDLE hProcess = psCommand.Release();

   // STEP 8: wait for the process to terminate
   switch(WaitForSingleObject(hProcess, INFINITE))
   {
    case WAIT_OBJECT_0:
     break;

    case WAIT_FAILED:
     TerminateProcess(GetCurrentProcess(), GetLastError());

    case WAIT_TIMEOUT:
    case WAIT_ABANDONED:
    default:
     TerminateProcess(GetCurrentProcess(), E_FAIL);
   }

   // STEP 9: return the exit code of the process
   DWORD dwExitCode;

   if(GetExitCodeProcess(hProcess, &dwExitCode))
   {
    if(dwExitCode == STILL_ACTIVE)
     dwExitCode = NO_ERROR;
   }
   else
    dwExitCode = GetLastError();

   TerminateProcess(GetCurrentProcess(), dwExitCode);
   assert(false);
  } // Command()

  static BOOL WINAPI ConsoleHandler(IN DWORD dwCtrlType)
  {
   switch(dwCtrlType)
   {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
     std::exit(0);

    case CTRL_SHUTDOWN_EVENT:
    default:
     return FALSE;
   }
  }
 } // namespace Run

 /*

  MAIN FUNCTION

 */
 extern "C" int _tmain(int argc, const _TCHAR * const argv[])
 {
  try
  {
   SetConsoleCtrlHandler(Run::ConsoleHandler, TRUE);

   int i;

   for(i = 1; i < argc; ++ i)
    IAM_WITHCONTEXT_(argv[i])
    {
     const _TCHAR * pszArgTail = argv[i];
     const Opt::Prefix eOptPrefix = Opt::ParsePrefix(&pszArgTail);

     // the argument has no prefix - it's the command
     if(eOptPrefix == Opt::NoPrefix)
      break;
     // the argument is "--" - the next argument is the command
     else if(eOptPrefix == Opt::MinusMinus && *pszArgTail == 0)
     {
      ++ i;
      break;
     }

     const Opt::Modifier eOptModifier = Opt::ParseModifier(&pszArgTail);
     const Opt::Type eOptType = Opt::ParseType(&pszArgTail);
     const Opt::ArgType eOptArgType = Opt::ParseArgType(&pszArgTail);

     static const Opt::Handler s_a_a_a_pfnHandlers
      [Opt::TypeCount][Opt::ModifierCount][Opt::ArgTypeCount] =
     {
      // unknown
      { { NULL, NULL, NULL }, { NULL, NULL, NULL } },
      // help
      { { Run::Help, NULL, NULL }, { NULL, NULL, NULL } },
      // about
      { { Run::About, NULL, NULL }, { NULL, NULL, NULL } },
      // nowait
      { { Set::NoWait, NULL, NULL }, { NULL, NULL, NULL } },
      // priv
      {
       // -priv
       { Priv::SetSpecific, Priv::SetMost, Priv::SetAll },
       // -#priv
       { Priv::SetSpecific, Priv::SetMost, Priv::SetAll }
      },
      // group
      {
       // -group
       { Group::SetSpecific, Group::SetMost, Group::SetAll },
       // -#group
       { Group::SetSpecific, Group::SetMost, Group::SetAll }
      },
      // rtoken
      { { Set::RToken, NULL, NULL }, { NULL, NULL, NULL } },
      // mrgroup
      {
       // -mrgroup
       { MRGroup::SetSpecific, MRGroup::SetMost, MRGroup::SetAll },
       // -#mrgroup
       { NULL, NULL, NULL }
      },
      // rgroup
      {
       // -rgroup
       { RGroup::SetSpecific, RGroup::SetMost, RGroup::SetAll },
       // -#rgroup
       { NULL, NULL, NULL }
      },
      // tokacl
      {
       // -tokacl
       { TokACL::SetSpecific, TokACL::SetAll, NULL },
       // -#tokacl
       { TokACL::SetSpecific, TokACL::SetAll, NULL }
      },
      // acl
      {
       // -acl
       { DefACL::SetSpecific, DefACL::SetAll, NULL },
       // -#acl
       { DefACL::SetSpecific, DefACL::SetAll, NULL }
      },
      // owner
      { { Set::Owner, Set::OwnerUser, NULL }, { NULL, NULL, NULL } },
      // pgroup
      { { Set::PGroup, NULL, NULL }, { NULL, NULL, NULL } },
      // typical
      { { Set::Typical, NULL, NULL }, { NULL, NULL, NULL } },
      // wincompat
      { { Set::WinCompat, NULL, NULL }, { NULL, NULL, NULL } }
     };

     Opt::Handler pfnOptHandler =
      s_a_a_a_pfnHandlers[eOptType][eOptModifier][eOptArgType];

     if(pfnOptHandler == NULL)
      throw EUnknownOption;

     const Transition eType =
      (eOptPrefix == Opt::Plus || eOptPrefix == Opt::PlusPlus) ?
       (eOptModifier == Opt::Hash ? DoUndelete : DoPositive) :
       (eOptModifier == Opt::Hash ? DoDelete : DoNegative);

     const bool fCritical =
      eOptPrefix == Opt::PlusPlus || eOptPrefix == Opt::MinusMinus;

     // call the handler
     pfnOptHandler(eType, fCritical, pszArgTail, argv[i]);

     // premature success
     if(g_.fDone) return EXIT_SUCCESS;
    }
    IAM_ENDCONTEXT_ // for(i = 1; i < argc; ++ i) IAM_WITHCONTEXT_(argv[i])

   int nCommandArgC;
   const _TCHAR * const * a_pszCommandArgs;

   // run a command
   if(i < argc && argv[i] != NULL)
   {
    nCommandArgC = argc - i;
    a_pszCommandArgs = argv + i;
   }
   else
    throw ENoCommand;

   // run
   Run::Command(nCommandArgC, a_pszCommandArgs);

   return EXIT_SUCCESS;
  } // try
  // System or application errors
  catch(KJK::Error& e)
  {
   if(g_.pszContext)
    std::_ftprintf
    (
     stderr,
     _T("iam: %s: %s: %s\n"),
     g_.pszContext,
     _T("error"),
     e.Message()
    );
   else
    std::_ftprintf
    (
     stderr,
     _T("iam: %s: %s\n"),
     _T("error"),
     e.Message()
    );

   return EXIT_FAILURE;
  }
  // C++ runtime errors
  catch(std::exception& e)
  {
   if(g_.pszContext)
    std::_ftprintf
    (
     stderr,
     _T("iam: %s: %s: %s\n"),
     g_.pszContext,
     _T("runtime error"),
     e.what()
    );
   else
    std::_ftprintf
    (
     stderr,
     _T("iam: %s: %s\n"),
     _T("runtime error"),
     e.what()
    );

   return EXIT_FAILURE;
  }
  // Unknown exceptions
  catch(...)
  {
   if(g_.pszContext)
    std::_ftprintf
    (
     stderr,
     _T("iam: %s: %s\n"),
     g_.pszContext,
     _T("unknown error")
    );
   else
    std::_ftprintf(stderr, _T("iam: %s: %s\n"), _T("unknown error"));

   return EXIT_FAILURE;
  }
 } // _tmain()
} // namespace IAM

// EOF
