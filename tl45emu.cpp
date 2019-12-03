#include "tl45emu.h"
#include "./ui_tl45emu.h"

TL45emu::TL45emu(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TL45emu)
{
    ui->setupUi(this);
    this->registerListModel = new TL45RegisterListModel;
    this->ui->register_view->setModel(this->registerListModel);
}

TL45emu::~TL45emu()
{
    delete ui;
    delete registerListModel;
}

