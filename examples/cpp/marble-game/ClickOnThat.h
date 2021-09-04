// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Abhinav Gangwar <abhgang@gmail.com>
//


#ifndef MARBLE_CLICK_ON_THAT
#define MARBLE_CLICK_ON_THAT

// Qt
#include <QObject>

// Marble
#include <marble/GeoDataCoordinates.h>

namespace Marble
{
class ClickOnThatPrivate;
class MarbleWidget;

class ClickOnThat : public QObject
{
    Q_OBJECT
public:
    explicit ClickOnThat( MarbleWidget *marbleWidget );
    ~ClickOnThat() override;

    /**
     * disable the GeoDataDocument which
     * shows the pin on map. This method
     * is called when this game quits.
     */
    void disablePinDocument();

public Q_SLOTS:
    void initiateGame();
    void postQuestion( QObject* );
    void updateSelectPin( bool, const GeoDataCoordinates& );
    void determineResult( qreal, qreal, GeoDataCoordinates::Unit );
    void highlightCorrectAnswer();

Q_SIGNALS:
    void gameInitialized();
    void updateResult( bool );
    void announceHighlight(qreal, qreal, GeoDataCoordinates::Unit );

private:
    ClickOnThatPrivate * const d;
};

}   // namespace Marble

#endif  // MARBLE_CLICK_ON_THAT
