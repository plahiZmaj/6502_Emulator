#include <gtest/gtest.h>
#include <iostream>
#include <stdint.h>
#include "main_6502.h"


class test_6502 : public testing::Test
{

public:

    Memory mem;
    CPU cpu;

    void TestLoadRegisterImidiateMode(Byte opcode, Byte CPU::* Register);
    void TestLoadRegisterZeroPage(Byte opcode, Byte CPU::* Register);
    void TestLoadRegisterAbsolute(Byte opcode, Byte CPU::* Register);
    void TestLoadRegisterZeroPageXorY(Byte opcode, Byte CPU::* Register, Byte CPU::* XorY);
    void TestLoadRegisterAbsoluteXorY(Byte opcode, Byte CPU::* Register, Byte CPU::* XorY);

    virtual void SetUp()
    {
      cpu.Reset(mem);
      //std::cout << "CPU reset" << std::endl;
    }

    virtual void TearDown()
    {

    }


};


static void VerifyUnmodifiedFlagsFromLDA(CPU cpu, CPU cpu_copy)
{
  EXPECT_EQ(cpu.C, cpu_copy.C);
  EXPECT_EQ(cpu.I, cpu_copy.I);
  EXPECT_EQ(cpu.D, cpu_copy.D);
  EXPECT_EQ(cpu.B, cpu_copy.B);
  EXPECT_EQ(cpu.V, cpu_copy.V);
}

TEST_F(test_6502, CPUDoesNothingWhenGiven0Cycles)
{
  int32_t NUM_CYCLES = 0;

  int32_t Cycles_Used = cpu.Execute(NUM_CYCLES, mem);

  EXPECT_EQ(Cycles_Used, 0);


}


TEST_F(test_6502, LoadAccumulatorImidiateMode1CycleReqstedInteadOf2)
{
  // hard code test f
  mem.Data[0xFFFC] = cpu.INS_LDA_IM;
  mem.Data[0xFFFC + 1] = 0x42;
  int32_t Cycles = 1;
  // before we execute we take a copy of the cpu for verifying status flags
  CPU cpu_copy = cpu;

  int32_t CyclesLeftover = cpu.Execute(Cycles, mem);

  EXPECT_EQ(CyclesLeftover, -1);
    
}


void test_6502::TestLoadRegisterImidiateMode(Byte opcode, Byte CPU::* Register)
{
  // hard code test f
  mem.Data[0xFFFC] = opcode;
  mem.Data[0xFFFC + 1] = 0x42;
  int32_t Cycles = 2;
  // before we execute we take a copy of the cpu for verifying status flags
  CPU cpu_copy = cpu;

  int32_t CyclesLeftover = cpu.Execute(Cycles, mem);

  // preverjamo ali je v Accumulatorju pravilna vrednost
  EXPECT_EQ(cpu.*Register, 0x42);
  // preverjamo ali smo porabili use predvidene cikle
  EXPECT_EQ(CyclesLeftover, 0);
  // potrebno je preveriti tudi status register
  EXPECT_EQ(cpu.N, 0x00);
  EXPECT_EQ(cpu.Z, 0x00);
    
  VerifyUnmodifiedFlagsFromLDA(cpu, cpu_copy);
}


TEST_F(test_6502, LoadAccumulatorImidiateMode)
{

  TestLoadRegisterImidiateMode(CPU::INS_LDA_IM, &CPU::A);
    
}

TEST_F(test_6502, LoadXRegisterImidiateMode)
{

  TestLoadRegisterImidiateMode(CPU::INS_LDX_IM, &CPU::X);
    
}

TEST_F(test_6502, LoadYRegisterImidiateMode)
{

  TestLoadRegisterImidiateMode(CPU::INS_LDY_IM, &CPU::Y);
    
}

void test_6502::TestLoadRegisterZeroPage(Byte opcode, Byte CPU::* Register)
{
  // hard code test f
  mem.Data[0xFFFC] = opcode;
  mem.Data[0xFFFC + 1] = 0x42;
  mem.Data[0x0042] = 0x00;
  int32_t Cycles = 3;
  // before we execute we take a copy of the cpu for verifying status flags
  CPU cpu_copy = cpu;

  int32_t CyclesLeftover = cpu.Execute(Cycles, mem);

  // preverjamo ali je v Accumulatorju pravilna vrednost
  EXPECT_EQ(cpu.*Register, 0x00);
  // preverjamo ali smo porabili use predvidene cikle
  EXPECT_EQ(CyclesLeftover, 0);
  // potrebno je preveriti tudi status register
  EXPECT_EQ(cpu.N, 0x00);
  EXPECT_EQ(cpu.Z, 0x01);

  VerifyUnmodifiedFlagsFromLDA(cpu, cpu_copy); 
}


TEST_F(test_6502, LoadAccumulatorZeroPage)
{

  TestLoadRegisterZeroPage(CPU::INS_LDA_ZP, &CPU::A);
  
}

TEST_F(test_6502, LoadXRegisterZeroPage)
{

  TestLoadRegisterZeroPage(CPU::INS_LDX_ZP, &CPU::X);
  
}

TEST_F(test_6502, LoadYRegisterZeroPage)
{

  TestLoadRegisterZeroPage(CPU::INS_LDY_ZP, &CPU::Y);
  
}



void test_6502::TestLoadRegisterZeroPageXorY(Byte opcode, Byte CPU::* Register, Byte CPU::* XorY)
{
  // hard code test f
  mem.Data[0xFFFC] = opcode;
  mem.Data[0xFFFC + 1] = 0x42;
  mem.Data[0x75] = 0x33;
  cpu.*XorY = 0x33;
  int32_t Cycles = 4;
  // before we execute we take a copy of the cpu for verifying status flags
  CPU cpu_copy = cpu;
  
  signed int CyclesLeftover = cpu.Execute(Cycles, mem);

  // preverjamo ali je v Accumulatorju pravilna vrednost
  EXPECT_EQ(cpu.*Register, 0x33);
  // preverjamo ali smo porabili use predvidene cikle
  EXPECT_EQ(CyclesLeftover, 0);

  // potrebno je preveriti tudi status register
  EXPECT_EQ(cpu.N, 0x00);
  EXPECT_EQ(cpu.Z, 0x00);
  
  VerifyUnmodifiedFlagsFromLDA(cpu, cpu_copy);
}


TEST_F(test_6502, LoadAccumulatorZeroPageAndAddX)
{
  
  TestLoadRegisterZeroPageXorY(CPU::INS_LDA_ZPX, &CPU::A, &CPU::X);
    
}

TEST_F(test_6502, LoadXRegisterZeroPageAndAddY)
{

  TestLoadRegisterZeroPageXorY(CPU::INS_LDX_ZPY, &CPU::X, &CPU::Y); 
    
}

TEST_F(test_6502, LoadYRegisterZeroPageAndAddX)
{

  TestLoadRegisterZeroPageXorY(CPU::INS_LDY_ZPX, &CPU::Y, &CPU::X); 
    
}


void test_6502::TestLoadRegisterAbsolute(Byte opcode, Byte CPU::* Register)
{
   // hard code test f
  mem.Data[0xFFFC] = opcode;
  mem.Data[0xFFFC + 1] = 0x00;
  mem.Data[0xFFFC + 2] = 0x01;
  mem.Data[0x0100] = 0x82;
  int32_t Cycles = 4;
  // before we execute we take a copy of the cpu for verifying status flags
  CPU cpu_copy = cpu;
  
  signed int CyclesLeftover = cpu.Execute(Cycles, mem);

  // preverjamo ali je v Accumulatorju pravilna vrednost
  EXPECT_EQ(cpu.*Register, 0x82);
  // preverjamo ali smo porabili use predvidene cikle
  EXPECT_EQ(CyclesLeftover, 0);

  // potrebno je preveriti tudi status register
  EXPECT_EQ(cpu.N, 0x01);
  EXPECT_EQ(cpu.Z, 0x00);
  
  VerifyUnmodifiedFlagsFromLDA(cpu, cpu_copy); 
}




TEST_F(test_6502, LoadAccumulatorAbsolute)
{
  
  TestLoadRegisterAbsolute(CPU::INS_LDA_ABS, &CPU::A);
    
}


TEST_F(test_6502, LoadXRegisterAbsolute)
{
  
  TestLoadRegisterAbsolute(CPU::INS_LDX_ABS, &CPU::X);
    
}

TEST_F(test_6502, LoadYRegisterAbsolute)
{
  
  TestLoadRegisterAbsolute(CPU::INS_LDY_ABS, &CPU::Y);
    
}

void test_6502::TestLoadRegisterAbsoluteXorY(Byte opcode, Byte CPU::* Register, Byte CPU::* XorY)
{
  // hard code test f
  mem.Data[0xFFFC] = opcode;
  mem.Data[0xFFFC + 1] = 0x01;
  mem.Data[0xFFFC + 2] = 0x00;
  cpu.*XorY = 0x01;
  mem.Data[0x0002] = 0x33;
  int32_t Cycles = 4;
  // before we execute we take a copy of the cpu for verifying status flags
  CPU cpu_copy = cpu;
  
  signed int CyclesLeftover = cpu.Execute(Cycles, mem);

  // preverjamo ali je v Accumulatorju pravilna vrednost
  EXPECT_EQ(cpu.*Register, 0x33);
  // preverjamo ali smo porabili use predvidene cikle
  EXPECT_EQ(CyclesLeftover, 0);

  // potrebno je preveriti tudi status register
  EXPECT_EQ(cpu.N, 0x00);
  EXPECT_EQ(cpu.Z, 0x00);
  
  VerifyUnmodifiedFlagsFromLDA(cpu, cpu_copy); 
    
}



TEST_F(test_6502, LoadAccumulatorAbsoluteX)
{

  TestLoadRegisterAbsoluteXorY(CPU::INS_LDA_ABSX, &CPU::A, &CPU::X);

}

TEST_F(test_6502, LoadAccumulatorAbsoluteY)
{

  TestLoadRegisterAbsoluteXorY(CPU::INS_LDA_ABSY, &CPU::A, &CPU::Y);

}

TEST_F(test_6502, LoadAccumulatorAbsoluteXTooManyCyclesExpected)
{
  // hard code test f
  mem.Data[0xFFFC] = cpu.INS_LDA_ABSX;
  mem.Data[0xFFFC + 1] = 0x01;
  mem.Data[0xFFFC + 2] = 0x00;
  cpu.X = 0x01;
  mem.Data[0x0002] = 0x33;
  int32_t Cycles = 5;
  

  // preverjamo ali bo throwalo error ce executa instrukcijo ki jo ne pozna
  EXPECT_THROW(cpu.Execute(Cycles, mem), std::runtime_error);
    
}


TEST_F(test_6502, LoadAccumulatorAbsoluteXPageCrossed)
{
  // hard code test f
  mem.Data[0xFFFC] = cpu.INS_LDA_ABSX;
  mem.Data[0xFFFC + 1] = 0xFC;
  mem.Data[0xFFFC + 2] = 0x00;
  cpu.X = 0x0A;
  mem.Data[0x0106] = 0x33;
  int32_t Cycles = 5;
  // before we execute we take a copy of the cpu for verifying status flags
  CPU cpu_copy = cpu;
  
  signed int CyclesLeftover = cpu.Execute(Cycles, mem);

  // preverjamo ali je v Accumulatorju pravilna vrednost
  EXPECT_EQ(cpu.A, 0x33);
  // preverjamo ali smo porabili use predvidene cikle
  EXPECT_EQ(CyclesLeftover, 0);

  // potrebno je preveriti tudi status register
  EXPECT_EQ(cpu.N, 0x00);
  EXPECT_EQ(cpu.Z, 0x00);
  
  VerifyUnmodifiedFlagsFromLDA(cpu, cpu_copy); 
    
}


TEST_F(test_6502, LoadAccumulatorAbsoluteYPageCrossed)
{
  // hard code test f
  mem.Data[0xFFFC] = cpu.INS_LDA_ABSY;
  mem.Data[0xFFFC + 1] = 0xFE;
  mem.Data[0xFFFC + 2] = 0x00;
  cpu.Y = 0x0A;
  mem.Data[0x0108] = 0x33;
  int32_t Cycles = 5;
  // before we execute we take a copy of the cpu for verifying status flags
  CPU cpu_copy = cpu;
  
  signed int CyclesLeftover = cpu.Execute(Cycles, mem);

  // preverjamo ali je v Accumulatorju pravilna vrednost
  EXPECT_EQ(cpu.A, 0x33);
  // preverjamo ali smo porabili use predvidene cikle
  EXPECT_EQ(CyclesLeftover, 0);

  // potrebno je preveriti tudi status register
  EXPECT_EQ(cpu.N, 0x00);
  EXPECT_EQ(cpu.Z, 0x00);
  
  VerifyUnmodifiedFlagsFromLDA(cpu, cpu_copy); 
    
}

TEST_F(test_6502, LoadXRegisterAbsoluteY)
{

  TestLoadRegisterAbsoluteXorY(CPU::INS_LDX_ABSY, &CPU::X, &CPU::Y);

}

TEST_F(test_6502, LoadYRegisterAbsoluteX)
{

  TestLoadRegisterAbsoluteXorY(CPU::INS_LDY_ABSX, &CPU::Y, &CPU::X);

}


TEST_F(test_6502, LoadAccumulatorIndirectX)
{
  // hard code test f
  mem.Data[0xFFFC] = cpu.INS_LDA_INDX;
  mem.Data[0xFFFC + 1] = 0x02;   // zero page address
  cpu.X = 0x02;                  // add X to the zero page address
  mem.Data[0x0004] = 0x37;
  mem.Data[0x0037] = 0x01;
  mem.Data[0x0038] = 0x02;
  mem.Data[0x0201] = 0xA0;
  int32_t Cycles = 6;
  // before we execute we take a copy of the cpu for verifying status flags
  CPU cpu_copy = cpu;
  
  signed int CyclesLeftover = cpu.Execute(Cycles, mem);

  // preverjamo ali je v Accumulatorju pravilna vrednost
  EXPECT_EQ(cpu.A, 0xA0);
  // preverjamo ali smo porabili use predvidene cikle
  EXPECT_EQ(CyclesLeftover, 0);

  // potrebno je preveriti tudi status register
  EXPECT_EQ(cpu.N, 0x01);
  EXPECT_EQ(cpu.Z, 0x00);
  
  VerifyUnmodifiedFlagsFromLDA(cpu, cpu_copy); 
    
}

TEST_F(test_6502, LoadAccumulatorIndirectXZeroPageWarpAround)
{
  // hard code test f
  mem.Data[0xFFFC] = cpu.INS_LDA_INDX;
  mem.Data[0xFFFC + 1] = 0xFF;   // zero page address
  cpu.X = 0x02;                  // add X to the zero page address
  mem.Data[0x0001] = 0x37;
  mem.Data[0x0037] = 0x01;
  mem.Data[0x0038] = 0x02;
  mem.Data[0x0201] = 0xA0;
  int32_t Cycles = 6;
  // before we execute we take a copy of the cpu for verifying status flags
  CPU cpu_copy = cpu;
  
  signed int CyclesLeftover = cpu.Execute(Cycles, mem);

  // preverjamo ali je v Accumulatorju pravilna vrednost
  EXPECT_EQ(cpu.A, 0xA0);
  // preverjamo ali smo porabili use predvidene cikle
  EXPECT_EQ(CyclesLeftover, 0);

  // potrebno je preveriti tudi status register
  EXPECT_EQ(cpu.N, 0x01);
  EXPECT_EQ(cpu.Z, 0x00);
  
  VerifyUnmodifiedFlagsFromLDA(cpu, cpu_copy); 
    
}



TEST_F(test_6502, LoadAccumulatorIndirectY)
{
  // hard code test f
  mem.Data[0xFFFC] = cpu.INS_LDA_INDY;
  mem.Data[0xFFFC + 1] = 0x02;   // zero page address
  cpu.Y = 0x02;                  // add X to the zero page address
  mem.Data[0x0002] = 0x37;
  mem.Data[0x0037] = 0x01;
  mem.Data[0x0038] = 0x02;
  mem.Data[0x0203] = 0xA0;
  int32_t Cycles = 5;
  // before we execute we take a copy of the cpu for verifying status flags
  CPU cpu_copy = cpu;
  
  signed int CyclesLeftover = cpu.Execute(Cycles, mem);

  // preverjamo ali je v Accumulatorju pravilna vrednost
  EXPECT_EQ(cpu.A, 0xA0);
  // preverjamo ali smo porabili use predvidene cikle
  EXPECT_EQ(CyclesLeftover, 0);

  // potrebno je preveriti tudi status register
  EXPECT_EQ(cpu.N, 0x01);
  EXPECT_EQ(cpu.Z, 0x00);
  
  VerifyUnmodifiedFlagsFromLDA(cpu, cpu_copy); 
    
}

TEST_F(test_6502, LoadAccumulatorIndirectYPageCrossedPlus1Cycle)
{
  // hard code test f
  mem.Data[0xFFFC] = cpu.INS_LDA_INDY;
  mem.Data[0xFFFC + 1] = 0x02;   // zero page address
  cpu.Y = 0x02;                  // add Y to the zero page address
  mem.Data[0x0002] = 0x37;
  mem.Data[0x0037] = 0xFF;
  mem.Data[0x0038] = 0x00;
  mem.Data[0x0101] = 0xA0;
  int32_t Cycles = 6;
  // before we execute we take a copy of the cpu for verifying status flags
  CPU cpu_copy = cpu;
  
  signed int CyclesLeftover = cpu.Execute(Cycles, mem);

  // preverjamo ali je v Accumulatorju pravilna vrednost
  EXPECT_EQ(cpu.A, 0xA0);
  // preverjamo ali smo porabili use predvidene cikle
  EXPECT_EQ(CyclesLeftover, 0);

  // potrebno je preveriti tudi status register
  EXPECT_EQ(cpu.N, 0x01);
  EXPECT_EQ(cpu.Z, 0x00);
  
  VerifyUnmodifiedFlagsFromLDA(cpu, cpu_copy); 
    
}







int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}



// 0x35   -> xxxx xxxx 
//           0011 0101

// 0x4A6F  -> xxxx xxxx xxxx xxxx
//            0100 1010 0110 1111