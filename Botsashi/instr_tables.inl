#define instruction(mask, val, func, dasm) {mask, val, bind(&Botsashi::m68k_##func, this, _1), bind(&Botsashi::m68kdis_##dasm, this, _1, _2)}

/*
  Inspired partly by 'dis68k' (https://github.com/wrm-za/dis68k), the following instruction
  table draws from the M68000 Programmer's Reference Manual, currently available at
  https://www.nxp.com/files-static/archives/doc/ref_manual/M68000PRM.pdf

  After each line is the internal page number on which documentation of a specific
  instruction mapping can be found, followed by the corresponding page number within the PDF
  linked above.

  Note: Using an initializer_list causes obscure compilation errors, so a vector is used here instead.
*/

vector<m68kmapping> funcmappings =
{
    instruction(0xF1F0, 0xC100, unknown, unknown), // 4-3 (p107)
    instruction(0xF1F0, 0x8100, unknown, unknown), // 4-171 (p275)
    instruction(0xFFC0, 0x4800, unknown, unknown), // 4-142 (p246)

    instruction(0xF1C0, 0xC000, and<Byte>, and<Byte>), // 4-15 (p119)
    instruction(0xF1C0, 0xC040, and<Word>, and<Word>), // 4-15 (p119)
    instruction(0xF1C0, 0xC080, and<Long>, and<Long>), // 4-15 (p119)

    instruction(0xF1C0, 0xC100, unknown, unknown), // 4-15 (p119)
    instruction(0xF1C0, 0xC140, unknown, unknown), // 4-15 (p119)
    instruction(0xF1C0, 0xC180, unknown, unknown), // 4-15 (p119)

    instruction(0xF1C0, 0x8000, or<Byte>, or<Byte>), // 4-150 (p254)
    instruction(0xF1C0, 0x8040, or<Word>, or<Word>), // 4-150 (p254)
    instruction(0xF1C0, 0x8080, or<Long>, or<Long>), // 4-150 (p254)

    instruction(0xF1C0, 0x8100, unknown, unknown), // 4-150 (p254)
    instruction(0xF1C0, 0x8140, unknown, unknown), // 4-150 (p254)
    instruction(0xF1C0, 0x8180, unknown, unknown), // 4-150 (p254)

    instruction(0xF1C0, 0xB100, unknown, unknown), // 4-100 (p204)
    instruction(0xF1C0, 0xB140, unknown, unknown), // 4-100 (p204)
    instruction(0xF1C0, 0xB180, unknown, unknown), // 4-100 (p204)

    instruction(0xFFC0, 0x0600, unknown, unknown), // 4-9 (p113)
    instruction(0xFFC0, 0x0640, unknown, unknown), // 4-9 (p113)
    instruction(0xFFC0, 0x0680, unknown, unknown), // 4-9 (p113)

    instruction(0xFFC0, 0x0200, unknown, unknown), // 4-18 (p122)
    instruction(0xFFC0, 0x0240, unknown, unknown), // 4-18 (p122)
    instruction(0xFFC0, 0x0280, unknown, unknown), // 4-18 (p122)

    instruction(0xFFC0, 0x0000, ori<Byte>, ori<Byte>), // 4-153 (p257)
    instruction(0xFFC0, 0x0040, ori<Word>, ori<Word>), // 4-153 (p257)
    instruction(0xFFC0, 0x0080, ori<Long>, ori<Long>), // 4-153 (p257)

    instruction(0xFFC0, 0x0A00, eori<Byte>, eori<Byte>), // 4-102 (p206)
    instruction(0xFFC0, 0x0A40, eori<Word>, eori<Word>), // 4-102 (p206)
    instruction(0xFFC0, 0x0A80, eori<Long>, eori<Long>), // 4-102 (p206)

    instruction(0xFFC0, 0x0400, unknown, unknown), // 4-179 (p283)
    instruction(0xFFC0, 0x0440, unknown, unknown), // 4-179 (p283)
    instruction(0xFFC0, 0x0480, unknown, unknown), // 4-179 (p283)

    instruction(0xF000, 0x1000, move<Byte>, move<Byte>), // 4-116 (p220)
    instruction(0xF000, 0x2000, move<Long>, move<Long>), // 4-116 (p220)
    instruction(0xF000, 0x3000, move<Word>, move<Word>), // 4-116 (p220)

    instruction(0xFFC0, 0x46C0, unknown, unknown), // 6-19 (p473)
    instruction(0xFFC0, 0x44C0, unknown, unknown), // 4-123 (p227)
    instruction(0xFFC0, 0x40C0, unknown, unknown), // 6-17 (p471)

    instruction(0xF1C0, 0xB000, unknown, unknown), // 4-75 (p179)
    instruction(0xF1C0, 0xB040, unknown, unknown), // 4-75 (p179)
    instruction(0xF1C0, 0xB080, unknown, unknown), // 4-75 (p179)

    instruction(0xF1C0, 0xB0C0, unknown, unknown), // 4-77 (p181)
    instruction(0xF1C0, 0xB1C0, unknown, unknown), // 4-77 (p181)

    instruction(0xFFC0, 0x0C00, unknown, unknown), // 4-79 (p183)
    instruction(0xFFC0, 0x0C40, unknown, unknown), // 4-79 (p183)
    instruction(0xFFC0, 0x0C80, unknown, unknown), // 4-79 (p183)

    instruction(0xF1F8, 0xB108, unknown, unknown), // 4-81 (p185)
    instruction(0xF1F8, 0xB148, unknown, unknown), // 4-81 (p185)
    instruction(0xF1F8, 0xB188, unknown, unknown), // 4-81 (p185)

    instruction(0xF000, 0x6000, unknown, unknown), // 4-25 (p129), 4-59 (p163) and 4-55 (p159)

    instruction(0xF1C0, 0x41C0, lea, lea), // 4-110 (p214)
    instruction(0xFFC0, 0x4840, unknown, unknown), // 4-159 (p263)

    instruction(0xF100, 0x7000, unknown, unknown), // 4-134 (p238)

    instruction(0xFFFF, 0x4E70, unknown, unknown), // 6-83 (p537)

    instruction(0xFFC0, 0x4EC0, unknown, unknown), // 4-108 (p212)
    instruction(0xFFC0, 0x4E80, unknown, unknown), // 4-109 (p213)
    instruction(0xFFFF, 0x4E75, unknown, unknown), // 4-169 (p273)

    instruction(0xF1C0, 0x9000, sub<Byte>, sub<Byte>), // 4-174 (p278)
    instruction(0xF1C0, 0x9040, sub<Word>, sub<Word>), // 4-174 (p278)
    instruction(0xF1C0, 0x9080, sub<Long>, sub<Long>), // 4-174 (p278)

    instruction(0xF1C0, 0x9100, subr<Byte>, subr<Byte>), // 4-174 (p278)
    instruction(0xF1C0, 0x9140, subr<Word>, subr<Word>), // 4-174 (p278)
    instruction(0xF1C0, 0x9180, subr<Long>, subr<Long>), // 4-174 (p278)

    instruction(0xF1C0, 0xD000, add<Byte>, add<Byte>), // 4-4 (p108)
    instruction(0xF1C0, 0xD040, add<Word>, add<Word>), // 4-4 (p108)
    instruction(0xF1C0, 0xD080, add<Long>, add<Long>), // 4-4 (p108)

    instruction(0xF1C0, 0xD100, addr<Byte>, addr<Byte>), // 4-4 (p108)
    instruction(0xF1C0, 0xD140, addr<Word>, addr<Word>), // 4-4 (p108)
    instruction(0xF1C0, 0xD180, addr<Long>, addr<Long>), // 4-4 (p108)

    instruction(0xF1C0, 0xD0C0, unknown, unknown), // 4-7 (p111)
    instruction(0xF1C0, 0xD1C0, unknown, unknown), // 4-7 (p111)
    instruction(0xF1C0, 0x90C0, unknown, unknown), // 4-177 (p281)
    instruction(0xF1C0, 0x91C0, unknown, unknown), // 4-177 (p281)

    instruction(0xF1C0, 0x5000, unknown, unknown), // 4-11 (p115)
    instruction(0xF1C0, 0x5040, unknown, unknown), // 4-11 (p115)
    instruction(0xF1C0, 0x5080, unknown, unknown), // 4-11 (p115)

    instruction(0xF1C0, 0x5100, unknown, unknown), // 4-181 (p285)
    instruction(0xF1C0, 0x5140, unknown, unknown), // 4-181 (p285)
    instruction(0xF1C0, 0x5180, unknown, unknown), // 4-181 (p285)

    instruction(0xF1F0, 0xD100, unknown, unknown), // 4-14 (p118)
    instruction(0xF1F0, 0xD140, unknown, unknown), // 4-14 (p118)
    instruction(0xF1F0, 0xD180, unknown, unknown), // 4-14 (p118)

    instruction(0xF1F0, 0x9100, unknown, unknown), // 4-184 (p288)
    instruction(0xF1F0, 0x9140, unknown, unknown), // 4-184 (p288)
    instruction(0xF1F0, 0x9180, unknown, unknown), // 4-184 (p288)

    instruction(0xF1C0, 0x0100, unknown, unknown), // 4-62 (p166)
    instruction(0xFFC0, 0x0800, unknown, unknown), // 4-63 (p167)

    instruction(0xF1C0, 0x0180, bclr, bclr), // 4-31 (p135)
    instruction(0xFFC0, 0x0880, bclrimm, bclrimm), // 4-32 (p136)

    instruction(0xF0C0, 0x50C0, unknown, unknown), // Scc: 4-173 (p276), DBcc: 4-91 (p195)

    instruction(0xFFC0, 0x4200, clear<Byte>, clear<Byte>), // 4-73 (p177)
    instruction(0xFFC0, 0x4240, clear<Word>, clear<Word>), // 4-73 (p177)
    instruction(0xFFC0, 0x4280, clear<Long>, clear<Long>), // 4-73 (p177)
    instruction(0xFFC0, 0x4400, unknown, unknown), // 4-144 (p248)
    instruction(0xFFC0, 0x4440, unknown, unknown), // 4-144 (p248)
    instruction(0xFFC0, 0x4480, unknown, unknown), // 4-144 (p248)
    instruction(0xFFC0, 0x4000, unknown, unknown), // 4-146 (p250)
    instruction(0xFFC0, 0x4040, unknown, unknown), // 4-146 (p250)
    instruction(0xFFC0, 0x4080, unknown, unknown), // 4-146 (p250)
    instruction(0xFFC0, 0x4600, not<Byte>, not<Byte>), // 4-148 (p252)
    instruction(0xFFC0, 0x4640, not<Word>, not<Word>), // 4-148 (p252)
    instruction(0xFFC0, 0x4680, not<Long>, not<Long>), // 4-148 (p252)

    instruction(0xF1D8, 0xE100, unknown, unknown), // 4-22 (p126)
    instruction(0xF1D8, 0xE140, unknown, unknown), // 4-22 (p126)
    instruction(0xF1D8, 0xE180, unknown, unknown), // 4-22 (p126)
    instruction(0xFFC0, 0xE1C0, unknown, unknown), // 4-22 (p126)

    instruction(0xF1D8, 0xE000, unknown, unknown), // 4-22 (p126)
    instruction(0xF1D8, 0xE040, unknown, unknown), // 4-22 (p126)
    instruction(0xF1D8, 0xE080, unknown, unknown), // 4-22 (p126)
    instruction(0xFFC0, 0xE0C0, unknown, unknown), // 4-22 (p126)

    instruction(0xF1D8, 0xE108, unknown, unknown), // 4-113 (p217)
    instruction(0xF1D8, 0xE148, unknown, unknown), // 4-113 (p217)
    instruction(0xF1D8, 0xE188, unknown, unknown), // 4-113 (p217)
    instruction(0xFFC0, 0xE3C0, unknown, unknown), // 4-113 (p217)

    instruction(0xF1D8, 0xE008, unknown, unknown), // 4-113 (p217)
    instruction(0xF1D8, 0xE048, unknown, unknown), // 4-113 (p217)
    instruction(0xF1D8, 0xE088, unknown, unknown), // 4-113 (p217)
    instruction(0xFFC0, 0xE2C0, unknown, unknown), // 4-113 (p217)

    instruction(0xF1D8, 0xE118, unknown, unknown), // 4-160 (p264)
    instruction(0xF1D8, 0xE158, unknown, unknown), // 4-160 (p264)
    instruction(0xF1D8, 0xE198, unknown, unknown), // 4-160 (p264)
    instruction(0xFFC0, 0xE7C0, unknown, unknown), // 4-160 (p264)

    instruction(0xF1D8, 0xE018, unknown, unknown), // 4-160 (p264)
    instruction(0xF1D8, 0xE058, unknown, unknown), // 4-160 (p264)
    instruction(0xF1D8, 0xE098, unknown, unknown), // 4-160 (p264)
    instruction(0xFFC0, 0xE6C0, unknown, unknown), // 4-160 (p264)

    instruction(0xF1D8, 0xE110, unknown, unknown), // 4-163 (p267)
    instruction(0xF1D8, 0xE150, unknown, unknown), // 4-163 (p267)
    instruction(0xF1D8, 0xE190, unknown, unknown), // 4-163 (p267)
    instruction(0xFFC0, 0xE5C0, unknown, unknown), // 4-163 (p267)

    instruction(0xF1D8, 0xE010, unknown, unknown), // 4-163 (p267)
    instruction(0xF1D8, 0xE050, unknown, unknown), // 4-163 (p267)
    instruction(0xF1D8, 0xE090, unknown, unknown), // 4-163 (p267)
    instruction(0xFFC0, 0xE4C0, unknown, unknown), // 4-163 (p267)

    instruction(0xFFC0, 0x48C0, unknown, unknown), // 4-128 (p232)
    instruction(0xFFC0, 0x4880, unknown, unknown), // 4-128 (p232)
    instruction(0xFFC0, 0x4CC0, unknown, unknown), // 4-128 (p232)
    instruction(0xFFC0, 0x4C80, unknown, unknown), // 4-128 (p232)

    instruction(0xF1F8, 0x0108, unknown, unknown), // 4-133 (p237)
    instruction(0xF1F8, 0x0148, unknown, unknown), // 4-133 (p237)
    instruction(0xF1F8, 0x0188, unknown, unknown), // 4-133 (p237)
    instruction(0xF1F8, 0x01C8, unknown, unknown), // 4-133 (p237)

    instruction(0xFFC0, 0x4A00, unknown, unknown), // 4-192 (p296)
    instruction(0xFFC0, 0x4A40, unknown, unknown), // 4-192 (p296)
    instruction(0xFFC0, 0x4A80, unknown, unknown), // 4-192 (p296)

    instruction(0xF1C0, 0xC0C0, mulu, mulu), // 4-139 (p243)
    instruction(0xF1C0, 0xC1C0, unknown, unknown), // 4-136 (p240)

    instruction(0xF1C0, 0x80C0, unknown, unknown), // 4-97 (p201)
    instruction(0xF1C0, 0x81C0, unknown, unknown), // 4-93 (p197)

    instruction(0xFFF0, 0x4E60, unknown, unknown), // 6-21 (p475)

    instruction(0xFFF0, 0x4E40, trap, trap), // 4-188 (p292)
    instruction(0xFFFF, 0x4E76, unknown, unknown), // 4-191 (p295)
    instruction(0xF1C0, 0x4180, unknown, unknown), // 4-69 (p173)

    instruction(0xFFFF, 0x4E77, unknown, unknown), // 4-168 (p272) [RTR]
    instruction(0xFFFF, 0x4E73, unknown, unknown), // 6-84 (p538) [RTE]

    instruction(0xFFFF, 0x4E71, unknown, unknown), // 8-13 (p469)

    instruction(0xF1F8, 0xC140, exgdreg, exgdreg), // 4-105 (p209)
    instruction(0xF1F8, 0xC148, unknown, unknown), // 4-105 (p209)
    instruction(0xF1F8, 0xC188, unknown, unknown), // 4-105 (p209)

    instruction(0xFFF8, 0x4840, swap, swap), // 4-185 (p289)

    instruction(0xFFFF, 0x027C, unknown, unknown),
    instruction(0xFFFF, 0x023C, unknown, unknown),
    instruction(0xFFFF, 0x0A7C, unknown, unknown),
    instruction(0xFFFF, 0x0A3C, unknown, unknown),
    instruction(0xFFFF, 0x007C, unknown, unknown),
    instruction(0xFFFF, 0x003C, unknown, unknown),

    instruction(0xF1C0, 0x0140, bchg, bchg), // 4-28 (p132)
    instruction(0xFFC0, 0x0840, bchgimm, bchgimm), // 4-29 (p133)
    instruction(0xF1C0, 0x01C0, bset, bset), // 4-57 (p161)
    instruction(0xFFC0, 0x08C0, bsetimm, bsetimm), // 4-58 (p162)

    instruction(0xFFC0, 0x4AC0, unknown, unknown), // 4-186 (p290)

    instruction(0xFFF8, 0x4880, unknown, unknown), // 4-106 (p210)
    instruction(0xFFF8, 0x48C0, unknown, unknown), // 4-106 (p210)

    instruction(0xFFF8, 0x4E50, unknown, unknown), // 4-111 (p215)
    instruction(0xFFF8, 0x4E58, unknown, unknown), // 4-194 (p298)

    instruction(0xFFFF, 0x4E72, stop, stop), // 6-85 (p539)
};