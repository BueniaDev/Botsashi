#include <iostream>
#include <array>
#include <vector>
#include <functional>
#include "botsashi.h"
using namespace std;
using namespace std::placeholders;
using namespace botsashi;

vector<uint8_t> memory(0x1000000, 0);

array<uint8_t, 24> program = 
{
  0x30, 0x3C, 0x01, 0x23, // move.w #$0123,d0
  0x12, 0x00, // move.b d0, d1
  0x34, 0x01, // move.w d1, d2
  0x26, 0x3C, 0xFF, 0xFF, 0x00, 0x00, // move.l #$FFFF0000, d3
  0x36, 0x00, // move.w d0, d3
  0x20, 0x03, // move.l d3, d0
  0x30, 0x02, // move.w d2, d0
  0x4E, 0x72, 0x27, 0x00 // stop #2700
};

/*
array<uint8_t, 35> program = 
{
  0x41, 0xFA, 0x00, 0x12,
	0x12, 0x18,
	0x67, 0x08,
	0x10, 0x3C, 0x00, 0x06,
	0x4E, 0x4F,
	0x60, 0xF4,
	0x4E, 0x72, 0x27, 0x00,
	0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x77, 0x6F, 0x72, 0x6C, 0x64, 0x21, 0x0D, 0x0A, 0x00,
};
*/

class TestInterface : public BotsashiInterface
{
  public:
    TestInterface();
    ~TestInterface();

    uint8_t readByte(uint32_t addr)
    {
      addr = (addr % 0x1000000);
      return memory[addr];
    }

    void writeByte(uint32_t addr, uint8_t val)
    {
      addr = (addr % 0x1000000);
      memory[addr] = val;
    }

    uint16_t readWord(uint32_t addr)
    {
      addr = (addr % 0x1000000);
      return ((readByte(addr) << 8) | readByte(addr + 1));
    }

    void writeWord(uint32_t addr, uint16_t val)
    {
      addr = (addr % 0x1000000);
      writeByte((addr + 1), (val & 0xFF));
      writeByte(addr, (val >> 8));
    }

    uint32_t readLong(uint32_t addr)
    {
      addr = (addr % 0x1000000);
      return ((readWord(addr) << 16) | readByte(addr + 2));
    }

    void writeLong(uint32_t addr, uint32_t val)
    {
      addr = (addr % 0x1000000);
      writeWord((addr + 2), (val & 0xFFFF));
      writeWord(addr, (val >> 16));
    }
};

TestInterface::TestInterface()
{

}

TestInterface::~TestInterface()
{

}

void trapfunction(int val, Botsashi *m68k)
{
  if (val == 15)
  {
    int tasknum = (m68k->m68kreg.getdatareg(0));
    switch (tasknum)
    {
      case 6:
      {
        uint8_t character = (m68k->m68kreg.getdatareg(1) & 0xFF);
        cout.put(character);
      }
      break;
      case 9:
      {
        m68k->stopped = true;
      }
      break;
      default: cout << "Unrecognized trap task of " << dec << (int)(tasknum) << endl; exit(1); break;
    }
  }
  else
  {
    return;
  }
}

void copyprogram()
{
  for (int i = 0; i < (int)(program.size()); i++)
  {
    memory[i] = program[i];
  }
}

Botsashi m68k;
TestInterface inter;

int main() 
{
  copyprogram();
  m68k.setinterface(&inter);
  m68k.settrapfunc(bind(&trapfunction, _1, _2));
  m68k.init();

  while (!m68k.stopped)
  {
    m68k.executenextinstr();
  }

  cout << "Program execution finished." << endl;

  return 0;
}