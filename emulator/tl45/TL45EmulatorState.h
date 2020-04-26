//
// Created by codetector on 12/3/19.
//

#ifndef TL45_EMU_TL45EMULATORSTATE_H
#define TL45_EMU_TL45EMULATORSTATE_H

#include "AbstractEmulatorState.h"
#include "tl45_emu_core.h"

class TL45EmulatorState : public AbstractEmulatorState {
  uint16_t getRegisterCount() override;

  std::string getRegisterName(uint16_t regID) override;

  bool hasRegisterAlias(uint16_t regID) override;

  uint64_t getRegisterValue(uint64_t regID) override;

  uint64_t getProgramCounterValue() override;

public:
  void clear() override;

  int load(std::string fileName) override;

public:
  TL45::tl45_state state;

  void step() override;

  uint16_t getRegisterBitSize(uint16_t regID) override;

  unsigned int getMemoryAddrWidth() override;

  unsigned int getMemoryDataWidth() override;

  MemoryMapping getMemoryMapping(uint64_t addr) override;

  uint64_t getMemoryValue(uint64_t addr) override;

  void setMemoryValue(uint64_t addr, uint64_t data) override;

  std::string getMemoryDisassembly(uint64_t &addr) override;

	void* getRawMemoryPtr() const override;
};


#endif //TL45_EMU_TL45EMULATORSTATE_H
