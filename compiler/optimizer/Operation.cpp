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

// #include "optimizer/Operation.hpp"
#include "optimizer/TreeInterpreter.hpp"
#include <cstring>

void
TR::TreeInterpreter::performOp(TR::Node * node){
   VALUE operand1, operand2, result;
   memset(&result, 0, sizeof(VALUE));
   // make sure the children exists.
   // TR_ASSERT_FATAL

   switch (node->getOpCodeValue()){
      case TR::BBStart:
         break;
      case TR::BBEnd:
         break;
      case TR::treetop:
         break;

      // long operations
      case TR::lconst:
         result.type = LONG;
         result.data.lconst = node->getLongInt();
         break;
      case TR::ladd:
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = LONG;
         result.data.lconst = operand1.data.lconst + operand2.data.lconst;
         break;
      case TR::lsub:
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = LONG;
         result.data.lconst = operand1.data.lconst - operand2.data.lconst;
         break;
      case TR::lmul:
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = LONG;
         result.data.lconst = operand1.data.lconst * operand2.data.lconst;
         break;
      case TR::ldiv:
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = LONG;
         result.data.lconst = operand1.data.lconst / operand2.data.lconst;
         break;
      case TR::lload:
         result.data.lconst = (int64_t)node->getSymbol();
         break;
      case TR::lstore:
         // node->setSymbolReference(comp()->getSymRefTab()->createSymbolReference(TR::Symbol::createShadow(comp()->trHeapMemory(), node->getDataType())));
         break;
      case TR::lreturn:
         result.data.lconst = nodeValueMap[node->getChild(0)->getGlobalIndex()].data.lconst;
         break;
      default:
         TR_ASSERT_FATAL(1, "Unexpected opcode for n%dn [%p]\n", node->getGlobalIndex(), node);
   }   

   // int numChildren = node->getNumChildren();
   // TR::Node *children[numChildren];
   // for (int childI = 0; childI < numChildren; childI++){
   //    children[childI] = operandStack.top();
   //    operandStack.pop();
   // }
   nodeValueMap[node->getGlobalIndex()] = result;
   return;
}
