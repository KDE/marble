//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Mikhail Ivchenko <ematirov@gmail.com>


#ifndef MARBLE_PLACEMARKEDITHEADER_H
#define MARBLE_PLACEMARKEDITHEADER_H

#include <QWidget>

#include "GeoDataCoordinates.h"
#include "MarbleGlobal.h"
#include "marble_export.h"

namespace Marble
{

class PlacemarkEditHeaderPrivate;

class MARBLE_EXPORT PlacemarkEditHeader : public QWidget
{
    Q_OBJECT

public:
    explicit PlacemarkEditHeader(QWidget *parent = 0,
                        GeoDataCoordinates::Notation notation = GeoDataCoordinates::DMS,
                        const QString& name = QString(),
                        const QString& iconLink = QString(),
                        const QString& id = QString(),
                        const QStringList &idFilter = QStringList() );
    ~PlacemarkEditHeader();
    QString name() const;
    QString iconLink() const;
    qreal latitude() const;
    qreal longitude() const;
    GeoDataCoordinates::Notation notation() const;
    bool positionVisible() const;
    QString id() const;
    QStringList idFilter() const;
    bool isIdVisible() const;
    bool isIdValid() const;
    QString targetId() const;
    QStringList targetIdList() const;
    bool isTargetIdVisible() const;
public Q_SLOTS:
    void setName(const QString &name);
    void setIconLink(const QString &iconLink);
    void setLatitude(qreal latitude);
    void setLongitude(qreal longitude);
    void setNotation(GeoDataCoordinates::Notation notation);
    void setPositionVisible( bool visible );
    QString setId( const QString &id );
    void setIdFilter( const QStringList &filter );
    void setIdVisible( bool visible );
    void setTargetId( const QString &targetId );
    void setTargetIdList( const QStringList &targetIdList );
    void setTargetIdVisible( bool visible );
    void selectAll();
    void setReadOnly(bool state);
Q_SIGNALS:
    void valueChanged();
private Q_SLOTS:
    void updateValues();
private:
    PlacemarkEditHeaderPrivate* const d;
    Q_PRIVATE_SLOT(d, void loadIconFile())
    friend class PlacemarkEditHeaderPrivate;
};

}

#endif
