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
TR::TreeInterpreter::create(TR::OptimizationManager *manager)
   {
   return new (manager->allocator()) TR::TreeInterpreter(manager);
   }


TR::TreeInterpreter::TreeInterpreter(TR::OptimizationManager *manager)
   : TR::Optimization(manager),
   nodeValueMap(std::less<ncount_t>(), comp()->trMemory()->currentStackRegion())
{
}

int32_t
TR::TreeInterpreter::perform()
{
   TR::TreeTop *firstTree = comp()->getStartTree();
   for ( TR::TreeTop * treeTop = firstTree; treeTop != NULL; treeTop = treeTop->getNextTreeTop()){
      TR::Node *treeTopNode = treeTop->getNode();
      process(treeTopNode);

      if (treeTopNode->getOpCodeValue() != TR::BBStart && treeTopNode->getOpCodeValue() != TR::BBEnd) {
         ncount_t treeTopIndex = treeTopNode->getGlobalIndex();
         VALUE *treeTopValue = nodeValueMap[treeTopIndex];
         // printf("typeString: %s; dataString: %#.8\n", this->VALUETYPE_NAME[treeTopValue->type], treeTopValue->data.lconst);
         // printf("treeTop n%dn [%p] processed, value(%s) = %#.8x\n",
         //    treeTopIndex, treeTopNode, VALUETYPE_NAME[treeTopValue->type], treeTopValue->data.lconst);
         // printf("%s\n", dumpNodeToValueMap());
         
         traceMsg(comp(), "treeTop n%dn [%p] processed, value(%s) = %#.8x\n",
            treeTopIndex, treeTopNode, VALUETYPE_NAME[treeTopValue->type], treeTopValue->data.lconst);
         traceMsg(comp(), "%s\n", dumpNodeToValueMap());
      }
   }
   return 1;
}

const char *
TR::TreeInterpreter::optDetailString() const throw()
   {
   return "O^O TreeInterpreter: ";
   }

void
TR::TreeInterpreter::process(TR::Node *node)
{
   if (nodeValueMap.find(node->getGlobalIndex()) != nodeValueMap.end()){
      traceMsg(comp(), "\tprocessing node n%dn [%p], value found in map\n", node->getGlobalIndex(), node);
   }

   int numChildren = node->getNumChildren();
   for (int i = 0; i < numChildren; i++){
      process(node->getChild(i));
   }
   traceMsg(comp(), "\tprocessing node n%dn [%p], evaluating\n", node->getGlobalIndex(), node);
   // traceMsg(comp(), "\t\tnodeDataType: %s\n", node->getDataType().getName(node->getDataType()));
   performOp(node);
   return;
}

char *
TR::TreeInterpreter::dumpNodeToValueMap()
{
   char *mapString = (char *) malloc(2048);
   char *endString = mapString;
   for(auto e : nodeValueMap){
      endString += sprintf(endString, "nodeGlobalIndex: n%dn;  ", e.first);
      endString += sprintf(endString, "VALUE: {type = %s, data = {aconst = %#.8x, iconst = %d, lconst = %ld}}\n",
         VALUETYPE_NAME[e.second->type], e.second->data.aconst, e.second->data.iconst, e.second->data.lconst);
    }
    return mapString;
}