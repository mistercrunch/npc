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

#ifndef IAM_ITEMS_H_INCLUDED_
#define IAM_ITEMS_H_INCLUDED_

namespace IAM
{
 /*

  ITEM STATE MAPS

 */
 enum State
 {
  Delete = -2,
  Negative = -1,
  Neutral = 0,
  Positive = 1
 };

 enum Transition
 {
  DoDelete = -1,
  DoNegative = 0,
  DoPositive = 1,
  DoUndelete = 2
 };

 struct ItemStateTransition
 {
  enum Transition eType;
  bool fCritical;
  LPCTSTR pszArgument;
  LPCTSTR pszText;

  ItemStateTransition
  (
   IN enum Transition eType,
   IN bool fCritical,
   IN LPCTSTR pszArgument,
   IN LPCTSTR pszText
  ):
   eType(eType),
   fCritical(fCritical),
   pszArgument(pszArgument),
   pszText(pszText)
  { }
 }; // struct ItemStateTransition

 struct ItemDefaultStepStateImpl:
  public std::binary_function<enum State, enum Transition, enum State>
 {
  ItemDefaultStepStateImpl::result_type operator()
  (
   IN ItemDefaultStepStateImpl::first_argument_type eState,
   IN ItemDefaultStepStateImpl::second_argument_type eType
  )
  {
   switch(eType)
   {
    /* "-" prefix */
    case DoNegative:
    {
     switch(eState)
     {
      case Positive:
       return Neutral;

      case Neutral:
      case Negative:
       return Negative;

      case Delete:
       return Delete;
     }

     break;
    }

    /* "+" prefix */
    case DoPositive:
    {
     switch(eState)
     {
      case Negative:
       return Neutral;

      case Neutral:
      case Positive:
       return Positive;

      case Delete:
       return Delete;
     }

     break;
    }

    /* "-#" prefix */
    case DoDelete: return Delete;

    /* "+#" prefix */
    case DoUndelete:
    {
     switch(eState)
     {
      case Positive:
      case Neutral:
      case Negative:
       return eState;

      case Delete:
       return Neutral;
     }

     break;
    }
   }

   return Neutral;
  } // operator()
 }; // struct ItemDefaultStepStateImpl

 template<class StepState_ = ItemDefaultStepStateImpl> struct ItemState
 {
  private:
   StepState_ m_fnStepState;

  public:
   enum State eState;
   bool fCritical;
   LPCTSTR pszArgument;
   LPCTSTR pszText;

   // CONSTRUCTORS
   // default constructor
   ItemState():
    eState(Neutral), fCritical(false), pszArgument(NULL), pszText(NULL)
   { }

   // conversion from enum State
   ItemState(IN enum State eState):
    eState(eState), fCritical(false), pszArgument(NULL), pszText(NULL)
   { }

   // copy constructor
   ItemState(IN const ItemState& Other):
    eState(Other.eState),
    fCritical(Other.fCritical),
    pszArgument(Other.pszArgument),
    pszText(Other.pszText)
   { }

   // transition
   ItemState(IN const ItemStateTransition& Xform):
    eState(StepState(eState, Xform.eType)),
    fCritical(Xform.fCritical),
    pszArgument(Xform.pszArgument),
    pszText(Xform.pszText)
   { }

   // OPERATORS
   bool operator==(IN enum State eState) const
   { return (this->eState - eState) == 0; }

   bool operator!=(IN enum State eState) const
   { return !(*this == eState); }

   bool operator==(IN const ItemState& Other) const
   { return *this == Other.eState; }

   bool operator!=(IN const ItemState& Other) const
   { return !(*this == Other); }

   ItemState operator<<(IN const ItemStateTransition& Xform) { return Xform; }

   ItemState& operator<<=(IN const ItemStateTransition& Xform)
   {
    eState = m_fnStepState(eState, Xform.eType);
    fCritical = Xform.fCritical;
    pszArgument = Xform.pszArgument;
    pszText = Xform.pszText;
    return *this;
   }
 }; // struct ItemState

 template<class I_, class State_, class StateTransition_> class ItemStates
 {
  private:
   typedef I_ ItemType;
   typedef State_ StateType;
   typedef StateTransition_ StateTransitionType;

   typedef std::map<ItemType, StateType> ItemMap;
   typedef ItemMap::const_iterator ItemMapIterator;

   StateType m_Global;
   ItemMap m_Specific;

  public:
   void State(IN const StateTransitionType& StateTransition)
   {
    m_Global <<= StateTransition;

    for(ItemMap::iterator p = m_Specific.begin(); p != m_Specific.end(); ++ p)
     if((p->second <<= StateTransition) == m_Global)
      m_Specific.erase(p);
   }

   void State
   (
    IN const ItemType& Item,
    IN const StateTransitionType& StateTransition
   )
   {
    ItemMap::iterator p = m_Specific.find(Item);

    if(p == m_Specific.end())
    {
     StateType State;

     if((State <<= StateTransition) != m_Global)
      m_Specific.insert(ItemMap::value_type(Item, State));
    }
    else if((p->second <<= StateTransition) == m_Global)
     m_Specific.erase(p);
   }

   const StateType& State() const { return m_Global; }

   const StateType& State(IN const ItemType& Item) const
   {
    ItemMap::const_iterator p = m_Specific.find(Item);

    if(p == m_Specific.end()) return m_Global;
    else return p->second;
   }

   ItemMap::size_type SpecificStateCount() const { return m_Specific.size(); }

   StateType ConsumeState(IN const ItemType& Item)
   {
    ItemMap::iterator p = m_Specific.find(Item);

    if(p == m_Specific.end())
     return m_Global;
    else
    {
     StateType temp = p->second;
     m_Specific.erase(p);
     return temp;
    }
   }

   bool CompareAndConsumeState
   (
    IN const ItemType& Item,
    IN const StateType& State
   )
   {
    ItemMap::iterator p = m_Specific.find(Item);

    if(p == m_Specific.end())
     return m_Global == State;
    else if(p->second == State)
    {
     m_Specific.erase(p);
     return true;
    }
    else
     return false;
   }

   typedef ItemMap::iterator Iterator;

   Iterator End() { return m_Specific.end(); }

   Iterator Begin() { return m_Specific.begin(); }

   Iterator Begin(IN const StateType& State)
   {
    Iterator p(m_Specific.begin());

    while(p != End() && p->second != State)
     ++ p;

    return p;
   }

   Iterator Next(IN const Iterator& Cur) { return ++ Iterator(Cur); }

   Iterator Next(IN const Iterator& Cur, IN const StateType& State)
   {
    Iterator p(Cur);

    while(p != End() && p->second != State)
     ++ p;

    return p;
   }
 }; // class ItemStates
};

#endif // IAM_ITEMS_H_INCLUDED_

// EOF




