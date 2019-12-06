//
// Created by codetector on 12/3/19.
//

#ifndef TL45_EMU_TL45EMULATORSTATE_H
#define TL45_EMU_TL45EMULATORSTATE_H

#include "AbstractEmulatorState.h"

class TL45EmulatorState : public AbstractEmulatorState {
  uint16_t getRegisterCount() override;

  std::string getRegisterName(uint16_t regID) override;

  bool hasRegisterAlias(uint16_t regID) override;

  uint64_t getRegisterValue(uint64_t regID) override;

  uint64_t getProgramCounterValue() override;

public:
  uint16_t getRegisterBitSize(uint16_t regID) override;

  uint64_t getMemoryMaxValue() override;
};


#endif //TL45_EMU_TL45EMULATORSTATE_H
