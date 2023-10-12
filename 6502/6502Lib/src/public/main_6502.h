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
  uint8_t Data[MAX_MEM];

  void Init();
  uint8_t operator[](uint32_t Address) const;

};

struct CPU 
{

  // declering registers
  // Program counter, stack pointer
  uint16_t PC;
  uint16_t SP;

  // X and Y are GPRS, A -> acumulator
  uint8_t X, Y, A;

  // procesor status resgister 
  // cpp bit fields ???, lahko usakega posebej dekleriras
  uint8_t C : 1;  
  uint8_t Z : 1;
  uint8_t I : 1;
  uint8_t D : 1;
  uint8_t B : 1;
  uint8_t V : 1;
  uint8_t N : 1;

  // opcodes
  // instruction load acumulator imidiate mode
  // 1 byte opcode 2 byte je data ki ga hocemo loadat
  static constexpr uint8_t INS_LDA_IM = 0xA9;
  // instruction load acumulator zero page
  // 1 byte opcode 2 byte je data ki ga hocemo loadat
  static constexpr uint8_t INS_LDA_ZP = 0xA5;
  // instruction load acumulator zero page, X
  // 1 byte opcode 2 byte je data ki ga hocemo loadat
  static constexpr uint8_t INS_LDA_ZPX = 0xB5;

  static constexpr uint8_t INS_LDA_ABS = 0xAD;

  static constexpr uint8_t INS_LDA_ABSX = 0xBD;

  static constexpr uint8_t INS_LDA_ABSY = 0xB9;
  // instruction jump to subrutine absolute, pushes the address (minus one) of the return point on to the stack and then sets the program counter to the target memory address.
  // 1 byte opcode 2 byte je address kamor skocimo
  static constexpr uint8_t INS_JSR_ABS = 0x20;

  // CPU functions

  void Reset(Memory& memory);
 
  int16_t Execute(int32_t& Cycles, Memory& memory);

  uint8_t Fetch_Byte(int32_t& Cycles, Memory& memory);

  uint16_t Fetch_Word(int32_t& Cycles, Memory& memory);

  void WriteWord(uint16_t Value, uint32_t Address, int32_t& Cycles, Memory& memory);

  uint8_t Read_Byte(int32_t& Cycles, uint8_t& ZeroPageAddress, Memory& memory);

  uint8_t Read_Byte_ABS(int32_t& Cycles, uint16_t& Abs_Adress, Memory& memory);

  // Addressing modes

  uint8_t Immediate(int32_t& Cycles, Memory& memory);

  uint8_t ZeroPage(int32_t& Cycles, Memory& memory);

  uint8_t ZeroPageX(int32_t& Cycles, Memory& memory);

  uint8_t ZeroPageY(int32_t& Cycles, Memory& memory);

  uint16_t Absolute(int32_t& Cycles, Memory& memory);
  
  uint16_t AbsoluteX(int32_t& Cycles, Memory& memory); 
  
  uint16_t AbsoluteY(int32_t& Cycles, Memory& memory);  

  // Instruction functions 
  void LDASetStatus();
};