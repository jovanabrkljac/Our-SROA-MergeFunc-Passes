#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Type.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/PromoteMemToReg.h"
#include "llvm/IR/Dominators.h" 
#include <map>

using namespace llvm;

namespace {

struct MySROA : public FunctionPass {
    static char ID;
    MySROA() : FunctionPass(ID) {}

    bool runOnFunction(Function &F) override {
    bool changed = false;

    errs() << "=== Running MySROA + mem2reg on function: " << F.getName() << " ===\n";
    
     unsigned LoadsBefore = 0, StoresBefore = 0, AllocasBefore = 0, InstBefore = 0;
        for (auto &B : F) {
            for (auto &I : B) {
                InstBefore++;
                if (isa<LoadInst>(&I)) LoadsBefore++;
                if (isa<StoreInst>(&I)) StoresBefore++;
                if (isa<AllocaInst>(&I)) AllocasBefore++;
            }
        }
    
    errs() << "SROA decomposition phase...\n";

    for (auto &B : F) {
        for (auto it = B.begin(); it != B.end();) {
            Instruction *I = &*it++;
            auto *A = dyn_cast<AllocaInst>(I);
            if (!A)
                continue;

            auto *S = dyn_cast<StructType>(A->getAllocatedType());
            if (!S)
                continue;

            IRBuilder<> Builder(A);
            StringRef base = A->getName();
            if (base.empty())
                base = "var";

            std::map<unsigned, AllocaInst*> FieldAllocas;
            for (unsigned i = 0; i < S->getNumElements(); ++i) {
                Type *ElemTy = S->getElementType(i);
                auto *NewA = Builder.CreateAlloca(ElemTy, nullptr,
                    base + "_field" + std::to_string(i));
                FieldAllocas[i] = NewA;
                errs() << "  Created alloca: " << NewA->getName() << "\n";
            }

           
            std::vector<GetElementPtrInst*> GEPs;
            for (auto *U : A->users())
                if (auto *G = dyn_cast<GetElementPtrInst>(U))
                    GEPs.push_back(G);

            for (auto *GEP : GEPs) {
                if (GEP->getNumIndices() != 2)
                    continue;

                auto *CI = dyn_cast<ConstantInt>(GEP->getOperand(2));
                if (!CI)
                    continue;

                unsigned FieldIndex = CI->getZExtValue();
                auto *FieldAlloca = FieldAllocas[FieldIndex];
                if (!FieldAlloca)
                    continue;

                
                for (auto *GU : llvm::make_early_inc_range(GEP->users())) {
                    if (auto *L = dyn_cast<LoadInst>(GU)) {
                        L->setOperand(0, FieldAlloca);
                        errs() << "    Replaced load for field "
                               << FieldIndex << " -> " << FieldAlloca->getName() << "\n";
                        changed = true;
                    } else if (auto *S = dyn_cast<StoreInst>(GU)) {
                        S->setOperand(1, FieldAlloca);
                        errs() << "    Replaced store for field "
                               << FieldIndex << " -> " << FieldAlloca->getName() << "\n";
                        changed = true;
                    }
                }

                if (GEP->use_empty()) {
                    GEP->eraseFromParent();
                    errs() << "    Removed GEP for field " << FieldIndex << "\n";
                }
            }

            if (A->use_empty()) {
                A->eraseFromParent();
                errs() << "  Removed original alloca " << base << "\n";
            }
        }
    }

    errs() << "SROA Cleanup phase...\n";

    unsigned RemovedAllocas = 0;
    unsigned Scalarized = 0;

    std::vector<AllocaInst*> ToRemove;
    for (auto &B : F) {
        for (auto &I : B) {
            if (auto *A = dyn_cast<AllocaInst>(&I)) {
                if (A->getName().contains("_field") && A->use_empty()) {
                    ToRemove.push_back(A);
                } else if (A->getName().contains("_field")) {
                    ++Scalarized;
                }
            }
        }
    }

    for (auto *A : ToRemove) {
        A->eraseFromParent();
        ++RemovedAllocas;
    }

    if (!ToRemove.empty())
        changed = true;

    if (verifyFunction(F, &errs())) {
        errs() << "Verification failed for function: " << F.getName() << "\n";
    } else {
        errs() << "Function verified successfully: " << F.getName() << "\n";
    }

    errs() << "Cleanup summary for " << F.getName() << ":\n";
    errs() << "  Scalarized allocas: " << Scalarized << "\n";
    errs() << "  Removed unused _field allocas: " << RemovedAllocas << "\n";

     errs() << "\n--- Running built-in mem2reg ---\n";

        DominatorTree DT(F);
        std::vector<AllocaInst*> Allocas;
        for (auto &B : F) {
            for (auto &I : B)
                if (auto *A = dyn_cast<AllocaInst>(&I))
                    Allocas.push_back(A);
        }

        if (!Allocas.empty()) {
            PromoteMemToReg(Allocas, DT);
            changed = true;
            errs() << "mem2reg promoted " << Allocas.size() << " allocas\n";
        } else {
            errs() << "No allocas found for mem2reg.\n";
        }
        
        unsigned LoadsAfter = 0, StoresAfter = 0, AllocasAfter = 0, InstAfter = 0;
        for (auto &B : F) {
            for (auto &I : B) {
                InstAfter++;
                if (isa<LoadInst>(&I)) LoadsAfter++;
                if (isa<StoreInst>(&I)) StoresAfter++;
                if (isa<AllocaInst>(&I)) AllocasAfter++;
            }
        }

        errs() << "\n=== Optimization Summary for " << F.getName() << " ===\n";
        errs() << "Loads before:  " << LoadsBefore << " → after: " << LoadsAfter
               << "  (removed: " << (int)(LoadsBefore - LoadsAfter) << ")\n";
        errs() << "Stores before: " << StoresBefore << " → after: " << StoresAfter
               << "  (removed: " << (int)(StoresBefore - StoresAfter) << ")\n";
        errs() << "Allocas before:" << AllocasBefore << " → after: " << AllocasAfter
               << "  (removed: " << (int)(AllocasBefore - AllocasAfter) << ")\n";
        errs() << "Instructions:  " << InstBefore << " → after: " << InstAfter
               << "  (removed: " << (int)(InstBefore - InstAfter) << ")\n";



    return changed;
}


};

}

char MySROA::ID = 0;
static RegisterPass<MySROA> X("mysroa", "Simple SROA");
