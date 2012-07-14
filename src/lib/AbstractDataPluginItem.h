//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_ABSTRACTDATAPLUGINITEM_H
#define MARBLE_ABSTRACTDATAPLUGINITEM_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QHash>

#include "GeoGraphicsItem.h"
#include "marble_export.h"

class QAction;

namespace Marble
{

class AbstractDataPluginItemPrivate;

class MARBLE_EXPORT AbstractDataPluginItem : public QObject, public GeoGraphicsItem
{
    Q_OBJECT

 public:
    explicit AbstractDataPluginItem( QObject *parent = 0 );
    virtual ~AbstractDataPluginItem();

    QString target();
    void setTarget( const QString& target );

    QString id() const;
    void setId( const QString& id );

    bool isFavorite() const;
    virtual void setFavorite( bool favorite );

    /**
      * Returning the angular resolution of the viewport when the item was added to it the last
      * time.
      */
    qreal addedAngularResolution() const;
    void setAddedAngularResolution( qreal resolution );

    /**
     * @brief Set the settings of the item.
     * This is usually called automatically before painting. If you reimplement this it would be
     * useful to check for changes before copying.
     */
    virtual void setSettings( const QHash<QString, QVariant>& settings );

    /**
     * Returns the action of this specific item.
     */
    virtual QAction *action();

    /**
     * Returns the type of this specific item.
     */
    virtual QString itemType() const = 0;

    virtual bool initialized() = 0;

    virtual void addDownloadedFile( const QString& url, const QString& type );

    virtual bool operator<( const AbstractDataPluginItem *other ) const = 0;

    virtual QList<QAction*> actions();

 Q_SIGNALS:
    void updated();
    void favoriteChanged( const QString& id, bool favorite );

 public Q_SLOTS:
   void toggleFavorite();

 private:
    AbstractDataPluginItemPrivate * const d;
};

} // Marble namespace

#endif
