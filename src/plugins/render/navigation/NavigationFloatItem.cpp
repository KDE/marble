// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2010 Bastian Holst <bastianholst@gmx.de>
// SPDX-FileCopyrightText: 2013 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "NavigationFloatItem.h"

#include <QContextMenuEvent>
#include <QPainter>
#include <QPixmapCache>
#include <QRect>
#include <QSlider>
#include <QWidget>
#include <qmath.h>

#include "MarbleGraphicsGridLayout.h"
#include "MarbleModel.h"
#include "MarbleWidget.h"
#include "PositionTracking.h"
#include "ViewportParams.h"
#include "WidgetGraphicsItem.h"
#include "ui_navigation.h"

using namespace Marble;
/* TRANSLATOR Marble::NavigationFloatItem */

NavigationFloatItem::NavigationFloatItem(const MarbleModel *marbleModel)
    : AbstractFloatItem(marbleModel, QPointF(-10, -30))
    , m_marbleWidget(nullptr)
    , m_widgetItem(nullptr)
    , m_navigationWidget(nullptr)
    , m_oldViewportRadius(0)
    , m_contextMenu(nullptr)
    , m_showHomeButton(true)
{
    // Plugin is visible by default on desktop systems
    const bool smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    setEnabled(!smallScreen);
    setVisible(true);

    setCacheMode(NoCache);
    setBackground(QBrush(QColor(Qt::transparent)));
    setFrame(NoFrame);
}

NavigationFloatItem::~NavigationFloatItem()
{
    delete m_navigationWidget;
}

QStringList NavigationFloatItem::backendTypes() const
{
    return QStringList(QStringLiteral("navigation"));
}

QString NavigationFloatItem::name() const
{
    return tr("Navigation");
}

QString NavigationFloatItem::guiString() const
{
    return tr("&Navigation");
}

QString NavigationFloatItem::nameId() const
{
    return QStringLiteral("navigation");
}

QString NavigationFloatItem::version() const
{
    return QStringLiteral("1.0");
}

QString NavigationFloatItem::description() const
{
    return tr("A mouse control to zoom and move the map");
}

QString NavigationFloatItem::copyrightYears() const
{
    return QStringLiteral("2008, 2010, 2013");
}

QList<PluginAuthor> NavigationFloatItem::pluginAuthors() const
{
    return QList<PluginAuthor>() << PluginAuthor(QStringLiteral("Dennis Nienhüser"), QStringLiteral("nienhueser@kde.org"))
                                 << PluginAuthor(QStringLiteral("Bastian Holst"), QStringLiteral("bastianholst@gmx.de"))
                                 << PluginAuthor(QStringLiteral("Mohammed Nafees"), QStringLiteral("nafees.technocool@gmail.com"));
}

QIcon NavigationFloatItem::icon() const
{
    return QIcon(QStringLiteral(":/icons/navigation.png"));
}

void NavigationFloatItem::initialize()
{
    auto navigationParent = new QWidget(nullptr);
    navigationParent->setAttribute(Qt::WA_NoSystemBackground, true);

    m_navigationWidget = new Ui::Navigation;
    m_navigationWidget->setupUi(navigationParent);

    m_widgetItem = new WidgetGraphicsItem(this);
    m_widgetItem->setWidget(navigationParent);

    auto layout = new MarbleGraphicsGridLayout(1, 1);
    layout->addItem(m_widgetItem, 0, 0);

    setLayout(layout);

    if (m_showHomeButton) {
        activateHomeButton();
    } else {
        activateCurrentPositionButton();
    }
}

bool NavigationFloatItem::isInitialized() const
{
    return m_widgetItem;
}

void NavigationFloatItem::setProjection(const ViewportParams *viewport)
{
    if (viewport->radius() != m_oldViewportRadius) {
        m_oldViewportRadius = viewport->radius();
        // The slider depends on the map state (zoom factor)
        update();
    }

    AbstractFloatItem::setProjection(viewport);
}

bool NavigationFloatItem::eventFilter(QObject *object, QEvent *e)
{
    if (!enabled() || !visible()) {
        return false;
    }

    auto widget = dynamic_cast<MarbleWidget *>(object);
    if (!widget) {
        return AbstractFloatItem::eventFilter(object, e);
    }

    if (m_marbleWidget != widget) {
        // Delayed initialization
        m_marbleWidget = widget;

        m_maxZoom = m_marbleWidget->maximumZoom();
        m_minZoom = m_marbleWidget->minimumZoom();

        m_navigationWidget->arrowDisc->setMarbleWidget(m_marbleWidget);
        connect(m_navigationWidget->arrowDisc, SIGNAL(repaintNeeded()), SIGNAL(repaintNeeded()));

        connect(m_navigationWidget->homeButton, SIGNAL(repaintNeeded()), SIGNAL(repaintNeeded()));
        if (m_showHomeButton) {
            activateHomeButton();
        } else {
            activateCurrentPositionButton();
        }

        connect(m_navigationWidget->zoomInButton, SIGNAL(repaintNeeded()), SIGNAL(repaintNeeded()));
        connect(m_navigationWidget->zoomInButton, SIGNAL(clicked()), m_marbleWidget, SLOT(zoomIn()));

        m_navigationWidget->zoomSlider->setMaximum(m_maxZoom);
        m_navigationWidget->zoomSlider->setMinimum(m_minZoom);
        connect(m_navigationWidget->zoomSlider, SIGNAL(repaintNeeded()), SIGNAL(repaintNeeded()));
        connect(m_navigationWidget->zoomSlider, SIGNAL(valueChanged(int)), m_marbleWidget, SLOT(setZoom(int)));

        connect(m_navigationWidget->zoomOutButton, SIGNAL(repaintNeeded()), SIGNAL(repaintNeeded()));
        connect(m_navigationWidget->zoomOutButton, SIGNAL(clicked()), m_marbleWidget, SLOT(zoomOut()));

        connect(m_marbleWidget, &MarbleWidget::zoomChanged, this, &NavigationFloatItem::updateButtons);
        updateButtons(m_marbleWidget->zoom());
        connect(m_marbleWidget, &MarbleWidget::themeChanged, this, &NavigationFloatItem::selectTheme);
    }

    return AbstractFloatItem::eventFilter(object, e);
}

void NavigationFloatItem::selectTheme(const QString &)
{
    if (m_marbleWidget) {
        m_maxZoom = m_marbleWidget->maximumZoom();
        m_minZoom = m_marbleWidget->minimumZoom();
        m_navigationWidget->zoomSlider->setMaximum(m_maxZoom);
        m_navigationWidget->zoomSlider->setMinimum(m_minZoom);
        updateButtons(m_marbleWidget->zoom());
    }
}

void NavigationFloatItem::updateButtons(int zoomValue)
{
    bool const zoomInEnabled = m_navigationWidget->zoomInButton->isEnabled();
    bool const zoomOutEnabled = m_navigationWidget->zoomOutButton->isEnabled();
    int const oldZoomValue = m_navigationWidget->zoomSlider->value();
    m_navigationWidget->zoomInButton->setEnabled(zoomValue < m_maxZoom);
    m_navigationWidget->zoomOutButton->setEnabled(zoomValue > m_minZoom);
    m_navigationWidget->zoomSlider->setValue(zoomValue);
    if (zoomInEnabled != m_navigationWidget->zoomInButton->isEnabled() || zoomOutEnabled != m_navigationWidget->zoomOutButton->isEnabled()
        || oldZoomValue != zoomValue) {
        update();
    }
}

QPixmap NavigationFloatItem::pixmap(const QString &id)
{
    QPixmap result;
    if (!QPixmapCache::find(id, &result)) {
        result = QPixmap(QLatin1StringView(":/") + id + QLatin1StringView(".png"));
        QPixmapCache::insert(id, result);
    }
    return result;
}

void NavigationFloatItem::paintContent(QPainter *painter)
{
    painter->drawPixmap(0, 0, pixmap(QStringLiteral("marble/navigation/navigational_backdrop_top")));
    painter->drawPixmap(0, 70, pixmap(QStringLiteral("marble/navigation/navigational_backdrop_center")));
    painter->drawPixmap(0, 311, pixmap(QStringLiteral("marble/navigation/navigational_backdrop_bottom")));
}

void NavigationFloatItem::contextMenuEvent(QWidget *w, QContextMenuEvent *e)
{
    if (!m_contextMenu) {
        m_contextMenu = contextMenu();

        m_activateCurrentPositionButtonAction = new QAction(QIcon(), tr("Current Location Button"), m_contextMenu);
        m_activateHomeButtonAction = new QAction(QIcon(QStringLiteral(":/icons/go-home.png")), tr("Home Button"), m_contextMenu);
        m_activateHomeButtonAction->setVisible(!m_showHomeButton);
        m_activateCurrentPositionButtonAction->setVisible(m_showHomeButton);
        m_contextMenu->addSeparator();
        m_contextMenu->addAction(m_activateCurrentPositionButtonAction);
        m_contextMenu->addAction(m_activateHomeButtonAction);

        connect(m_activateCurrentPositionButtonAction, SIGNAL(triggered()), SLOT(activateCurrentPositionButton()));
        connect(m_activateHomeButtonAction, SIGNAL(triggered()), SLOT(activateHomeButton()));
    }

    Q_ASSERT(m_contextMenu);
    m_contextMenu->exec(w->mapToGlobal(e->pos()));
}

void NavigationFloatItem::activateCurrentPositionButton()
{
    if (!isInitialized()) {
        return;
    }

    QIcon icon;
    icon.addPixmap(pixmap(QStringLiteral("marble/navigation/navigational_currentlocation")), QIcon::Normal);
    icon.addPixmap(pixmap(QStringLiteral("marble/navigation/navigational_currentlocation_hover")), QIcon::Active);
    icon.addPixmap(pixmap(QStringLiteral("marble/navigation/navigational_currentlocation_pressed")), QIcon::Selected);
    m_navigationWidget->homeButton->setProperty("icon", QVariant(icon));

    if (m_contextMenu) {
        m_activateCurrentPositionButtonAction->setVisible(false);
        m_activateHomeButtonAction->setVisible(true);
    }

    if (m_marbleWidget) {
        disconnect(m_navigationWidget->homeButton, SIGNAL(clicked()), m_marbleWidget, SLOT(goHome()));
    }
    connect(m_navigationWidget->homeButton, SIGNAL(clicked()), SLOT(centerOnCurrentLocation()));
    Q_EMIT repaintNeeded();
    m_showHomeButton = false;
    Q_EMIT settingsChanged(nameId());
}

void NavigationFloatItem::activateHomeButton()
{
    if (!isInitialized()) {
        return;
    }

    QIcon icon;
    icon.addPixmap(pixmap(QStringLiteral("marble/navigation/navigational_homebutton")), QIcon::Normal);
    icon.addPixmap(pixmap(QStringLiteral("marble/navigation/navigational_homebutton_hover")), QIcon::Active);
    icon.addPixmap(pixmap(QStringLiteral("marble/navigation/navigational_homebutton_press")), QIcon::Selected);
    m_navigationWidget->homeButton->setProperty("icon", QVariant(icon));

    if (m_contextMenu) {
        m_activateCurrentPositionButtonAction->setVisible(true);
        m_activateHomeButtonAction->setVisible(false);
    }

    disconnect(m_navigationWidget->homeButton, SIGNAL(clicked()), this, SLOT(centerOnCurrentLocation()));
    if (m_marbleWidget) {
        connect(m_navigationWidget->homeButton, SIGNAL(clicked()), m_marbleWidget, SLOT(goHome()));
    }
    Q_EMIT repaintNeeded();
    m_showHomeButton = true;
    Q_EMIT settingsChanged(nameId());
}

void NavigationFloatItem::centerOnCurrentLocation()
{
    if (m_marbleWidget->model()->positionTracking()->currentLocation().isValid()) {
        m_marbleWidget->centerOn(m_marbleWidget->model()->positionTracking()->currentLocation(), true);
    }
}

QHash<QString, QVariant> NavigationFloatItem::settings() const
{
    QHash<QString, QVariant> settings = AbstractFloatItem::settings();
    settings.insert(QStringLiteral("showHomeButton"), m_showHomeButton);
    return settings;
}

void NavigationFloatItem::setSettings(const QHash<QString, QVariant> &settings)
{
    AbstractFloatItem::setSettings(settings);
    m_showHomeButton = settings.value(QStringLiteral("showHomeButton"), true).toBool();
    if (m_showHomeButton) {
        activateHomeButton();
    } else {
        activateCurrentPositionButton();
    }
}

#include "moc_NavigationFloatItem.cpp"
