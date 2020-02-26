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

#include "codegen/CodeGenerator.hpp"
#include "compile/CompilationTypes.hpp"
#include "compile/Method.hpp"
#include "control/CompileMethod.hpp"
#include "env/CompilerEnv.hpp"
#include "env/FrontEnd.hpp"
#include "env/IO.hpp"
#include "env/RawAllocator.hpp"
#include "ilgen/IlGeneratorMethodDetails_inlines.hpp"
#include "ilgen/MethodBuilder.hpp"
#include "ilgen/TypeDictionary.hpp"
#include "runtime/CodeCache.hpp"
#include "runtime/Runtime.hpp"
#include "runtime/JBJitConfig.hpp"

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

   Store("c",
      Add(
         Load("a"),
         Load("b")));

   Store("d",
      Mul(
         Load("a"),
         Load("c")));
   
   Return(
      Load("d"));

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

   printf("Step 3: compile method builder\n");
   AddMethod AddMethod(&types);
   void *entry=0;
   int32_t rc = compileMethodBuilder(&AddMethod, &entry);
   // if (rc != 0)
   //    {
   //    fprintf(stderr,"FAIL: compilation error %d\n", rc);
   //    exit(-2);
   //    }
   // printf("Step 4: invoke compiled code\n");
   // AddFunctionType *addMethod = (AddFunctionType *)entry;
   // int64_t r = addMethod();

   int32_t rc = interpretMethodBuilder(&AddMethod, &entry);

   // printf("7 + 2 is %ld\n", r);

   // printf ("Step 5: shutdown JIT\n");
   // shutdownJit();

   // printf("PASS\n");
   }

int32_t interpretMethodBuilder(OMR::JitBuilder::MethodBuilder * methodBuilder, void ** entryPoint)
{
   
   TR::MethodBuilder *m = static_cast<TR::MethodBuilder *>(static_cast<TR::IlBuilder *>(methodBuilder != NULL ? methodBuilder->_impl : NULL))

   TR::ResolvedMethod resolvedMethod(static_cast<TR::MethodBuilder *>(this));
   TR::IlGeneratorMethodDetails details(&resolvedMethod);

   int32_t rc=0;
   *entryPoint = (void *) compileMethodFromDetails(NULL, details, noOpt, rc);
   // typeDictionary()->NotifyCompilationDone();
   return rc;
}


// int32_t compileMethodBuilder(OMR::JitBuilder::MethodBuilder * methodBuilder, void ** entryPoint)
// {
//     auto ret = internal_compileMethodBuilder(static_cast<TR::MethodBuilder *>(static_cast<TR::IlBuilder *>(methodBuilder != NULL ? methodBuilder->_impl : NULL)), entryPoint);
//     return ret;
// }

// internal_compileMethodBuilder(TR::MethodBuilder *m, void **entry)
// {
//    auto rc = m->Compile(entry);
//    return rc;
// }

// int32_t
// OMR::MethodBuilder::Compile(void **entry)
// {
//    TR::ResolvedMethod resolvedMethod(static_cast<TR::MethodBuilder *>(this));
//    TR::IlGeneratorMethodDetails details(&resolvedMethod);

//    int32_t rc=0;
//    *entry = (void *) compileMethodFromDetails(NULL, details, warm, rc);
//    typeDictionary()->NotifyCompilationDone();
//    return rc;
// }