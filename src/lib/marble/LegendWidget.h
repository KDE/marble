//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_LEGENDWIDGET_H
#define MARBLE_LEGENDWIDGET_H

// Marble
#include "marble_export.h"

// Qt
#include <QWidget>

namespace Marble
{

class LegendWidgetPrivate;

class MarbleModel;

class MARBLE_EXPORT LegendWidget : public QWidget
{
    Q_OBJECT

 public:
    explicit LegendWidget( QWidget *parent = 0, Qt::WindowFlags f = 0 );
    ~LegendWidget();

    /**
     * @brief Set a MarbleModel associated to this widget.
     * @param model  the MarbleModel to be set.
     */
    void setMarbleModel( MarbleModel *model );

 Q_SIGNALS:
    void propertyValueChanged( const QString &propertyName, bool value );
    void tourLinkClicked( const QString &url );

 private:
    Q_DISABLE_COPY( LegendWidget )

    LegendWidgetPrivate * const d;
};

}

#endif
