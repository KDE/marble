//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012       Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2012       Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_SEARCHINPUTWIDGET_H
#define MARBLE_SEARCHINPUTWIDGET_H

#include <QSortFilterProxyModel>

#include "MarbleLineEdit.h"
#include "MarbleGlobal.h"
#include "marble_export.h"

class QAbstractItemModel;

class QCompleter;
class QModelIndex;

namespace Marble {

class GeoDataCoordinates;

class MARBLE_EXPORT SearchInputWidget : public MarbleLineEdit
{
    Q_OBJECT

public:
    explicit SearchInputWidget( QWidget* parent = 0 );

    void setCompletionModel( QAbstractItemModel *completionModel );

public Q_SLOTS:
    void disableSearchAnimation();

Q_SIGNALS:
    void search( const QString &searchTerm, SearchMode searchMode );

    void centerOn( const GeoDataCoordinates &coordinates );

private Q_SLOTS:
    void search();

    void centerOnSearchSuggestion( const QModelIndex &suggestionIndex );

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
