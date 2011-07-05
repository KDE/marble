//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>
//

#ifndef OPENCACHINGITEM_H
#define OPENCACHINGITEM_H

#include "AbstractDataPluginItem.h"
#include "OpenCachingCache.h"

#include <QtCore/QDate>
#include <QtGui/QPixmap>

class QFont;

namespace Ui
{
    class OpenCachingCacheDialog;
}

namespace Marble
{

class OpenCachingItem : public AbstractDataPluginItem
{
    Q_OBJECT

public:
    OpenCachingItem( const OpenCachingCache& cache, QObject *parent );

    ~OpenCachingItem();

    // Returns the item type of the item.
    QString itemType() const;

    // Returns true if the item is paintable
    bool initialized();

    // Here the item gets painted
    void paint( GeoPainter *painter, ViewportParams *viewport,
                const QString& renderPos, GeoSceneLayer * layer = 0 );

    QAction *action();

    bool operator<( const AbstractDataPluginItem *other ) const;

public slots:
    void showInfoDialog();

private:
    void updateTooltip();

    QDialog *infoDialog();

    OpenCachingCache m_cache;

    Ui::OpenCachingCacheDialog *m_ui;
    QDialog *m_infoDialog;
    QAction *m_action;

    int m_logIndex;

    static QFont s_font;
    static QPixmap s_icon;

private slots:
    void updateDescriptionLanguage( QString language );

    void nextLogEntry();

    void previousLogEntry();
};

}
#endif // OPENCACHINGITEM_H
