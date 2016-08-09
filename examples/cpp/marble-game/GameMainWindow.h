//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#ifndef MARBLE_GAMEMAINWINDOW_H
#define MARBLE_GAMEMAINWINDOW_H

#include <marble/GeoDataCoordinates.h>

#include <QMainWindow>

namespace Marble
{

class Private;
class MarbleWidget;

class MainWindow: public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow( const QString &marbleDataPath, QWidget *parent = 0, Qt::WindowFlags flags = 0 );
    virtual ~MainWindow();
    MarbleWidget *marbleWidget();

Q_SIGNALS:
    void announceHighlight(qreal, qreal, GeoDataCoordinates::Unit );
    void postQuestion( QObject* );

private Q_SLOTS:
    void createQuestion();
    void browseMapButtonClicked();
    void disableGames();
    void enableCountryShapeGame();
    void enableCountryFlagGame();
    void enableClickOnThatGame();
    void displayResult( bool );

protected:
    void resizeEvent( QResizeEvent *event );

private:
    Private * const d;
};

}   // namespace Marble

#endif  // MARBLE_GAMEMAINWINDOW_H
