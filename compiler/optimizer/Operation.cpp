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

#include "optimizer/Operation.hpp"

void * TR::Operation::performOp(void **children){
    
}

class iAddOperation : public TR::Operation {
    public:

    void * performOp(void **children) {
       int32_t * result = (int32_t*)malloc(sizeof(int32_t));
       *result = *(int32_t*)children[0] + *(int32_t*)children[1];
      return result;
    }
};

class lAddOperation : public TR::Operation {
    public:

    void * performOp(void **children) {
       int64_t * result = (int64_t*)malloc(sizeof(int64_t));
       *result = *(int64_t*)children[0] + *(int64_t*)children[1];
      return result;
    }
};

class iStoreOperation : public TR::Operation {
    public:

    void * performOp(void **children) {
         TR::Node * result;///////////////////////////// fix this
         return result;
      }
};

class lStoreOperation : public TR::Operation {
    public:

    void * performOp(void **children) {
         TR::Node * result;///////////////////////////// fix this
         return result;
      }
};

class iLoadOperation : public TR::Operation {
    public:
    
    void * performOp(void **children){
      TR::Node * result; ///////////////////////////// fix this
      return result;
    }
};

class lLoadOperation : public TR::Operation {
    public:
    
    void * performOp(void **children){
      TR::Node * result;///////////////////////////// fix this
      return result;
    }
};

class iConstOperation : public TR::Operation {
   public:

   void * performOp(void **children) {
      return children[0];
   }
};

class lConstOperation : public TR::Operation {
   public:

   lConstOperation(int64_t val){
      this->value = malloc(sizeof(int64_t));
      memcpy(this->value, (const void *)&val, sizeof(int64_t));
   }

   void * performOp(void **children) {
      return this->value;
   }
};

class iReturnOperation : public TR::Operation {
   public:

   void * performOp(void **children) {
      return children[0];///////////////////////////// fix this
   }
};

class lReturnOperation : public TR::Operation {
   public:

   void * performOp(void **children) {
      return children[0];///////////////////////////// fix this
   }
};

TR::Operation * TR::GetOperation(TR::Node * node){

   switch (node->getOpCodeValue()){
      case TR::iadd:
         return new iAddOperation;
      case TR::ladd:
         return new lAddOperation;
      case TR::istore:
         return new iStoreOperation;
      case TR::lstore:
         return new lStoreOperation;
      case TR::iload:
         return new iLoadOperation;
      case TR::lload:
         return new lLoadOperation;
      case TR::iconst:
         return new iConstOperation;
      case TR::lconst:
         return new lConstOperation(node->getLongInt());
      case TR::ireturn:
         return new iReturnOperation;
      case TR::lreturn:
         return new lReturnOperation;
      case TR::BBStart:
         return new Operation;
      case TR::BBEnd:
         return new Operation;
      case TR::treetop:
         return new Operation;
      default:
         throw std::runtime_error("Opcode unrecognized");
   }   
}