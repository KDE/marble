// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2005-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2014 Adam Dabrowski <adamdbrw@gmail.com>
//

#ifndef MARBLE_MARBLEWIDGETINPUTHANDLER_H
#define MARBLE_MARBLEWIDGETINPUTHANDLER_H

#include <QObject>
#include <QSharedPointer>
#include <QPinchGesture>
#include "MarbleInputHandler.h"

namespace Marble
{

class MarbleWidget;
class RenderPlugin;
class MarbleWidgetInputHandlerPrivate;

class MARBLE_EXPORT MarbleWidgetInputHandler : public MarbleDefaultInputHandler
{
    Q_OBJECT

public:
    MarbleWidgetInputHandler(MarbleAbstractPresenter *marblePresenter, MarbleWidget *marbleWidget);

    void setDebugModeEnabled(bool enabled);

private Q_SLOTS:
    void installPluginEventFilter(RenderPlugin *renderPlugin) override;
    void showLmbMenu(int x, int y) override;
    void showRmbMenu(int x, int y) override;
    void openItemToolTip() override;
    void setCursor(const QCursor &cursor) override;

private:
    bool handleKeyPress(QKeyEvent* event) override;
    bool handleTouch(QTouchEvent *event) override;
    bool handleGesture(QGestureEvent *e) override;
    AbstractSelectionRubber *selectionRubber() override;
    bool layersEventFilter(QObject *o, QEvent *e) override;

    void handlePinchGesture(QPinchGesture *pinch);

    using MarbleWidgetInputHandlerPrivatePtr = QSharedPointer<MarbleWidgetInputHandlerPrivate>;
    MarbleWidgetInputHandlerPrivatePtr d;
    friend class MarbleWidgetInputHandlerPrivate;

    Q_DISABLE_COPY(MarbleWidgetInputHandler)
};

}

#endif
