// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
//

// Own
#include "TileCreatorDialog.h"

// Qt
#include <QPushButton>
#include <QTimer>

// Marble
#include "MarbleDebug.h"
#include "TileCreator.h"

#include "ui_TileCreatorDialog.h"

namespace Marble
{

class TileCreatorDialogPrivate
{
public:
    Ui::TileCreatorDialog uiWidget;

    TileCreator *m_creator = nullptr;
};

TileCreatorDialog::TileCreatorDialog(TileCreator *creator, QWidget *parent)
    : QDialog(parent)
    , d(new TileCreatorDialogPrivate)
{
    d->m_creator = creator;

    d->uiWidget.setupUi(this);

    connect(d->m_creator, &TileCreator::progress, this, &TileCreatorDialog::setProgress, Qt::QueuedConnection);
    connect(d->uiWidget.buttonBox, &QDialogButtonBox::rejected, this, &TileCreatorDialog::cancelTileCreation);

    // Start the creation process
    d->m_creator->start();
}

void TileCreatorDialog::cancelTileCreation()
{
    d->uiWidget.buttonBox->button(QDialogButtonBox::Cancel)->setEnabled(false);

    /** @todo: Cancelling mostly crashes Marble. Fix that and uncomment below */
    // d->m_creator->cancelTileCreation();
}

TileCreatorDialog::~TileCreatorDialog()
{
    disconnect(d->m_creator, SIGNAL(progress(int)), this, SLOT(setProgress(int)));

    if (d->m_creator->isRunning())
        d->m_creator->cancelTileCreation();
    d->m_creator->wait();
    d->m_creator->deleteLater();
    delete d;
}

void TileCreatorDialog::setProgress(int progress)
{
    d->uiWidget.progressBar->setValue(progress);

    if (progress == 100) {
        QTimer::singleShot(0, this, SLOT(accept()));
    }
}

void TileCreatorDialog::setSummary(const QString &name, const QString &description)
{
    const QString summary = QLatin1StringView("<b>") + QCoreApplication::translate("DGML", name.toUtf8().constData()) + QLatin1StringView("</b><br>")
        + QCoreApplication::translate("DGML", description.toUtf8().constData());
    d->uiWidget.descriptionLabel->setText(summary);
}

}

#include "moc_TileCreatorDialog.cpp"
