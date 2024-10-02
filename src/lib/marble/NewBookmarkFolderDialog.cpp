// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
// SPDX-FileCopyrightText: 2012 Thibaut Gridel <tgridel@free.fr>
//
//

#include "NewBookmarkFolderDialog.h"
#include "MarbleDebug.h"

namespace Marble
{

NewBookmarkFolderDialog::NewBookmarkFolderDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
}

NewBookmarkFolderDialog::~NewBookmarkFolderDialog() = default;

QString NewBookmarkFolderDialog::folderName() const
{
    return m_name->text();
}

void NewBookmarkFolderDialog::setFolderName(const QString &name)
{
    m_name->setText(name);
    m_name->selectAll();
}

}

#include "moc_NewBookmarkFolderDialog.cpp"
