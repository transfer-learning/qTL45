//
// Created by codetector on 12/6/19.
//

#ifndef TL45_EMU_SCOMPEMULATOR_H
#define TL45_EMU_SCOMPEMULATOR_H

#include <emulator/AbstractEmulatorState.h>

class SCOMPEmulator : public AbstractEmulatorState {
public:
  uint16_t getRegisterCount() override;

  std::string getRegisterName(uint16_t regID) override;

  bool hasRegisterAlias(uint16_t regID) override;

  uint16_t getRegisterBitSize(uint16_t regID) override;

  uint64_t getRegisterValue(uint64_t regID) override;

  uint64_t getProgramCounterValue() override;

  uint64_t getMemoryMaxValue() override;

};


#endif //TL45_EMU_SCOMPEMULATOR_H
