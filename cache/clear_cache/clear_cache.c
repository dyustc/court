// Copyright (c) 2010 ARM Ltd
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>

#include "clear_cache.h"

// ----------------------------------------------------------------
// The first test function. The emitted code will start off with a branch to
// this function.
void f1(void)
{
    printf("Function 1 (old).\n");
}

// ----------------------------------------------------------------
// The second test function. The emitted code will be modified to call this
// function.
void f2(void)
{
    printf("Function 2 (new).\n");
}

// ----------------------------------------------------------------
// A simple function to append an instruction to a buffer.
void emit(uint32_t ** code, uint32_t ins)
{
    **code = ins;
    (*code)++;
}

// ----------------------------------------------------------------
// Entry point.
int main(void)
{
    // --------------------------------
    // Uninteresting setup code: Get some writable, executable memory and point
    // a C function pointer at it.

    // A function pointer to our test code, so that the C compiler knows how to
    // call it.
    test_func_t     test_func;

    // A pointer to some code that we will patch.
    uint32_t *  patch;

    // A useful swap pointer.
    uint32_t *  caret;

    // We need some memory for our run-time-generated function. Significantly,
    // it needs to have at least write+execute rights!
    uint32_t *  code = mmap(
            NULL,
            16 * 4,  // Space for 16 instructions. (More than enough.)
            PROT_READ | PROT_WRITE | PROT_EXEC,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1,
            0);
    if (code == MAP_FAILED) {
        printf("Could not mmap a memory buffer with the proper permissions.\n");
        return -1;
    }

    // Set the test_func function pointer to point to the writable code block
    // we just created.
    test_func = (test_func_t) code;

    // --------------------------------
    // Initialize test_func with some simple code.

    // Write some code into test_func.
    caret = code;
    emit(&caret, PUSH(rmask(4) | rmask(lr)));
    patch = caret;  // Store the location of the code that we want to patch.
#ifndef ARMv5_COMPATIBLE
    emit(&caret, MOVW(ip, (uint32_t)f1));
    emit(&caret, MOVT(ip, (uint32_t)f1));
#else
    // Legacy architecture support. Note that this sequence isn't terribly good
    // code. A load from a literal pool would be better, but that is more
    // complicated to set up for a trivial example like this.
    emit(&caret, MOV(ip, OP2_BYTE0 | (((uint32_t)f1 >>  0) & 0xff)));
    emit(&caret, ORR(ip, OP2_BYTE1 | (((uint32_t)f1 >>  8) & 0xff)));
    emit(&caret, ORR(ip, OP2_BYTE2 | (((uint32_t)f1 >> 16) & 0xff)));
    emit(&caret, ORR(ip, OP2_BYTE3 | (((uint32_t)f1 >> 24) & 0xff)));
#endif
    emit(&caret, BLX(ip));
    emit(&caret, POP(rmask(4) | rmask(pc)));
    // Synchronize the cache to ensure that we get the initial state correct.
    __clear_cache((char*)code, (char*)caret);

    // Call the test function. This should in turn call f1.
    test_func();

    // --------------------------------
    // Demonstrate the Harvard caches.

    // Re-write the code to branch to Function 2. This affects the D-cache
    // only. It may then propagate into the unified memory outside the D-cache
    // and then back into the I-cache, but there's no practical way to test
    // this.
    caret = patch;  // Return to the patch location.
#ifndef ARMv5_COMPATIBLE
    emit(&caret, MOVW(ip, (uint32_t)f2));
    emit(&caret, MOVT(ip, (uint32_t)f2));
#else
    // Legacy architecture support.
    emit(&caret, MOV(ip, OP2_BYTE0 | (((uint32_t)f2 >>  0) & 0xff)));
    emit(&caret, ORR(ip, OP2_BYTE1 | (((uint32_t)f2 >>  8) & 0xff)));
    emit(&caret, ORR(ip, OP2_BYTE2 | (((uint32_t)f2 >> 16) & 0xff)));
    emit(&caret, ORR(ip, OP2_BYTE3 | (((uint32_t)f2 >> 24) & 0xff)));
#endif

    // Call the test function. This may call either f1 or f2, or may just crash
    // if one of the MOVW and MOVT instructions is propagated to the I-cache
    // but the other is not. A crash is unlikely in this particular test, as
    // mmap probably returns page-aligned addresses and so the patched
    // instructions will be in the same cache line.
    test_func();

    // --------------------------------
    // Synchronize the caches manually and try again.

    // Ensure that the I-cache includes the new code, branching to f2.
    __clear_cache((char*)patch, (char*)caret);

    // Call the test function again. This should now call f2.
    test_func();

    return 0;
}

