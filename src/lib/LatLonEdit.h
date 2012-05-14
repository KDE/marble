//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>
// Copyright 2011 Friedrich W. H. Kossebau <kossebau@kde.org>

#ifndef MARBLE_LATLONEDIT_H
#define MARBLE_LATLONEDIT_H

#include <QtGui/QWidget>

#include "MarbleGlobal.h"
#include "marble_export.h"

namespace Marble
{

class LatLonEditPrivate;

class MARBLE_EXPORT  LatLonEdit : public QWidget
{
    Q_OBJECT
    //FIXME: make the dimension enum work
    //Q_PROPERTY( qreal value READ value WRITE setValue )
    //Q_PROPERTY( int dimension READ dimension WRITE setDimension )

public:
    /**
     * @brief enum used to specify the notation / numerical system
     */
    enum Notation {
        Decimal, ///< "Decimal" notation (base-10)
        DMS,     ///< "Sexagesimal DMS" notation (base-60)
        DM       ///< "Sexagesimal DM" notation (base-60)
    };

public:
    explicit LatLonEdit(QWidget *parent = 0, Dimension dimension = Longitude, Notation notation = DMS);
    ~LatLonEdit();
    qreal value() const;
    Dimension dimension() const;
    Notation notation() const;
public Q_SLOTS:
    void setValue(qreal newvalue);
    void setDimension( Dimension dimension );
    void setNotation(Notation notation);
Q_SIGNALS:
    void valueChanged( qreal value );
private Q_SLOTS:
    void checkIntValueOverflow();
    void checkUIntValueOverflow();
    void checkFloatValueOverflow();
    void onSignChanged();
private:
    // recalculates m_value based on spinboxes
    void recalculate();
private:
    LatLonEditPrivate * const d;
};

}

#endif
