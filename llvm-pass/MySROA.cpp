#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

struct MySROA : public FunctionPass {
    static char ID;
    MySROA() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
        errs() << "Analyzing function: " << F.getName() << "\n";

        for (auto &B : F)
            for (auto &I : B)

                if (auto *A = dyn_cast<AllocaInst>(&I)) {
                    Type *T = A->getAllocatedType();

                    if (auto *S = dyn_cast<StructType>(T)) {
                        errs() << "  Found struct allocation: ";
                        S->print(errs());
                        errs() << "\n";

                    } else if (auto *Arr = dyn_cast<ArrayType>(T)) {
                        errs() << "  Found array allocation of element type: ";
                        Arr->getElementType()->print(errs());
                        errs() << " [" << Arr->getNumElements() << " elements]\n";
                    }
                }

        return false;
    }

};

}

char MySROA::ID = 0;
static RegisterPass<MySROA> X("mysroa", "Simple SROA detection");
