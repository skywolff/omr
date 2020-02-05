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

void TR::TreeInterpreter::performOp(TR::Node * node){
   switch (node->getOpCodeValue()){
      case TR::BBStart:
         return;
      case TR::BBEnd:
         return;
      case TR::treetop:
         return;

      // long operations
      case TR::lconst:
         return;
      case TR::ladd:
         performLongAdd(node);
         return;
      case TR::lsub:
         performLongSub(node);
         return;
      case TR::lmul:
         performLongMul(node);
         return;
      case TR::ldiv:
         performLongDiv(node);
         return;
      case TR::lload:
         return;
      case TR::lstore:
         return;
      case TR::lreturn:
         return;
      default:
         throw std::runtime_error("Opcode unrecognized");
   }
}

void TR::TreeInterpreter::performLongAdd(TR::Node * node){
   node->setLongInt(node->getChild(0)->getLongInt() + node->getChild(1)->getLongInt());
}

void TR::TreeInterpreter::performLongSub(TR::Node * node)
{
   node->setLongInt(node->getChild(0)->getLongInt() - node->getChild(1)->getLongInt());
}

void TR::TreeInterpreter::performLongMul(TR::Node * node)
{
   node->setLongInt(node->getChild(0)->getLongInt() * node->getChild(1)->getLongInt());
}

void TR::TreeInterpreter::performLongDiv(TR::Node * node)
{
   node->setLongInt(node->getChild(0)->getLongInt() / node->getChild(1)->getLongInt());
}