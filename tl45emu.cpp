#include <iostream>
#include "tl45emu.h"
#include "./ui_tl45emu.h"



TLEmulator::TLEmulator(AbstractEmulatorState *state, QWidget *parent)
        : QMainWindow(parent), ui(new Ui::TLEmulator) {
  this->machine_state = state;
  ui->setupUi(this);
  this->registerListModel = new TLEmuRegisterListModel(this->machine_state);
  this->memoryModel= new TLEmuMemoryModel(this->machine_state);
  this->ui->register_view->setModel(this->registerListModel);
  this->ui->memory_view->setModel(memoryModel);



  // Setup Toolbar
  QAction* openFile = new QAction("Load File", this);
  connect(openFile, &QAction::triggered, this, &TLEmulator::onMenuItemClick);
  this->ui->toolBar->addAction(openFile);
}

TLEmulator::~TLEmulator() {
  delete ui;
  delete registerListModel;
  delete memoryModel;
}

void TLEmulator::onMenuItemClick(bool checked) {
  static int currentIndex = 0;
  printf("Button: %d \n", checked);
  fflush(stdout);
  ui->memory_view->setCurrentIndex(memoryModel->index(currentIndex+=100));
}

