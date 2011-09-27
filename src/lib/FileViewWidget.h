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
#include <QtGui/QWidget>

class QModelIndex;

namespace Marble
{

class FileViewModel;
class GeoDataPlacemark;
class GeoDataLatLonBox;
class GeoDataTreeModel;

class FileViewWidgetPrivate;

class MARBLE_EXPORT FileViewWidget : public QWidget
{
    Q_OBJECT

 public:
    explicit FileViewWidget( QWidget *parent = 0, Qt::WindowFlags f = 0 );
    ~FileViewWidget();

    void setFileViewModel( FileViewModel *model );

    void setTreeModel( GeoDataTreeModel *model );

 public Q_SLOTS:
    void enableFileViewActions();

 Q_SIGNALS:
    void centerOn( const GeoDataPlacemark &, bool animated );
    void centerOn( const GeoDataLatLonBox &, bool animated );

 private Q_SLOTS:
    void mapCenterOnTreeViewModel( const QModelIndex & );

 private:
    Q_DISABLE_COPY( FileViewWidget )

    FileViewWidgetPrivate * const d;
};

}

#endif
