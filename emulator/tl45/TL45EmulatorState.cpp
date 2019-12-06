//
// Created by codetector on 12/3/19.
//

#include "TL45EmulatorState.h"

uint16_t TL45EmulatorState::getRegisterCount() {
  return 16;
}

std::string TL45EmulatorState::getRegisterName(uint16_t regID) {
  if (regID < 16) {
    return "r" + std::to_string(regID);
  }
  return "UNKNOWN";
}

uint16_t TL45EmulatorState::getRegisterBitSize(uint16_t regID) {
  return 32;
}

bool TL45EmulatorState::hasRegisterAlias(uint16_t regID) {
  return false;
}

uint64_t TL45EmulatorState::getRegisterValue(uint64_t regID) {
  return 0;
}

uint64_t TL45EmulatorState::getProgramCounterValue() {
  return 0;
}

unsigned int TL45EmulatorState::getMemoryAddrWidth() {
  return 32;
}

unsigned int TL45EmulatorState::getMemoryDataWidth() {
  return 8;
}

MemoryMapping TL45EmulatorState::getMemoryMapping(uint64_t addr) {
  return MemoryMapping::STANDARD_MEMORY;
}

uint64_t TL45EmulatorState::getMemoryValue(uint64_t addr) {
  return std::rand();
}

void TL45EmulatorState::setMemoryValue(uint64_t addr, uint64_t data) {

}

std::string TL45EmulatorState::getMemoryDisassembly(uint64_t &addr) {
  return std::string("memes");
}

