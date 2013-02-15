//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef MARBLE_MAPTHEMEDOWNLOADDIALOG_H
#define MARBLE_MAPTHEMEDOWNLOADDIALOG_H

#include <QtGui/QDialog>

#include "marble_export.h"

namespace Marble {

class MARBLE_EXPORT MapThemeDownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MapThemeDownloadDialog(QWidget *parent = 0);
    ~MapThemeDownloadDialog();

private:
    Q_DISABLE_COPY( MapThemeDownloadDialog )
    class Private;
    Private *const d;
};

}

#endif
