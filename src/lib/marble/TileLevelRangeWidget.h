// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef MARBLE_TILELEVELRANGEWIDGET_H
#define MARBLE_TILELEVELRANGEWIDGET_H

#include <QWidget>

#include "marble_export.h"

namespace Marble
{

class MARBLE_EXPORT TileLevelRangeWidget: public QWidget
{
    Q_OBJECT

 public:
    explicit TileLevelRangeWidget( QWidget * const parent = nullptr, Qt::WindowFlags const f = Qt::WindowFlags() );
    ~TileLevelRangeWidget() override;

    QSize sizeHint() const override;

    void setAllowedLevelRange( int const minimumLevel, int const maximumLevel );
    void setDefaultLevel( int const );

    int topLevel() const;
    int bottomLevel() const;

 Q_SIGNALS:
    void topLevelChanged( int );
    void bottomLevelChanged( int );

 private Q_SLOTS:
    void setMaximumTopLevel( int const );
    void setMinimumBottomLevel( int const );

 private:
    Q_DISABLE_COPY( TileLevelRangeWidget )
    class Private;
    Private * const d;
};

}

#endif
