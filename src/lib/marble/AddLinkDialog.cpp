// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Mikhail Ivchenko <ematirov@gmail.com>
//

// self
#include "AddLinkDialog.h"
#include "ui_AddLinkDialog.h"

// Qt
#include <QDebug>
#include <QMessageBox>
#include <QPushButton>

namespace Marble
{

class Q_DECL_HIDDEN AddLinkDialog::Private : public Ui::UiAddLinkDialog
{
public:
    Private();
    ~Private();
};

AddLinkDialog::Private::Private()
    : Ui::UiAddLinkDialog()
{
    // nothing to do
}

AddLinkDialog::Private::~Private() = default;

AddLinkDialog::AddLinkDialog(QWidget *parent)
    : QDialog(parent)
    , d(new Private())
{
    d->setupUi(this);

    d->buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);
    connect(d->buttonBox->button(QDialogButtonBox::Ok), &QAbstractButton::pressed, this, &AddLinkDialog::checkFields);
    connect(this, &QDialog::finished, this, &QObject::deleteLater);
}

AddLinkDialog::~AddLinkDialog()
{
    delete d;
}

QString AddLinkDialog::name() const
{
    return d->m_name->text();
}

QString AddLinkDialog::url() const
{
    return d->m_url->text();
}

void AddLinkDialog::checkFields()
{
    if (d->m_url->text().isEmpty()) {
        QMessageBox::warning(this, tr("No URL specified"), tr("Please specify a URL for this link."));
    } else if (d->m_name->text().isEmpty()) {
        QMessageBox::warning(this, tr("No name specified"), tr("Please specify a name for this link."));
    } else {
        accept();
    }
}

}

#include "moc_AddLinkDialog.cpp"
