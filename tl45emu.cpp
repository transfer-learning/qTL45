#include <vector>
#include <algorithm>

#include <iostream>
#include <QShortcut>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QInputDialog>
#include "tl45emu.h"
#include "./ui_tl45emu.h"


TLEmulator::TLEmulator(TL45EmulatorState *state, QWidget *parent)
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

  auto *gotoAction = new QAction("Go to", this);
  connect(gotoAction, &QAction::triggered, this, &TLEmulator::gotoMemoryClick);
  this->ui->toolBar->addAction(gotoAction);

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

  {
    auto *run = new QAction("Run", this);

    connect(run, &QAction::triggered, this, [=](bool checked) {
      state->run();

      registerListModel->registersChanged();
      memoryModel->memoryChanged();
    });

    this->ui->toolBar->addAction(run);
  }

  {
    auto *printProfile = new QAction("Print Profile", this);

    connect(printProfile, &QAction::triggered, this, [=](bool checked) {
      // display profiling info
      std::vector<TL45::branch> branches;
      auto& profile = state->state.profile;
      for (auto it = profile.branch_count.begin(); it != profile.branch_count.end(); ++it) {
        branches.push_back(it->first);
      }
      std::sort(branches.begin(), branches.end(), [&profile](const auto& a, const auto& b) -> bool { 
        return profile.branch_count[a] < profile.branch_count[b]; // we want sort in descending order
      });
      printf("top branch report:\n");
      for (int i = 0; i < 200 && i < branches.size(); i++) {
        auto& taint_set = profile.branch_taint[branches[i]];
        if (taint_set.size() == 0)
          continue;
        printf("%08x:%c | %08x hits, tainted by: ", branches[i].first, branches[i].second ? 't': 'f', profile.branch_count[branches[i]]);
        for (uint32_t x : taint_set) {
          printf("%u ", x);
        }
        printf("\n");
      }
    });

    this->ui->toolBar->addAction(printProfile);
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
  machine_state->clear();
  machine_state->load(filename.toStdString());
  char* input = R"({"menu":{"id":"file","value":"File","popup":{"menuitem":[{"value":"New","onclick":"CreateNewDoc"},{"value":"Open","onclick":"OpenDoc","memes":129},{"value":"Close","onclick":"CloseDoc"}]}},"yeet":99})";
  machine_state->writeMemory(INPUT_TAINT_START, strlen(input), input);
}

void TLEmulator::gotoMemoryClick(bool checked) {
  bool ok;
  QString addr = QInputDialog::getText(this, tr("Enter Address (hex)"), "",
          QLineEdit::Normal, "", &ok);
  if (!ok) return;
  int32_t i = addr.toInt(&ok, 16);
  if (!ok) return;
  printf("goto %x\n", i);
  this->memoryModel->setBaseAddress(i);
}

