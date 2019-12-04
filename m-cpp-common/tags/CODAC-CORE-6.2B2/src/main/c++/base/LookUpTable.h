/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/LookUpTable.h $
* $Id: LookUpTable.h 101457 2019-08-08 12:51:16Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Infrastructure tools - Prototype
*
* Author        : Bertrand Bauvir
*
* Copyright (c) : 2010-2019 ITER Organization,
*                 CS 90 046
*                 13067 St. Paul-lez-Durance Cedex
*                 France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

/**
 * @file LookUpTable.h
 * @brief Header file for PVAccessRPCClient class.
 * @date 01/06/20189
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2018 ITER Organization
 * @detail This header file contains the definition of the LookUpTable class.
 */

#ifndef _LookUpTable_h_
#define _LookUpTable_h_

// Global header files

#include <map> // std::map
#include <utility> // std::pair

// Local header files

#include "BasicTypes.h" // Global type definition
#include "AtomicLock.h"
#include "SysTools.h"

// Constants

#ifndef EOT_KEYWORD
#define EOT_KEYWORD "endoftable"
#endif // EOT_KEYWORD

// Type definition

namespace ccs {

namespace base {

template <typename Type> struct NameValuePair 
{

  ccs::types::string name;
  Type value;

};

/**
 * @brief Implementation class providing support for LookUp table.
 * @detail Templated class providing support for storing and accessing
 * elements by name. The class inherits from ccs::base::AtomicLock but
 * the management of any locking mechanism required by the application
 * must be performed by the application explicitly.
 * @note The element keys are the names provided upon registration.
 * There is no guarantee that accessing elements by index will return
 * the elements in the order at which they were registered. The index
 * oriented API is provided to simplify application-side which may
 * prefer to refer to recall integers, etc. rather than strings, or
 * simply loop through the table for display purposes.
 */

template <typename Type> class LookUpTable : public AtomicLock
{

  private:

    /**
     * @brief Attribute. 
     * @detail Implementation is based on std::map.
     */

    std::map<std::string,Type> __lutable;
    std::map<ccs::types::uint32,std::string> __indexes;

    /**
     * @brief Attribute. 
     * @note GetName requires persistent storage across function call.
     */

    ccs::types::string __name; // For GetName

  protected:

  public:

    /**
     * @brief Constructor. NOOP.
     */

    LookUpTable (void);

    /**
     * @brief Copy constructor.
     */

    LookUpTable (const LookUpTable<Type>& type);

    /**
     * @brief Constructor.
     * @detail Instantiate LookUpTable and pre-fill it with the content
     * of a table of name/value pairs.
     * @code
       static ccs::base::NameValuePair<ccs::types::uint32> __table [] = {
         { "zero", 0u },
         { "one",  1u },
         { "two",  2u },
         { "three", 3u },
         { "four",  4u },
         { EOT_KEYWORD, 0u } // End-of-table marker
       };

       // Instantiate LookUpTable
       ccs::base::LookUpTable<ccs::types::uint32> table (__table);

       bool status = (5u == table.GetSize());
       @endcode
     */

    explicit LookUpTable (const NameValuePair<Type> * const table);

    /**
     * @brief Destructor. NOOP.
     */

    virtual ~LookUpTable (void);

    /**
     * @brief Accessor.
     * @return Number of registered elements.
     */

    ccs::types::uint32 GetSize (void) const;

    /**
     * @brief Accessor.
     * @param name Element entry key.
     * @return True if the named element exists.
     */

    bool IsValid (const ccs::types::char8 * const name) const;

    /**
     * @brief Accessor.
     * @param index Element entry key.
     * @return True if the indexed element exists.
     */

    bool IsValid (const ccs::types::uint32 index) const;

    /**
     * @brief Accessor.
     * @param name Element entry key.
     * @return Index associated to the key; exception thrown if invalid.
     */

    ccs::types::uint32 GetIndex (const ccs::types::char8 * const name) const;

    /**
     * @brief Accessor.
     * @param index Element entry key.
     * @return Key of element at index; exception thrown if invalid.
     */

    const ccs::types::char8* GetName (const ccs::types::uint32 index) const;

    /**
     * @brief Accessor.
     * @param name Element entry key.
     * @return Element registered with name; exception thrown if invalid.
     */

    Type GetElement (const ccs::types::char8 * const name) const;

    /**
     * @brief Accessor.
     * @param index Element entry key.
     * @return Element registered at index; exception thrown if invalid.
     */

    Type GetElement (const ccs::types::uint32 index) const;

    /**
     * @brief Accessor.
     * @param name Element entry key.
     * @param elem Placeholder for returned element.
     * @return True if successful.
     */

    bool GetElement (const ccs::types::char8 * const name, Type& elem) const;

    /**
     * @brief Accessor.
     * @param index Element entry key.
     * @param elem Placeholder for returned element.
     * @return True if successful.
     */

    bool GetElement (const ccs::types::uint32 index, Type& elem) const;

    /**
     * @brief Accessor.
     * @param name Element entry key.
     * @param elem Replacement element.
     * @return True if successful.
     */

    bool SetElement (const ccs::types::char8 * const name, const Type& elem);

    /**
     * @brief Accessor.
     * @param index Element entry key.
     * @param elem Replacement element.
     * @return True if successful.
     */

    bool SetElement (const ccs::types::uint32 index, const Type& elem);

    /**
     * @brief Accessor.
     * @detail Registers keyed element into the store.
     * @param name Element entry key.
     * @param elem Element to store with name key.
     * @return True if successful.
     */

    bool Register (const ccs::types::char8 * const name, const Type& elem);
  
    /**
     * @brief Accessor.
     * @detail Removes keyed element from the store.
     * @param name Element entry key.
     * @return True if successful.
     */

    bool Remove (const ccs::types::char8 * const name);

    /**
     * @brief Accessor.
     * @detail Removes keyed element from the store.
     * @param index Element entry key.
     * @return True if successful.
     */

    bool Remove (const ccs::types::uint32 index);

    /**
     * @brief Accessor.
     * @detail Removes all elements from the store.
     * @return True if successful.
     */

    bool Remove (void);

    /**
     * @brief Copy assignment operator.
     */

    LookUpTable<Type>& operator= (const LookUpTable<Type>& type);

};

// Global variables

// Function declaration

// Function definition

template <typename Type> ccs::types::uint32 LookUpTable<Type>::GetSize (void) const { return static_cast<ccs::types::uint32>(__lutable.size()); }

template <typename Type> bool LookUpTable<Type>::IsValid (const ccs::types::char8 * const name) const
{ 

  bool status = (NULL_PTR_CAST(ccs::types::char8*) != name);
  
  if (status)
    {
      status = (__lutable.end() != __lutable.find(std::string(name)));
    }
  
  return status; 
  
}
 
template <typename Type> bool LookUpTable<Type>::IsValid (const ccs::types::uint32 index) const
{ 

  bool status = (__indexes.end() != __indexes.find(index));
  
  return status; 
  
}
 
template <typename Type> ccs::types::uint32 LookUpTable<Type>::GetIndex (const ccs::types::char8 * const name) const
{

  bool status = IsValid(name);

  if (!status)
    {
      throw std::runtime_error("No such element");
    }

  std::string __name (name);
  ccs::types::uint32 index;

  for (auto iter = __indexes.begin(); iter != __indexes.end(); ++iter)
    {
      if (__name == iter->second)
        {
          index = iter->first;
          break;
        }
    }

  return index;
  
}

template <typename Type> const ccs::types::char8* LookUpTable<Type>::GetName (const ccs::types::uint32 index) const
{

  bool status = IsValid(index);

  if (!status)
    {
      throw std::runtime_error("No such element");
    }

  if (status)
    {
      std::string name = __indexes.find(index)->second;
      ccs::HelperTools::SafeStringCopy(const_cast<ccs::types::char8*>(__name), name.c_str(), ccs::types::MaxStringLength);
    }
  
  return __name;

}

template <typename Type> Type LookUpTable<Type>::GetElement (const ccs::types::char8 * const name) const 
{

  bool status = IsValid(name);

  if (!status)
    {
      throw std::runtime_error("No such element");
    }
  
  Type elem;

  if (status)
    {
      elem = __lutable.find(std::string(name))->second;
    }
  
  return elem;
  
}
  
template <typename Type> Type LookUpTable<Type>::GetElement (const ccs::types::uint32 index) const 
{

  bool status = IsValid(index);

  if (!status)
    {
      throw std::runtime_error("No such element");
    }
  
  Type elem;

  if (status)
    {
      std::string name = __indexes.find(index)->second;
      elem = __lutable.find(name)->second;
    }
    
  return elem;
  
}
  
template <typename Type> bool LookUpTable<Type>::GetElement (const ccs::types::char8 * const name, Type& elem) const 
{

  bool status = IsValid(name);
  
  if (status)
    {
      elem = GetElement(name);
    }
  
  return status;
  
}

template <typename Type> bool LookUpTable<Type>::GetElement (const ccs::types::uint32 index, Type& elem) const 
{

  bool status = IsValid(index);
  
  if (status)
    {
      elem = GetElement(index);
    }
  
  return status;
  
}

template <typename Type> bool LookUpTable<Type>::SetElement (const ccs::types::char8 * const name, const Type& elem)
{

  bool status = IsValid(name);
  
  if (status)
    {
      //status = __lutable.insert_or_assign(std::string(name), elem).second; // WARNING - std=c++17
      __lutable.find(std::string(name))->second = elem;
    }
  else // Register ..
    {
      status = Register(name, elem);
    }
  
  return status;
  
}

template <typename Type> bool LookUpTable<Type>::SetElement (const ccs::types::uint32 index, const Type& elem)
{

  bool status = IsValid(index);

  std::string name;

  if (status)
    {
      name = __indexes.find(index)->second;
    }

  if (status)
    {
      //status = __lutable.insert_or_assign(std::string(name), elem).second; // WARNING - std=c++17
      __lutable.find(name)->second = elem;
    }

  return status;
  
}

template <typename Type> bool LookUpTable<Type>::Register (const ccs::types::char8 * const name, const Type& elem) 
{

  bool status = !IsValid(name);

  std::string __name (name);

  if (status)
    {
      status = __lutable.insert(std::pair<std::string,Type>(__name,elem)).second;
    }
  
  if (status)
    {
      status = __indexes.insert(std::pair<ccs::types::uint32,std::string>(__indexes.size(),__name)).second;
    }
  
  return status;
  
}

template <typename Type> bool LookUpTable<Type>::Remove (const ccs::types::char8 * const name)
{

  bool status = IsValid(name);

  ccs::types::uint32 index;

  if (status)
    {
      index = GetIndex(name);
    }

  if (status)
    {
      status = (1u == __indexes.erase(index));
    }

  if (status)
    {
      status = (1u == __lutable.erase(std::string(name)));
    }
  
  // Compress indexes
  ccs::types::uint32 tmp = index + 1u;

  while (status && (__indexes.end() != __indexes.find(tmp)))
    {
      std::string name = __indexes.find(tmp)->second;
      status = __indexes.insert(std::pair<ccs::types::uint32,std::string>(tmp - 1u,name)).second;

      if (status)
        {
          status = (1u == __indexes.erase(tmp));
        }

      tmp++;
    }

  return status;
  
}

template <typename Type> bool LookUpTable<Type>::Remove (const ccs::types::uint32 index)
{

  bool status = IsValid(index);

  std::string name;

  if (status)
    {
      name = __indexes.find(index)->second;
    }

  if (status)
    {
      status = (1u == __indexes.erase(index));
    }

  if (status)
    {
      status = (1u == __lutable.erase(name));
    }

  // Compress indexes
  ccs::types::uint32 tmp = index + 1u;

  while (status && (__indexes.end() != __indexes.find(tmp)))
    {
      std::string name = __indexes.find(tmp)->second;
      status = __indexes.insert(std::pair<ccs::types::uint32,std::string>(tmp - 1u,name)).second;

      if (status)
        {
          status = (1u == __indexes.erase(tmp));
        }

      tmp++;
    }

  return status;
  
}

template <typename Type> bool LookUpTable<Type>::Remove (void)
{

  bool status = (0u < GetSize());

  if (status)
    {
      __lutable.clear(); 
      __indexes.clear(); 
    }

  status = (0u == GetSize());

  return status; 

}

template <typename Type> LookUpTable<Type>& LookUpTable<Type>::operator= (const LookUpTable<Type>& table)
{

  bool status = (this != &table);

  if (status)
    {
      status = Remove();
    }

  for (ccs::types::uint32 index = 0u; (status && (index < table.GetSize())); index += 1u)
    {
      status = Register(table.GetName(index), table.GetElement(index));
    }

  return *this;

}

template <typename Type> LookUpTable<Type>::LookUpTable (void) : AtomicLock()
{

  // Initialise attributes
  ccs::HelperTools::SafeStringCopy(__name, STRING_UNDEFINED, STRING_MAX_LENGTH);

  return;

}

template <typename Type> LookUpTable<Type>::LookUpTable (const LookUpTable<Type>& table) : AtomicLock()
{

  // Initialise attributes
  ccs::HelperTools::SafeStringCopy(__name, STRING_UNDEFINED, STRING_MAX_LENGTH);

  bool status = (0u < table.GetSize());

  for (ccs::types::uint32 index = 0u; (status && (index < table.GetSize())); index += 1u)
    {
      status = Register(table.GetName(index), table.GetElement(index));
    }

  return;

}

template <typename Type> LookUpTable<Type>::LookUpTable (const NameValuePair<Type> * const table) : AtomicLock()
{

  // Initialise attributes
  ccs::HelperTools::SafeStringCopy(__name, STRING_UNDEFINED, STRING_MAX_LENGTH);

  ccs::types::uint32 index = 0u;

  while (std::string(table[index].name) != EOT_KEYWORD)
    {
      (void)Register(table[index].name, table[index].value);
      index += 1u;
    }

  return;

}

template <typename Type> LookUpTable<Type>::~LookUpTable (void)
{ 

  (void)Remove(); 

  return; 

}

} // namespace base

} // namespace ccs

#endif // _LookUpTable_h_

