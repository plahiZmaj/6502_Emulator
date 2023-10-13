#include "main_6502.h"

void Memory::Init()
{
  for (uint32_t i = 0; i < MAX_MEM; i++)
  {
    Data[i] = 0;
  }  
}

// ustaviramo operator za [] da nam vrne byte iz memorija 

uint8_t Memory::operator[](uint32_t Address) const
{
  // assert here Adress is < MAX_MEM
  return Data[Address];
}

/* Nastavimo zacetno vrednost registrov , in statusnih bitov  

input: passamo not memory by reference
*/


void CPU::Reset(Memory& memory)
{
  // initial PC and SP values
  PC = 0xFFFC;
  SP = 0x100;

  // Reset internal registers
  C = D = Z = I = B = V = N = 0;   // status register
  A = X = Y = 0;

  memory.Init();

}


uint8_t CPU::Fetch_Byte(int32_t& Cycles, Memory& memory)
{
  uint8_t Data = memory[PC];
  PC++;
  Cycles--;

  return Data;
}

uint16_t CPU::Fetch_Word(int32_t& Cycles, Memory& memory)
{
  // 6502 is little endian (prvo preberemo LSB)
  uint16_t Data = memory[PC];
  PC++;
  
  Data |= (memory[PC] << 8);
  PC++;

  Cycles-=2;
  return Data;
}

void CPU::WriteWord(uint16_t Value, uint32_t Address, int32_t& Cycles, Memory& memory)
{
  memory.Data[Address] = Value & 0xFF;
  Cycles--;
  memory.Data[Address + 1] = (Value << 8);
  Cycles--;
}

uint8_t CPU::Read_Byte(int32_t& Cycles, uint8_t& ZeroPageAddress, Memory& memory)
{
  // ne inkrimentamo PC ker ne executamo code ampak samo beremo memory
  uint8_t Data = memory[ZeroPageAddress];
  Cycles--;

  return Data;
}

uint8_t CPU::Read_Byte_ABS(int32_t& Cycles, uint16_t& Abs_Adress, Memory& memory)
{
  // ne inkrimentamo PC ker ne executamo code ampak samo beremo memory
  uint8_t Data = memory[Abs_Adress];
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


int16_t CPU::Execute(int32_t& Cycles, Memory& memory)
{
  const int32_t CyclesRequested = Cycles;

  while (Cycles > 0)
  {
    // dobimo naslednjo instrukcijo iz memorija da vemo kaj bomo executali
    uint8_t Ins = Fetch_Byte(Cycles , memory);

    switch (Ins)
    {
    case INS_LDA_IM:
      {
        // na naslednji poziciji je shranjen data ki ga je potrebno nalozit v A
        //uint8_t Value = Fetch_Byte(Cycles, memory);
        //A = Value;

        A = Immediate(Cycles, memory);

        LDASetStatus();

      }break;

      case INS_LDA_ZP:
      {
        // na naslednji poziciji je shranjen data ki ga je potrebno nalozit v A
        //uint8_t ZeroPageAdress = Fetch_Byte(Cycles, memory);
        // 3 clock cycle imamo ker rabi prebrat se kar je shranjeno na zero page addressu in ga dat v A
        //A = Read_Byte(Cycles, ZeroPageAdress, memory);

        A = ZeroPage(Cycles, memory);

        LDASetStatus();

      }break;

      case INS_LDA_ZPX:
      {
        // na naslednji poziciji je shranjen data kateremu pristejemo vrednost X in nalozimo v A
        //uint8_t ZeroPageAdress = Fetch_Byte(Cycles, memory); 
        // 4 clock cycle imamo ker rabi prebrat kar je shranjeno na zero page addressu in se pristet X
        //ZeroPageAdress += X;

        // Todo assert ce adress overflowa
        //A = Read_Byte(Cycles, ZeroPageAdress, memory);
        //Cycles--;

        A = ZeroPageX(Cycles, memory);
  
        LDASetStatus();

      }break;

      case INS_LDA_ABS:
      {
        uint16_t Abs_Address = Absolute(Cycles, memory);

        A = Read_Byte_ABS(Cycles, Abs_Address, memory);
  
        LDASetStatus();

      }break;

      case INS_LDA_ABSX:
      {
        uint16_t Abs_Address = AbsoluteX(Cycles, memory);

        A = Read_Byte_ABS(Cycles, Abs_Address, memory);
  
        LDASetStatus();

      }break;

      case INS_LDA_ABSY:
      {
        uint16_t Abs_Address = AbsoluteY(Cycles, memory);

        A = Read_Byte_ABS(Cycles, Abs_Address, memory);
  
        LDASetStatus();

      }break;

      case INS_JSR_ABS:
      {
        // preberemo naslov instrukcije na katero bomo skocli
        uint16_t SubAdress = Fetch_Word(Cycles, memory); 
        // push the return add - 1 to the stack
        WriteWord(PC - 1, SP, Cycles, memory);
        
        //Stack raste navzdol zato ga decrementamo
        SP--;
        PC = SubAdress;
        Cycles--;

      }break;
    
    default:
      std::cout << "Instruction not handeld: " << static_cast<int16_t>(Ins) << std::endl;
      // we want to throw an error if istruction is not handeld
      throw -1;
      // TODO assert
      break;
    }
  }  

  // TODO
  // popravi boljse to ker zdej je Cycles uint torej ce gre pod 0 wrappa around
  return Cycles;
}


/*  implementations of adressing modes

    not all instructions use all addressing modes, look it up in the datasheet

    Implicit
    Accumulator
    Immediate
    Zero Page
    Zero Page,X
    Zero Page,Y
    Relative
    Absolute
    Absolute,X
    Absolute,Y
    Indirect
    Indirect,X
    Indirect,Y

*/

uint8_t CPU::Immediate(int32_t& Cycles, Memory& memory)
{

  // na naslednji poziciji je shranjen data ki ga je potrebno nalozit v A
  uint8_t temp = Fetch_Byte(Cycles, memory);
  return temp;

}

uint8_t CPU::ZeroPage(int32_t& Cycles, Memory& memory)
{

  // na naslednji poziciji je shranjen data ki ga je potrebno nalozit v A
  uint8_t ZeroPageAdress = Fetch_Byte(Cycles, memory);
  // 3 clock cycle imamo ker rabi prebrat se kar je shranjeno na zero page addressu in ga dat v A
  uint8_t temp = Read_Byte(Cycles, ZeroPageAdress, memory);
  return temp;
}

uint8_t CPU::ZeroPageX(int32_t& Cycles, Memory& memory)
{

  // na naslednji poziciji je shranjen data ki ga je potrebno nalozit v A
  uint8_t ZeroPageAdress = Fetch_Byte(Cycles, memory);

  ZeroPageAdress += X;
  Cycles--;
  // 3 clock cycle imamo ker rabi prebrat se kar je shranjeno na zero page addressu in ga dat v A
  uint8_t temp = Read_Byte(Cycles, ZeroPageAdress, memory);
  return temp;
}

uint8_t CPU::ZeroPageY(int32_t& Cycles, Memory& memory)
{

  // na naslednji poziciji je shranjen data ki ga je potrebno nalozit v A
  uint8_t ZeroPageAdress = Fetch_Byte(Cycles, memory);

  ZeroPageAdress += Y;
  //porabimo se en cikel zaradi pristevanja y registra
  Cycles--;
  // 3 clock cycle imamo ker rabi prebrat se kar je shranjeno na zero page addressu in ga dat v A
  uint8_t temp = Read_Byte(Cycles, ZeroPageAdress, memory);
  return temp;
}

uint16_t CPU::Absolute(int32_t& Cycles, Memory& memory)
{

  // na naslednji poziciji je shranjen data ki ga je potrebno nalozit v A
  uint8_t AbsAddressLOW = Fetch_Byte(Cycles, memory);
  uint8_t AbsAddressHIGH = Fetch_Byte(Cycles, memory);

  uint16_t AbsAddress = (AbsAddressHIGH << 8) | AbsAddressLOW;
  // verjetno porabi se en cikel treba preverit zakaj
  //Cycles--;

  return AbsAddress;
}

uint16_t CPU::AbsoluteX(int32_t& Cycles, Memory& memory)
{

  // na naslednji poziciji je shranjen data ki ga je potrebno nalozit v A
  uint8_t AbsAddressLOW = Fetch_Byte(Cycles, memory);
  uint8_t AbsAddressHIGH = Fetch_Byte(Cycles, memory);
  uint16_t AbsAddress = (AbsAddressHIGH << 8) | AbsAddressLOW;

  uint16_t AbsAddressX = AbsAddress + X;
  // porabi en cikel vec ce je page crossed
  if ((AbsAddress <= 0x00FF) && (AbsAddressX > 0x00FF))
  {
    Cycles--;
  }

  return AbsAddressX;
}

uint16_t CPU::AbsoluteY(int32_t& Cycles, Memory& memory)
{

  // na naslednji poziciji je shranjen data ki ga je potrebno nalozit v A
  uint8_t AbsAddressLOW = Fetch_Byte(Cycles, memory);
  uint8_t AbsAddressHIGH = Fetch_Byte(Cycles, memory);
  uint16_t AbsAddress = (AbsAddressHIGH << 8) | AbsAddressLOW;
  
  uint16_t AbsAddressY = AbsAddress + Y;
  // porabi en cikel vec ce je page crossed
  if ((AbsAddress <= 0x00FF) && (AbsAddressY > 0x00FF))
  {
    Cycles--;
  }

  return AbsAddressY;
}



