#ifndef TL45EMU_H
#define TL45EMU_H

#include <QMainWindow>
#include "TL45RegisterListModel.h"
#include "tl45_emu_core.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TL45emu; }
QT_END_NAMESPACE

class TL45emu : public QMainWindow
{
    Q_OBJECT

public:
    TL45emu(QWidget *parent = nullptr);
    ~TL45emu();

private:
    Ui::TL45emu *ui;
    TL45RegisterListModel *registerListModel;
    tl45_state* machine_state;
};
#endif // TL45EMU_H