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

    EclipsesItem( QObject *parent = 0 );
    ~EclipsesItem();

    void setDateTime( const QDateTime dateTime );
    QDateTime dateTime() const;

    void setPhase( EclipsesItem::EclipsePhase phase );
    EclipsesItem::EclipsePhase phase() const;
    QString phaseText() const;

private:
    QDateTime m_dateTime;
    EclipsesItem::EclipsePhase m_phase;
};

}

#endif // MARBLE_ECLIPSESITEM_H

