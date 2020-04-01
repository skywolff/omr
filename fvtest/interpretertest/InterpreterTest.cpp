/*******************************************************************************
 * Copyright (c) 2017, 2019 IBM Corp. and others
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

// #include "OpCodeTest.hpp"
#include "default_compiler.hpp"
#include "omrformatconsts.h"

#include <cmath>

#if defined(J9ZOS390) || defined(AIXPPC)
namespace std
{
   using ::isnan;
}
#endif

template <typename T>
T add(T l, T r) {
    return l + r;
}

template <typename T>
T sub(T l, T r) {
    return l - r;
}

template <typename T>
T mul(T l, T r) {
    return l * r;
}

template <typename T>
T div(T l, T r) {
    return l / r;
}

template <typename T>
T udiv(T l, T r) {
    return l / r;
}

template <typename T>
T rem(T l, T r) {
    return l % r;
}

template <typename T>
T urem(T l, T r) {
    return l % r;
}

// int64_t lmulh(int64_t l, int64_t r) {
//     int128_t x = static_cast<int128_t>(l) * static_cast<int128_t>(r);
//     return static_cast<int64_t>(x >> 64); // upper 64 bits
// }

class Int64Arithmetic : public TRTest::BinaryOpTest<int64_t> {};

TEST_P(Int64Arithmetic, UsingConst) {
    char inputTrees[1024] = {0};
    std::snprintf(inputTrees, sizeof(inputTrees),
      "(method return=Int64"
      "  (block"
      "    (lreturn"
      "      (%s"
      "        (lconst %ld)"
      "        (lconst %ld)))))",
      "ladd",
      7,
      2
      );
    auto trees = parseString(inputTrees);

    ASSERT_NOTNULL(trees);

    Tril::DefaultCompiler compiler(trees);
    
    // Hardcoded 2 + 7 = 9
    ASSERT_EQ(9, compiler.interpret()) << "Interpretation failed unexpectedly\n" << "Input trees: " << inputTrees;
}