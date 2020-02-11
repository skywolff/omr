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

#include "optimizer/TreeInterpreter.hpp"
#include <stddef.h>
#include <stdint.h>
#include <map>
#include "infra/forward_list.hpp"
#include "codegen/CodeGenerator.hpp"
#include "env/FrontEnd.hpp"
#include "compile/Compilation.hpp"
#include "compile/SymbolReferenceTable.hpp"
#include "control/Options.hpp"
#include "control/Options_inlines.hpp"
#include "env/CompilerEnv.hpp"
#include "env/IO.hpp"
#include "env/StackMemoryRegion.hpp"
#include "env/jittypes.h"
#include "il/AutomaticSymbol.hpp"
#include "il/Block.hpp"
#include "il/DataTypes.hpp"
#include "il/ILOpCodes.hpp"
#include "il/ILOps.hpp"
#include "il/MethodSymbol.hpp"
#include "il/Node.hpp"
#include "il/NodePool.hpp"
#include "il/Node_inlines.hpp"
#include "il/ResolvedMethodSymbol.hpp"
#include "il/Symbol.hpp"
#include "il/SymbolReference.hpp"
#include "il/TreeTop.hpp"
#include "il/TreeTop_inlines.hpp"
#include "infra/Assert.hpp"
#include "infra/BitVector.hpp"
#include "infra/ILWalk.hpp"
#include "infra/List.hpp"
#include "optimizer/Optimization.hpp"
#include "optimizer/Optimization_inlines.hpp"
#include "optimizer/OptimizationManager.hpp"
#include "optimizer/Optimizations.hpp"
#include "optimizer/Optimizer.hpp"
#include "optimizer/TransformUtil.hpp"
#include "ras/Debug.hpp"


TR::Optimization *
TR::TI::TreeInterpreter::create(TR::OptimizationManager *manager)
   {
   return new (manager->allocator()) TR::TI::TreeInterpreter(manager);
   }


TR::TI::TreeInterpreter::TreeInterpreter(TR::OptimizationManager *manager)
   : TR::Optimization(manager)
{
   // printf("TreeInterpreter constructor, manager address: %p\n", manager);
}

int32_t
TR::TI::TreeInterpreter::perform()
{
   TR::TreeTop *firstTree = comp()->getStartTree();
   for ( TR::TreeTop * treeTop = firstTree; treeTop != NULL; treeTop = treeTop->getNextTreeTop()){
         process(treeTop->getNode());
   }
   // traceMsg
   return 1;
}

const char *
TR::TI::TreeInterpreter::optDetailString() const throw()
   {
   return "O^O TreeInterpreter: ";
   }

TR::TI::VALUE
TR::TI::TreeInterpreter::process(TR::Node *node)
{
   traceMsg(comp(), "processing node n%dn [%p]\n", node->getGlobalIndex(), node);

   // node exists in the map, return value form map
   if (nodeValuesMap.find(node->getGlobalIndex()) != nodeValuesMap.end()){
      traceMsg(comp(), "value exists in map; value = processing node n%dn [%p]\n", node->getGlobalIndex(), node);
      return nodeValuesMap[node->getGlobalIndex()];
   }

   int numChildren = node->getNumChildren();
   for (int i = 0; i < numChildren; i++){
      process(node->getChild(i));
   }
   return performOp(node);
}