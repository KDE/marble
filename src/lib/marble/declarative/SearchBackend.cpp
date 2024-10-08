// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Gábor Péterffy <peterffy95@gmail.com>
//

#include "SearchBackend.h"

#include "MarbleModel.h"
#include "SearchRunnerManager.h"

#include <QCompleter>

using namespace Marble;

SearchBackend::SearchBackend(QObject *parent)
    : QObject(parent)
    , m_completionModel(new MarblePlacemarkModel)
    , m_completionContainer(new QList<GeoDataPlacemark *>())
    , m_selectedPlacemark()
{
    m_model.setSortRole(MarblePlacemarkModel::PopularityIndexRole);
    m_model.sort(0);
    m_model.setDynamicSortFilter(true);
    m_completionModel->setPlacemarkContainer(m_completionContainer);
}

void SearchBackend::search(const QString &place)
{
    if (m_marbleQuickItem) {
        m_searchManager->searchPlacemarks(place);
    }
}

void SearchBackend::setCompletionPrefix(const QString &prefix)
{
    if (m_completer != nullptr && m_completer->completionPrefix() != prefix) {
        if (!m_lastSuccessfulCompletion.isEmpty() && prefix.startsWith(m_lastSuccessfulCompletion) && prefix.size() > m_lastSuccessfulCompletion.size()) {
            return;
        }

        m_completionModel->removePlacemarks(QStringLiteral("Completion model"), 0, m_completionModel->rowCount());
        m_completionContainer->clear();
        QString const lastPrefix = m_completer->completionPrefix();
        m_completer->setCompletionPrefix(prefix);
        if (prefix.isEmpty()) {
            Q_EMIT completionModelChanged(m_completionModel);
            return;
        }
        auto container = new QList<GeoDataPlacemark *>();
        auto model = qobject_cast<QAbstractProxyModel *>(m_completer->completionModel());
        for (int i = 0; i < m_completer->completionModel()->rowCount(); ++i) {
            QModelIndex index = model->mapToSource(model->index(i, 0));
            QVariant data = m_marbleQuickItem->model()->placemarkModel()->data(index, MarblePlacemarkModel::ObjectPointerRole);
            GeoDataPlacemark *placemark = placemarkFromQVariant(data);
            if (placemark != nullptr) {
                container->append(placemark);
            }
        }
        if (container->isEmpty() && prefix.startsWith(lastPrefix)) {
            m_lastSuccessfulCompletion = lastPrefix;
        } else if (!container->isEmpty()) {
            m_lastSuccessfulCompletion.clear();
        }
        m_completionModel->setPlacemarkContainer(container);
        m_completionModel->addPlacemarks(0, container->size());
        delete m_completionContainer;
        m_completionContainer = container;
        Q_EMIT completionModelChanged(m_completionModel);
    }
}

QObject *SearchBackend::marbleQuickItem()
{
    return m_marbleQuickItem;
}

MarblePlacemarkModel *SearchBackend::completionModel()
{
    return m_completionModel;
}

const QObject *SearchBackend::marbleQuickItem() const
{
    return m_marbleQuickItem;
}

Placemark *SearchBackend::selectedPlacemark()
{
    return &m_selectedPlacemark;
}

void SearchBackend::setSelectedPlacemark(int placemarkIndex)
{
    QVariant data = m_placemarkModel->data(m_placemarkModel->index(placemarkIndex), MarblePlacemarkModel::ObjectPointerRole);
    GeoDataPlacemark *placemark = placemarkFromQVariant(data);
    if (placemark == nullptr) {
        return;
    }

    m_selectedPlacemark.setGeoDataPlacemark(*placemark);
    m_marbleQuickItem->centerOn(*placemark, true);
    Q_EMIT selectedPlacemarkChanged(&m_selectedPlacemark);
}

void SearchBackend::setMarbleQuickItem(QObject *marbleQuickItem)
{
    auto item = qobject_cast<MarbleQuickItem *>(marbleQuickItem);
    if (m_marbleQuickItem == item) {
        return;
    }

    if (item) {
        delete m_searchManager;
        delete m_completer;
        m_marbleQuickItem = item;
        m_searchManager = new SearchRunnerManager(m_marbleQuickItem->model(), this);

        connect(m_searchManager, SIGNAL(searchResultChanged(QAbstractItemModel *)), this, SLOT(updateSearchResult(QAbstractItemModel *)));
        connect(m_searchManager, SIGNAL(searchFinished(QString)), this, SIGNAL(searchFinished(QString)));

        m_completer = new QCompleter();
        m_completer->setModel(m_marbleQuickItem->model()->placemarkModel());
        m_completer->setCompletionRole(Qt::DisplayRole);
        m_completer->setCaseSensitivity(Qt::CaseInsensitive);

        Q_EMIT marbleQuickItemChanged(marbleQuickItem);
    }
}

void SearchBackend::updateSearchResult(QAbstractItemModel *result)
{
    m_placemarkModel = qobject_cast<MarblePlacemarkModel *>(result);
    Q_EMIT searchResultChanged(m_placemarkModel);
}

GeoDataPlacemark *SearchBackend::placemarkFromQVariant(const QVariant &data)
{
    if (!data.isValid()) {
        return nullptr;
    }
    auto object = qvariant_cast<GeoDataObject *>(data);
    if (object == nullptr) {
        return nullptr;
    }
    auto placemark = dynamic_cast<GeoDataPlacemark *>(object);
    if (placemark == nullptr) {
        return nullptr;
    }
    return placemark;
}

#include "moc_SearchBackend.cpp"
