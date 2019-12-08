//
// Created by codetector on 11/23/19.
//

#ifndef TL45_EMU_TL45_EMU_CORE_H
#define TL45_EMU_TL45_EMU_CORE_H

namespace TL45 {

struct DecodedInstruction {
  uint32_t raw;
  int opcode;
  bool RI;
  bool LH;
  bool ZS;
  int DR;
  int SR1;
  int SR2;
  uint16_t raw_imm;

  static bool decode(uint32_t raw, DecodedInstruction &instr) {
    instr.raw = raw;
    instr.opcode = (raw >> (32U - 5U)) & 0x1FU;
    instr.RI = raw >> (32U - 6U) & 1U;
    instr.LH = raw >> (32U - 7U) & 1U;
    instr.ZS = raw >> (32U - 8U) & 1U;
    instr.DR = raw >> (32U - 12U) & 0xFU;
    instr.SR1 = raw >> (32U - 16U) & 0xFU;
    instr.SR2 = raw >> (32U - 20U) & 0xFU;
    instr.raw_imm = raw & 0xFFFFU;

    return true; // valid
  }

  uint32_t decode_imm() const {
    uint32_t imm = this->raw_imm;
    if (LH) {
      imm <<= 16u;
    } else if (ZS && ((imm >> 15u) & 1u)) {
      imm |= (0xFFFFu << 16u);
    }
    return imm;
  }

};
struct tl45_flags {
  uint8_t zf:1;
  uint8_t cf:1;
  uint8_t sf:1;
  uint8_t of:1;
};

struct tl45_state {
  uint32_t registers[15];
  uint32_t pc;
  tl45_flags flags;
  uint8_t *memory;
  size_t max_memory_length; // must be multiple of 4

  void write_reg(unsigned reg, uint32_t value) {
    if (reg != 0) {
      registers[reg - 1] = value;
    }
  }

  uint32_t read_reg(unsigned reg) const {
    if (reg == 0) {
      return 0;
    }

    return registers[reg - 1];
  }

  uint32_t read_word(uint32_t addr) const {
    uint32_t aligned_addr = addr & ~0b11UL;
    uint32_t value = memory[aligned_addr];
    value <<= 8;
    value |= memory[aligned_addr+1];
    value <<= 8;
    value |= memory[aligned_addr+2];
    value <<= 8;
    value |= memory[aligned_addr+3];
    return value;
  }
};

void tick(tl45_state *state);

}

#endif //TL45_EMU_TL45_EMU_CORE_H
