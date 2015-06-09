//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2013 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_MAPTHEMEDOWNLOADDIALOG_H
#define MARBLE_MAPTHEMEDOWNLOADDIALOG_H

#include <QDialog>

#include "marble_export.h"

namespace Marble {

class MarbleWidget;

class MARBLE_EXPORT MapThemeDownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MapThemeDownloadDialog( MarbleWidget *marbleWidget );
    ~MapThemeDownloadDialog();

private:
    Q_DISABLE_COPY( MapThemeDownloadDialog )
    class Private;
    Private *const d;
};

}

#endif
