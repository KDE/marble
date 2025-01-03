// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "SearchInputWidget.h"

#include "GeoDataCoordinates.h"
#include "MarblePlacemarkModel.h"

#include <QCompleter>
#include <QMenu>

namespace Marble
{

SearchInputWidget::SearchInputWidget(QWidget *parent)
    : MarbleLineEdit(parent)
    , m_completer(new QCompleter(this))
    , m_areaSearch(false)
{
    updatePlaceholderText();
    QPixmap const decorator = QPixmap(QStringLiteral(":/icons/16x16/edit-find.png"));
    Q_ASSERT(!decorator.isNull());
    setDecorator(decorator);

    connect(this, SIGNAL(clearButtonClicked()), this, SLOT(search()));
    connect(this, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(this, &MarbleLineEdit::decoratorButtonClicked, this, &SearchInputWidget::showDropDownMenu);

    m_sortFilter.setSortRole(MarblePlacemarkModel::PopularityIndexRole);
    m_sortFilter.sort(0, Qt::AscendingOrder);
    m_sortFilter.setDynamicSortFilter(true);

    m_completer->setCompletionRole(Qt::DisplayRole);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setModel(&m_sortFilter);
    setCompleter(m_completer);
    connect(m_completer, SIGNAL(activated(QModelIndex)), this, SLOT(centerOnSearchSuggestion(QModelIndex)));
}

void SearchInputWidget::setCompletionModel(QAbstractItemModel *completionModel)
{
    m_sortFilter.setSourceModel(completionModel);
}

void SearchInputWidget::search()
{
    QString const searchTerm = text();
    if (!searchTerm.isEmpty()) {
        setBusy(true);
    }
    Q_EMIT search(searchTerm, m_areaSearch ? AreaSearch : GlobalSearch);
}

void SearchInputWidget::disableSearchAnimation()
{
    setBusy(false);
}

void SearchInputWidget::centerOnSearchSuggestion(const QModelIndex &index)
{
    QAbstractItemModel const *model = completer()->completionModel();
    QVariant const value = model->data(index, MarblePlacemarkModel::CoordinateRole);
    auto const coordinates = value.value<GeoDataCoordinates>();
    Q_EMIT centerOn(coordinates);
}

void SearchInputWidget::showDropDownMenu()
{
    QMenu menu(this);
    QAction *globalSearch = menu.addAction(tr("Global Search"), this, SLOT(setGlobalSearch()));
    globalSearch->setCheckable(true);
    globalSearch->setChecked(!m_areaSearch);
    QAction *areaSearch = menu.addAction(tr("Area Search"), this, SLOT(setAreaSearch()));
    areaSearch->setCheckable(true);
    areaSearch->setChecked(m_areaSearch);
    menu.exec(mapToGlobal(QPoint(0, size().height())));
}

void SearchInputWidget::setGlobalSearch()
{
    m_areaSearch = false;
    updatePlaceholderText();
}

void SearchInputWidget::setAreaSearch()
{
    m_areaSearch = true;
    updatePlaceholderText();
}

void SearchInputWidget::updatePlaceholderText()
{
    setPlaceholderText(m_areaSearch ? tr("Area Search") : tr("Global Search"));
}

}

#include "moc_SearchInputWidget.cpp"
