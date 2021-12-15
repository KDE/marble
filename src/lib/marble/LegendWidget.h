// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Bastian Holst <bastianholst@gmx.de>
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
    explicit LegendWidget( QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags() );
    ~LegendWidget() override;

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
