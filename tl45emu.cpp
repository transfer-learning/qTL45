#include "tl45emu.h"
#include "./ui_tl45emu.h"

TL45emu::TL45emu(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TL45emu)
{
    this->machine_state = new tl45_state;
    ui->setupUi(this);
    this->registerListModel = new TL45RegisterListModel(this->machine_state);
    this->ui->register_view->setModel(this->registerListModel);
}

TL45emu::~TL45emu()
{
    delete ui;
    delete registerListModel;
}

