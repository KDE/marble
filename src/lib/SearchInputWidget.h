//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012       Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_SEARCHINPUTWIDGET_H
#define MARBLE_SEARCHINPUTWIDGET_H

#include <QtCore/QModelIndex>

#include "GeoDataTreeModel.h"
#include "MarblePlacemarkModel.h"
#include "MarbleLineEdit.h"
#include "marble_export.h"

namespace Marble {

class MarbleWidget;

class MARBLE_EXPORT SearchInputWidget : public MarbleLineEdit
{
    Q_OBJECT

public:
    SearchInputWidget( QWidget* parent = 0 );

    void setMarbleWidget( MarbleWidget* marbleWidget );

public Q_SLOTS:
    void disableSearchAnimation();

Q_SIGNALS:
    void search( const QString &searchTerm );

private Q_SLOTS:
    void search();

    void centerOnSearchSuggestion( const QModelIndex &suggestionIndex );

private:
    MarbleWidget* m_marbleWidget;
};

}

#endif
