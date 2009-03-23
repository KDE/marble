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


#ifndef __MARBLECACHESETTINGSWIDGET_H
#define __MARBLECACHESETTINGSWIDGET_H


/** @file
 * This file contains the multiple inheritance ui-wrapper for the
 * MarbleCacheSettingsWidget ui file.
 * 
 * @author Torsten Rahn  <torsten@kde.org>
 */

#include <QtGui/QWidget>
#include "ui_MarbleCacheSettingsWidget.h"

#include "marble_export.h"

namespace Marble
{

/** 
 * @short A public class that adds methods to the UI Cache Settings Widget.
 *
 */

class MARBLE_EXPORT MarbleCacheSettingsWidget : public QWidget, private Ui::MarbleCacheSettingsWidget
{
    Q_OBJECT

 public:
    MarbleCacheSettingsWidget( QWidget *parent = 0 );


 Q_SIGNALS:
    void clearVolatileCache();
    void clearPersistentCache();
};

}

#endif // MARBLECACHESETTINGSWIDGET_H
