#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/BasicBlock.h"
#include <iostream>
#include <vector>
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include <unordered_map>
#include <algorithm>
#include "llvm/ADT/Hashing.h"



using namespace llvm;

namespace {
  struct MergeFuncPass : public ModulePass {
    static char ID;
    MergeFuncPass() : ModulePass(ID) {}
    std::unordered_map<Value*, Value*> VariablesMap;


    bool isConstant(Value* val){
        return isa<ConstantInt>(val);
    }
    int constValue(Value* val){
        ConstantInt *ConstInt = dyn_cast<ConstantInt>(val);
        return ConstInt->getSExtValue();
    }
    bool eqOperands(Value* Op1, Value* Op2){
        if(VariablesMap.count(Op1)){ // Ako u mapi imamo nas levi operand ali se on ne slaze sa levim operandom iz druge funkcije
                                                    // To znaci da nisu iste instrukcije
            if (VariablesMap[Op1] != Op2)
                    return false;
            }
        else if ((isConstant(Op1) && !isConstant(Op2)) || 
             (isConstant(Op2) && !isConstant(Op1)) || 
             (isConstant(Op1) && isConstant(Op2) && constValue(Op1) != constValue(Op2))) return false;
        
        return true;
    }

    //Pravimo strukturalni hash koji opisuje kakva je funkcija iznutra
    //Ideja je da  ako dve funkcije imaju istu strukturu i instrukcije, njihov hash bude isti
    // hash combine radi ovo:
    //hash = hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
    //U sustini cini da redosled ima VELIKI UTICAJ sto je nama potrebno u nasem slucaju jer je funkcija
    //Strukturno ista ako je svaka instrukcija redom ista, bitan je redosled!
    //FUNKCIJE SA RAZLICITIM HASH SIGURNO NISU ISTE
    //FUNKCIJE SA ISTIM HASHOM TREBA PROVERITI -> znacajno manje provera

    //Medjutim sta se desi zbog toga sto je i redosled operanada bitan, ovde gubimo svojstvo komutativnosti, tako da mora malo da se promeni
    //Nacin rada za komutativne binarne instrukcije

    //CFG hash OPISUJE I TOK KONTROLE A NE SAMO INSTRUKCIJE
    size_t hashFunction(Function &F){
        size_t H = hash_value(F.getFunctionType()); //pocetna vrednost zavisi od povratne vr, broja i tipova argumenata
        H = hash_combine(H,F.size());
        for(auto &BB:F){
            H = hash_combine(H,BB.size());
           /* if (const TerminatorInst * T = BB.getTerminator()) {
            H = hash_combine(H, T->getOpcode());     
            H = hash_combine(H, T->getNumSuccessors()); 
        }*/
            for(auto &I:BB){
                H = hash_combine(H,I.getOpcode()); // U zavisnosti od tipa iinstrukcije kombinujemo hash

                if(BinaryOperator *BinaryOp = dyn_cast<BinaryOperator>(&I)){
                        if(BinaryOp->isCommutative()){ //Ova komutaticnost radi za x+1 i 1+x, i dve konst ali ne i x+y i y+x
                                                        // Zbog problema sa store koji je previse dubok da se njime bavim u obliku ove optimizacije
                                                        //Cija nije svrha manipulisanje tim stvarima
                            Value* L = I.getOperand(0);
                            Value* R = I.getOperand(1);
                            std::vector <size_t> helperVec;
                            size_t leftH  = isConstant(L) ? hash_value(constValue(L)) : hash_value(L->getType()->getTypeID());
                            size_t rightH = isConstant(R) ? hash_value(constValue(R)) : hash_value(R->getType()->getTypeID());
                            helperVec = {leftH,rightH};
                            std::sort(helperVec.begin(), helperVec.end());
                            for (auto h : helperVec){
                                H = hash_combine(H,h);
                            }
                            continue;
                        }
                    }


                for (unsigned i=0; i< I.getNumOperands(); i++){
                    Value* Op = I.getOperand(i);
                    if(isConstant(Op)){
                        H = hash_combine(H,constValue(Op)); // Ako je konstanta operand bukv ce je dodati na hash tako da x+1 i x+2 nece biti ista stvar
                    }else{
                        H = hash_combine(H,Op->getType()->getTypeID()); //Ako nije onda samo pravi bucket sa tipom operanda
                    }

                }
            }
        }
        return H;
    }


    
    bool sameReturnValue(Function* F1, Function *F2){ //getFunctionType vraca false ako nemaju istu povratnu vrednost, isti br argumenata
                                                      // i iste tipove argumenata
        if(F1->getFunctionType() == F2->getFunctionType())
            return true;
        return false;
    }

    bool sameFunctionBody(Function *F1, Function *F2){
        if (!sameReturnValue(F1,F2)) return false;
        VariablesMap.clear();
        std::vector<BasicBlock*> BBFunc1;
        std::vector <BasicBlock*> BBFunc2;
        if(F1->size() != F2->size()) return false; //Broj Basic Blokova u funkcijama treba da bude isti
        auto arg1 = F1->arg_begin(); // iterira kroz argumente funkcije 1
        auto arg2 = F2->arg_begin(); // iterira kroz argumente funkcije 2
        for(; arg1 != F1->arg_end(); arg1++, arg2++){
            VariablesMap[&*arg1] = &*arg2; // mapiramo prvi argument F1 na prvi argument F2 itd..
                                           // Na primer ako je funkcija F1(int x, int y) i F2(int a, int b)
                                           // onda cemo u mapi cuvati X->a i y->b
        }
        for (BasicBlock& BB:*F1){
            BBFunc1.push_back(&BB);
        }
        for (BasicBlock& BB:*F2){
            BBFunc2.push_back(&BB);
        }
        for (int i=0;i<F1->size();i++){
            if (BBFunc1[i]->size() != BBFunc2[i]->size()) return false; //Broj instrukcija unutar istog Basic Blocka mora biti isti
            auto it1 = BBFunc1[i]->begin();
            auto it2 = BBFunc2[i]->begin();
            for(; it1 != BBFunc1[i]->end();it1++,it2++){
                if(it1->getOpcode() != it2->getOpcode()) { //getOpcode vraca ID operacije tako da iste instrukcije moraju imati isti ID
                                                           // Npr add ima svoj ID, mul ima svoj ID itd..
                    return false;
                }   
                    if(BinaryOperator *BinaryOp = dyn_cast<BinaryOperator>(&*it1)){
                        if(BinaryOp->isCommutative()){ //Ova komutaticnost radi za x+1 i 1+x, i dve konst ali ne i x+y i y+x
                                                        // Zbog problema sa store koji je previse dubok da se njime bavim u obliku ove optimizacije
                                                        //Cija nije svrha manipulisanje tim stvarima
                            Value* L1 = it1->getOperand(0);
                            Value* R1 = it1->getOperand(1);
                            Value* L2 = it2->getOperand(0);
                            Value* R2 = it2->getOperand(1);

                            bool direct = eqOperands(L1,L2) && eqOperands(R1,R2);
                            bool swapped = eqOperands(L1,R2) && eqOperands(L2,R1);
                            if(!direct && !swapped) return false; //Ako nije ni isto normalno ni komutativno onda vrati false 
                        }
                        else{
                            Value* L1 = it1->getOperand(0);
                            Value* R1 = it1->getOperand(1);
                            Value* L2 = it2->getOperand(0);
                            Value* R2 = it2->getOperand(1);
                            bool direct = eqOperands(L1,L2) && eqOperands(R1,R2);
                            if(!direct) return false;
                        }
                    }else{
                        for(int i=0; i< it1->getNumOperands(); i++){
                            Value* Op1 = it1->getOperand(i);
                            Value* Op2 = it2->getOperand(i);
                            if(!eqOperands(Op1,Op2)) return false;
                    }
                }
                 VariablesMap[&*it1] = &*it2; // Ubacujemo instrukcije da su iste npr %1 (iz F1)-> %1 (iz F2) 


            }
            
        }

        return true;
    }
    bool runOnModule(Module &M) override {
      std::unordered_map<size_t, std::vector<Function*> > HashGroups;
      //Ova mapa za svaki hash cuva funkcije koje imaju taj hash
      for(auto &F : M){ //Popunim hash vrednosti jednim prolazom kroz modul
        size_t H = hashFunction(F);
        HashGroups[H].push_back(&F);
      }
      for (auto &pair : HashGroups){
        auto &Functions = pair.second;
        if(Functions.size() < 2) continue;

        auto cmpFunctions = [](Function *A, Function *B) {
            return A->getName() < B->getName();
        }; //ako se promeni redosled kompajliranja modula, ovo omogucava da se funkcije uvek spajaju u istom redosledu
std::sort(Functions.begin(), Functions.end(), cmpFunctions);

        for(size_t i=0; i<Functions.size(); ++i){
            Function* F1 = Functions[i];
            for(size_t j=i+1;j<Functions.size();){
                Function* F2 = Functions[j];
                if(sameFunctionBody(F1,F2)){
                    F2->replaceAllUsesWith(F1);
                    F2->eraseFromParent();
                    Functions.erase(Functions.begin() + j);
                    ++j;
                }else{
                    ++j;
                }
            }
        }
      }

      return false;
    }
  };
}

char MergeFuncPass::ID = 0;
static RegisterPass<MergeFuncPass> X("merge-pass", "Mergefunc pass using hash");
