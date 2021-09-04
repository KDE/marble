// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2013 Dennis Nienhüser <nienhueser@kde.org>
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
    ~MapThemeDownloadDialog() override;

private:
    Q_DISABLE_COPY( MapThemeDownloadDialog )
    class Private;
    Private *const d;
};

}

#endif
