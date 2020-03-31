/*******************************************************************************
 * Copyright (c) 2016, 2018 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 * or the Apache License, Version 2.0 which accompanies this distribution and
 * is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following
 * Secondary Licenses when the conditions for such availability set
 * forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 * General Public License, version 2 with the GNU Classpath
 * Exception [1] and GNU General Public License, version 2 with the
 * OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include "AddMethod.hpp"

AddMethod::AddMethod(OMR::JitBuilder::TypeDictionary *types)
   : OMR::JitBuilder::MethodBuilder(types)
   {
   DefineLine(LINETOSTR(__LINE__));
   DefineFile(__FILE__);

   DefineName("addMethod");
   DefineReturnType(Int64);
   }

bool
AddMethod::buildIL()
   {
   Store("a",
      ConstInt64(7));

   Store("b",
      ConstInt64(2));

   Return(
      Add(
         Load("a"),
         Load("b")));

   return true;
   }


int
main(int argc, char *argv[])
   {
   printf("Step 1: initialize JIT\n");
   bool initialized = initializeJit();
   if (!initialized)
      {
      fprintf(stderr, "FAIL: could not initialize JIT\n");
      exit(-1);
      }

   printf("Step 2: define relevant types\n");
   OMR::JitBuilder::TypeDictionary types;

   // printf("Step 3: compile method builder\n");
   AddMethod AddMethod(&types);
   void *entry=0;
   // printf("interpreting...\n");
   int64_t result = interpretMethodBuilder(&AddMethod);
   printf("interpretation finished with a value %d\n", result);
   // int32_t rc = compileMethodBuilder(&AddMethod, &entry);
   // if (rc != 0)
      // {
      // fprintf(stderr,"FAIL: compilation error %d\n", rc);
      // exit(-2);
      // }

   // printf("Step 4: invoke compiled code\n");
   // AddFunctionType *addMethod = (AddFunctionType *)entry;
   // int64_t r = addMethod();

   // printf("7 + 2 is %ld\n", r);
   // printf ("Step 5: shutdown JIT\n");
   // shutdownJit();
   // printf("PASS\n");
   }
