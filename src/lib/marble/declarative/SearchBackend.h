//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Gábor Péterffy <peterffy95@gmail.com>
//

#ifndef MARBLE_SEARCHBACKEND_H
#define MARBLE_SEARCHBACKEND_H

#include <QObject>
#include <QSortFilterProxyModel>

#include "MarbleQuickItem.h"
#include "MarblePlacemarkModel.h"
#include "Placemark.h"

class QCompleter;

namespace Marble
{
class SearchRunnerManager;

class SearchBackend : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject *marbleQuickItem READ marbleQuickItem WRITE setMarbleQuickItem NOTIFY marbleQuickItemChanged)
    Q_PROPERTY(MarblePlacemarkModel *completionModel READ completionModel NOTIFY completionModelChanged)
    Q_PROPERTY(Placemark* selectedPlacemark READ selectedPlacemark NOTIFY selectedPlacemarkChanged)

public:
    explicit SearchBackend(QObject *parent = 0);
    Q_INVOKABLE void search(const QString &place);
    Q_INVOKABLE void setCompletionPrefix(const QString &prefix);
    QObject *marbleQuickItem();
    MarblePlacemarkModel *completionModel();
    const QObject* marbleQuickItem() const;
    Placemark* selectedPlacemark();

Q_SIGNALS:
    void marbleQuickItemChanged(QObject *marbleQuickItem);
    void completionModelChanged(MarblePlacemarkModel *model);
    void searchResultChanged(MarblePlacemarkModel *model);
    void searchFinished(const QString &searchTerm);
    void selectedPlacemarkChanged(Placemark * selectedPlacemark);

public Q_SLOTS:
    Q_INVOKABLE void setSelectedPlacemark(int placemarkIndex);
    void setMarbleQuickItem(QObject *marbleQuickItem);
    void updateSearchResult(QAbstractItemModel *result);

private:
    GeoDataPlacemark *placemarkFromQVariant(const QVariant &data) const;
    QSortFilterProxyModel m_model;
    SearchRunnerManager *m_searchManager;
    MarbleQuickItem *m_marbleQuickItem;
    MarblePlacemarkModel *m_placemarkModel;
    QCompleter *m_completer;
    MarblePlacemarkModel *m_completionModel;
    QVector<GeoDataPlacemark*> *m_completionContainer;
    Placemark m_selectedPlacemark;
    QString m_lastSuccessfulCompletion;
};

}
#endif
