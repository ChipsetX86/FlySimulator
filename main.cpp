#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>

#include "AppEngine.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    StartMap startPos;
    startPos[QPoint(0, 1)] = 1;
    AppSettings settings{QSize(5, 5), 5, 5, startPos};
    AppEngine flySimulator(settings, &app);
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
