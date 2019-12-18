#ifndef BOTSASHI_H
#define BOTSASHI_H

#include <cstdint>
#include <iostream>
#include <array>
#include <functional>
#include <stddef.h>
using namespace std;

#define BYTE_WIDTH 0
#define WORD_WIDTH 1
#define LONG_WIDTH 2

namespace botsashi
{

  inline bool TestBit(uint32_t reg, int bit)
  {
    return (reg & (1 << bit)) ? true : false;
  }

  inline uint32_t BitSet(uint32_t reg, int bit)
  {
    return (reg | (1 << bit));
  }

  inline uint32_t BitReset(uint32_t reg, int bit)
  {
    return (reg & ~(1 << bit));
  }

  inline uint32_t BitChange(uint32_t reg, int bit, bool cond)
  {
    return (cond) ? BitSet(reg, bit) : BitReset(reg, bit);
  }

  class BotsashiInterface
  {
    public:
      BotsashiInterface();
      ~BotsashiInterface();

      virtual uint8_t readByte(uint32_t addr) = 0;
      virtual void writeByte(uint32_t addr, uint8_t val) = 0;
      virtual uint16_t readWord(uint32_t addr) = 0;
      virtual void writeWord(uint32_t addr, uint16_t val) = 0;
      virtual uint32_t readLong(uint32_t addr) = 0;
      virtual void writeLong(uint32_t addr, uint32_t val) = 0;
  };

  class Botsashi
  {
    public:
      Botsashi();
      ~Botsashi();

      using trapfunc = function<void(int, Botsashi*)>;
      trapfunc trapexcep;

      inline void settrapfunc(trapfunc cb)
      {
        trapexcep = cb;
      }

      BotsashiInterface *inter = NULL;

      struct m68kregisters
      {
        array<uint32_t, 8> datareg;
        array<uint32_t, 8> addrreg;
        uint32_t usp = 0;
        uint32_t ssp = 0;
        uint32_t pc = 0;
        uint16_t sr = 0;

        bool getx()
        {
          return TestBit(sr, 4);
        }

        bool getz()
        {
          return TestBit(sr, 2);
        }

        void setflags(bool x, bool n, bool z, bool v, bool c)
        {
          sr = BitChange(sr, 4, x);
          sr = BitChange(sr, 3, n);
          sr = BitChange(sr, 2, z);
          sr = BitChange(sr, 1, v);
          sr = BitChange(sr, 0, c);
        }

        uint32_t getdatareg(int reg)
        {
          return datareg[reg];
        }

        uint32_t getaddrreg(int reg)
        {
          return addrreg[reg];
        }

        void setdatareg(int reg, uint32_t val)
        {
          datareg[reg] = val;
        }

        void setaddrreg(int reg, uint32_t val)
        {
          if (reg == 7)
          {
            usp = val;
          }

          addrreg[reg] = val;
        }
      };

      m68kregisters m68kreg;

      void init();
      void executenextinstr();
      void executeinstr(uint16_t instr);

      uint16_t currentinstr = 0;
      bool stopped = false;

      void setinterface(BotsashiInterface *connected)
      {
        inter = connected;
      }

      uint8_t readByte(uint32_t addr)
      {
        if (inter != NULL)
        {
          return inter->readByte(addr);
        }
        else
        {
          return 0xFF;
        }
      }

      uint16_t readWord(uint32_t addr)
      {
        if (inter != NULL)
        {
          return inter->readWord(addr);
        }
        else
        {
          return 0xFFFF;
        }
      }

      uint32_t readLong(uint32_t addr)
      {
        if (inter != NULL)
        {
          return inter->readLong(addr);
        }
        else
        {
          return 0xFFFFFFFF;
        }
      }

      inline uint32_t srcaddressingmode(int mode, int reg, int buswidth)
      {
        uint32_t temp = 0;

        switch (mode)
        {
          case 0:
          {
            uint32_t datareg = m68kreg.getdatareg(reg);

            switch (buswidth)
            {
              case 0:
              {
                temp = (datareg & 0xFF);
              }
              break;
              case 1:
              {
                temp = (datareg & 0xFFFF);
              }
              break;
              case 2:
              {
                temp = datareg;
              }
              break;
            }
          }
          break;
          case 3:
          {
            uint32_t addr = m68kreg.getaddrreg(reg);

            switch (buswidth)
            {
              case 0:
              {
                temp = readByte(addr);
                m68kreg.setaddrreg(reg, (addr + 1));
              }
              break;
              case 1:
              {
                temp = readWord(addr);
                m68kreg.setaddrreg(reg, (addr + 2));
              }
              break;
              case 2:
              {
                temp = readLong(addr);
                m68kreg.setaddrreg(reg, (addr + 4));
              }
              break;
            }
          }
          break;
          case 7:
          {
            switch (reg)
            {
              case 2:
              {
                temp = m68kreg.pc;
                uint16_t immdis = readWord(m68kreg.pc);
                m68kreg.pc += 2;
                temp += (uint32_t)(int16_t)(immdis);
              }
              break;
              case 4:
              {
                switch (buswidth)
                {
                  case 0:
                  {
                    temp = (readWord(m68kreg.pc) & 0xFF);
                    m68kreg.pc += 2;
                  }
                  break;
                  case 1:
                  {
                    temp = (readWord(m68kreg.pc));
                    m68kreg.pc += 2;
                  }
                  break;
                  case 2:
                  {
                    temp = (readLong(m68kreg.pc));
                    m68kreg.pc += 4;
                  }
                  break;
                }
              }
              break;
              default: cout << "Unrecognized register mode of " << dec << (int)(reg) << endl; exit(1); break;
            }
          }
          break;
          default: cout << "Unrecognized addressing mode of " << dec << (int)(mode) << endl; exit(1); break;
        }

        return temp;
      }

      inline void dstaddressingmode(int mode, int reg, int buswidth, uint32_t val)
      {
        switch (mode)
        {
          case 0:
          {
            uint32_t temp = (m68kreg.getdatareg(reg));

            switch (buswidth)
            {
              case 0:
              {
                temp = ((temp & ~0xFF) | val);
              }
              break;
              case 1:
              {
                temp = ((temp & ~0xFFFF) | val);
              }
              break;
              case 2:
              {
                temp = val;
              }
              break;
            }

            m68kreg.setdatareg(reg, temp);
          }
          break;
          default: cout << "Unrecognized addressing mode of " << dec << (int)(mode) << endl; exit(1); break;
        }
      }

      inline bool getcond(int val)
      {
        bool temp = false;

        switch (val)
        {
          case 0: temp = true; break;
          case 1: temp = false; break;
          case 6: temp = !m68kreg.getz(); break;
          case 7: temp = m68kreg.getz(); break;
          default: cout << "Unrecognized condition code of " << dec << (int)(val) << endl; exit(1); break;
        }

        return temp;
      }
  };
};

#endif // BOTSASHI_H