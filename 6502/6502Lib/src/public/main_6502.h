#pragma once
#include <iostream>
#include <stdint.h>

using Byte = uint8_t;
using Word = uint16_t;

struct Memory 
{
  // maximalna vrednost nasega memorija
  static constexpr uint32_t MAX_MEM = 1024 * 64;
  // naredimo memory
  Byte Data[MAX_MEM];

  void Init();
  Byte operator[](uint32_t Address) const;

};

struct CPU 
{

  // declering registers
  // Program counter, stack pointer
  Word PC;
  Word SP;

  // X and Y are GPRS, A -> acumulator
  Byte X, Y, A;

  // procesor status resgister 
  // cpp bit fields ???, lahko usakega posebej dekleriras
  Byte C : 1;  
  Byte Z : 1;
  Byte I : 1;
  Byte D : 1;
  Byte B : 1;
  Byte V : 1;
  Byte N : 1;

  // opcodes
  // instruction load acumulator imidiate mode
  // 1 byte opcode 2 byte je data ki ga hocemo loadat
  static constexpr Byte INS_LDA_IM = 0xA9;
  // instruction load acumulator zero page
  // 1 byte opcode 2 byte je data ki ga hocemo loadat
  static constexpr Byte INS_LDA_ZP = 0xA5;
  // instruction load acumulator zero page, X
  // 1 byte opcode 2 byte je data ki ga hocemo loadat
  static constexpr Byte INS_LDA_ZPX = 0xB5;
  // instruction jump to subrutine absolute, pushes the address (minus one) of the return point on to the stack and then sets the program counter to the target memory address.
  // 1 byte opcode 2 byte je address kamor skocimo
  static constexpr Byte INS_JSR_ABS = 0x20;


  // CPU functions

  void Reset(Memory& memory);
 
  void Execute(uint32_t& Cycles, Memory& memory);

  Byte Fetch_Byte(uint32_t& Cycles, Memory& memory);

  Word Fetch_Word(uint32_t& Cycles, Memory& memory);

  void WriteWord(Word Value, uint32_t Address, uint32_t& Cycles, Memory& memory);

  Byte Read_Byte(uint32_t& Cycles, uint8_t& ZeroPageAddress, Memory& memory);


  void LDASetStatus();
};