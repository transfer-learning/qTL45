#include <iostream>
#include <QShortcut>
#include "tl45emu.h"
#include "./ui_tl45emu.h"


TLEmulator::TLEmulator(AbstractEmulatorState *state, QWidget *parent)
        : QMainWindow(parent), ui(new Ui::TLEmulator) {
  this->machine_state = state;
  ui->setupUi(this);
  this->registerListModel = new TLEmuRegisterListModel(this->machine_state);
  this->memoryModel = new TLEmuMemoryModel(this->machine_state);
  this->ui->register_view->setModel(this->registerListModel);
  this->ui->memory_view->setModel(memoryModel);



  // Setup Toolbar
  QAction *openFile = new QAction("Load File", this);
  connect(openFile, &QAction::triggered, this, &TLEmulator::onMenuItemClick);
  this->ui->toolBar->addAction(openFile);

  QAction *step = new QAction("Step", this);

  connect(step, &QAction::triggered, this, [=](bool checked) {
    state->step();

    registerListModel->registersChanged();
    memoryModel->memoryChanged();
  });

  this->ui->toolBar->addAction(step);

  {
    QAction *step = new QAction("Step 10K", this);

    connect(step, &QAction::triggered, this, [=](bool checked) {
      for (int i = 0; i < 10000; i++) {
        state->step();
      }

      registerListModel->registersChanged();
      memoryModel->memoryChanged();
    });

    this->ui->toolBar->addAction(step);
  }

  auto shortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
  connect(shortcut, &QShortcut::activated, this, [=]() {
    state->step();

    registerListModel->registersChanged();
    memoryModel->memoryChanged();
  });

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
  ui->memory_view->setCurrentIndex(memoryModel->index(currentIndex += 100));
}

