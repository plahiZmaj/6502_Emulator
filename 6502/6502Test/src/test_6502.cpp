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


TEST_F(test_6502, smalltest)
{
  // hard code test f
    mem.Data[0xFFFC] = cpu.INS_LDA_IM;
    mem.Data[0xFFFC + 1] = 0x42;
    uint32_t Cycles = 2;
    cpu.Execute(Cycles, mem);

    EXPECT_EQ(cpu.A, 0x42);


}



int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
