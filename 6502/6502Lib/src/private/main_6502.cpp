#include "main_6502.h"

void Memory::Init()
{
  for (uint32_t i = 0; i < MAX_MEM; i++)
  {
    Data[i] = 0;
  }  
}

// ustaviramo operator za [] da nam vrne byte iz memorija 

Byte Memory::operator[](uint32_t Address) const
{
  // assert here Adress is < MAX_MEM
  return Data[Address];
}

/* Nastavimo zacetno vrednost registrov , in statusnih bitov  

input: passamo not memory by reference
*/


void CPU::Reset(Memory& memory)
{
  PC = 0xFFFC;
  SP = 0x100;
  C = D = Z = I = B = V = N = 0;
  A = X = Y = 0;

  memory.Init();

}


Byte CPU::Fetch_Byte(uint32_t& Cycles, Memory& memory)
{
  Byte Data = memory[PC];
  PC++;
  Cycles--;

  return Data;
}

Word CPU::Fetch_Word(uint32_t& Cycles, Memory& memory)
{
  // 6502 is little endian (prvo preberemo LSB)
  Word Data = memory[PC];
  PC++;
  
  Data |= (memory[PC] << 8);
  PC++;

  Cycles-=2;
  return Data;
}

void CPU::WriteWord(Word Value, uint32_t Address, uint32_t& Cycles, Memory& memory)
{
  memory.Data[Address] = Value & 0xFF;
  Cycles--;
  memory.Data[Address + 1] = (Value << 8);
  Cycles--;
}

Byte CPU::Read_Byte(uint32_t& Cycles, uint8_t& ZeroPageAddress, Memory& memory)
{
  // ne inkrimentamo PC ker ne executamo code ampak samo beremo memory
  Byte Data = memory[ZeroPageAddress];
  Cycles--;

  return Data;
}


// itak ima access do A zato nerabimo passat notri
void CPU::LDASetStatus()
{
  // seting the Z(zero) and N(negative) flag... Glej dokumentacijo
  // ce je A = 0 setamo Z
  if (A == 0) Z = 1;
  // ce je 7 bit A = 1 setamo N
  if ((A & 0b10000000) > 0) N = 1;
}


void CPU::Execute(uint32_t& Cycles, Memory& memory)
{
  while (Cycles > 0)
  {
    // dobimo naslednjo instrukcijo iz memorija da vemo kaj bomo executali
    Byte Ins = Fetch_Byte(Cycles , memory);

    switch (Ins)
    {
    case INS_LDA_IM:
      {
        // na naslednji poziciji je shranjen data ki ga je potrebno nalozit v A
        Byte Value = Fetch_Byte(Cycles, memory);
        A = Value;

        LDASetStatus();

      }break;

      case INS_LDA_ZP:
      {
        // na naslednji poziciji je shranjen data ki ga je potrebno nalozit v A
        Byte ZeroPageAdress = Fetch_Byte(Cycles, memory);
        // 3 clock cycle imamo ker rabi prebrat se kar je shranjeno na zero page addressu in ga dat v A
        A = Read_Byte(Cycles, ZeroPageAdress, memory);
  
        LDASetStatus();

      }break;

      case INS_LDA_ZPX:
      {
        // na naslednji poziciji je shranjen data ki ga je potrebno nalozit v A
        Byte ZeroPageAdress = Fetch_Byte(Cycles, memory); 
        // 3 clock cycle imamo ker rabi prebrat se kar je shranjeno na zero page addressu in ga dat v A
        ZeroPageAdress += X;

        // Todo assert ce adress overflowa
        A = Read_Byte(Cycles, ZeroPageAdress, memory);
        Cycles--;
  
        LDASetStatus();

      }break;

      case INS_JSR_ABS:
      {
        // preberemo naslov instrukcije na katero bomo skocli
        Word SubAdress = Fetch_Word(Cycles, memory); 
        // push the return add - 1 to the stack
        WriteWord(PC - 1, SP, Cycles, memory);
        
        //Stack raste navzdol zato ga decrementamo
        SP--;
        PC = SubAdress;
        Cycles--;

      }break;
    
    default:
      std::cout << "Instruction not handeld: " << static_cast<int16_t>(Ins) << std::endl;

      // TODO assert
      break;
    }
  }  
}