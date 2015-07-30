//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Gábor Péterffy <peterffy95@gmail.com>
//

#ifndef MARBLE_SEARCHBARBACKEND_H
#define MARBLE_SEARCHBARBACKEND_H

#include <QObject>
#include <QSortFilterProxyModel>
#include <QCompleter>

#include "MarbleQuickItem.h"
#include "SearchRunnerManager.h"
#include "MarblePlacemarkModel.h"

namespace Marble
{

class SearchBarBackend : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject *marbleQuickItem READ marbleQuickItem WRITE setMarbleQuickItem NOTIFY marbleQuickItemChanged)
    Q_PROPERTY(MarblePlacemarkModel *completionModel READ completionModel NOTIFY completionModelChanged)

public:
    SearchBarBackend(QObject *parent = 0);
    Q_INVOKABLE void search(const QString &place);
    Q_INVOKABLE void setCompletionPrefix(const QString &prefix);
    QObject *marbleQuickItem();
    MarblePlacemarkModel *completionModel();
    const QObject* marbleQuickItem() const;

signals:
    void marbleQuickItemChanged(QObject *marbleQuickItem);
    void completionModelChanged(MarblePlacemarkModel *model);
    void updateSearchResults(MarblePlacemarkModel *model);

public slots:
    Q_INVOKABLE void updateMap(int placemarkIndex);
    void setMarbleQuickItem(QObject *marbleQuickItem);
    void searchFinished(QAbstractItemModel *result);

private:
    GeoDataPlacemark *placemarkFromQVariant(const QVariant &data) const;
    QSortFilterProxyModel m_model;
    SearchRunnerManager *m_searchManager;
    MarbleQuickItem *m_marbleQuickItem;
    MarblePlacemarkModel *m_placemarkModel;
    QCompleter *m_completer;
    MarblePlacemarkModel *m_completionModel;
    QVector<GeoDataPlacemark*> *m_completionContainer;
};

}
#endif
