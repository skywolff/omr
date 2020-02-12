/*******************************************************************************
 * Copyright (c) 2000, 2019 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at http://eclipse.org/legal/epl-2.0
 * or the Apache License, Version 2.0 which accompanies this distribution
 * and is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following Secondary
 * Licenses when the conditions for such availability set forth in the
 * Eclipse Public License, v. 2.0 are satisfied: GNU General Public License,
 * version 2 with the GNU Classpath Exception [1] and GNU General Public
 * License, version 2 with the OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/

#ifndef TR_TREEINTERPRETER_INCL
#define TR_TREEINTERPRETER_INCL

#include <stdint.h>
#include "env/TRMemory.hpp"
#include "infra/List.hpp"
#include "optimizer/Optimization.hpp"
#include "optimizer/Operation.hpp"
#include <stack>
#include <string>


namespace TR { class Block; }
namespace TR { class OptimizationManager; }
namespace TR { class TreeTop; }

namespace TR
{


class TreeInterpreter : public TR::Optimization
{
   public:
   // aconst - address constant (zero value means NULL)
   // iconst - integer constant (32-bit signed 2's complement)
   // lconst - long integer constant (64-bit signed 2's complement)
   // fconst - float constant (32-bit ieee fp)
   // dconst - double constant (64-bit ieee fp)
   // bconst - byte integer constant (8-bit signed 2's complement)
   // sconst - short integer constant (16-bit signed 2's complement)
   char VALUETYPE_NAME[7][10] = {"ADDRESS", "INTEGER", "LONG", "FLOAT", "DOUBLE", "BYTE_INT", "SHORT_INT"};
   typedef enum {
      ADDRESS,
      INTEGER,
      LONG,
      FLOAT,
      DOUBLE,
      BYTE_INT,
      SHORT_INT
   } DATATYPE;
   typedef union{
      uintptrj_t  aconst;
      int32_t     iconst;
      int64_t     lconst;
   } DATA;
   
   typedef struct {
      DATATYPE type;
      DATA data;
   } VALUE;


   // key value comparator, allocator
   // std::map<ncount_t, VALUE> nodeValueMap;
   typedef std::pair<ncount_t const, VALUE *> NodeToValueMapEntry;
   typedef TR::typed_allocator<NodeToValueMapEntry, TR::Region&> NodeToValueMapAlloc;
   typedef std::map<ncount_t, VALUE *, std::less<ncount_t>, NodeToValueMapAlloc> NodeToValueMap;
   NodeToValueMap nodeValueMap;


   TreeInterpreter(TR::OptimizationManager *manager);
   static TR::Optimization *create(TR::OptimizationManager *manager);
   virtual int32_t perform();
   virtual const char * optDetailString() const throw();

   private:
   VALUE process(TR::Node *node);

   // operations
   VALUE performOp(TR::Node * node);
};
}

#endif