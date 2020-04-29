#include "stdint.h"
#include <cstdio>
#include <stdexcept>
#include <assert.h>

#include "tl45_emu_core.h"

// Created by codetector on 11/22/19.
//
#define DEBUG

using namespace TL45;

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

void TL45::tick(tl45_state *state) {
  uint32_t instruction = state->fetch_instruction(state->pc);
  DecodedInstruction instr{};
  if (!DecodedInstruction::decode(instruction, instr)) {
    // RIP
		printf("%08x: Decode error\n", state->pc);
    return;
  }

	state->pc += 4;
  auto opcode = instr.opcode;
  switch (opcode) {
    case 0x0:
      break; // NOP
    case 0x1: { // ADD
      regcell cell2 = state->read_reg(instr.SR2);
      if (instr.RI) { // IMM Mode
        cell2 = regcell(instr.decode_imm());
      }
      regcell cell1 = state->read_reg(instr.SR1);
    	regcell result = cell1 + cell2;
      state->write_reg(instr.DR, result);

    	uint64_t val1 = (uint64_t)cell1.value;
	    uint64_t val2 = (uint64_t)cell2.value;
      uint64_t long_result = val1 + val2;
	    tl45_flags new_flags;
      new_flags.of = (sign_bit(val1) == sign_bit(val2) && sign_bit(val1) != sign_bit(long_result));
      new_flags.cf = (long_result >> 32u) & 1u;
      new_flags.zf = (uint32_t) long_result == 0;
      new_flags.sf = sign_bit(long_result);
			state->write_flags(cell<tl45_flags>(new_flags, result.taint));
      break;
    }
    case 0x2: { // SUB
      regcell cell2 = state->read_reg(instr.SR2);
      if (instr.RI) { // IMM Mode
        cell2 = regcell(instr.decode_imm());
      }
    	regcell cell1 = state->read_reg(instr.SR1);
			regcell result = cell1 - cell2;
    	if (!instr.RI && instr.SR2 == instr.SR1) {
				result.taint.clear(); // sub x,x zeroes taint
    	}
			state->write_reg(instr.DR, result);

    	uint64_t val1 = (uint64_t)cell1.value;
	    uint64_t val2 = -(int64_t)cell2.value & 0x1FFFFFFFF;
      uint64_t long_result = val1 + val2;
    	tl45_flags new_flags;
      new_flags.of = (sign_bit(val1) == sign_bit(val2) && sign_bit(val1) != sign_bit(long_result));
      new_flags.cf = (long_result >> 32u) & 1u;
      new_flags.zf = (uint32_t) long_result == 0;
      new_flags.sf = sign_bit(long_result);
    	state->write_flags(cell<tl45_flags>(new_flags, result.taint));
      break;
    }
    case 0x3: { // MUL
      regcell cell2 = state->read_reg(instr.SR2);
      if (instr.RI) { // IMM Mode
        cell2 = regcell(instr.decode_imm());
      }
      regcell cell1 = state->read_reg(instr.SR1);
    	regcell result = cell1 * cell2;
      state->write_reg(instr.DR, result);
      break;
    }
    case 0x6: // OR
    case 0x7: // XOR
    case 0x8: // AND
    case 0x9: { // NOT
      regcell cell2 = state->read_reg(instr.SR2);
      if (instr.RI) { // IMM Mode
        cell2 = regcell(instr.decode_imm());
      }
      regcell cell1 = state->read_reg(instr.SR1);
      regcell result = regcell(0);
      switch (opcode) {
        case 0x6:
          result = cell1 | cell2;
          break;
        case 0x7:
          result = cell1 ^ cell2;
					if (!instr.RI && instr.SR2 == instr.SR1) {
						result.taint.clear(); // xor x,x zeroes taint
    			}
          break;
        case 0x8:
          result = cell1 & cell2;
      		if (instr.RI && cell2.value == 0) {
						result.taint.clear(); // and x,imm0 zeroes taint
    			}
          break;
        case 0x9:
          result = ~cell1;
          break;
        default:
          assert(0);
      }
      state->write_reg(instr.DR, result);

    	tl45_flags new_flags;
      new_flags.of = 0;
      new_flags.cf = 0;
      new_flags.zf = result.value == 0;
      new_flags.sf = sign_bit(result.value);
    	state->write_flags(cell<tl45_flags>(new_flags, result.taint));
      break;
    }
    case 0xA: // SHL
    case 0xB: // SHR
    case 0x5: { // SHRA
      regcell cell2 = state->read_reg(instr.SR2);
      if (instr.RI) { // IMM Mode
        cell2 = regcell(instr.decode_imm());
      }
      regcell cell1 = state->read_reg(instr.SR1);
      regcell result = regcell(0);
      switch (opcode) {
        case 0xA:
          result = cell1 << cell2;
          break;
        case 0xB:
          result = cell1 >> cell2;
          break;
        case 0x5:
          result = (cell<uint32_t>) ((cell<int32_t>) cell1 >> (cell<int32_t>) cell2);
          break;
        default:
          assert(0);
      }
      state->write_reg(instr.DR, result);
      break;
    }
    case 0xC: // JUMPS
    {
			cell<tl45_flags> flags_cell = state->read_flags();
			tl45_flags flags = flags_cell.value;
      bool do_jump;
      switch (instr.DR) {
        case 0:
          do_jump = flags.of;
          break; /* jo*/
        case 1:
          do_jump = !flags.of;
          break; // jno
        case 2:
          do_jump = flags.sf;
          break;// js
        case 3:
          do_jump = !flags.sf;
          break; // jns
        case 4:
          do_jump = flags.zf;
          break; // je
        case 5:
          do_jump = !flags.zf;
          break; // jne
        case 6:
          do_jump = flags.cf;
          break; // jb
        case 7:
          do_jump = !flags.cf;
          break; // jnb
        case 8:
          do_jump = (flags.cf | flags.zf);
          break;// jbe
        case 9:
          do_jump = (!flags.cf) && (!flags.zf);
          break;// ja
        case 10:
          do_jump = flags.sf ^ flags.of;
          break;// jl
        case 11:
          do_jump = (flags.sf == flags.of);
          break; // jge
        case 12:
          do_jump = flags.zf || (flags.sf ^ flags.of);
          break; // jle
        case 13:
          do_jump = (!flags.zf) && (flags.sf == flags.of);
          break; // jg
        default:
          do_jump = true; // jmp
      }

      // update profiling info
			uint32_t instr_addr = state->pc - 4;
			state->profile.branch_count[{instr_addr, do_jump}]++;
			state->profile.branch_taint[{instr_addr, do_jump}].insert(flags_cell.taint.set.begin(), flags_cell.taint.set.end());

      if (do_jump) {
        uint32_t target_address = state->read_reg(instr.SR1).value + ((int32_t) (int16_t) instr.raw_imm); // SR1 + SEXT IMM
        state->pc = target_address;
      }
    }
      break; // JUMPs
    case 0xD: { // CALL
      uint32_t addr = state->read_reg(instr.SR1).value + (int32_t) (int16_t) instr.raw_imm;
      regcell decr_sp = state->read_reg(instr.DR);
			decr_sp.value -= 4;
      state->write_reg(instr.DR, decr_sp);
			state->write_word(decr_sp.value, regcell(state->pc)); // we assume pc has no taint
      state->pc = addr;
      break;
    }
    case 0xE: { // RET
      regcell incr_sp = state->read_reg(instr.DR);
			state->pc = state->read_word(incr_sp.value).value;
			incr_sp.value += 4;
      state->write_reg(instr.DR, incr_sp);
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
      uint32_t addr = state->read_reg(instr.SR1).value + (int32_t) (int16_t) instr.raw_imm;
      regcell value_to_write = state->read_reg(instr.DR);
      regcell value_read = regcell(0);
      bool is_read = false;
			//printf("access to %08x\n", addr);

      switch (opcode) {
        case 0x10: // LHW
        case 0x11: { // LHSWE
          is_read = true;
          cell<uint16_t> raw_data = state->read_halfword(addr);
          if (opcode == 0x11) { // LHSWE
            value_read = (cell<uint32_t>) (cell<int32_t>) (cell<int16_t>) raw_data; // sext
          } else {
            value_read = (cell<uint32_t>) raw_data; // zext
          }
          break;
        }
        case 0x16: { // SHW
					state->write_halfword(addr, (cell<uint16_t>) value_to_write);
          break;
        }
        case 0x0F: // LBSE
        case 0x12: { // LB
          is_read = true;
          cell<uint8_t> raw_data = state->read_byte(addr);
          if (opcode == 0x0F) { // LBSWE
            value_read = (cell<uint32_t>) (cell<int32_t>) (cell<int8_t>) raw_data; // sext
          } else {
            value_read = (cell<uint32_t>) raw_data;
          }
          break;
        }
        case 0x13: { // SB
					state->write_byte(addr, (cell<uint8_t>) value_to_write);
          break;
        }
        case 0x14: { // LW
          is_read = true;
          value_read = state->read_word(addr);
          break;
        }
        case 0x15: { // SW
					state->write_word(addr, value_to_write);
          break;
        }
        default:
          assert(0);
      }

      if (is_read) {
        state->write_reg(instr.DR, value_read);
      }

      if (!is_read && addr == 0x696969) {
        printf("%08x: %08x\n", state->pc - 4, value_to_write.value);
      }

      break;
    }

    case 0x17: // SDIV
    case 0x18: { // UDIV
      regcell cell2 = state->read_reg(instr.SR2);
      if (instr.RI) { // IMM Mode
        cell2 = regcell(instr.decode_imm());
      }
      regcell cell1 = state->read_reg(instr.SR1);
      regcell result = regcell(0);
      switch (opcode) {
        case 0x17: // SDIV
          result = (cell<uint32_t>) ((cell<int32_t>) cell1 / (cell<int32_t>) cell2);
          break;
        case 0x18: // UDIV
          result = cell1 / cell2;
          break;
        default:
          assert(0);
      }
      state->write_reg(instr.DR, result);
      break;
    }
    default:
  		printf("%08x: Decode error\n", state->pc);
			state->pc -= 4;
      break; // DECODE ERROR
  }
}
