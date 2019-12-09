//
// Created by Will Gulian on 12/6/19.
//

#include "tl45_isa.h"
#include "tl45_emu_core.h"

#include <string>
#include <stdint.h>
#include <fmt/format.h>

namespace TL45 {
std::string disassemble(uint32_t instruction) {

  static const std::string OpNames[0x19] = {"NOP", "ADD", // 0x1
                                "SUB", "MUL", "_NOP", // 0x4,
                                "SHRA", "OR", "XOR", // 0x7
                                "AND", "NOT", "SHL", // 0xa
                                "SHR", "J", "CALL", // 0xd
                                "RET", "LBSE", "LHW", // 0x10
                                "LHWSE", "LB", "SB", // 0x13
                                "LW", "SW", "SHW", // 0x16
                                "SDIV", "UDIV" // 0x18
                                };
  static const std::string JumpCodes[] = {"O", "NO", "S", "NS",
                                          "E", "NE", "B", "NB",
                                          "BE", "A", "L", "GE",
                                          "LE", "G", "UMP", "UMP"};

  DecodedInstruction inst{};
  TL45::DecodedInstruction::decode(instruction, inst);
  std::string disassembly;
  switch (inst.opcode) {
    case 0x0:
      disassembly = "NOP ";
      break;
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x6:
    case 0x7:
    case 0x8:
    case 0x9:
      // Arithmetic RI LH RRI / RRR
      {
      disassembly = OpNames[inst.opcode];
      if (inst.RI) {
        if (inst.LH) {
          disassembly = fmt::format("{}ih r{}, r{}, {}", disassembly, inst.DR, inst.SR1, (int16_t) inst.raw_imm);
        } else {
          disassembly = fmt::format("{}i r{}, r{}, {}", disassembly, inst.DR, inst.SR1, (int16_t) inst.raw_imm);
        }
      } else {
        disassembly = fmt::format("{} r{}, r{}, r{}", disassembly, inst.DR, inst.SR1, inst.SR2);
      }
    }
      break;
    case 0x5:
    case 0xa:
    case 0xb:
      // Shifts
      disassembly = fmt::format("{} {}", OpNames[inst.opcode], inst.raw_imm);
      break;
    case 0xc:
      disassembly = fmt::format("J{} 0x{:X}(r{})", JumpCodes[inst.DR], (int16_t)inst.raw_imm, inst.SR1);
      break;
    case 0xd:
      disassembly = fmt::format("CALL 0x{:X}(r{})", (int16_t)inst.raw_imm, inst.SR1);
      break;
    case 0xe:
      disassembly = "RET";
      break;
    case 0xf:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
      // Load and stores
      disassembly = fmt::format("{} r{} {}(r{})", OpNames[inst.opcode], inst.DR, (uint16_t)inst.raw_imm, inst.SR1);
      break;
    default:
      disassembly = "INVALID";
      break;
  }
  std::string operand_part;
  return disassembly;
}

};
