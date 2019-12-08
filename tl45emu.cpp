#include <iostream>
#include <QShortcut>
#include <QtWidgets/QFileDialog>
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
  auto *openFile = new QAction("Load File", this);
  connect(openFile, &QAction::triggered, this, &TLEmulator::onMenuItemClick);
  this->ui->toolBar->addAction(openFile);

  auto *step = new QAction("Step", this);

  connect(step, &QAction::triggered, this, [=](bool checked) {
    state->step();

    registerListModel->registersChanged();
    memoryModel->memoryChanged();
  });

  this->ui->toolBar->addAction(step);

  {
    auto *step10K = new QAction("Step 10K", this);

    connect(step10K, &QAction::triggered, this, [=](bool checked) {
      for (int i = 0; i < 10000; i++) {
        state->step();
      }

      registerListModel->registersChanged();
      memoryModel->memoryChanged();
    });

    this->ui->toolBar->addAction(step10K);
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
  QString filename = QFileDialog::getOpenFileName(this, tr("Open Binary File"), "",
          tr("All Files (*)"));
  machine_state->load(filename.toStdString());
}

