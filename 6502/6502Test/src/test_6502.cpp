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
      std::cout << "CPU reset" << std::endl;
    }

    virtual void TearDown()
    {

    }


};


TEST_F(test_6502, LoadAccumulatorImidiateMode)
{
    // hard code test f
    mem.Data[0xFFFC] = cpu.INS_LDA_IM;
    mem.Data[0xFFFC + 1] = 0x42;
    uint32_t Cycles = 2;
    cpu.Execute(Cycles, mem);

    // preverjamo ali je v Accumulatorju pravilna vrednost
    EXPECT_EQ(cpu.A, 0x42);
    // potrebno je preveriti tudi status register
    EXPECT_EQ(cpu.C, 0x00);
    EXPECT_EQ(cpu.Z, 0x00);
    EXPECT_EQ(cpu.I, 0x00);
    EXPECT_EQ(cpu.D, 0x00);
    EXPECT_EQ(cpu.B, 0x00);
    EXPECT_EQ(cpu.V, 0x00);
    EXPECT_EQ(cpu.N, 0x00);
}

TEST_F(test_6502, LoadAccumulatorZeroPage)
{
    // hard code test f
    mem.Data[0xFFFC] = cpu.INS_LDA_ZP;
    mem.Data[0xFFFC + 1] = 0x42;
    mem.Data[0x0042] = 0x00;
    uint32_t Cycles = 3;
    cpu.Execute(Cycles, mem);

    // preverjamo ali je v Accumulatorju pravilna vrednost
    EXPECT_EQ(cpu.A, 0x00);
    // potrebno je preveriti tudi status register
    EXPECT_EQ(cpu.C, 0x00);
    EXPECT_EQ(cpu.Z, 0x01);
    EXPECT_EQ(cpu.I, 0x00);
    EXPECT_EQ(cpu.D, 0x00);
    EXPECT_EQ(cpu.B, 0x00);
    EXPECT_EQ(cpu.V, 0x00);
    EXPECT_EQ(cpu.N, 0x00);
}


TEST_F(test_6502, LoadAccumulatorZeroPageAndAddX)
{
    // hard code test f
    mem.Data[0xFFFC] = cpu.INS_LDA_ZPX;
    mem.Data[0xFFFC + 1] = 0x42;
    mem.Data[0x75] = 0x33;
    cpu.X = 0x33;
    uint32_t Cycles = 4;
    cpu.Execute(Cycles, mem);

    // preverjamo ali je v Accumulatorju pravilna vrednost
    EXPECT_EQ(cpu.A, 0x33);
    // potrebno je preveriti tudi status register
    EXPECT_EQ(cpu.C, 0x00);
    EXPECT_EQ(cpu.Z, 0x00);
    EXPECT_EQ(cpu.I, 0x00);
    EXPECT_EQ(cpu.D, 0x00);
    EXPECT_EQ(cpu.B, 0x00);
    EXPECT_EQ(cpu.V, 0x00);
    EXPECT_EQ(cpu.N, 0x00);
}




int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
