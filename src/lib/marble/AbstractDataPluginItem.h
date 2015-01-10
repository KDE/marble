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

#include <QObject>
#include <QString>
#include <QHash>

#include "BillboardGraphicsItem.h"
#include "marble_export.h"

class QAction;

namespace Marble
{

class AbstractDataPluginItemPrivate;

class MARBLE_EXPORT AbstractDataPluginItem : public QObject, public BillboardGraphicsItem
{
    Q_OBJECT

    Q_PROPERTY( QString identifier READ id WRITE setId NOTIFY idChanged )
    Q_PROPERTY( bool favorite READ isFavorite WRITE setFavorite NOTIFY favoriteChanged )
    Q_PROPERTY( bool sticky READ isSticky WRITE setSticky NOTIFY stickyChanged )

 public:
    explicit AbstractDataPluginItem( QObject *parent = 0 );
    virtual ~AbstractDataPluginItem();

    /**
     * Returns the item's tool tip.
     */
    QString toolTip() const;

    /**
     * Set the tool tip for the item.
     */
    void setToolTip( const QString& toolTip );

    QString id() const;
    void setId( const QString& id );

    bool isFavorite() const;
    virtual void setFavorite( bool favorite );

    bool isSticky() const;
    void setSticky( bool sticky );

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

    virtual bool initialized() const = 0;

    virtual void addDownloadedFile( const QString& url, const QString& type );

    virtual bool operator<( const AbstractDataPluginItem *other ) const = 0;

    virtual QList<QAction*> actions();

 Q_SIGNALS:
    void updated();
    void idChanged();
    void favoriteChanged( const QString& id, bool favorite );
    void stickyChanged();

 public Q_SLOTS:
   void toggleFavorite();

 private:
    friend class AbstractDataPluginModel;

    /**
     * Returning the angular resolution of the viewport when the item was added to it the last
     * time.
     */
    qreal addedAngularResolution() const;
    void setAddedAngularResolution( qreal resolution );

    AbstractDataPluginItemPrivate * const d;
};

} // Marble namespace

#endif
