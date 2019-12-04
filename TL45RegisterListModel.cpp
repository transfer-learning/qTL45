//
// Created by codetector on 12/3/19.
//
#include <iostream>
#include <QtWidgets/QLabel>
#include "TL45RegisterListModel.h"

int TL45RegisterListModel::rowCount(const QModelIndex &parent) const {
  return 10;
}

QVariant TL45RegisterListModel::data(const QModelIndex &index, int role) const {
  if (role == Qt::DisplayRole) {
    return QVariant("LMAO");
  }
  return QVariant();
}

TL45RegisterListModel::TL45RegisterListModel(tl45_state *machine_state) {
  this->machine_state = machine_state;
}
