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
  return (struct u32_carry) {retval, carry};
}

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

static bool sign_bit(uint32_t data) {
  return data >> 31u;
}

void tick(tl45_state *state) {
  uint32_t instruction = fetch_instruction(state->memory, state->pc);
  uint8_t opcode = (instruction >> (32U - 5U)) & 0x1FU; // Extract Opcode

  DecodedInstruction instr{};
  if (!DecodedInstruction::decode(instruction, instr)) {
    // RIP
    return;
  }

#ifdef DEBUG
  printf("0x%08X => OP: %02X, DR: %X, SR1: %X, SR2: %X, IMM16: %04X\n", state->pc, opcode, instr.DR, instr.SR1,
         instr.SR2, instr.raw_imm);
#endif
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
    case 0xC:
      break; // JUMPs
    case 0xD: { // CALL

      uint32_t addr = state->read_reg(instr.SR1) + (int32_t) (int16_t) instr.raw_imm;
      
      uint32_t decr_sp = state->read_reg(instr.DR) - 4;
      state->write_reg(instr.DR, decr_sp);
      
      state->memory[decr_sp] = state->pc;
      
      state->pc = addr;
      
      break;
    }
    case 0xE: { // RET

      uint32_t incr_sp = state->read_reg(instr.DR) + 4;
      state->write_reg(instr.DR, incr_sp);

      state->pc = state->memory[incr_sp - 4];
      
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
