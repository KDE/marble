//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "DeclarativeMapThemeManager.h"

#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"

#include <QIcon>
#include <QStandardItemModel>
#include <QDebug>

MapThemeImageProvider::MapThemeImageProvider() :
        QQuickImageProvider( QQuickImageProvider::Pixmap )
{
    // nothing to do
}

QPixmap MapThemeImageProvider::requestPixmap( const QString &id, QSize *size, const QSize &requestedSize )
{
    QSize const resultSize = requestedSize.isValid() ? requestedSize : QSize( 128, 128 );
    if ( size ) {
        *size = resultSize;
    }

    QStandardItemModel *model = m_mapThemeManager.mapThemeModel();
    for( int i = 0; i < model->rowCount(); ++i ) {
        if ( model->data( model->index( i, 0 ), Qt::UserRole + 1 ) == id ) {
            QIcon icon = model->data( model->index( i, 0 ), Qt::DecorationRole ).value<QIcon>();
            QPixmap result = icon.pixmap( resultSize );
            return result;
        }
    }

    QPixmap empty( resultSize );
    empty.fill();
    return empty;
}

MapThemeManager::MapThemeManager( QObject *parent ) : QObject( parent )
{
    // nothing to do
}

QStringList MapThemeManager::mapThemeIds() const
{
    return m_mapThemeManager.mapThemeIds();
}

#include "moc_DeclarativeMapThemeManager.cpp"
