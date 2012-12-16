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

#include <QtCore/QDate>
#include <QtGui/QPixmap>
#include <QVariant>

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
    OpenCachingComItem( QVariantMap cache, QObject *parent );

    ~OpenCachingComItem();

    QString itemType() const;

    bool initialized();

    void paint( QPainter *painter );

    QAction *action();

    bool operator<( const AbstractDataPluginItem *other ) const;

    void addDownloadedFile( const QString &url, const QString &type );

public slots:
    void showInfoDialog();

private slots:
    void dialogCloseClicked();
    void openWww();

private:
    void updateTooltip();

    QString iconName() const;

    QString dateString(const QVariant& ms); ///< create a date string from millisecondssinceepoch in a qvariant

    const QString ratingNumberString(QVariant number) const; ///< "1" -> "1.0"

    QString formatDistance(qreal spheredistance) const; ///< nice string from a Marble::distanceSphere()

    void fillDialogTabs(); ///< loads description, hint and logs into the dialog tabs

    Ui::CacheDetailsDialog *m_ui; ///< 0 unless dialog exists
    OpenCachingComModel *m_model;
    QVariantMap m_cache;
    QAction *m_action;
    QStringList m_images;
};

}
#endif // OPENCACHINGCOMITEM_H
