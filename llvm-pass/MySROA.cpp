#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

namespace {

struct MySROA : public FunctionPass {
    static char ID;
    MySROA() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
        bool changed = false;

        errs() << "Running struct decomposition on function: " << F.getName() << "\n";

        for (auto &B : F) {
            for (auto it = B.begin(); it != B.end();) {
                Instruction *I = &*it++;
                if (auto *A = dyn_cast<AllocaInst>(I)) {
                    Type *T = A->getAllocatedType();

                    if (auto *S = dyn_cast<StructType>(T)) {
                        errs() << "  Decomposing struct: ";
                        S->print(errs());
                        errs() << "\n";

                        IRBuilder<> Builder(A);
                        for (unsigned i = 0; i < S->getNumElements(); ++i) {
                            Type *ElemType = S->getElementType(i);
                            StringRef name = A->getName();
                            AllocaInst *NewAlloca =
                                Builder.CreateAlloca(ElemType, nullptr,
                                                      name + "_field" + std::to_string(i));
                            errs() << "    Created alloca: " << NewAlloca->getName() << "\n";
                        }
                        changed = true;
                    }
                }
            }
        }

        return changed;
    }


};

}

char MySROA::ID = 0;
static RegisterPass<MySROA> X("mysroa", "Simple SROA");
