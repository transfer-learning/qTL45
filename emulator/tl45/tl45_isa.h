//
// Created by Will Gulian on 12/6/19.
//

#ifndef TL45_EMU_TL45_ISA_H
#define TL45_EMU_TL45_ISA_H

#include <string>
#include <stdint.h>

namespace TL45 {

std::string disassemble(uint32_t instruction);

};

#endif //TL45_EMU_TL45_ISA_H
