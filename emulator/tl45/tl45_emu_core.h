//
// Created by codetector on 11/23/19.
//

#ifndef TL45_EMU_TL45_EMU_CORE_H
#define TL45_EMU_TL45_EMU_CORE_H

namespace TL45 {

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

  uint32_t read_reg(unsigned reg) {
    if (reg == 0) {
      return 0;
    }

    return registers[reg - 1];
  }
};

void tick(tl45_state *state);

}

#endif //TL45_EMU_TL45_EMU_CORE_H
