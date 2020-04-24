#include "stdint.h"
#include <cstdio>
#include <stdexcept>
#include <assert.h>
#include "tl45_emu_core.h"

// Created by codetector on 11/22/19.
//
#define DEBUG

namespace TL45 {

uint32_t fetch_instruction(const uint8_t *memory, uint32_t location) {
  uint32_t value = 0;
  for (int i = 0; i < 4; ++i) {
    value = (value << 8U) | memory[location + i];
  }
  return value;
}

struct u32_carry {
  uint32_t res;
  unsigned carry;
};

struct u32_carry add_carryout(uint32_t x, uint32_t y) {
  uint32_t retval = x + y;
  unsigned carry = (retval < x);
  return u32_carry{retval, carry};
}


static bool sign_bit(uint32_t data) {
  return data >> 31u;
}

void tick(tl45_state *state) {
  uint32_t instruction = fetch_instruction(state->memory, state->pc);
  DecodedInstruction instr{};
  if (!DecodedInstruction::decode(instruction, instr)) {
    // RIP
    return;
  }

  state->pc += 4;

  auto opcode = instr.opcode;
  switch (opcode) {
    case 0x0:
      break; // NOP
    case 0x1: { // ADD
      uint64_t val2 = state->read_reg(instr.SR2);
      if (instr.RI) { // IMM Mode
        val2 = instr.decode_imm();
      }

      uint64_t val1 = state->read_reg(instr.SR1);

      uint64_t long_result = val1 + val2;

      state->write_reg(instr.DR, (uint32_t) long_result);

      state->flags.of = (sign_bit(val1) == sign_bit(val2) && sign_bit(val1) != sign_bit(long_result));
      state->flags.cf = (long_result >> 32u) & 1u;
      state->flags.zf = (uint32_t) long_result == 0;
      state->flags.sf = sign_bit(long_result);
      break;
    }
    case 0x2: { // SUB
      uint64_t val2 = (uint32_t) -state->read_reg(instr.SR2);
      if (instr.RI) { // IMM Mode
        val2 = (uint32_t) -instr.decode_imm();
      }

      uint64_t val1 = state->read_reg(instr.SR1);

      uint64_t long_result = val1 + val2;

      state->write_reg(instr.DR, (uint32_t) long_result);

      state->flags.of = (sign_bit(val1) == sign_bit(val2) && sign_bit(val1) != sign_bit(long_result));
      state->flags.cf = (long_result >> 32u) & 1u;
      state->flags.zf = (uint32_t) long_result == 0;
      state->flags.sf = sign_bit(long_result);
      break;
    }
    case 0x3: { // MUL
      uint32_t val2 = state->read_reg(instr.SR2);
      if (instr.RI) { // IMM Mode
        val2 = instr.decode_imm();
      }

      uint32_t val1 = state->read_reg(instr.SR1);

      uint32_t result = val1 * val2;

      state->write_reg(instr.DR, result);

      break;
    }
    case 0x6: // OR
    case 0x7: // XOR
    case 0x8: // AND
    case 0x9: { // NOT
      uint32_t val2 = state->read_reg(instr.SR2);
      if (instr.RI) { // IMM Mode
        val2 = instr.decode_imm();
      }

      uint32_t val1 = state->read_reg(instr.SR1);

      uint32_t result = 0;
      switch (opcode) {
        case 0x6:
          result = val1 | val2;
          break;
        case 0x7:
          result = val1 ^ val2;
          break;
        case 0x8:
          result = val1 & val2;
          break;
        case 0x9:
          result = ~val1;
          break;
        default:
          assert(0);
      }

      // TODO change all reg access
      state->write_reg(instr.DR, result);

      state->flags.of = 0;
      state->flags.cf = 0;
      state->flags.zf = result == 0;
      state->flags.sf = sign_bit(result);

      break;
    }
    case 0xA: // SHL
    case 0xB: // SHR
    case 0x5: { // SHRA
      uint32_t val2 = state->read_reg(instr.SR2);
      if (instr.RI) { // IMM Mode
        val2 = instr.decode_imm();
      }

      uint32_t val1 = state->read_reg(instr.SR1);

      uint32_t result = 0;
      switch (opcode) {
        case 0xA:
          result = val1 << val2;
          break;
        case 0xB:
          result = val1 >> val2;
          break;
        case 0x5:
          result = (uint32_t) ((int32_t) val1 >> (int32_t) val2);
          break;
        default:
          assert(0);
      }

      // TODO change all reg access
      state->write_reg(instr.DR, result);

      break;
    }
    case 0xC: // JUMPS
    {
      bool do_jump;
      switch (instr.DR) {
        case 0:
          do_jump = state->flags.of;
          break; /* jo*/
        case 1:
          do_jump = !state->flags.of;
          break; // jno
        case 2:
          do_jump = state->flags.sf;
          break;// js
        case 3:
          do_jump = !state->flags.sf;
          break; // jns
        case 4:
          do_jump = state->flags.zf;
          break; // je
        case 5:
          do_jump = !state->flags.zf;
          break; // jne
        case 6:
          do_jump = state->flags.cf;
          break; // jb
        case 7:
          do_jump = !state->flags.cf;
          break; // jnb
        case 8:
          do_jump = (state->flags.cf | state->flags.zf);
          break;// jbe
        case 9:
          do_jump = (!state->flags.cf) && (!state->flags.zf);
          break;// ja
        case 10:
          do_jump = state->flags.sf ^ state->flags.of;
          break;// jl
        case 11:
          do_jump = (state->flags.sf == state->flags.of);
          break; // jge
        case 12:
          do_jump = state->flags.zf || (state->flags.sf ^ state->flags.of);
          break; // jle
        case 13:
          do_jump = (!state->flags.zf) && (state->flags.sf == state->flags.of);
          break; // jg
        default:
          do_jump = true; // jmp
      }
      if (do_jump) {
        uint32_t target_address = state->read_reg(instr.SR1) + ((int32_t) (int16_t) instr.raw_imm); // SR1 + SEXT IMM
        state->pc = target_address;
      }
    }
      break; // JUMPs
    case 0xD: { // CALL

      uint32_t addr = state->read_reg(instr.SR1) + (int32_t) (int16_t) instr.raw_imm;

      uint32_t decr_sp = state->read_reg(instr.DR) - 4;
      state->write_reg(instr.DR, decr_sp);

      state->memory[decr_sp + 0] = (state->pc >> 24u) & 0xFFu;
      state->memory[decr_sp + 1] = (state->pc >> 16u) & 0xFFu;
      state->memory[decr_sp + 2] = (state->pc >> 8u) & 0xFFu;
      state->memory[decr_sp + 3] = (state->pc) & 0xFFu;

      state->pc = addr;

      break;
    }
    case 0xE: { // RET

      uint32_t incr_sp = state->read_reg(instr.DR) + 4;
      state->write_reg(instr.DR, incr_sp);

      uint32_t addr = incr_sp - 4;
      state->pc = (uint32_t) (state->memory[addr] << 24u)
                  | (uint32_t) (state->memory[addr + 1] << 16u)
                  | (uint32_t) (state->memory[addr + 2] << 8u)
                  | (uint32_t) (state->memory[addr + 3]);

      break;
    }

    case 0x10: // LHW
    case 0x11: // LHSWE
    case 0x16: // SHW
    case 0x0F: // LBSE
    case 0x12: // LB
    case 0x13: // SB
    case 0x14: // LW
    case 0x15: { // SW
      uint32_t addr = state->read_reg(instr.SR1) + (int32_t) (int16_t) instr.raw_imm;
      uint32_t value_to_write = state->read_reg(instr.DR);
      uint32_t value_read = 0;
      bool is_read = false;

      switch (opcode) {
        case 0x10: // LHW
        case 0x11: { // LHSWE

          is_read = true;
          uint16_t raw_data = (uint16_t) (state->memory[addr] << 8u) | (state->memory[addr + 1]);
          if (opcode == 0x11) { // LHSWE
            value_read = (uint32_t) (int32_t) (int16_t) raw_data;
          } else {
            value_read = (uint32_t) raw_data;
          }

          break;
        }
        case 0x16: { // SHW
          state->memory[addr + 0] = (uint8_t) ((value_to_write >> 8u) & 0xFFu);
          state->memory[addr + 1] = (uint8_t) ((value_to_write >> 0u) & 0xFFu);
          break;
        }
        case 0x0F: // LBSE
        case 0x12: { // LB

          is_read = true;
          uint8_t raw_data = state->memory[addr];
          if (opcode == 0x0F) { // LHSWE
            value_read = (uint32_t) (int32_t) (int8_t) raw_data;
          } else {
            value_read = (uint32_t) raw_data;
          }

          break;
        }
        case 0x13: { // SB

          state->memory[addr] = (uint8_t) (value_to_write & 0xFFu);
          break;
        }
        case 0x14: { // LW

          is_read = true;
          value_read = (uint32_t) (state->memory[addr] << 24u)
                       | (uint32_t) (state->memory[addr + 1] << 16u)
                       | (uint32_t) (state->memory[addr + 2] << 8u)
                       | (uint32_t) (state->memory[addr + 3]);

          break;
        }
        case 0x15: { // SW

          state->memory[addr + 0] = (uint8_t) ((value_to_write >> 24u) & 0xFFu);
          state->memory[addr + 1] = (uint8_t) ((value_to_write >> 16u) & 0xFFu);
          state->memory[addr + 2] = (uint8_t) ((value_to_write >> 8u) & 0xFFu);
          state->memory[addr + 3] = (uint8_t) ((value_to_write >> 0u) & 0xFFu);
          break;
        }
        default:
          assert(0);
      }

      if (is_read) {
        state->write_reg(instr.DR, value_read);
      }

      break;
    }

    case 0x17: // SDIV
    case 0x18: { // UDIV
      uint32_t val2 = state->read_reg(instr.SR2);
      if (instr.RI) { // IMM Mode
        val2 = instr.decode_imm();
      }

      uint32_t val1 = state->read_reg(instr.SR1);

      uint32_t result = 0;
      switch (opcode) {
        case 0x17:
          result = (uint32_t) ((int32_t) val1 / (int32_t) val2);
          break;
        case 0x18:
          result = val1 / val2;
          break;
        default:
          assert(0);
      }

      // TODO change all reg access
      state->write_reg(instr.DR, result);

      break;
    }
    default:
      break; // DECODE ERROR
  }
}

}
