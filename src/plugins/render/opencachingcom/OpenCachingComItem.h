//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Anders Lund <anders@alweb.dk>
//

#ifndef OPENCACHINGCOMITEM_H
#define OPENCACHINGCOMITEM_H

#include "AbstractDataPluginItem.h"
// #include "OpenCachingComCache.h"

#include <QVariantMap>

class QAction;

namespace Ui {
    class CacheDetailsDialog;
}

namespace Marble
{

class OpenCachingComModel;

class OpenCachingComItem : public AbstractDataPluginItem
{
    Q_OBJECT

public:
    OpenCachingComItem( const QVariantMap& cache, OpenCachingComModel *parent );

    ~OpenCachingComItem();

    bool initialized() const;

    void paint( QPainter *painter );

    QAction *action();

    bool operator<( const AbstractDataPluginItem *other ) const;

    void addDownloadedFile( const QString &url, const QString &type );

public Q_SLOTS:
    void showInfoDialog();

private Q_SLOTS:
    void dialogCloseClicked();
    void openWww();

private:
    void updateTooltip();

    QString iconName() const;

    static QString dateString(const QVariant& ms); ///< create a date string from millisecondssinceepoch in a qvariant

    static const QString ratingNumberString(const QVariant& number); ///< "1" -> "1.0"

    QString formatDistance(qreal spheredistance) const; ///< nice string from a Marble::distanceSphere()

    void fillDialogTabs(); ///< loads description, hint and logs into the dialog tabs

    Ui::CacheDetailsDialog *m_ui; ///< 0 unless dialog exists
    OpenCachingComModel *const m_model;
    QVariantMap m_cache;
    QAction *const m_action;
    QStringList m_images;
};

}
#endif // OPENCACHINGCOMITEM_H
