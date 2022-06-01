#define instruction(mask, val, func, dasm) {mask, val, bind(&Botsashi::m68k_##func, this, _1), bind(&Botsashi::m68kdis_##dasm, this, _1, _2, _3)}

/*
  Inspired partly by 'dis68k' (https://github.com/wrm-za/dis68k), the following instruction
  table draws from the M68000 Programmer's Reference Manual, currently available at
  https://www.nxp.com/files-static/archives/doc/ref_manual/M68000PRM.pdf

  After each line is the internal page number on which documentation of a specific
  instruction mapping can be found, followed by the corresponding page number within the PDF
  linked above.

  Note: Using an initializer_list causes obscure compilation errors, so a vector is used here instead.
*/

// Instruction table is 180 instructions
array<int, 17> table_offsets = {
     0,  36,  37, 38,  39,  82,  90,  93, 
    94, 103, 114, 115, 126, 138, 149, 181, 182
};

vector<m68kmapping> funcmappings =
{
    // 0x0
    instruction(0xFFC0, 0x0600, addi<Byte>, addi<Byte>), // 4-9 (p113)
    instruction(0xFFC0, 0x0640, addi<Word>, addi<Word>), // 4-9 (p113)
    instruction(0xFFC0, 0x0680, addi<Long>, addi<Long>), // 4-9 (p113)

    instruction(0xFFC0, 0x0200, andi<Byte>, andi<Byte>), // 4-18 (p122)
    instruction(0xFFC0, 0x0240, andi<Word>, andi<Word>), // 4-18 (p122)
    instruction(0xFFC0, 0x0280, andi<Long>, andi<Long>), // 4-18 (p122)

    instruction(0xFFC0, 0x0000, ori<Byte>, ori<Byte>), // 4-153 (p257)
    instruction(0xFFC0, 0x0040, ori<Word>, ori<Word>), // 4-153 (p257)
    instruction(0xFFC0, 0x0080, ori<Long>, ori<Long>), // 4-153 (p257)

    instruction(0xFFC0, 0x0A00, eori<Byte>, eori<Byte>), // 4-102 (p206)
    instruction(0xFFC0, 0x0A40, eori<Word>, eori<Word>), // 4-102 (p206)
    instruction(0xFFC0, 0x0A80, eori<Long>, eori<Long>), // 4-102 (p206)

    instruction(0xFFC0, 0x0400, subi<Byte>, subi<Byte>), // 4-179 (p283)
    instruction(0xFFC0, 0x0440, subi<Word>, subi<Word>), // 4-179 (p283)
    instruction(0xFFC0, 0x0480, subi<Long>, subi<Long>), // 4-179 (p283)

    instruction(0xFFC0, 0x0C00, cmpi<Byte>, cmpi<Byte>), // 4-79 (p183)
    instruction(0xFFC0, 0x0C40, cmpi<Word>, cmpi<Word>), // 4-79 (p183)
    instruction(0xFFC0, 0x0C80, cmpi<Long>, cmpi<Long>), // 4-79 (p183)

    instruction(0xF1C0, 0x0100, btst, btst), // 4-62 (p166)
    instruction(0xFFC0, 0x0800, btstimm, btstimm), // 4-63 (p167)

    instruction(0xF1C0, 0x0180, bclr, bclr), // 4-31 (p135)
    instruction(0xFFC0, 0x0880, bclrimm, bclrimm), // 4-32 (p136)

    instruction(0xF1C0, 0x0140, bchg, bchg), // 4-28 (p132)
    instruction(0xFFC0, 0x0840, bchgimm, bchgimm), // 4-29 (p133)
    instruction(0xF1C0, 0x01C0, bset, bset), // 4-57 (p161)
    instruction(0xFFC0, 0x08C0, bsetimm, bsetimm), // 4-58 (p162)

    instruction(0xF1F8, 0x0108, unknown, unknown), // 4-133 (p237)
    instruction(0xF1F8, 0x0148, unknown, unknown), // 4-133 (p237)
    instruction(0xF1F8, 0x0188, unknown, unknown), // 4-133 (p237)
    instruction(0xF1F8, 0x01C8, unknown, unknown), // 4-133 (p237)

    instruction(0xFFFF, 0x027C, andi_to_sr, andi_to_sr), // 6-2 (p456)
    instruction(0xFFFF, 0x023C, andi_to_ccr, andi_to_ccr), // 4-20 (p124)
    instruction(0xFFFF, 0x0A7C, eori_to_sr, eori_to_sr), // 6-10 (p464)
    instruction(0xFFFF, 0x0A3C, eori_to_ccr, eori_to_ccr), // 4-104 (p208)
    instruction(0xFFFF, 0x007C, ori_to_sr, ori_to_sr), // 6-27 (p481)
    instruction(0xFFFF, 0x003C, ori_to_ccr, ori_to_ccr), // 4-155 (p259)

    // 0x1
    instruction(0xF000, 0x1000, move<Byte>, move<Byte>), // 4-116 (p220)
    // 0x2
    instruction(0xF000, 0x2000, move<Long>, move<Long>), // 4-116 (p220)
    // 0x3
    instruction(0xF000, 0x3000, move<Word>, move<Word>), // 4-116 (p220)

    // 0x4
    instruction(0xFFC0, 0x4800, unknown, unknown), // 4-142 (p246)

    instruction(0xFFC0, 0x46C0, move_to_sr, move_to_sr), // 6-19 (p473)
    instruction(0xFFC0, 0x44C0, move_to_ccr, move_to_ccr), // 4-123 (p227)
    instruction(0xFFC0, 0x40C0, move_from_sr, move_from_sr), // 6-17 (p471)

    instruction(0xFFFF, 0x4E70, unknown, unknown), // 6-83 (p537)

    instruction(0xFFC0, 0x4EC0, jmp, jmp), // 4-108 (p212)
    instruction(0xFFC0, 0x4E80, jsr, jsr), // 4-109 (p213)
    instruction(0xFFFF, 0x4E75, rts, rts), // 4-169 (p273)

    instruction(0xFFC0, 0x4200, clear<Byte>, clear<Byte>), // 4-73 (p177)
    instruction(0xFFC0, 0x4240, clear<Word>, clear<Word>), // 4-73 (p177)
    instruction(0xFFC0, 0x4280, clear<Long>, clear<Long>), // 4-73 (p177)
    instruction(0xFFC0, 0x4400, neg<Byte>, neg<Byte>), // 4-144 (p248)
    instruction(0xFFC0, 0x4440, neg<Word>, neg<Word>), // 4-144 (p248)
    instruction(0xFFC0, 0x4480, neg<Long>, neg<Long>), // 4-144 (p248)
    instruction(0xFFC0, 0x4000, unknown, unknown), // 4-146 (p250)
    instruction(0xFFC0, 0x4040, unknown, unknown), // 4-146 (p250)
    instruction(0xFFC0, 0x4080, unknown, unknown), // 4-146 (p250)
    instruction(0xFFC0, 0x4600, not<Byte>, not<Byte>), // 4-148 (p252)
    instruction(0xFFC0, 0x4640, not<Word>, not<Word>), // 4-148 (p252)
    instruction(0xFFC0, 0x4680, not<Long>, not<Long>), // 4-148 (p252)

    instruction(0xFFC0, 0x48C0, movem, movem), // 4-128 (p232)
    instruction(0xFFC0, 0x4880, movem, movem), // 4-128 (p232)
    instruction(0xFFC0, 0x4CC0, movem, movem), // 4-128 (p232)
    instruction(0xFFC0, 0x4C80, movem, movem), // 4-128 (p232)

    instruction(0xFFC0, 0x4A00, tst<Byte>, tst<Byte>), // 4-192 (p296)
    instruction(0xFFC0, 0x4A40, tst<Word>, tst<Word>), // 4-192 (p296)
    instruction(0xFFC0, 0x4A80, tst<Long>, tst<Long>), // 4-192 (p296)

    instruction(0xFFF0, 0x4E60, move_usp, move_usp), // 6-21 (p475)

    instruction(0xFFF0, 0x4E40, trap, trap), // 4-188 (p292)
    instruction(0xFFFF, 0x4E76, unknown, unknown), // 4-191 (p295)
    instruction(0xF1C0, 0x4180, unknown, unknown), // 4-69 (p173)

    instruction(0xFFFF, 0x4E77, rtr, rtr), // 4-168 (p272)
    instruction(0xFFFF, 0x4E73, unknown, unknown), // 6-84 (p538)

    instruction(0xFFC0, 0x4AC0, unknown, unknown), // 4-186 (p290)

    instruction(0xFFF8, 0x4880, ext, ext), // 4-106 (p210)
    instruction(0xFFF8, 0x48C0, ext, ext), // 4-106 (p210)

    instruction(0xFFF8, 0x4E50, unknown, unknown), // 4-111 (p215)
    instruction(0xFFF8, 0x4E58, unknown, unknown), // 4-194 (p298)

    instruction(0xF1C0, 0x41C0, lea, lea), // 4-110 (p214)
    instruction(0xFFC0, 0x4840, pea, pea), // 4-159 (p263)

    instruction(0xFFF8, 0x4840, swap, swap), // 4-185 (p289)

    instruction(0xFFFF, 0x4E72, stop, stop), // 6-85 (p539)
    instruction(0xFFFF, 0x4E71, nop, nop), // 4-147 (p251)

    // 0x5

    instruction(0xF1C0, 0x5000, addq<Byte>, addq<Byte>), // 4-11 (p115)
    instruction(0xF1C0, 0x5040, addq<Word>, addq<Word>), // 4-11 (p115)
    instruction(0xF1C0, 0x5080, addq<Long>, addq<Long>), // 4-11 (p115)

    instruction(0xF1C0, 0x5100, subq<Byte>, subq<Byte>), // 4-181 (p285)
    instruction(0xF1C0, 0x5140, subq<Word>, subq<Word>), // 4-181 (p285)
    instruction(0xF1C0, 0x5180, subq<Long>, subq<Long>), // 4-181 (p285)

    instruction(0xF0C0, 0x50C0, scc, scc), // 4-173 (p276) (Scc)
    instruction(0xF0F8, 0x50C8, dbcc, dbcc), // 4-91 (p195) (DBcc)

    // 0x6

    instruction(0xF000, 0x6000, bcc, bcc), // 4-25 (p129) (Bcc)
    instruction(0xFF00, 0x6100, bsr, bsr), // 4-59 (p163) (BSR)
    instruction(0xFF00, 0x6000, bra, bra), // 4-55 (p159) (BRA)

    // 0x7

    instruction(0xF100, 0x7000, moveq, moveq), // 4-134 (p238)

    // 0x8

    instruction(0xF1C0, 0x8000, or<Byte>, or<Byte>), // 4-150 (p254) (ea OR Dn -> Dn)
    instruction(0xF1C0, 0x8040, or<Word>, or<Word>), // 4-150 (p254) (ea OR Dn -> Dn)
    instruction(0xF1C0, 0x8080, or<Long>, or<Long>), // 4-150 (p254) (ea OR Dn -> Dn)

    instruction(0xF1C0, 0x8100, orr<Byte>, orr<Byte>), // 4-150 (p254) (Dn OR ea -> ea)
    instruction(0xF1C0, 0x8140, orr<Word>, orr<Word>), // 4-150 (p254) (Dn OR ea -> ea)
    instruction(0xF1C0, 0x8180, orr<Long>, orr<Long>), // 4-150 (p254) (Dn OR ea -> ea)

    instruction(0xF1C0, 0x80C0, divu, divu), // 4-97 (p201)
    instruction(0xF1C0, 0x81C0, unknown, unknown), // 4-93 (p197)
    instruction(0xF1F0, 0x8100, sbcd, sbcd), // 4-171 (p275)

    // 0x9

    instruction(0xF1C0, 0x9000, sub<Byte>, sub<Byte>), // 4-174 (p278) (ea - Dn -> Dn)
    instruction(0xF1C0, 0x9040, sub<Word>, sub<Word>), // 4-174 (p278) (ea - Dn -> Dn)
    instruction(0xF1C0, 0x9080, sub<Long>, sub<Long>), // 4-174 (p278) (ea - Dn -> Dn)

    instruction(0xF1C0, 0x9100, subr<Byte>, subr<Byte>), // 4-174 (p278) (Dn - ea -> ea)
    instruction(0xF1C0, 0x9140, subr<Word>, subr<Word>), // 4-174 (p278) (Dn - ea -> ea)
    instruction(0xF1C0, 0x9180, subr<Long>, subr<Long>), // 4-174 (p278) (Dn - ea -> ea)

    instruction(0xF1C0, 0x90C0, suba<Word>, suba<Word>), // 4-177 (p281)
    instruction(0xF1C0, 0x91C0, suba<Long>, suba<Long>), // 4-177 (p281)

    instruction(0xF1F0, 0x9100, unknown, unknown), // 4-184 (p288)
    instruction(0xF1F0, 0x9140, unknown, unknown), // 4-184 (p288)
    instruction(0xF1F0, 0x9180, unknown, unknown), // 4-184 (p288)

    // 0xA
    instruction(0xF000, 0xA000, unknown, unknown), // LINE 1010 exception

    // 0xB

    instruction(0xF1C0, 0xB100, eor<Byte>, eor<Byte>), // 4-100 (p204)
    instruction(0xF1C0, 0xB140, eor<Word>, eor<Word>), // 4-100 (p204)
    instruction(0xF1C0, 0xB180, eor<Long>, eor<Long>), // 4-100 (p204)

    instruction(0xF1C0, 0xB000, cmp<Byte>, cmp<Byte>), // 4-75 (p179)
    instruction(0xF1C0, 0xB040, cmp<Word>, cmp<Word>), // 4-75 (p179)
    instruction(0xF1C0, 0xB080, cmp<Long>, cmp<Long>), // 4-75 (p179)

    instruction(0xF1C0, 0xB0C0, cmpa<Word>, cmpa<Word>), // 4-77 (p181)
    instruction(0xF1C0, 0xB1C0, cmpa<Long>, cmpa<Long>), // 4-77 (p181)

    instruction(0xF1F8, 0xB108, unknown, unknown), // 4-81 (p185)
    instruction(0xF1F8, 0xB148, unknown, unknown), // 4-81 (p185)
    instruction(0xF1F8, 0xB188, unknown, unknown), // 4-81 (p185)

    // 0xC
    instruction(0xF1C0, 0xC000, and<Byte>, and<Byte>), // 4-15 (p119) (ea AND Dn -> Dn)
    instruction(0xF1C0, 0xC040, and<Word>, and<Word>), // 4-15 (p119) (ea AND Dn -> Dn)
    instruction(0xF1C0, 0xC080, and<Long>, and<Long>), // 4-15 (p119) (ea AND Dn -> Dn)

    instruction(0xF1C0, 0xC100, andr<Byte>, andr<Byte>), // 4-15 (p119) (Dn AND ea -> ea)
    instruction(0xF1C0, 0xC140, andr<Word>, andr<Word>), // 4-15 (p119) (Dn AND ea -> ea)
    instruction(0xF1C0, 0xC180, andr<Long>, andr<Long>), // 4-15 (p119) (Dn AND ea -> ea)

    instruction(0xF1C0, 0xC0C0, mulu, mulu), // 4-139 (p243)
    instruction(0xF1C0, 0xC1C0, unknown, unknown), // 4-136 (p240)

    instruction(0xF1F0, 0xC100, unknown, unknown), // 4-3 (p107)

    instruction(0xF1F8, 0xC140, exgdreg, exgdreg), // 4-105 (p209)
    instruction(0xF1F8, 0xC148, unknown, unknown), // 4-105 (p209)
    instruction(0xF1F8, 0xC188, unknown, unknown), // 4-105 (p209)

    // 0xD

    instruction(0xF1C0, 0xD000, add<Byte>, add<Byte>), // 4-4 (p108) (ea + Dn -> Dn)
    instruction(0xF1C0, 0xD040, add<Word>, add<Word>), // 4-4 (p108) (ea + Dn -> Dn) 
    instruction(0xF1C0, 0xD080, add<Long>, add<Long>), // 4-4 (p108) (ea + Dn -> Dn)

    instruction(0xF1C0, 0xD0C0, adda<Word>, adda<Word>), // 4-7 (p111)
    instruction(0xF1C0, 0xD1C0, adda<Long>, adda<Long>), // 4-7 (p111)

    instruction(0xF1C0, 0xD100, addr<Byte>, addr<Byte>), // 4-4 (p108) (Dn + ea -> ea)
    instruction(0xF1C0, 0xD140, addr<Word>, addr<Byte>), // 4-4 (p108) (Dn + ea -> ea)
    instruction(0xF1C0, 0xD180, addr<Long>, addr<Byte>), // 4-4 (p108) (Dn + ea -> ea)

    instruction(0xF1F0, 0xD100, unknown, unknown), // 4-14 (p118)
    instruction(0xF1F0, 0xD140, unknown, unknown), // 4-14 (p118)
    instruction(0xF1F0, 0xD180, unknown, unknown), // 4-14 (p118)

    // 0xE

    instruction(0xF1D8, 0xE100, asl<Byte>, asl<Byte>), // 4-22 (p126)
    instruction(0xF1D8, 0xE140, asl<Word>, asl<Word>), // 4-22 (p126)
    instruction(0xF1D8, 0xE180, asl<Long>, asl<Long>), // 4-22 (p126)
    instruction(0xFFC0, 0xE1C0, unknown, unknown), // 4-22 (p126)

    instruction(0xF1D8, 0xE000, asr<Byte>, asr<Byte>), // 4-22 (p126)
    instruction(0xF1D8, 0xE040, asr<Word>, asr<Word>), // 4-22 (p126)
    instruction(0xF1D8, 0xE080, asr<Long>, asr<Long>), // 4-22 (p126)
    instruction(0xFFC0, 0xE0C0, unknown, unknown), // 4-22 (p126)

    instruction(0xF1D8, 0xE108, lsl<Byte>, lsl<Byte>), // 4-113 (p217)
    instruction(0xF1D8, 0xE148, lsl<Word>, lsl<Word>), // 4-113 (p217)
    instruction(0xF1D8, 0xE188, lsl<Long>, lsl<Long>), // 4-113 (p217)
    instruction(0xFFC0, 0xE3C0, unknown, unknown), // 4-113 (p217)

    instruction(0xF1D8, 0xE008, lsr<Byte>, lsr<Byte>), // 4-113 (p217)
    instruction(0xF1D8, 0xE048, lsr<Word>, lsr<Word>), // 4-113 (p217)
    instruction(0xF1D8, 0xE088, lsr<Long>, lsr<Long>), // 4-113 (p217)
    instruction(0xFFC0, 0xE2C0, unknown, unknown), // 4-113 (p217)

    instruction(0xF1D8, 0xE118, rol<Byte>, unknown), // 4-160 (p264)
    instruction(0xF1D8, 0xE158, rol<Word>, unknown), // 4-160 (p264)
    instruction(0xF1D8, 0xE198, rol<Long>, unknown), // 4-160 (p264)
    instruction(0xFFC0, 0xE7C0, unknown, unknown), // 4-160 (p264)

    instruction(0xF1D8, 0xE018, ror<Byte>, unknown), // 4-160 (p264)
    instruction(0xF1D8, 0xE058, ror<Word>, unknown), // 4-160 (p264)
    instruction(0xF1D8, 0xE098, ror<Long>, unknown), // 4-160 (p264)
    instruction(0xFFC0, 0xE6C0, unknown, unknown), // 4-160 (p264)

    instruction(0xF1D8, 0xE110, roxl<Byte>, unknown), // 4-163 (p267)
    instruction(0xF1D8, 0xE150, roxl<Word>, unknown), // 4-163 (p267)
    instruction(0xF1D8, 0xE190, roxl<Long>, unknown), // 4-163 (p267)
    instruction(0xFFC0, 0xE5C0, unknown, unknown), // 4-163 (p267)

    instruction(0xF1D8, 0xE010, roxr<Byte>, unknown), // 4-163 (p267)
    instruction(0xF1D8, 0xE050, roxr<Word>, unknown), // 4-163 (p267)
    instruction(0xF1D8, 0xE090, roxr<Long>, unknown), // 4-163 (p267)
    instruction(0xFFC0, 0xE4C0, unknown, unknown), // 4-163 (p267)

    // 0xF
    instruction(0xF000, 0xF000, unknown, unknown), // LINE 1111 exception
};