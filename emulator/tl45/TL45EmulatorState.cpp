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
