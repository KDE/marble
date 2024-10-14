// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Utku Aydın <utkuaydin34@gmail.com>
//

#include "CloudRoutesDialog.h"
#include "ui_CloudRoutesDialog.h"

#include "CloudRouteModel.h"
#include "RouteItemDelegate.h"

#include <QTimer>

namespace Marble
{

class Q_DECL_HIDDEN CloudRoutesDialog::Private : public Ui::CloudRoutesDialog
{
public:
    explicit Private(CloudRouteModel *model);
    CloudRouteModel *const m_model;
};

CloudRoutesDialog::Private::Private(CloudRouteModel *model)
    : Ui::CloudRoutesDialog()
    , m_model(model)
{
}

CloudRoutesDialog::CloudRoutesDialog(CloudRouteModel *model, QWidget *parent)
    : QDialog(parent)
    , d(new Private(model))
{
    d->setupUi(this);

    auto delegate = new RouteItemDelegate(d->listView, d->m_model);
    connect(delegate, &RouteItemDelegate::downloadButtonClicked, this, &CloudRoutesDialog::downloadButtonClicked);
    connect(delegate, &RouteItemDelegate::openButtonClicked, this, &CloudRoutesDialog::openButtonClicked);
    connect(delegate, &RouteItemDelegate::deleteButtonClicked, this, &CloudRoutesDialog::deleteButtonClicked);
    connect(delegate, &RouteItemDelegate::removeFromCacheButtonClicked, this, &CloudRoutesDialog::removeFromCacheButtonClicked);
    connect(delegate, &RouteItemDelegate::uploadToCloudButtonClicked, this, &CloudRoutesDialog::uploadToCloudButtonClicked);
    connect(d->m_model, &QAbstractItemModel::modelReset, this, &CloudRoutesDialog::updateNoRouteLabel);

    d->progressBar->setHidden(true);
    d->labelNoRoute->setHidden(true);

    d->listView->setItemDelegate(delegate);
    d->listView->setModel(d->m_model);
}

CloudRoutesDialog::~CloudRoutesDialog()
{
    delete d;
}

CloudRouteModel *CloudRoutesDialog::model()
{
    return d->m_model;
}

void CloudRoutesDialog::updateListDownloadProgressbar(qint64 received, qint64 total)
{
    d->progressBar->setHidden(false);
    d->progressBar->setValue(qRound(100.0 * qreal(received) / total));
    if (received == total) {
        QTimer::singleShot(1000, d->progressBar, &CloudRoutesDialog::hide);
    }
}

void CloudRoutesDialog::updateNoRouteLabel()
{
    bool const isEmpty = d->listView->model()->rowCount() == 0;
    d->listView->setHidden(isEmpty);
    d->labelNoRoute->setVisible(isEmpty);
}

}

#include "moc_CloudRoutesDialog.cpp"
