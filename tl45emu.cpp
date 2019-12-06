#include <iostream>
#include "tl45emu.h"
#include "./ui_tl45emu.h"



TL45emu::TL45emu(AbstractEmulatorState *state, QWidget *parent)
        : QMainWindow(parent), ui(new Ui::TL45emu) {
  this->machine_state = state;
  ui->setupUi(this);
  this->registerListModel = new TL45RegisterListModel(this->machine_state);
  this->memoryModel= new TLEmuMemoryModel(this->machine_state);
  this->ui->register_view->setModel(this->registerListModel);
  this->ui->memory_view->setModel(memoryModel);

  // Setup Toolbar
  QAction* openFile = new QAction("Load File", this);
  connect(openFile, &QAction::triggered, this, &TL45emu::onMenuItemClick);
  this->ui->toolBar->addAction(openFile);
}

TL45emu::~TL45emu() {
  delete ui;
  delete registerListModel;
  delete memoryModel;
}

void TL45emu::onMenuItemClick(bool checked) {
  static int currentIndex = 0;
  printf("Button: %d \n", checked);
  fflush(stdout);
  ui->memory_view->setCurrentIndex(memoryModel->index(currentIndex+=100));
}

