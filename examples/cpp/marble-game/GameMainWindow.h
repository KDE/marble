// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#ifndef MARBLE_GAMEMAINWINDOW_H
#define MARBLE_GAMEMAINWINDOW_H

#include <marble/GeoDataCoordinates.h>

#include <QMainWindow>

namespace Marble
{

class Private;
class MarbleWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(const QString &marbleDataPath, QWidget *parent = nullptr);
    ~MainWindow() override;
    MarbleWidget *marbleWidget();

Q_SIGNALS:
    void announceHighlight(qreal, qreal, GeoDataCoordinates::Unit);
    void postQuestion(QObject *);

private Q_SLOTS:
    void createQuestion();
    void browseMapButtonClicked();
    void disableGames();
    void enableCountryShapeGame();
    void enableCountryFlagGame();
    void enableClickOnThatGame();
    void displayResult(bool);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Private *const d;
};

} // namespace Marble

#endif // MARBLE_GAMEMAINWINDOW_H
