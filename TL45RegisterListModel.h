//
// Created by codetector on 12/3/19.
//
#include <QtCore/QStringListModel>
#include <QtWidgets/QAbstractItemDelegate>
#include "tl45_emu_core.h"

class TL45RegisterListModel : public QAbstractListModel{
public:
  TL45RegisterListModel(tl45_state *machine_state);
  int rowCount(const QModelIndex &parent) const override;

  QVariant data(const QModelIndex &index, int role) const override;
private:
  tl45_state *machine_state;
};
