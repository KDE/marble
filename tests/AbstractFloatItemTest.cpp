// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "AbstractFloatItem.h"
#include "MarbleModel.h"
#include "PluginManager.h"

#include <QMetaType>
#include <QTest>

Q_DECLARE_METATYPE(const Marble::AbstractFloatItem *)

using namespace Marble;

class NullFloatItem : public AbstractFloatItem
{
public:
    NullFloatItem()
        : AbstractFloatItem(nullptr)
    {
    }

    NullFloatItem(const MarbleModel *model)
        : AbstractFloatItem(model)
    {
    }

    QString name() const override
    {
        return QStringLiteral("Null Float Item");
    }
    QString nameId() const override
    {
        return QStringLiteral("null");
    }
    QString version() const override
    {
        return QStringLiteral("0.0");
    }
    QString description() const override
    {
        return QStringLiteral("A null float item just for testing.");
    }
    QIcon icon() const override
    {
        return QIcon();
    }
    QString copyrightYears() const override
    {
        return QStringLiteral("2013");
    }
    QList<PluginAuthor> pluginAuthors() const override
    {
        return QList<PluginAuthor>() << PluginAuthor(QStringLiteral("Bernhard Beschow"), QStringLiteral("bbeschow@cs.tu-berlin.de"));
    }
    void initialize() override
    {
    }
    bool isInitialized() const override
    {
        return true;
    }
    QStringList backendTypes() const override
    {
        return QStringList(QStringLiteral("null"));
    }
    QString guiString() const override
    {
        return QStringLiteral("Null");
    }
    RenderPlugin *newInstance(const MarbleModel *) const override
    {
        return nullptr;
    }
};

class AbstractFloatItemTest : public QObject
{
    Q_OBJECT

public:
    AbstractFloatItemTest();

private Q_SLOTS:
    void defaultConstructor();

    void newInstance_data();
    void newInstance();

    void setSettings_data();
    void setSettings();

    void setPosition_data();
    void setPosition();

private:
    MarbleModel m_model;
    QList<const AbstractFloatItem *> m_factories;
};

AbstractFloatItemTest::AbstractFloatItemTest()
{
    const auto plugins = m_model.pluginManager()->renderPlugins();
    for (const RenderPlugin *plugin : plugins) {
        const AbstractFloatItem *const factory = qobject_cast<const AbstractFloatItem *>(plugin);
        if (!factory)
            continue;

        m_factories << factory;
    }
}

void AbstractFloatItemTest::defaultConstructor()
{
    const NullFloatItem item;

    QCOMPARE(item.renderPolicy(), QString("ALWAYS"));
    QCOMPARE(item.renderPosition(), QStringList("FLOAT_ITEM"));
    QCOMPARE(item.cacheMode(), AbstractFloatItem::ItemCoordinateCache);
    QCOMPARE(item.frame(), AbstractFloatItem::RectFrame);
    QCOMPARE(item.padding(), 4.);
    QCOMPARE(item.contentSize(), QSizeF(150, 50));
    QCOMPARE(item.position(), QPointF(10, 10));
    QCOMPARE(item.visible(), true);
    QCOMPARE(item.positionLocked(), true);
    QVERIFY(item.settings().contains("position"));
}

void AbstractFloatItemTest::newInstance_data()
{
    QTest::addColumn<const AbstractFloatItem *>("factory");

    for (const AbstractFloatItem *factory : std::as_const(m_factories)) {
        QTest::newRow(factory->nameId().toLatin1().constData()) << factory;
    }
}

void AbstractFloatItemTest::newInstance()
{
    QFETCH(const AbstractFloatItem *, factory);

    RenderPlugin *const instance = factory->newInstance(&m_model);

    QVERIFY(qobject_cast<AbstractFloatItem *>(instance) != nullptr);
    QVERIFY(instance->settings().contains("position"));

    delete instance;
}

void AbstractFloatItemTest::setSettings_data()
{
    QTest::addColumn<const AbstractFloatItem *>("factory");

    for (const AbstractFloatItem *factory : std::as_const(m_factories)) {
        QTest::newRow(factory->nameId().toLatin1().constData()) << factory;
    }
}

void AbstractFloatItemTest::setSettings()
{
    QFETCH(const AbstractFloatItem *, factory);

    AbstractFloatItem *const instance = qobject_cast<AbstractFloatItem *>(factory->newInstance(&m_model));

    QVERIFY(instance != nullptr);

    const QPointF position(1.318, 4.005);

    instance->restoreDefaultSettings();

    QVERIFY(instance->settings().value("position", position).toPointF() != position);
    QVERIFY(instance->position() != position);

    QHash<QString, QVariant> settings;
    settings.insert("position", position);

    instance->setSettings(settings);

    QCOMPARE(instance->settings().value("position", QPointF(0, 0)).toPointF(), position);
    QCOMPARE(instance->position(), position);

    delete instance;
}

void AbstractFloatItemTest::setPosition_data()
{
    QTest::addColumn<const AbstractFloatItem *>("factory");

    for (const AbstractFloatItem *factory : std::as_const(m_factories)) {
        QTest::newRow(factory->nameId().toLatin1().constData()) << factory;
    }
}

void AbstractFloatItemTest::setPosition()
{
    QFETCH(const AbstractFloatItem *, factory);

    AbstractFloatItem *const instance = qobject_cast<AbstractFloatItem *>(factory->newInstance(&m_model));

    QVERIFY(instance != nullptr);

    const QPointF position(1.318, 4.005);

    QVERIFY(instance->position() != position);
    QVERIFY(instance->settings().value("position", position).toPointF() != position);

    QHash<QString, QVariant> settings;
    settings.insert("position", QPointF(0, 0));

    instance->setSettings(settings);
    instance->setPosition(position); // position of settings should be overwritten

    QCOMPARE(instance->position(), position);
    QCOMPARE(instance->settings().value("position", QPointF(1, 0)).toPointF(), position);

    delete instance;
}

QTEST_MAIN(AbstractFloatItemTest)

#include "AbstractFloatItemTest.moc"
