#include "main_6502.h"
#include <stdexcept>

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

uint16_t CPU::Read_Word_ZeroPage(int32_t& Cycles,uint8_t ZeroPageAddress, Memory& memory)
{
  // 6502 is little endian (prvo preberemo LSB)
  uint16_t DataLSB = memory[ZeroPageAddress];
  uint16_t DataMSB = memory[ZeroPageAddress + 1];
  //uint8_t addplus1 = memory[AbsAddress + 1];
  uint16_t Data = (DataMSB << 8) | DataLSB; 

  Cycles--;
  //std::cout << "AbsAddress plus 1 is : " << AbsAddress + 1 << std::endl;
  //std::cout << "AbsAddress is : " << AbsAddress << std::endl;
  //std::cout << "Data is : " << Data << std::endl;
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

// itak ima access do A zato nerabimo passat notri
void CPU::LDXSetStatus()
{
  // seting the Z(zero) and N(negative) flag... Glej dokumentacijo
  // ce je A = 0 setamo Z
  if (X == 0) Z = 1;
  // ce je 7 bit A = 1 setamo N
  if ((X & 0b10000000) > 0) N = 1;
}

// itak ima access do A zato nerabimo passat notri
void CPU::LDYSetStatus()
{
  // seting the Z(zero) and N(negative) flag... Glej dokumentacijo
  // ce je A = 0 setamo Z
  if (Y == 0) Z = 1;
  // ce je 7 bit A = 1 setamo N
  if ((Y & 0b10000000) > 0) N = 1;
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

      case INS_LDA_INDX:
      {
        uint16_t AbsTargetAddressPlusX = IndirectX(Cycles, memory);

        A = Read_Byte_ABS(Cycles, AbsTargetAddressPlusX, memory);
  
        LDASetStatus();

      }break;

      case INS_LDA_INDY:
      {
        uint16_t AbsTargetAddressPlusY = IndirectY(Cycles, memory);

        A = Read_Byte_ABS(Cycles, AbsTargetAddressPlusY, memory);
  
        LDASetStatus();

      }break;

      case INS_LDX_IM:
      {
        // na naslednji poziciji je shranjen data ki ga je potrebno nalozit v A
        //uint8_t Value = Fetch_Byte(Cycles, memory);
        //A = Value;

        X = Immediate(Cycles, memory);

        LDXSetStatus();

      }break;

      case INS_LDX_ZP:
      {
        // na naslednji poziciji je shranjen data ki ga je potrebno nalozit v A
        //uint8_t ZeroPageAdress = Fetch_Byte(Cycles, memory);
        // 3 clock cycle imamo ker rabi prebrat se kar je shranjeno na zero page addressu in ga dat v A
        //A = Read_Byte(Cycles, ZeroPageAdress, memory);

        X = ZeroPage(Cycles, memory);

        LDXSetStatus();

      }break;

      case INS_LDX_ZPY:
      {
        // na naslednji poziciji je shranjen data kateremu pristejemo vrednost X in nalozimo v A
        //uint8_t ZeroPageAdress = Fetch_Byte(Cycles, memory); 
        // 4 clock cycle imamo ker rabi prebrat kar je shranjeno na zero page addressu in se pristet X
        //ZeroPageAdress += X;

        // Todo assert ce adress overflowa
        //A = Read_Byte(Cycles, ZeroPageAdress, memory);
        //Cycles--;

        X = ZeroPageY(Cycles, memory);
  
        LDXSetStatus();

      }break;

      case INS_LDX_ABS:
      {
        uint16_t Abs_Address = Absolute(Cycles, memory);

        X = Read_Byte_ABS(Cycles, Abs_Address, memory);
  
        LDXSetStatus();

      }break;

      case INS_LDX_ABSY:
      {
        uint16_t Abs_Address = AbsoluteY(Cycles, memory);

        X = Read_Byte_ABS(Cycles, Abs_Address, memory);
  
        LDXSetStatus();

      }break;

      case INS_LDY_IM:
      {
        // na naslednji poziciji je shranjen data ki ga je potrebno nalozit v A
        //uint8_t Value = Fetch_Byte(Cycles, memory);
        //A = Value;

        Y = Immediate(Cycles, memory);

        LDYSetStatus();

      }break;

      case INS_LDY_ZP:
      {
        // na naslednji poziciji je shranjen data ki ga je potrebno nalozit v A
        //uint8_t ZeroPageAdress = Fetch_Byte(Cycles, memory);
        // 3 clock cycle imamo ker rabi prebrat se kar je shranjeno na zero page addressu in ga dat v A
        //A = Read_Byte(Cycles, ZeroPageAdress, memory);

        Y = ZeroPage(Cycles, memory);

        LDYSetStatus();

      }break;

      case INS_LDY_ZPX:
      {
        // na naslednji poziciji je shranjen data kateremu pristejemo vrednost X in nalozimo v A
        //uint8_t ZeroPageAdress = Fetch_Byte(Cycles, memory); 
        // 4 clock cycle imamo ker rabi prebrat kar je shranjeno na zero page addressu in se pristet X
        //ZeroPageAdress += X;

        // Todo assert ce adress overflowa
        //A = Read_Byte(Cycles, ZeroPageAdress, memory);
        //Cycles--;

        Y = ZeroPageX(Cycles, memory);
  
        LDYSetStatus();

      }break;

      case INS_LDY_ABS:
      {
        uint16_t Abs_Address = Absolute(Cycles, memory);

        Y = Read_Byte_ABS(Cycles, Abs_Address, memory);
  
        LDYSetStatus();

      }break;

      case INS_LDY_ABSX:
      {
        uint16_t Abs_Address = AbsoluteX(Cycles, memory);

        Y = Read_Byte_ABS(Cycles, Abs_Address, memory);
  
        LDYSetStatus();

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
      throw std::runtime_error(" Exepction thrown.\n ");
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
  // porabi en cikel vec ce je page crossed, preverimo ce se je zgornji bajt spremenil
  if ((AbsAddress & 0xFF00) != (AbsAddressX & 0xFF00))
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
  // porabi en cikel vec ce je page crossed, preverimo ce se je zgornji bajt spremenil
  if ((AbsAddress & 0xFF00) != (AbsAddressY & 0xFF00))
  {
    Cycles--;
  }

  return AbsAddressY;
}


uint16_t CPU::IndirectX(int32_t& Cycles, Memory& memory)
{
  // na naslednjem mestu je zeropage address kateremu je treba pristet X, in nato je na tem addressu LSB od target addressa
  uint8_t ZeroPageIndirectAdress = Fetch_Byte(Cycles, memory);
  uint8_t ZeroPageIndirectAdressPlusX = ZeroPageIndirectAdress + X;
  //std::cout << "Vrednost zeropageadd + X : " << static_cast<int16_t>(ZeroPageIndirectAdressPlusX) << std::endl;
  Cycles--;
  // TODO zeropage wrap around, ta zero page wrap around je ze resen semizdi ker uporabljam uint8_t st

  uint8_t IndirectTargetAddLSB = Read_Byte(Cycles, ZeroPageIndirectAdressPlusX, memory);

  uint16_t IndirectTarget = IndirectTargetAddLSB;

  uint16_t AbsTargetAddress = Read_Word_ZeroPage(Cycles, IndirectTarget, memory);
  
  return AbsTargetAddress;
}

uint16_t CPU::IndirectY(int32_t& Cycles, Memory& memory)
{
  // na naslednjem mestu je zeropage address kateremu je treba pristet X, in nato je na tem addressu LSB od target addressa
  uint8_t ZeroPageIndirectAdress = Fetch_Byte(Cycles, memory);
  // TODO zeropage wrap around
  uint8_t IndirectTargetAddLSB = Read_Byte(Cycles, ZeroPageIndirectAdress, memory);

  uint16_t AbsTargetAddress = Read_Word_ZeroPage(Cycles, IndirectTargetAddLSB, memory);
  //std::cout << "Vrednost zeropageadd : " << AbsTargetAddress << std::endl;
  uint16_t AbsTargetAddressPlusY = AbsTargetAddress + Y;
  //std::cout << "Vrednost zeropageadd + Y : " << AbsTargetAddressPlusY << std::endl;

  if ((AbsTargetAddress & 0xFF00) != (AbsTargetAddressPlusY & 0xFF00))
  {
    Cycles--;
  }
  
  return AbsTargetAddressPlusY;
}



