// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Dennis Nienhüser <nienhueser@kde.org>
//

#include <GeoDataTreeModel.h>
#include <MarbleModel.h>
#include <MarbleWidget.h>
#include <RenderPlugin.h>

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QPainter>
#include <QPixmap>
#include <QProcess>
#include <QThread>

using namespace Marble;

class Cheeeeze : private QThread
{
public:
    using QThread::msleep;
};

QPixmap resize(const QPixmap &pixmap)
{
    if (QProcess::execute("convert", QStringList() << "-version") == 0) {
        QString const inputFile = QDir::tempPath() + QLatin1StringView("/marble-preview.png");
        QString const outputFile = QDir::tempPath() + QLatin1StringView("/marble-preview-scaled.png");
        if (pixmap.save(inputFile)
            && QProcess::execute("convert",
                                 QStringList() << inputFile << "-resize" << "130x130"
                                               << "-sharpen" << "1x1" << outputFile)
                == 0) {
            QPixmap result(outputFile);
            if (!result.isNull()) {
                return result;
            }
        }
    }

    qDebug() << "Warning: Unable to resize pixmap properly. Check imagemagick installation.";
    return pixmap.scaled(130, 130, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    if (argc < 2) {
        qDebug() << "Usage: " << argv[0] << " <maptheme> [output.png]";
        qDebug() << "Where <maptheme> is a map theme id a la 'body/maptheme/maptheme.dgml'.";
        return 1;
    }

    MarbleWidget *mapWidget = new MarbleWidget;
    mapWidget->setMapThemeId(argv[1]);
    QStringList const features = QStringList() << "atmosphere";
    for (RenderPlugin *plugin : mapWidget->renderPlugins()) {
        if (!features.contains(plugin->nameId())) {
            plugin->setEnabled(false);
        }
    }
    double const scale = 4.0;
    mapWidget->resize(130 * scale, 130 * scale);
    mapWidget->centerOn(49.59526, 18.48104);
    mapWidget->setRadius(120 * scale / 2.0);

    QPixmap canvas(":/canvas.png");
    Q_ASSERT(!canvas.isNull());
    QPainter globePainter(&canvas);

    // Hack: Wait up to ten seconds for six documents to be parsed.
    // We need proper load progress signalization in Marble
    for (int i = 0; i < 400 && mapWidget->model()->treeModel()->rowCount() < 6; ++i) {
        Cheeeeze::msleep(25);
        app.processEvents();
    }
    QPixmap const globe = mapWidget->grab();
    globePainter.drawPixmap(QPoint(2, 2), resize(globe));
    globePainter.end();
    canvas.save(argc > 2 ? argv[2] : "preview.png");

    return 0;
}
