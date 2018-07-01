//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2005-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2014      Adam Dabrowski <adamdbrw@gmail.com>
//

#ifndef MARBLE_MARBLEWIDGETINPUTHANDLER_H
#define MARBLE_MARBLEWIDGETINPUTHANDLER_H

#include <QObject>
#include <QSharedPointer>
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
    AbstractSelectionRubber *selectionRubber() override;
    bool layersEventFilter(QObject *o, QEvent *e) override;

    using MarbleWidgetInputHandlerPrivatePtr = QSharedPointer<MarbleWidgetInputHandlerPrivate>;
    MarbleWidgetInputHandlerPrivatePtr d;
    friend class MarbleWidgetInputHandlerPrivate;

    Q_DISABLE_COPY(MarbleWidgetInputHandler)
};

}

#endif
