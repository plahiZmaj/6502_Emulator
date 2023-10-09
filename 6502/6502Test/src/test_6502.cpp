#include <gtest/gtest.h>
#include <iostream>
#include <stdint.h>
#include "main_6502.h"


class test_6502 : public testing::Test
{

public:

    Memory mem;
    CPU cpu;

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

  signed int CyclesLeftover = cpu.Execute(Cycles, mem);

  EXPECT_EQ(CyclesLeftover, -1);
    
}


TEST_F(test_6502, LoadAccumulatorImidiateMode)
{
  // hard code test f
  mem.Data[0xFFFC] = cpu.INS_LDA_IM;
  mem.Data[0xFFFC + 1] = 0x42;
  int32_t Cycles = 2;
  // before we execute we take a copy of the cpu for verifying status flags
  CPU cpu_copy = cpu;

  signed int CyclesLeftover = cpu.Execute(Cycles, mem);

  // preverjamo ali je v Accumulatorju pravilna vrednost
  EXPECT_EQ(cpu.A, 0x42);
  // preverjamo ali smo porabili use predvidene cikle
  EXPECT_EQ(CyclesLeftover, 0);
  // potrebno je preveriti tudi status register
  EXPECT_EQ(cpu.N, 0x00);
  EXPECT_EQ(cpu.Z, 0x00);
    
  VerifyUnmodifiedFlagsFromLDA(cpu, cpu_copy);  
    
}

TEST_F(test_6502, LoadAccumulatorZeroPage)
{
  // hard code test f
  mem.Data[0xFFFC] = cpu.INS_LDA_ZP;
  mem.Data[0xFFFC + 1] = 0x42;
  mem.Data[0x0042] = 0x00;
  int32_t Cycles = 3;
  // before we execute we take a copy of the cpu for verifying status flags
  CPU cpu_copy = cpu;

  signed int CyclesLeftover = cpu.Execute(Cycles, mem);

  // preverjamo ali je v Accumulatorju pravilna vrednost
  EXPECT_EQ(cpu.A, 0x00);
  // preverjamo ali smo porabili use predvidene cikle
  EXPECT_EQ(CyclesLeftover, 0);
  // potrebno je preveriti tudi status register
  EXPECT_EQ(cpu.N, 0x00);
  EXPECT_EQ(cpu.Z, 0x01);

  VerifyUnmodifiedFlagsFromLDA(cpu, cpu_copy); 
  
}


TEST_F(test_6502, LoadAccumulatorZeroPageAndAddX)
{
  // hard code test f
  mem.Data[0xFFFC] = cpu.INS_LDA_ZPX;
  mem.Data[0xFFFC + 1] = 0x42;
  mem.Data[0x75] = 0x33;
  cpu.X = 0x33;
  int32_t Cycles = 4;
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




int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
