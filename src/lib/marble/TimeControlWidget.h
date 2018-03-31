//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Harshit Jain   <hjain.itbhu@gmail.com>
//

#ifndef MARBLE_TIMECONTROLWIDGET_H
#define MARBLE_TIMECONTROLWIDGET_H

// Qt
#include <QDateTime>
#include <QDialog>

// Marble
#include "marble_export.h"

namespace Ui
{
    class TimeControlWidget;
}

namespace Marble
{

class MarbleClock;

class MARBLE_EXPORT TimeControlWidget : public QDialog
{
    Q_OBJECT

 public:
    explicit TimeControlWidget( MarbleClock* clock, QWidget* parent = nullptr );
    ~TimeControlWidget() override;

 private Q_SLOTS:
    /**
    * @brief  apply the settings to internal clock
    */
    void apply();

    /**
    * @brief  set the newDateTimeEdit to current system time
    */
    void nowClicked();

    /**
     * @brief Set the refreshIntervalSpinBox value to @p seconds.
     */
    void updateRefreshRate( int seconds );

    /**
    * @brief  set the text of speedLabel to @p speed
    */
    void updateSpeedLabel( int speed );

    /**
    * @brief  update the currentDateTimeEdit to current internal time
    */
    void updateDateTime();

 protected:
    Q_DISABLE_COPY( TimeControlWidget )

    void showEvent( QShowEvent* event ) override;

    Ui::TimeControlWidget *m_uiWidget;

    MarbleClock *m_clock;
    QDateTime    m_lastDateTime;
};

}

#endif  //MARBLE_TIMECONTROLWIDGET_H
