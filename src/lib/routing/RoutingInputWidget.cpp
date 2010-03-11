/*
    Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "RoutingInputWidget.h"

#include <QtCore/QTimer>
#include <QtGui/QLineEdit>
#include <QtGui/QHBoxLayout>
#include <QtGui/QPushButton>
#include <QtGui/QMovie>
#include <QtGui/QIcon>

#include "MarbleRunnerManager.h"
#include "MarblePlacemarkModel.h"
#include "MarbleDebug.h"

namespace Marble {

class RoutingInputWidgetPrivate
{
public:
    QPushButton *m_stateButton;

    QLineEdit *m_lineEdit;

    QPushButton *m_removeButton;

    QPushButton *m_pickButton;

    MarbleRunnerManager *m_runnerManager;

    RoutingInputWidgetPrivate(QWidget *parent);

    MarblePlacemarkModel* m_placemarkModel;

    bool m_hasTarget;

    GeoDataCoordinates m_target;

    QMovie m_progress;

    QTimer m_progressTimer;

    QIcon m_stateIcon;
};

RoutingInputWidgetPrivate::RoutingInputWidgetPrivate(QWidget *parent) :
        m_lineEdit(0), m_runnerManager(new MarbleRunnerManager(parent)),
        m_placemarkModel(0), m_hasTarget(false), m_progress(":/data/bitmaps/progress.mng"),
        m_stateIcon(":/data/bitmaps/routing_via.png")
{
    m_stateButton = new QPushButton(parent);
    m_stateButton->setToolTip("Activate this destination");
    m_stateButton->setIcon(m_stateIcon);
    m_stateButton->setEnabled(false);
    m_stateButton->setFlat(true);
    m_stateButton->setMaximumWidth(22);
    m_stateButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_removeButton = new QPushButton(parent);
    /** @todo: Use an icon instead */
    m_removeButton->setText("X");
    m_removeButton->setToolTip("Remove this destination");
    m_removeButton->setFlat(true);
    m_removeButton->setMaximumWidth(12);

    m_pickButton = new QPushButton(parent);
    m_pickButton->setIcon(QIcon(":/data/bitmaps/routing_select.png"));
    m_pickButton->setToolTip("Choose destination from the map");
    m_pickButton->setCheckable(true);
    m_pickButton->setFlat(true);
    m_pickButton->setMaximumWidth(22);
    m_pickButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_lineEdit = new QLineEdit(parent);

    m_progressTimer.setInterval(100);
}

RoutingInputWidget::RoutingInputWidget(QWidget *parent) :
        QWidget(parent), d(new RoutingInputWidgetPrivate(this))
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);

    layout->addWidget(d->m_pickButton);
    layout->addWidget(d->m_stateButton);
    layout->addWidget(d->m_lineEdit);
    layout->addWidget(d->m_removeButton);

    connect(d->m_stateButton, SIGNAL(clicked()),
            this, SLOT(requestActivity()));
    connect(d->m_pickButton, SIGNAL(clicked()),
            this, SLOT(requestMapPosition()));
    connect(d->m_removeButton, SIGNAL(clicked()),
            this, SLOT(requestRemoval()));

    connect(d->m_runnerManager, SIGNAL(modelChanged(MarblePlacemarkModel*)),
            this, SLOT(setPlacemarkModel(MarblePlacemarkModel*)));
    connect(d->m_lineEdit, SIGNAL(returnPressed()),
            this, SLOT(findPlacemarks()));
    connect(d->m_lineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(setInvalid()));
    connect(&d->m_progressTimer, SIGNAL(timeout()),
            this, SLOT(updateProgress()));
    connect(d->m_runnerManager, SIGNAL(searchFinished(QString)),
            this, SLOT(finishSearch()));
}

RoutingInputWidget::~RoutingInputWidget()
{
    delete d;
}

void RoutingInputWidget::setPlacemarkModel(MarblePlacemarkModel* model)
{
    d->m_placemarkModel = model;
}

void RoutingInputWidget::setTargetPosition(const GeoDataCoordinates &position)
{
    if (!hasInput() || d->m_pickButton->isChecked()) {
        d->m_lineEdit->setText( position.toString() );
    }

    d->m_pickButton->setChecked(false);
    d->m_target = position;
    d->m_progressTimer.stop();
    d->m_stateButton->setIcon(d->m_stateIcon);
    d->m_stateButton->setEnabled(true);
    if (!d->m_hasTarget) {
        d->m_hasTarget = true;
        emit targetValidityChanged(d->m_hasTarget);
    }
}

bool RoutingInputWidget::hasTargetPosition() const
{
    return d->m_hasTarget;
}

GeoDataCoordinates RoutingInputWidget::targetPosition() const
{
    return d->m_target;
}

void RoutingInputWidget::findPlacemarks()
{
    QString text = d->m_lineEdit->text();
    if (text.isEmpty()) {
        setInvalid();
    }
    else {
        d->m_progressTimer.start();
        d->m_runnerManager->newText(text);
    }
}

MarblePlacemarkModel* RoutingInputWidget::searchResultModel()
{
    return d->m_placemarkModel;
}

void RoutingInputWidget::requestActivity()
{
    if (hasTargetPosition()) {
        emit activityRequest(this);
    }
}

void RoutingInputWidget::requestRemoval()
{
    emit removalRequest(this);
}

void RoutingInputWidget::setSimple(bool simple)
{
    d->m_removeButton->setVisible(!simple);
    d->m_stateButton->setVisible(!simple);
    d->m_pickButton->setVisible(!simple);
}

bool RoutingInputWidget::hasInput() const
{
    return !d->m_lineEdit->text().isEmpty();
}

void RoutingInputWidget::requestMapPosition()
{
   emit mapInputRequest(this);
}

void RoutingInputWidget::updateProgress()
{
    d->m_progress.jumpToNextFrame();
    QPixmap frame = d->m_progress.currentPixmap();
    d->m_stateButton->setIcon(frame);
}

void RoutingInputWidget::finishSearch()
{
    d->m_progressTimer.stop();
    d->m_stateButton->setIcon(d->m_stateIcon);
    d->m_stateButton->setEnabled(true);
    emit searchFinished(this);
}

void RoutingInputWidget::setInvalid()
{
    if (d->m_hasTarget) {
        d->m_hasTarget = false;
        emit targetValidityChanged(d->m_hasTarget);
    }
}

} // namespace Marble

#include "RoutingInputWidget.moc"
