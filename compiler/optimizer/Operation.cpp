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

TR::TI::nodeValue
TR::TI::TreeInterpreter::performOp(TR::Node * node){
   nodeValue dummy;
   switch (node->getOpCodeValue()){
      case TR::BBStart:
         return dummy;
      case TR::BBEnd:
         return dummy;
      case TR::treetop:
         return dummy;

      // long operations
      case TR::lconst:
         return dummy;
      case TR::ladd:
         performLongAdd(node);
      case TR::lsub:
         performLongSub(node);
      case TR::lmul:
         performLongMul(node);
      case TR::ldiv:
         performLongDiv(node);
      case TR::lload:
      case TR::lstore:
      case TR::lreturn:
      default:
         throw std::runtime_error("Opcode unrecognized");
   }   

   int numChildren = node->getNumChildren();
   TR::Node *children[numChildren];
   for (int childI = 0; childI < numChildren; childI++){
      children[childI] = operandStack.top();
      operandStack.pop();
   }
}

void
TR::TI::TreeInterpreter::performLongAdd(TR::Node * node){
   node->setLongInt(node->getChild(0)->getLongInt() + node->getChild(1)->getLongInt());
}

void
TR::TI::TreeInterpreter::performLongSub(TR::Node * node)
{
   node->setLongInt(node->getChild(0)->getLongInt() - node->getChild(1)->getLongInt());
}

void
TR::TI::TreeInterpreter::performLongMul(TR::Node * node)
{
   node->setLongInt(node->getChild(0)->getLongInt() * node->getChild(1)->getLongInt());
}

void
TR::TI::TreeInterpreter::performLongDiv(TR::Node * node)
{
   node->setLongInt(node->getChild(0)->getLongInt() / node->getChild(1)->getLongInt());
}