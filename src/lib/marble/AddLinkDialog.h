// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Mikhail Ivchenko <ematirov@gmail.com>
//

#ifndef ADDLINKDIALOG_H
#define ADDLINKDIALOG_H

#include "marble_export.h"

#include <QDialog>


namespace Marble {

class MARBLE_EXPORT AddLinkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddLinkDialog(QWidget *parent = nullptr );
    ~AddLinkDialog() override;
    QString name() const;
    QString url() const;

private Q_SLOTS:
    void checkFields();

private:
    class Private;
    Private * const d;
};

}

#endif
