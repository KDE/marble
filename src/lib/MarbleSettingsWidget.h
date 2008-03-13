//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//


#ifndef MARBLESETTINGSWIDGET_H
#define MARBLESETTINGSWIDGET_H


/** @file
 * This file contains the headers for MarbleSettingsWidget.
 *
 * @author Torsten Rahn <tackat@kde.org>
 */


#include <QtGui/QWidget>

#include "marble_export.h"


class MarbleSettingsWidgetPrivate;

/** 
 * @short A widget class that contains settings controls for a
 * MarbleWidget.
 *
 * This widget lets the user control control the settings of a 
 * MarbleWidget.
 *
 * @see MarbleWidget
 */

class MARBLE_EXPORT MarbleSettingsWidget : public QWidget
{
    Q_OBJECT

 public:
    /**
     * @brief Construct a new MarbleSettingsWidget
     * @param parent the parent widget
     */
    MarbleSettingsWidget(QWidget *parent = 0);
    ~MarbleSettingsWidget();
 
 private:
    MarbleSettingsWidgetPrivate  * const d;
};

#endif // MARBLESETTINGSWIDGET_H
