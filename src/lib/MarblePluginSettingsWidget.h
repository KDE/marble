//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
//


#ifndef __MARBLEPLUGINSETTINGSWIDGET_H
#define __MARBLEPLUGINSETTINGSWIDGET_H


/** @file
 * This file contains the multiple inheritance ui-wrapper for the
 * MarblePluginSettingsWidget ui file.
 * 
 * @author Torsten Rahn  <torsten@kde.org>
 */

#include <QtGui/QWidget>
#include "ui_MarblePluginSettingsWidget.h"

#include "marble_export.h"

/** 
 * @short A public class that adds methods to the UI Plugins Settings Widget.
 *
 */

class QStandardItemModel;

namespace Marble
{

class MARBLE_EXPORT MarblePluginSettingsWidget : public QWidget, private Ui::MarblePluginSettingsWidget
{
    Q_OBJECT

 public:
    MarblePluginSettingsWidget( QWidget *parent = 0 );

    void setModel( QStandardItemModel* );

 Q_SIGNALS:
    void pluginListViewClicked();
};

}

#endif // MARBLEPLUGINSETTINGSWIDGET_H
