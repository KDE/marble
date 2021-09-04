// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Henry de Valence <hdevalence@gmail.com>
// SPDX-FileCopyrightText: 2011 Friedrich W. H. Kossebau <kossebau@kde.org>

#ifndef MARBLE_LATLONEDIT_H
#define MARBLE_LATLONEDIT_H

#include <QWidget>

#include "GeoDataCoordinates.h"
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
     * @brief This enum is used to choose the dimension.
     */
    enum Dimension {
        Latitude,             ///< Latitude
        Longitude            ///< Longitude
    };

    explicit LatLonEdit(QWidget *parent = nullptr, Dimension dimension = Longitude,
                        GeoDataCoordinates::Notation notation = GeoDataCoordinates::DMS);
    ~LatLonEdit() override;
    qreal value() const;
    Dimension dimension() const;
    GeoDataCoordinates::Notation notation() const;
public Q_SLOTS:
    void setValue(qreal newvalue);
    void setDimension( Dimension dimension );
    void setNotation(GeoDataCoordinates::Notation notation);
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
