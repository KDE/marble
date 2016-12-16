//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012       Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_SEARCHWIDGET_H
#define MARBLE_SEARCHWIDGET_H

#include "marble_export.h"

#include <QWidget>

namespace Marble {

class GeoDataPlacemark;
class MarbleWidget;
class SearchWidgetPrivate;

class MARBLE_EXPORT SearchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SearchWidget( QWidget * parent = 0, Qt::WindowFlags flags = 0 );

    void setMarbleWidget( MarbleWidget* widget );

    ~SearchWidget() override;

private:
    Q_PRIVATE_SLOT( d, void setSearchResult( const QVector<GeoDataPlacemark*>& ) )
    Q_PRIVATE_SLOT( d, void search( const QString &searchTerm, SearchMode searchMode ) )
    Q_PRIVATE_SLOT( d, void centerMapOn( const QModelIndex &index ) )
    Q_PRIVATE_SLOT( d, void handlePlanetChange() )

    SearchWidgetPrivate* const d;

};

}

#endif
