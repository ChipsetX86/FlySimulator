#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>

#include "AppEngine.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    QMap<QPoint, quint64> startPos;
    startPos[QPoint(0, 1)] = 2;
    startPos[QPoint(0, 2)] = 2;
    startPos[QPoint(0, 3)] = 1;
    AppSettings settings{QSize(5, 5), 5, 5, startPos};
    AppEngine flySimulator(settings, &app);
    flySimulator.startSimulation();
    engine.rootContext()->setContextProperty(QStringLiteral("flySimulator"), &flySimulator);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
