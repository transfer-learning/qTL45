//
// Created by codetector on 12/3/19.
//
#include <QtCore/QStringListModel>
#include <QtWidgets/QAbstractItemDelegate>

class TL45RegisterListModel : public QAbstractListModel{
public:
  int rowCount(const QModelIndex &parent) const override;

  QVariant data(const QModelIndex &index, int role) const override;
};
