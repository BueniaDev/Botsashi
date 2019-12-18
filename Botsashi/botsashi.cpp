#include "botsashi.h"
#include "botsashi_tables.h"
using namespace botsashi;

BotsashiInterface::BotsashiInterface()
{

}

BotsashiInterface::~BotsashiInterface()
{

}

Botsashi::Botsashi()
{

}

Botsashi::~Botsashi()
{

}

void Botsashi::init()
{
  for (int i = 0; i < 8; i++)
  {
    m68kreg.addrreg[i] = 0;
  }
}

void Botsashi::executenextinstr()
{
  currentinstr = readWord(m68kreg.pc);
  m68kreg.pc += 2;
  // cout << hex << (int)(currentinstr) << endl;
  executeinstr(currentinstr);
}

void Botsashi::executeinstr(uint16_t instr)
{
  for (int i = 0; i < (int)(masktable.size()); i++)
  {
    if ((instr & masktable[i]) == resulttable[i])
    {
      // cout << hex << (int)(resulttable[i]) << endl;
      functable[resulttable[i]](this);
    }
  }
}