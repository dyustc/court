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

This bundle includes a Linux demonstration program which performs the
following:

 - Allocate a writable, executable memory block so we can emit some code and
   then run it.
 - Dump a simple bit of code into the new memory block. The code calls function
   "f1", which prints something like "Function 1" to stdout (so you can see
   what's being run).
 - The test function is run to prove that it works.
 - Modify the test code to call function "f2", which prints something like
   "Function 2" to stdout.
 - Without synchronizing the caches, run the test function again.
 - Finally, synchronize the caches (using __clear_cache) and run the test
   function once more.

The output should look like this:

Function 1 (old).
Function 1 (old).
Function 2 (new).

It's possible that you may see "Function 2" twice; if this happens, it is
because the I-cache and D-cache have synchronized automatically. It is
impractical to determine when this happens, so unfortunately it is not possible
to make this demonstration entirely foolproof, but in this particular case it
should be unlikely and you should see the expected output.

It's also possible (though unlikely) that the caches will be synchronized
automatically whilst the code is being modified. In that case, the program will
probably crash as it'll be branching to an incomplete address.

----

To build the example using GCC, simply run the "build.sh" script.

----

In case you don't have an ARMv7 platform that can run MOVW and MOVT
instructions, I've provided the ability for the program to emit legacy
instructions. The build script automatically produces a binary named
'clear_cache_armv5' for this purpose. (Note that this will also work on an
ARMv7 platform.)

----

Note that the example uses the BLX instruction, which is only available from
ARMv5T onwards.

