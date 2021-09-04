// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Abhinav Gangwar <abhgang@gmail.com>
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
    ~CountryByShape() override;

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
