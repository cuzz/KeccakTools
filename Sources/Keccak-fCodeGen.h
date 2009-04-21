/*
Tools for the Keccak sponge function family.
Authors: Guido Bertoni, Joan Daemen, Michaël Peeters and Gilles Van Assche

This code is hereby put in the public domain. It is given as is, 
without any guarantee.

For more information, feedback or questions, please refer to our website:
http://keccak.noekeon.org/
*/

#ifndef _KECCAKFCODEGEN_H_
#define _KECCAKFCODEGEN_H_

#include "Keccak-f.h"
#include "Keccak-fParts.h"

using namespace std;

/**
  * Class implementing code generation for the Keccak-f permutations.
  */
class KeccakFCodeGen : public KeccakF {
protected:
     /** The interleaving factor, i.e., the ratio between the lane size and the
       * target word size. For instance, to generate 32-bit interleaved code
       * for Keccak-f[1600], interleavingFactor must be set to 2 (=64/32). By 
       * default, the interleavingFactor is 1, meaning no interleaving. The 
       * interleavingFactor must divide the lane size.
       */
    unsigned int interleavingFactor;
    /** The word size, which is equal to laneSize/interleavingFactor.
      */
    unsigned int wordSize;
    /** Boolean that indicates whether macros should be used instead of 
      * standard Boolean operators in C.
      */
    bool outputMacros;
public:
    /**
      * The constructor. See KeccakF() for more details.
      */
    KeccakFCodeGen(unsigned int aWidth, unsigned int aNrRounds=0);
    /**
      * Method to set the interleaving factor.
      *
      * @param  anInterleavingFactor    The interleaving factor, see
      *                 interleavingFactor.
      */
    void setInterleavingFactor(unsigned int anInterleavingFactor);
    /**
      * Method to specify if macros should be used instead of standard
      * Boolean operators in C.
      *
      * @param  anOutputMacros  The desired value of outputMacros.
      */
    void setOutputMacros(bool anOutputMacros);
    /**
      * Method that displays the round constants.
      */
    void displayRoundConstants();
    /**
      * Method that displays the translation offsets for ρ.
      */
    void displayRhoOffsets(bool moduloWordLength);
    /**
      * Method that displays the lane moves for π.
      */
    void displayPi();
    /**
      * Method that generates declarations for the C code produced by 
      * genCodeForRound().
      *
      * @param  fout    The output stream where the code is generated.
      */
    void genDeclarations(ostream& fout) const;
    /**
      * Method that generates C code to compute one round.
      * The produced code assumes that the state is stored in the variables
      * starting with letter A. It also assumes that the variables starting with
      * D are the lanes that will be XORed into the state to perform θ.
      * The generated code then XORs the D's into the A's for θ, moves
      * the lanes and rotates them into the B's to perform ρ and π.
      * The evaluation of χ is done from the B's into the variables starting
      * with E. As χ is computed, the generated code optionally also computes
      * sheet parities into 5 variables starting with C.
      * The generated code also assumes that the lanes are complemented
      * according to patterns @a inChiMask (after the linear steps, before χ)
      * and @a outChiMask (after χ, before θ).
      *
      * @param  fout    The output stream where the code is generated.
      * @param prepareTheta A Boolean value telling whether the sheet
      *                 parities are to be computed into the C's.
      * @param  inChiMask   The lane complementing pattern at the input of χ
      *                 (or after ρ and π).
      * @param  outChiMask  The lane complementing pattern at the output of χ
      *                 (or before θ).
      * @param  A       The string with the variable name for A.
      * @param  B       The string with the variable name for B.
      * @param  C       The string with the variable name for C.
      * @param  D       The string with the variable name for D.
      * @param  E       The string with the variable name for E.
      * @param  header  A string to output before the generated code.
      */
    void genCodeForRound(ostream& fout, bool prepareTheta, SliceValue inChiMask=0, SliceValue outChiMask=0, 
        string A = "A", string B = "B", string C = "C", string D = "D", string E = "E", string header = "") const;
    /**
      * Method that generates C code to compute the sheet parities 
      * (C's variables) for θ in the first round.
      *
      * @param  fout    The output stream where the code is generated.
      * @param  A       The string with the variable name for A.
      * @param  C       The string with the variable name for C.
      */
    void genCodeForPrepareTheta(ostream& fout, string A = "A", string C = "C") const;
    /**
      * Method that generates the round constants for ι.
      *
      * @param  fout    The output stream where the code is generated.
      */
    void genRoundConstants(ostream& fout) const;
    /**
      * Method that generates code to copy from an array of words into the state 
      * variables. It can xors a given number of words along the way.
      *
      * @param  fout    The output stream where the code is generated.
      * @param  bitsToXor   The number of bits to xor (i.e., the rate of the sponge function).
      * @param  A       The string with the variable name for A.
      * @param  state   The string with the variable name for the state as an array of words.
      * @param  input   The string with the variable name for the input as an array of words.
      */
    void genCopyFromStateAndXor(ostream& fout, unsigned int bitsToXor, string A="X##", string state="state", string input="input") const;
    /**
      * Method that generates code to copy from the state variables into an 
      * array of words.
      *
      * @param  fout    The output stream where the code is generated.
      * @param  A       The string with the variable name for A.
      * @param  state   The string with the variable name for the state as an array of words.
      * @param  input   The string with the variable name for the input as an array of words.
      */
    void genCopyToState(ostream& fout, string A="X##", string state="state", string input="input") const;
    /**
      * Method that generates code to copy from the state variables into other 
      * state variables.
      *
      * @param  fout    The output stream where the code is generated.
      * @param  X       The string with the variable name for target X.
      * @param  Y       The string with the variable name for target Y.
      */
    void genCopyStateVariables(ostream& fout, string X="X##", string Y="Y##") const;
    /**
      * Method that generates all the macros for the C code.
      *
      * @param  fout    The output stream where the code is generated.
      * @param  laneComplementing   If the output must contain code using 
      *                 the lane complementing technique, in addition to the
      *                 straightforward code.
      */
    void genMacroFile(ostream& fout, bool laneComplementing=false) const;
protected:
    string buildWordName(const string& prefixSymbol, unsigned int x, unsigned int y, unsigned int z) const;
    string buildWordName(const string& prefixSymbol, unsigned int x, unsigned int z) const;
    void genDeclarationsLanes(ostream& fout, const string& prefixSymbol) const;
    void genDeclarationsSheets(ostream& fout, const string& prefixSymbol) const;
    string strANDORnot(const string& A, const string& B, bool LC1, bool LC2, bool LOR) const;
    string strConst(const string& A) const;
    string strNOT(const string& A, bool complement=true) const;
    string strROL(const string& symbol, unsigned int amount) const;
    string strXOR(const string& A, const string& B) const;
    string strXOReq(const string& A, const string& B) const;
};

#endif
