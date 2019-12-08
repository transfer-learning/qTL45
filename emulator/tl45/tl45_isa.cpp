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
      {
      disassembly = OpNames[inst.opcode];
      if (inst.RI) {
        disassembly += "i";
        if (inst.LH) {
          disassembly += "h";
        }
        disassembly = fmt::format("{} r{}, r{}, {}", disassembly, inst.DR, inst.SR1, (int16_t)inst.raw_imm);
      } else {
        disassembly = fmt::format("{} r{}, r{}, r{}", disassembly, inst.DR, inst.SR1, inst.SR2);
      }
    }
      break;
    case 0x5:
    case 0xa:
      disassembly = "SHL ";
      break;
    case 0xb:
      disassembly = "SHR ";
      break;
    case 0xc:
      disassembly = "J";
      break;
    case 0xd:
      disassembly = "CALL ";
      break;
    case 0xe:
      disassembly = "RET ";
      break;
    case 0xf:
      disassembly = "LBSE ";
      break;
    case 0x10:
      disassembly = "LHW ";
      break;
    case 0x11:
      disassembly = "LHWSE ";
      break;
    case 0x12:
      disassembly = "LB ";
      break;
    case 0x13:
      disassembly = "SB ";
      break;
    case 0x14:
      disassembly = "LW ";
      break;
    case 0x15:
      disassembly = "SW ";
      break;
    default:
      disassembly = "INVALID";
      break;
  }
  if (inst.opcode == 0xc) {
    switch (inst.DR) {
      case 0x0:
        disassembly += "O ";
        break;
      case 0x1:
        disassembly += "NO ";
        break;
      case 0x2:
        disassembly += "S ";
        break;
      case 0x3:
        disassembly += "NS ";
        break;
      case 0x4:
        disassembly += "E ";
        break;
      case 0x5:
        disassembly += "NE ";
        break;
      case 0x6:
        disassembly += "B ";
        break;
      case 0x7:
        disassembly += "NB ";
        break;
      case 0x8:
        disassembly += "BE ";
        break;
      case 0x9:
        disassembly += "A ";
        break;
      case 0xa:
        disassembly += "L ";
        break;
      case 0xb:
        disassembly += "GE ";
        break;
      case 0xc:
        disassembly += "LE ";
        break;
      case 0xd:
        disassembly += "G ";
        break;
      default:
        disassembly += "UMP ";
        break;
    }
  }
  std::string operand_part;
  return disassembly;
}

};
