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

#ifndef IAM_H_INCLUDED_
#define IAM_H_INCLUDED_

#include "Items.h"

namespace IAM
{
 typedef std::set<KJK::Sid> GroupSet;

 /*

  COMMAND-LINE OPTIONS PARSING

 */
 namespace Opt
 {
  enum Prefix
  {
   NoPrefix,   // option
   Minus,      // -option
   MinusMinus, // --option
   Plus,       // +option
   PlusPlus    // ++option
  };

  enum Modifier
  {
   NoModifier, // -option
   Hash,       // -#option

   ModifierCount
  };

  enum Type
  {
   Unknown,   // none (error)
   Help,      // show help
   About,     // show version and copyright
   NoWait,    // don't wait for the command to terminate
   Priv,      // enable/disable privilege
   Group,     // enable/disable group
   RToken,    // always use a restricted token
   MRGroup,   // set the main restricting group
   RGroup,    // add a restricting group
   TokAcl,    // edit the token's ACL
   Acl,       // edit the default ACL
   Owner,     // set the default owner
   PGroup,    // set the default primary group
   Typical,   // set the most typical options
   WinCompat, // behave like the built-in Windows sandbox

   TypeCount
  };

  enum ArgType
  {
   TextArg,  // -opt=<text>
   Star,     // -opt=*
   StarStar, // -opt=**

   ArgTypeCount
  };

  typedef void (* Handler)(IN Transition, IN bool, IN LPCTSTR, IN LPCTSTR);
 } // namespace Opt

 namespace Set
 {
  void RToken(IN Transition, IN bool, IN LPCTSTR, IN LPCTSTR);
 } // namespace Set
} // namespace IAM

#endif // IAM_H_INCLUDED_

// EOF
