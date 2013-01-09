//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Rene Kuettner <rene@bitkanal.net>
//

#ifndef MARBLE_ECLIPSESITEM_H
#define MARBLE_ECLIPSESITEM_H

#include <QtCore/QObject>
#include <QDateTime>

namespace Marble
{

class EclipsesItem : public QObject
{
    Q_OBJECT
public:

    enum EclipsePhase {
        PartialSun              = 1,
        NonCentralAnnularSun    = 2,
        NonCentralTotalSun      = 3,
        AnnularSun              = 4,
        TotalSun                = 5,
        AnnularTotalSun         = 6
    };

    EclipsesItem( int index, QObject *parent = 0 );
    ~EclipsesItem();

    int index() const;

    void setDateTime( const QDateTime dateTime );
    QDateTime dateTime() const;

    void setPhase( EclipsesItem::EclipsePhase phase );
    EclipsesItem::EclipsePhase phase() const;
    QString phaseText() const;

    void setMagnitude( double magnitude );
    double magnitude() const;

    bool takesPlaceAt( const QDateTime &dateTime ) const;

private:
    int m_index;
    QDateTime m_dateTime;
    EclipsesItem::EclipsePhase m_phase;
    double m_magnitude;
};

}

#endif // MARBLE_ECLIPSESITEM_H

