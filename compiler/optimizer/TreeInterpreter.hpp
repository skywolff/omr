/*******************************************************************************
 * Copyright (c) 2020 IBM Corp. and others
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
#include "optimizer/Optimization.hpp"

namespace TR
{
class Block;
class OptimizationManager;
class TreeTop;

static const char ValueTypes[9][10] = {
   "NoType",
   "Int8",
   "Int16",
   "Int32",
   "Int64",
   "Float",
   "Double",
   "Address",
   "Boolean"
};

class TreeInterpreter : public TR::Optimization
{
   public:
   enum Datatype {
      NoType=0,
      Int8,
      Int16,
      Int32,
      Int64,
      Float,
      Double,
      Address
   };
   union Data{
      uintptr_t  aconst;
      int8_t      bconst;
      int16_t     sconst;
      int32_t     iconst;
      int64_t     lconst;
      float_t     fconst;
      double_t    dconst;
   };
   
   struct Value {
      enum Datatype type;
      union Data data;
   };


   typedef std::pair<ncount_t const, struct Value> NodeToValueMapEntry;
   typedef TR::typed_allocator<NodeToValueMapEntry, TR::Region&> NodeToValueMapAlloc;
   typedef std::map<ncount_t, struct Value, std::less<ncount_t>, NodeToValueMapAlloc> NodeToValueMap;
   NodeToValueMap nodeValueMap;

   typedef std::pair<TR::Symbol * const, struct Value> SymbolTableEntry;
   typedef TR::typed_allocator<SymbolTableEntry, TR::Region&> SymbolTableAlloc;
   typedef std::map<TR::Symbol *, struct Value, std::less<TR::Symbol *>, SymbolTableAlloc> SymbolTable;
   SymbolTable symbolTable;

   explicit TreeInterpreter(TR::OptimizationManager *manager);
   static TR::Optimization *create(TR::OptimizationManager *manager);
   virtual int32_t perform();
   virtual const char * optDetailString() const throw();

   private:
   void process(TR::Node *node);

   // operations
   void performOp(TR::Node * node);
   void dumpNodeToValueMap();
   void dumpSymbolTable();
};
}

#endif
