//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_FILEVIEWWIDGET_H
#define MARBLE_FILEVIEWWIDGET_H

// Marble
#include "marble_export.h"

// Qt
#include <QWidget>

class QModelIndex;

namespace Marble
{

class GeoDataPlacemark;
class GeoDataLatLonBox;
class MarbleWidget;

class FileViewWidgetPrivate;

class MARBLE_EXPORT FileViewWidget : public QWidget
{
    Q_OBJECT

 public:
    explicit FileViewWidget( QWidget *parent = nullptr, Qt::WindowFlags f = nullptr );
    ~FileViewWidget() override;


    void setMarbleWidget( MarbleWidget *widget );

 Q_SIGNALS:
    void centerOn( const GeoDataPlacemark &, bool animated );
    void centerOn( const GeoDataLatLonBox &, bool animated );

 private Q_SLOTS:
    void mapCenterOnTreeViewModel( const QModelIndex & );

 private:
    Q_PRIVATE_SLOT( d, void enableFileViewActions() )
    Q_PRIVATE_SLOT( d, void saveFile() )
    Q_PRIVATE_SLOT( d, void closeFile() )
    Q_PRIVATE_SLOT( d, void contextMenu(const QPoint&) )
    Q_PRIVATE_SLOT( d, void showPlacemarkDialog() )
    Q_DISABLE_COPY( FileViewWidget )

    FileViewWidgetPrivate * const d;
};

}

#endif
