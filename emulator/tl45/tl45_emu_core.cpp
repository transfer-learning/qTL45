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
      uint64_t val2 = state->registers[instr.SR2 - 1];
      if (instr.RI) { // IMM Mode
        val2 = instr.decode_imm();
      }

      uint64_t val1 = state->registers[instr.SR1 - 1];

      uint64_t long_result = val1 + val2;

      state->registers[instr.DR - 1] = (uint32_t) long_result;

      state->flags.of = (sign_bit(val1) == sign_bit(val2) && sign_bit(val1) != sign_bit(long_result));
      state->flags.cf = (long_result >> 32u) & 1u;
      state->flags.zf = (uint32_t)long_result == 0;
      state->flags.sf = sign_bit(long_result);
      break;
    }
    case 0x2: { // SUB
      uint64_t val2 = (uint32_t) -state->registers[instr.SR2 - 1];
      if (instr.RI) { // IMM Mode
        val2 = (uint32_t) -instr.decode_imm();
      }

      uint64_t val1 = state->registers[instr.SR1 - 1];

      uint64_t long_result = val1 + val2;

      state->registers[instr.DR - 1] = (uint32_t) long_result;

      state->flags.of = (sign_bit(val1) == sign_bit(val2) && sign_bit(val1) != sign_bit(long_result));
      state->flags.cf = (long_result >> 32u) & 1u;
      state->flags.zf = (uint32_t)long_result == 0;
      state->flags.sf = sign_bit(long_result);
      break;
    }
    case 0x3: { // MUL
      uint32_t val2 = state->registers[instr.SR2 - 1];
      if (instr.RI) { // IMM Mode
        val2 = instr.decode_imm();
      }

      uint32_t val1 = state->registers[instr.SR1 - 1];

      uint32_t result = val1 * val2;

      state->registers[instr.DR - 1] = result;

      break;
    }
    case 0x6: // OR
    case 0x7: // XOR
    case 0x8: // AND
    case 0x9: { // NOT
      uint32_t val2 = state->registers[instr.SR2 - 1];
      if (instr.RI) { // IMM Mode
        val2 = instr.decode_imm();
      }

      uint32_t val1 = state->registers[instr.SR1 - 1];

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
      state->registers[instr.DR - 1] = result;

      state->flags.of = 0;
      state->flags.cf = 0;
      state->flags.zf = result == 0;
      state->flags.sf = sign_bit(result);

      break;
    }
    case 0xA: // SHL
    case 0xB: // SHR
    case 0x5: { // SHRA
      uint32_t val2 = state->registers[instr.SR2 - 1];
      if (instr.RI) { // IMM Mode
        val2 = instr.decode_imm();
      }

      uint32_t val1 = state->registers[instr.SR1 - 1];

      uint32_t result = 0;
      switch (opcode) {
        case 0xA:
          result = val1 << val2;
          break;
        case 0xB:
          result = val1 >> val2;
          break;
        case 0x5:
          result = (uint32_t) ((int32_t)val1 >> (int32_t)val2);
          break;
        default:
          assert(0);
      }

      // TODO change all reg access
      state->registers[instr.DR - 1] = result;

      break;
    }
    case 0xC:
      break; // JUMPs
    case 0xD:
      break; // CALL
    case 0xE:
      break; // RET

    case 0x10: // LHW
    case 0x11: // LHSWE
    case 0x16: // SHW
    case 0x0F: // LBSE
    case 0x12: // LB
    case 0x13: // SB
    case 0x14: // LW
    case 0x15: { // SW
      uint32_t addr = state->registers[instr.SR1 - 1] + (int32_t) (int16_t) instr.raw_imm;
      uint32_t value_to_write = state->registers[instr.DR - 1];
      uint32_t value_read = 0;
      bool is_read = false;

      switch (opcode) {
        case 0x10: // LHW
        case 0x11: // LHSWE

          is_read = true;
          uint16_t raw_data = (uint16_t)(state->memory[addr] << 8u) | (state->memory[addr + 1]);
          if (opcode == 0x11) {
            value_read = (uint32_t) (int32_t) (int16_t) raw_data;
          } else {
            value_read = (uint32_t) raw_data;
          }

          break;
        case 0x16: // SHW
          break;
        case 0x0F: // LBSE
          break;
        case 0x12: // LB
          break;
        case 0x13: // SB
          break;
        case 0x14: // LW
          break;
        case 0x15: // SW
          break;
        default:
          assert(0);
      }

      if (is_read) {
        state->registers[instr.DR - 1] = value_read;
      }

      break;
    }

    case 0x17: // SDIV
    case 0x18: { // UDIV
      uint32_t val2 = state->registers[instr.SR2 - 1];
      if (instr.RI) { // IMM Mode
        val2 = instr.decode_imm();
      }

      uint32_t val1 = state->registers[instr.SR1 - 1];

      uint32_t result = 0;
      switch (opcode) {
        case 0x17:
          result = (uint32_t) ((int32_t)val1 / (int32_t)val2);
          break;
        case 0x18:
          result = val1 / val2;
          break;
        default:
          assert(0);
      }

      // TODO change all reg access
      state->registers[instr.DR - 1] = result;

      break;
    }
    default:
      break; // DECODE ERROR
  }
}

}
