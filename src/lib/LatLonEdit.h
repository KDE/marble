//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Henry de Valence <hdevalence@gmail.com>

#ifndef MARBLE_LATLONEDIT_H
#define MARBLE_LATLONEDIT_H

#include <QtGui/QWidget>

#include "global.h"
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
    explicit LatLonEdit(QWidget *parent = 0, Dimension dimension = Longitude );
    ~LatLonEdit();
    qreal value() const;
    Dimension dimension() const;
public Q_SLOTS:
    void setValue(qreal newvalue);
    void setDimension( Dimension dimension );
Q_SIGNALS:
    void valueChanged( qreal value );
private Q_SLOTS:
    // changes value based on combobox
    void onSignChanged();
    void checkSecOverflow();
    void checkMinOverflow();
    void checkDegOverflow();
private:
    // recalculates m_value based on spinboxes
    void recalculate();
private:
    LatLonEditPrivate * const d;
};

}

#endif
