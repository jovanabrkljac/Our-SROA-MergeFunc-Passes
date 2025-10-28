# MySROA & MyMergeFunc – Custom LLVM Optimization Passes

### Overview

This project implements two **custom LLVM optimization passes** for **basic and simplified cases**:  
- **MySROA** – *Scalar Replacement of Aggregates*  
- **MyMergeFunc** – *Function Merging using Structural Hashing*  

Developed as a **study project**, these passes demonstrate how to build and combine simple LLVM optimizations at the IR level using the **legacy pass manager (LLVM 14)**.  
The implementations are designed to work on **fundamental examples and basic structures**, illustrating the core concepts rather than handling all edge cases.

---

### Features

#### MySROA (Scalar Replacement of Aggregates)
- **Struct decomposition:**  
  Splits aggregate types (like `structs`) into individual scalar `allocas`.

- **Instruction rewrite:**  
  Replaces `load` and `store` operations on aggregates with equivalent scalar operations.

- **Automatic mem2reg:**  
  Runs LLVM’s built-in **mem2reg** pass to promote variables to SSA form.

- **Statistics:**  
  Displays counts of loads, stores, and allocas before and after optimization, along with total instruction count.

#### MyMergeFunc (Function Merging)
- **Function hashing:**  
  Generates structural hashes for functions to detect duplicates or similar function bodies.

- **Function comparison:**  
  Compares instruction-level structure and operands between candidate functions.

- **Automatic merging:**  
  Replaces identical or structurally equivalent function bodies with a single representative function.

- **Statistics:**  
  Reports the number of merged functions, total instruction reduction, and module verification status.

---

**Explanation for implementation** – In Serbian.  
📘 [View implementation explanation (Serbian)](link-ka-objasnjenju)

---

###  Testing

All test cases (for MySROA, MyMergeFunc, and combined) are executed automatically using the provided test.sh script:

```bash
cd test
./test.sh
```
If you want to test the passes manually (outside the provided /test examples),
you can compile any C file into LLVM IR and then run the passes using clang and opt.


