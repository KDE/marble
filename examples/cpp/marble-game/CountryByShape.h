//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#ifndef MARBLE_COUNTRY_BY_SHAPE
#define MARBLE_COUNTRY_BY_SHAPE

// Qt
#include <QObject>

// Marble
#include <marble/GeoDataCoordinates.h>

namespace Marble
{
class CountryByShapePrivate;
class MarbleWidget;

class CountryByShape : public QObject
{
    Q_OBJECT
public:
    explicit CountryByShape( MarbleWidget *widget );
    ~CountryByShape();

Q_SIGNALS:
    void gameInitialized();
    void announceHighlight(qreal, qreal, GeoDataCoordinates::Unit );

public Q_SLOTS:
    void postQuestion( QObject *gameObject );
    void initiateGame();

private:
    CountryByShapePrivate * const d;
};

}   // namespace Marble

#endif  // MARBLE_COUNTRY_BY_SHAPE
