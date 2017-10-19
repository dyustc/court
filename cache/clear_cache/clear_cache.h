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

// This is a utility header for the clear_cache example. The code in here
// exists purely to simplify the clear_cache.c implementation by abstracting
// away unimportant details.

// Define some macros to wrap the encoding of the instruction we emit.
// For simplicity's sake, there are no assertions or error checking here;
// passing in inappropriate values will result in invalid instructions.

// PUSH(rmask) emits this: "PUSH {<rmask>}"
#define PUSH(rmask) (0xe92d0000 | (rmask))
// POP(rmask) emits this: "POP {<rmask>}"
#define POP(rmask)  (0xe8bd0000 | (rmask))
// MOVW(rd,x) emits this: "MOVW rd, #:lower16:x"
#define MOVW(rd,x)  (0xe3000000 | (((x)<<4)&0xf0000) | ((rd)<<12) | ((x)&0xfff))
// MOVT(rd,x) emits this: "MOVT rd, #:upper16:x"
#define MOVT(rd,x)  (0xe3400000 | (((x)>>12)&0xf0000) | ((rd)<<12) | ((x)>>16))
// BLX(rm) emits this: "BLX rm"
#define BLX(rm)     (0xe12fff30 | (rm))

// ------------------------

// The following instruction macros are provided for compatibility with legacy
// architectures. Note that these all hard-code part of the operand 2 immediate
// field. The caller specifies only the 8-bit immediate.

// These fields set the 'rotation' field of an operand 2 immediate. Refer to
// the ARMv7-AR ARM for a description of the possible values for this field.
#define OP2_BYTE0   (0x0 << 8)
#define OP2_BYTE1   (0xc << 8)
#define OP2_BYTE2   (0x8 << 8)
#define OP2_BYTE3   (0x4 << 8)

// MOV(rd,x) emits this: "MOV rd, #(imm)"
//      ... where "imm" is derived from "x", and "x" is an operand 2 immediate
//      encoded in 12 bits. Refer to the ARMv7-AR ARM for a description of the
//      possible values for this field.
#define MOV(rd,x)   (0xe3a00000 | ((x)&0xfff) | ((rd)<<12))

// ORR(rd,x) emits this: "ORR rd, rd, #(imm)"
#define ORR(rd,x)   (0xe3800000 | ((x)&0xfff) | ((rd)<<12) | ((rd)<<16))

// ------------------------

// Register numbers.
unsigned int const  ip = 12;
unsigned int const  lr = 14;
unsigned int const  pc = 15;

// Turn a register number into an ORable mask for PUSH or POP.
#define rmask(r)    (1<<(r))

// Declare the built-in __clear_cache function.
extern void __clear_cache (char*, char*);

// Define a type for our test function, to avoid the nasty C syntax for
// function pointer casts.
typedef void (*test_func_t) (void);

