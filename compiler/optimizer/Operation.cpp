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

#include "optimizer/TreeInterpreter.hpp"
#include <cstring>

void
TR::TreeInterpreter::performOp(TR::Node * node){
   TR::TreeInterpreter::Value operand1, operand2, result;
   std::memset(&result, 0, sizeof(TR::TreeInterpreter::Value));

   switch (node->getOpCodeValue()){
      case TR::BBStart:
         break;
      case TR::BBEnd:
         break;
      case TR::treetop:
         break;

      // long operations
      case TR::lconst:
      {
         result.type = Int64;
         result.data.lconst = node->getLongInt();
         break;
      }
      case TR::lload:
      {
         result = symbolTable[node->getSymbol()];
         break;
      }
      case TR::lloadi:
      {
         result.type = TR::TreeInterpreter::Datatype::Address;
         result.data.aconst = reinterpret_cast<uintptr_t>(&(symbolTable[node->getSymbol()]));
         break;
      }
      case TR::lstore:
      {
         symbolTable[node->getSymbol()] = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         break;
      }
      case TR::ladd:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = operand1.data.lconst + operand2.data.lconst;
         break;
      }
      case TR::lsub:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = operand1.data.lconst - operand2.data.lconst;
         break;
      }
      case TR::lmul:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = operand1.data.lconst * operand2.data.lconst;
         break;
      }
      case TR::ldiv:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = operand1.data.lconst / operand2.data.lconst;
         break;
      }
      case TR::lrem:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = operand1.data.lconst % operand2.data.lconst;
         break;
      }
      case TR::lneg:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = -operand1.data.lconst;
         break;
      }
      case TR::labs:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         result.type = Int64;
         if (operand1.data.lconst >= 0)
            result.data.lconst = operand1.data.lconst;
         else
            result.data.lconst = -(operand1.data.lconst);
         break;
      }
      case TR::lshl:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         TR_ASSERT_FATAL(operand2.data.lconst >= 0, "lshl cannot have negative shift amount %l\n", operand2.data.lconst);
         TR_ASSERT_FATAL(operand2.data.lconst < 64, "lshl cannot shift more than the size of the container (64, is %l)\n", operand2.data.lconst);

         int i;
         if ((operand1.data.lconst == 0) || (operand2.data.lconst == 0)) {
            result.data.lconst = operand1.data.lconst;
            break;
         }
         else if (operand1.data.lconst > 0) {
            for (i = 62; i >=0; --i){
               if (operand1.data.lconst & (1<<i)) break;
            }
         }
         else {
            for (i = 62; i >=0; --i){
               if ((operand1.data.lconst & (1<<i))==0) break;
            }
         }
         TR_ASSERT_FATAL(operand2.data.lconst <= 62 - i, "lshl will overflow with value %l and shift %l\n", operand1.data.lconst, operand2.data.lconst);
         result.data.lconst = operand1.data.lconst << operand2.data.lconst;
         break;
      }
      case TR::lshr:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         TR_ASSERT_FATAL(operand2.data.lconst >= 0, "lshr cannot have negative shift amount %l\n", operand2.data.lconst);
         TR_ASSERT_FATAL(operand2.data.lconst < 64, "lshr cannot shift more than the size of the container (64, is %l)\n", operand2.data.lconst);

         int i;
         if (operand1.data.lconst < 0) {
            result.data.lconst = operand1.data.lconst;
            for (int i = 0; i < operand2.data.lconst; ++i) {
               result.data.lconst = result.data.lconst >> 1;
               result.data.lconst = result.data.lconst | (1<<63);
            }
         } else {
            result.data.lconst = operand1.data.lconst >> operand2.data.lconst;
         }
         break;
      }
      case TR::lrol:
      {
         //TODO: this needs error handling. Possible undefined behaviours with signed overflow.
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = (operand1.data.lconst << (operand2.data.lconst%64)) 
                              | (operand1.data.lconst >> (64 - (operand2.data.lconst%64)));
         break;
      }
      case TR::land:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = operand1.data.lconst & operand2.data.lconst;
         break;
      }
      case TR::lor:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = operand1.data.lconst | operand2.data.lconst;
         break;
      }
      case TR::lxor:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = operand1.data.lconst ^ operand2.data.lconst;
         break;
      }
      case TR::l2i:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         result.type = Int32;
         result.data.iconst = static_cast<int32_t>(operand1.data.lconst);
         break;
      }
      case TR::l2f:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         result.type = Float;
         result.data.fconst = static_cast<float_t>(operand1.data.lconst);
         break;
      }
      case TR::l2d:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         result.type = Double;
         result.data.dconst = static_cast<double_t>(operand1.data.lconst);
         break;
      }
      case TR::l2b:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         result.type = Int8;
         result.data.bconst = static_cast<int8_t>(operand1.data.lconst);
         break;
      }
      case TR::l2s:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         result.type = Int16;
         result.data.sconst = static_cast<int16_t>(operand1.data.lconst);
         break;
      }
      case TR::l2a:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         result.type = Address;
         result.data.aconst = static_cast<uintptr_t>(operand1.data.lconst);
         break;
      }
      case TR::lcmpeq:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = operand1.data.lconst == operand2.data.lconst ? 1 : 0;
         break;
      }
      case TR::lcmpne:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = operand1.data.lconst != operand2.data.lconst ? 1 : 0;
         break;
      }
      case TR::lcmplt:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = operand1.data.lconst <  operand2.data.lconst ? 1 : 0;
         break;
      }
      case TR::lcmpge:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = operand1.data.lconst >= operand2.data.lconst ? 1 : 0;
         break;
      }
      case TR::lcmpgt:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = operand1.data.lconst >  operand2.data.lconst ? 1 : 0;
         break;
      }
      case TR::lcmple:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = operand1.data.lconst <= operand2.data.lconst ? 1 : 0;
         break;
      }
      case TR::lcmp:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = operand1.data.lconst >  operand2.data.lconst ? 1 :
                              operand1.data.lconst == operand2.data.lconst ? 0 : -1;
         break;
      }
      case TR::lternary:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = operand1.data.lconst
                                 ? nodeValueMap[node->getChild(1)->getGlobalIndex()].data.lconst
                                 : nodeValueMap[node->getChild(2)->getGlobalIndex()].data.lconst;
         break;
      }
      case TR::lmulh:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         int64_t hi1,lo1,hi2,lo2;
         hi1 = operand1.data.lconst >> 32;
         lo1 = operand1.data.lconst & 0xFFFFFFFF;
         hi2 = operand2.data.lconst >> 32;
         lo2 = operand2.data.lconst & 0xFFFFFFFF;
         int64_t high_word = hi1 * hi2;
         high_word += (lo1 * hi2)>>32;
         high_word += (lo2 * hi1)>>32;
         result.type = Int64;
         result.data.lconst = high_word;
         break;
      }
      case TR::lbits2d:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         result.type = Double;
         std::memcpy(&result.data.dconst, &operand1.data.lconst, 8);
         break;
      }
      case TR::lcmpset:
      {
         int64_t * pointer = (int64_t *)&nodeValueMap[node->getChild(0)->getGlobalIndex()].data.lconst;
         operand1 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(2)->getGlobalIndex()];
         result.type = Int64;
         if (*pointer == operand1.data.lconst){
            *pointer = operand2.data.lconst;
            result.data.lconst = 0;
         }
         else{
            result.data.lconst = 1;
         }
         break;
      }
      case TR::lexp:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.type = Int64;
         result.data.lconst = pow(operand1.data.lconst,operand2.data.lconst);
         break;
      }
      case TR::lmax:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.data.lconst = std::max(operand1.data.lconst, operand2.data.lconst);
         break;
      }
      case TR::lmin:
      {
         operand1 = nodeValueMap[node->getChild(0)->getGlobalIndex()];
         operand2 = nodeValueMap[node->getChild(1)->getGlobalIndex()];
         result.data.lconst = std::min(operand1.data.lconst, operand2.data.lconst);
         break;
      }
      default:
      {
         TR_ASSERT_FATAL(false, "Unexpected opcode %s:%d for n%dn [%p]\n", node->getOpCode().getName(), node->getOpCodeValue(), node->getGlobalIndex(), node);
      }
   }
   if (node->getReferenceCount() != 0){
      nodeValueMap[node->getGlobalIndex()] = result;
   }
   return;
}
