# MySROA – Custom LLVM Scalar Replacement of Aggregates Pass

### Overview

**MySROA** is a **simple, custom LLVM optimization pass** that demonstrates the idea of  
**Scalar Replacement of Aggregates (SROA)** on **basic examples and simple structures**.

---

### Features

- **SROA decomposition:**  
  Breaks down aggregate types (e.g., structs) into individual scalar `allocas`.

- **Instruction rewrite:**  
  Replaces `load` and `store` instructions that access aggregates with equivalent scalar operations.

- **Automatic mem2reg promotion:**  
  After decomposition, the pass invokes LLVM’s built-in `mem2reg` to promote scalar variables to SSA form.

- **Optimization statistics:**  
  Displays counts of loads, stores, and allocas before and after optimization, along with total instruction count.

- **Legacy pass manager support:**  
  Implemented using LLVM’s **legacy pass manager** for compatibility with LLVM 14 and the `opt-14` tool.

---

**Explanation for implementation** – In Serbian.  
📘 [View implementation explanation (Serbian)](link-ka-objasnjenju)

---
### Build Instructions

To build the project, make sure **LLVM 14** and **CMake** are installed.  
Then follow these steps:


```bash
mkdir build && cd build
cmake ..
make
```
---
###  Testing

All test cases are executed automatically using the provided **test.sh** script:

```bash
cd test
./test.sh
```
If you want to test the pass manually (outside the provided `/test` examples),  
you can compile any C file into LLVM IR and then run the pass using `clang` and `opt`.

```bash
clang-14 -O0 -Xclang -disable-O0-optnone -emit-llvm -S -g -fno-discard-value-names your_test.c -o your_test.ll

opt-14 -enable-new-pm=0 -load ../llvm-pass/build/libMySROA.so -mysroa -S your_test.ll -o your_test_out.ll
```
