// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_SEARCHINPUTWIDGET_H
#define MARBLE_SEARCHINPUTWIDGET_H

#include <QSortFilterProxyModel>

#include "MarbleGlobal.h"
#include "MarbleLineEdit.h"
#include "marble_export.h"

class QAbstractItemModel;

class QCompleter;
class QModelIndex;

namespace Marble
{

class GeoDataCoordinates;

class MARBLE_EXPORT SearchInputWidget : public MarbleLineEdit
{
    Q_OBJECT

public:
    explicit SearchInputWidget(QWidget *parent = nullptr);

    void setCompletionModel(QAbstractItemModel *completionModel);

public Q_SLOTS:
    void disableSearchAnimation();

Q_SIGNALS:
    void search(const QString &searchTerm, SearchMode searchMode);

    void centerOn(const GeoDataCoordinates &coordinates);

private Q_SLOTS:
    void search();

    void centerOnSearchSuggestion(const QModelIndex &suggestionIndex);

    void showDropDownMenu();

    void setGlobalSearch();

    void setAreaSearch();

private:
    void updatePlaceholderText();

    QSortFilterProxyModel m_sortFilter;
    QCompleter *const m_completer;
    bool m_areaSearch;
};

}

#endif
