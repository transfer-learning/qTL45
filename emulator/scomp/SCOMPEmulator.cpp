//
// Created by codetector on 12/6/19.
//

#include "SCOMPEmulator.h"

uint16_t SCOMPEmulator::getRegisterCount() {
  return 1;
}

std::string SCOMPEmulator::getRegisterName(uint16_t regID) {
  if (regID == 0) {
    return "AC";
  }
  return std::string();
}

bool SCOMPEmulator::hasRegisterAlias(uint16_t regID) {
  return false;
}

uint16_t SCOMPEmulator::getRegisterBitSize(uint16_t regID) {
  if (regID == 0) {
    return 16;
  }
  return 0;
}

uint64_t SCOMPEmulator::getRegisterValue(uint64_t regID) {
  return 0;
}

uint64_t SCOMPEmulator::getProgramCounterValue() {
  return 0;
}

uint64_t SCOMPEmulator::getMemoryMaxValue() {
  return (1UL << 16U) - 1;
}
