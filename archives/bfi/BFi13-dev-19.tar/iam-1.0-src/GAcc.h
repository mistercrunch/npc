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

#ifndef IAM_GACC_H_INCLUDED_
#define IAM_GACC_H_INCLUDED_

namespace IAM
{
 /*

  INTUITIVE GENERIC ACCESS MASKS

 */
 enum IntuitiveGenericAccess
 {
  IntuitiveNone = 0,
  IntuitiveGenericRead = 1 << 0,
  IntuitiveGenericWrite = 1 << 1,
  IntuitiveGenericExecute = 1 << 2,
  IntuitiveGenericAllExtra = 1 << 3,
  IntuitiveGenericChange = IntuitiveGenericRead | IntuitiveGenericWrite,
  IntuitiveGenericAll =
   IntuitiveGenericRead |
   IntuitiveGenericWrite |
   IntuitiveGenericExecute |
   IntuitiveGenericAllExtra
 };

 class IntuitiveGenericMapping: public GENERIC_MAPPING
 {
  private:
   static ACCESS_MASK& GetAceAccessMask(IN const ACE_HEADER * paceh)
   {
    switch(paceh->AceType)
    {
     case ACCESS_ALLOWED_ACE_TYPE:
      return CONTAINING_RECORD(paceh, ACCESS_ALLOWED_ACE, Header)->Mask;

     case ACCESS_DENIED_ACE_TYPE:
      return CONTAINING_RECORD(paceh, ACCESS_DENIED_ACE, Header)->Mask;

     case SYSTEM_AUDIT_ACE_TYPE:
      return CONTAINING_RECORD(paceh, SYSTEM_AUDIT_ACE, Header)->Mask;

     case SYSTEM_ALARM_ACE_TYPE:
      return CONTAINING_RECORD(paceh, SYSTEM_ALARM_ACE, Header)->Mask;

     case ACCESS_ALLOWED_OBJECT_ACE_TYPE:
      return CONTAINING_RECORD(paceh, ACCESS_ALLOWED_OBJECT_ACE, Header)->Mask;

     case ACCESS_DENIED_OBJECT_ACE_TYPE:
      return CONTAINING_RECORD(paceh, ACCESS_DENIED_OBJECT_ACE, Header)->Mask;

     case SYSTEM_AUDIT_OBJECT_ACE_TYPE:
      return CONTAINING_RECORD(paceh, SYSTEM_AUDIT_OBJECT_ACE, Header)->Mask;

     case SYSTEM_ALARM_OBJECT_ACE_TYPE:
      return CONTAINING_RECORD(paceh, SYSTEM_ALARM_OBJECT_ACE, Header)->Mask;

     case ACCESS_ALLOWED_CALLBACK_ACE_TYPE:
      return
       CONTAINING_RECORD(paceh, ACCESS_ALLOWED_CALLBACK_ACE, Header)->Mask;

     case ACCESS_DENIED_CALLBACK_ACE_TYPE:
      return
       CONTAINING_RECORD(paceh, ACCESS_DENIED_CALLBACK_ACE, Header)->Mask;

     case ACCESS_ALLOWED_CALLBACK_OBJECT_ACE_TYPE:
      return
       CONTAINING_RECORD(paceh, ACCESS_ALLOWED_CALLBACK_OBJECT_ACE, Header)
        ->Mask;
     case ACCESS_DENIED_CALLBACK_OBJECT_ACE_TYPE:
      return
        CONTAINING_RECORD(paceh, ACCESS_DENIED_CALLBACK_OBJECT_ACE, Header)
         ->Mask;

     case SYSTEM_AUDIT_CALLBACK_ACE_TYPE:
      return
       CONTAINING_RECORD(paceh, SYSTEM_AUDIT_CALLBACK_ACE, Header)->Mask;

     case SYSTEM_ALARM_CALLBACK_ACE_TYPE:
      return
       CONTAINING_RECORD(paceh, SYSTEM_ALARM_CALLBACK_ACE, Header)->Mask;

     case SYSTEM_AUDIT_CALLBACK_OBJECT_ACE_TYPE:
      return
       CONTAINING_RECORD(paceh, SYSTEM_AUDIT_CALLBACK_OBJECT_ACE, Header)
        ->Mask;

     case SYSTEM_ALARM_CALLBACK_OBJECT_ACE_TYPE:
      return
       CONTAINING_RECORD(paceh, SYSTEM_ALARM_CALLBACK_OBJECT_ACE, Header)
        ->Mask;

     default: // TODO
      throw 0;
    }
   }

  public:
   ACCESS_MASK GenericAllExtra;

   IntuitiveGenericMapping
   (
    IN ACCESS_MASK amGenericRead,
    IN ACCESS_MASK amGenericWrite,
    IN ACCESS_MASK amGenericExecute,
    IN ACCESS_MASK amGenericAll
   )
   {
    static const ACCESS_MASK c_amGenericMask =
     GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE | GENERIC_ALL;

    assert(!(amGenericRead & c_amGenericMask));
    assert(!(amGenericWrite & c_amGenericMask));
    assert(!(amGenericExecute & c_amGenericMask));
    assert(!(amGenericAll & c_amGenericMask));

    GenericRead = amGenericRead;
    GenericWrite = amGenericWrite;
    GenericExecute = amGenericExecute;
    GenericAll = amGenericAll;

    assert
    (
     (GenericAll | (GenericRead | GenericWrite | GenericExecute)) == GenericAll
    );

    // access to be added to RWX to obtain F. Combines:
    GenericAllExtra =
     // F minus RWX
     (GenericAll & ~(GenericRead | GenericWrite | GenericExecute)) |
     // all the standard rights required for F access to this object type
     (GenericAll & STANDARD_RIGHTS_REQUIRED);

    assert
    (
     (GenericRead | GenericWrite | GenericExecute | GenericAllExtra) ==
     GenericAll
    );

    assert
    (
     (GenericAllExtra & STANDARD_RIGHTS_REQUIRED) ==
     (GenericAll & STANDARD_RIGHTS_REQUIRED)
    );
   }

   ACCESS_MASK MapFromIntuitive(IN IntuitiveGenericAccess amToMap) const
   {
    assert((amToMap | IntuitiveGenericAll) == IntuitiveGenericAll);

    ACCESS_MASK amMapped = 0;

    if(amToMap & IntuitiveGenericRead)
     amMapped |= this->GenericRead;

    if(amToMap & IntuitiveGenericWrite)
     amMapped |= this->GenericWrite;

    if(amToMap & IntuitiveGenericExecute)
     amMapped |= this->GenericExecute;

    if(amToMap & IntuitiveGenericAllExtra)
     amMapped |= this->GenericAllExtra;

    return amMapped;
   } // ACCESS_MASK MapFromIntuitive(IN IntuitiveGenericAccess)

   void MapFromIntuitive(IN OUT PACL paclToMap) const
   {
    for(WORD i = 0; i < paclToMap->AceCount; ++ i)
    {
     PVOID pAce;

     // TODO
     GetAce(paclToMap, DWORD(i), &pAce);

     ACCESS_MASK& am = GetAceAccessMask(reinterpret_cast<PACE_HEADER>(pAce));

     assert((am & ~IntuitiveGenericAll) == 0);
     am = MapFromIntuitive((IntuitiveGenericAccess)am);
    }
   } // void MapFromIntuitive(IN OUT PACL)

   ACCESS_MASK MapGenericFromIntuitive(IN IntuitiveGenericAccess amToMap) const
   {
    assert((amToMap | IntuitiveGenericAll) == IntuitiveGenericAll);

    ACCESS_MASK amMapped = 0;

    if(amToMap == IntuitiveGenericAll)
     amMapped = GENERIC_ALL;
    else
    {
     if(amToMap & IntuitiveGenericRead)
      amMapped |= GENERIC_READ;

     if(amToMap & IntuitiveGenericWrite)
      amMapped |= GENERIC_WRITE;

     if(amToMap & IntuitiveGenericExecute)
      amMapped |= GENERIC_EXECUTE;

     if(amToMap & IntuitiveGenericAllExtra)
      amMapped |= STANDARD_RIGHTS_REQUIRED;
    }

    return amMapped;
   } // ACCESS_MASK MapGenericFromIntuitive(IN IntuitiveGenericAccess)

   void MapGenericFromIntuitive(IN OUT PACL paclToMap) const
   {
    for(WORD i = 0; i < paclToMap->AceCount; ++ i)
    {
     PVOID pAce;

     // TODO
     GetAce(paclToMap, DWORD(i), &pAce);

     ACCESS_MASK& am = GetAceAccessMask(reinterpret_cast<PACE_HEADER>(pAce));

     assert((am & ~IntuitiveGenericAll) == 0);
     am = MapGenericFromIntuitive((IntuitiveGenericAccess)am);
    }
   } // void MapGenericFromIntuitive(IN OUT PACL)

   IntuitiveGenericAccess MapToIntuitive(IN ACCESS_MASK amToMap) const
   {
    static const ACCESS_MASK c_amGenericMask =
     GENERIC_READ | GENERIC_WRITE | GENERIC_EXECUTE | GENERIC_ALL;

    IntuitiveGenericAccess amMapped = IntuitiveNone;

    // the mask only contains generic rights
    if((amToMap | c_amGenericMask) == c_amGenericMask)
    {
     if(amToMap & GENERIC_ALL)
      return IntuitiveGenericAll;

     if(amToMap & GENERIC_READ)
      amMapped |= IntuitiveGenericRead;

     if(amToMap & GENERIC_WRITE)
      amMapped |= IntuitiveGenericWrite;

     if(amToMap & GENERIC_EXECUTE)
      amMapped |= IntuitiveGenericExecute;
    }
    // the mask contains specific rights
    else
    {
     // the mask contains some generic rights: map them, first
     if(amToMap & c_amGenericMask)
      MapGenericMask
      (
       &amToMap,
       const_cast<PGENERIC_MAPPING>(static_cast<const GENERIC_MAPPING *>(this))
      );

     if(amToMap & this->GenericRead)
      amMapped |= IntuitiveGenericRead;

     if(amToMap & this->GenericWrite)
      amMapped |= IntuitiveGenericWrite;

     if(amToMap & this->GenericExecute)
      amMapped |= IntuitiveGenericExecute;

     if(amToMap & this->GenericAllExtra)
      amMapped |= IntuitiveGenericAllExtra;

     // can't (fully) map the provided mask to an intuitive generic access mask
     if(MapFromIntuitive(amMapped) != amToMap)
      // TODO: throw
      throw 0;
    }

    return amMapped;
   } // IntuitiveGenericAccess MapToIntuitive(IN ACCESS_MASK)

   void MapToIntuitive(IN OUT PACL paclToMap) const
   {
    for(WORD i = 0; i < paclToMap->AceCount; ++ i)
    {
     PVOID pAce;

     // TODO
     GetAce(paclToMap, DWORD(i), &pAce);

     ACCESS_MASK& am = GetAceAccessMask(reinterpret_cast<PACE_HEADER>(pAce));

     am = (ACCESS_MASK)MapToIntuitive(am);
    }
   } // void MapToIntuitive(IN OUT PACL)
 }; // class IntuitiveGenericMapping
} // namespace IAM

#endif // IAM_GACC_H_INCLUDED_

// EOF
