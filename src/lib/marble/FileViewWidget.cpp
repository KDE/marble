// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "FileViewWidget.h"

// Qt
#include <QAction>
#include <QFileDialog>
#include <QMenu>
#include <QPointer>

// Marble
#include "EditPlacemarkDialog.h"
#include "FileManager.h"
#include "GeoDataContainer.h"
#include "GeoDataDocument.h"
#include "GeoDataDocumentWriter.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTreeModel.h"
#include "KmlElementDictionary.h"
#include "MarbleModel.h"
#include "MarblePlacemarkModel.h"
#include "MarbleWidget.h"
#include "TreeViewDecoratorModel.h"

using namespace Marble;
// Ui
#include "ui_FileViewWidget.h"

namespace Marble
{

class FileViewWidgetPrivate
{
public:
    explicit FileViewWidgetPrivate(FileViewWidget *parent);
    void setTreeModel(GeoDataTreeModel *model);
    void setFileManager(FileManager *manager);

public Q_SLOTS:
    void saveFile();
    void closeFile();
    void enableFileViewActions();
    void contextMenu(const QPoint &pt);
    void showPlacemarkDialog();

public:
    FileViewWidget *q;
    Ui::FileViewWidget m_fileViewUi;
    MarbleWidget *m_widget;
    TreeViewDecoratorModel m_treeSortProxy;
    FileManager *m_fileManager;

    QMenu *m_contextMenu;
    QAction *m_viewPropertiesAction;
};

FileViewWidgetPrivate::FileViewWidgetPrivate(FileViewWidget *parent)
    : q(parent)
    , m_widget(nullptr)
    , m_fileManager(nullptr)
{
    m_contextMenu = new QMenu(q);
    m_viewPropertiesAction = new QAction(q);
    m_viewPropertiesAction->setText(QObject::tr("View Properties"));
    m_contextMenu->addAction(m_viewPropertiesAction);
    QObject::connect(m_viewPropertiesAction, SIGNAL(triggered()), q, SLOT(showPlacemarkDialog()));
}

FileViewWidget::FileViewWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , d(new FileViewWidgetPrivate(this))
{
    d->m_fileViewUi.setupUi(this);
    layout()->setContentsMargins({});
}

FileViewWidget::~FileViewWidget()
{
    delete d;
}

void FileViewWidget::setMarbleWidget(MarbleWidget *widget)
{
    d->m_widget = widget;
    d->setTreeModel(d->m_widget->model()->treeModel());
    d->setFileManager(d->m_widget->model()->fileManager());

    connect(this, SIGNAL(centerOn(GeoDataPlacemark, bool)), d->m_widget, SLOT(centerOn(GeoDataPlacemark, bool)));
    connect(this, SIGNAL(centerOn(GeoDataLatLonBox, bool)), d->m_widget, SLOT(centerOn(GeoDataLatLonBox, bool)));
}

void FileViewWidgetPrivate::setTreeModel(GeoDataTreeModel *model)
{
    m_treeSortProxy.setSourceModel(model);
    m_treeSortProxy.setDynamicSortFilter(true);
    m_fileViewUi.m_treeView->setModel(&m_treeSortProxy);
    m_fileViewUi.m_treeView->setSortingEnabled(true);
    m_fileViewUi.m_treeView->sortByColumn(0, Qt::AscendingOrder);
    m_fileViewUi.m_treeView->resizeColumnToContents(3);
    m_fileViewUi.m_treeView->setColumnWidth(0, 200);
    m_fileViewUi.m_treeView->setColumnWidth(1, 80);
    m_fileViewUi.m_treeView->setColumnWidth(2, 50);
    m_fileViewUi.m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);

    QObject::connect(m_fileViewUi.m_treeView, SIGNAL(expanded(QModelIndex)), &m_treeSortProxy, SLOT(trackExpandedState(QModelIndex)));
    QObject::connect(m_fileViewUi.m_treeView, SIGNAL(collapsed(QModelIndex)), &m_treeSortProxy, SLOT(trackCollapsedState(QModelIndex)));
    QObject::connect(m_fileViewUi.m_treeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), q, SLOT(enableFileViewActions()));
    QObject::connect(m_fileViewUi.m_treeView, SIGNAL(activated(QModelIndex)), q, SLOT(mapCenterOnTreeViewModel(QModelIndex)));
    QObject::connect(m_fileViewUi.m_treeView, SIGNAL(customContextMenuRequested(QPoint)), q, SLOT(contextMenu(QPoint)));
}

void FileViewWidgetPrivate::setFileManager(FileManager *manager)
{
    m_fileManager = manager;
    QObject::connect(m_fileViewUi.m_saveButton, SIGNAL(clicked()), q, SLOT(saveFile()));
    QObject::connect(m_fileViewUi.m_closeButton, SIGNAL(clicked()), q, SLOT(closeFile()));
}

void FileViewWidgetPrivate::saveFile()
{
    QModelIndex index = m_fileViewUi.m_treeView->selectionModel()->selectedRows().first();
    const GeoDataObject *object = index.model()->data(index, MarblePlacemarkModel::ObjectPointerRole).value<GeoDataObject *>();
    const auto document = geodata_cast<GeoDataDocument>(object);
    if (document && !document->fileName().isEmpty()) {
        const QString saveFileName = QFileDialog::getSaveFileName(q, QObject::tr("Select filename for KML document"));
        GeoDataDocumentWriter::write(saveFileName, *document, QString::fromLatin1(kml::kmlTag_nameSpaceOgc22));
    }
}

void FileViewWidgetPrivate::closeFile()
{
    QModelIndex index = m_fileViewUi.m_treeView->selectionModel()->selectedRows().first();
    auto object = index.model()->data(index, MarblePlacemarkModel::ObjectPointerRole).value<GeoDataObject *>();
    auto document = geodata_cast<GeoDataDocument>(object);
    if (document) {
        m_fileManager->closeFile(document);
    }
}

void FileViewWidgetPrivate::enableFileViewActions()
{
    bool isUserDocument = false;
    if (!m_fileViewUi.m_treeView->selectionModel()->selectedRows().isEmpty()) {
        QModelIndex index = m_fileViewUi.m_treeView->selectionModel()->selectedRows().first();
        const GeoDataObject *object = index.model()->data(index, MarblePlacemarkModel::ObjectPointerRole).value<GeoDataObject *>();
        const auto document = geodata_cast<GeoDataDocument>(object);
        if (document) {
            isUserDocument = document->documentRole() == Marble::UserDocument;
        }
    }
    m_fileViewUi.m_saveButton->setEnabled(isUserDocument);
    m_fileViewUi.m_closeButton->setEnabled(isUserDocument);
}

void FileViewWidgetPrivate::contextMenu(const QPoint &pt)
{
    const QModelIndex index = m_fileViewUi.m_treeView->indexAt(pt);
    const QAbstractItemModel *model = m_fileViewUi.m_treeView->model();
    if (index.isValid()) {
        const GeoDataObject *obj = model->data(index, MarblePlacemarkModel::ObjectPointerRole).value<GeoDataObject *>();
        const auto placemark = geodata_cast<GeoDataPlacemark>(obj);

        if (placemark) {
            m_contextMenu->popup(m_fileViewUi.m_treeView->mapToGlobal(pt));
        }
    }
}

void FileViewWidgetPrivate::showPlacemarkDialog()
{
    const QModelIndex index = m_fileViewUi.m_treeView->currentIndex();
    const QAbstractItemModel *model = m_fileViewUi.m_treeView->model();

    auto obj = model->data(index, MarblePlacemarkModel::ObjectPointerRole).value<GeoDataObject *>();
    auto placemark = geodata_cast<GeoDataPlacemark>(obj);
    if (placemark) {
        QPointer<EditPlacemarkDialog> dialog = new EditPlacemarkDialog(placemark, nullptr, q);
        dialog->setReadOnly(true);
        dialog->exec();
        delete dialog;
    }
}

void FileViewWidget::mapCenterOnTreeViewModel(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    const GeoDataObject *object = index.model()->data(index, MarblePlacemarkModel::ObjectPointerRole).value<GeoDataObject *>();
    if (const auto placemark = geodata_cast<GeoDataPlacemark>(object)) {
        d->m_widget->model()->placemarkSelectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
        Q_EMIT centerOn(*placemark, true);
    } else if (const auto container = dynamic_cast<const GeoDataContainer *>(object)) {
        const GeoDataLatLonAltBox box = container->latLonAltBox();
        Q_EMIT centerOn(box, true);
    }
}

}

#include "moc_FileViewWidget.cpp"
