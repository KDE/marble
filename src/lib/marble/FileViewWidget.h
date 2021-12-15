// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Bastian Holst <bastianholst@gmx.de>
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
    explicit FileViewWidget( QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags() );
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
