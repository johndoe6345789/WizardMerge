#include <QCommandLineParser>
#include <QGuiApplication>
#include <QNetworkAccessManager>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <iostream>

/**
 * @brief Main entry point for WizardMerge Qt6 frontend
 *
 * This application provides a native desktop interface for WizardMerge,
 * supporting both standalone mode (with embedded backend) and client mode
 * (connecting to a remote backend server).
 */
int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  app.setApplicationName("WizardMerge");
  app.setApplicationVersion("1.0.0");
  app.setOrganizationName("WizardMerge");
  app.setOrganizationDomain("wizardmerge.dev");

  // Command line parser
  QCommandLineParser parser;
  parser.setApplicationDescription(
      "WizardMerge - Intelligent Merge Conflict Resolution");
  parser.addHelpOption();
  parser.addVersionOption();

  QCommandLineOption backendUrlOption(
      QStringList() << "b" << "backend-url",
      "Backend server URL (default: http://localhost:8080)", "url",
      "http://localhost:8080");
  parser.addOption(backendUrlOption);

  QCommandLineOption standaloneOption(
      QStringList() << "s" << "standalone",
      "Run in standalone mode with embedded backend");
  parser.addOption(standaloneOption);

  parser.addPositionalArgument("file", "File to open (optional)");

  parser.process(app);

  // Get command line arguments
  QString backendUrl = parser.value(backendUrlOption);
  bool standalone = parser.isSet(standaloneOption);
  QStringList positionalArgs = parser.positionalArguments();
  QString filePath =
      positionalArgs.isEmpty() ? QString() : positionalArgs.first();

  // Create QML engine
  QQmlApplicationEngine engine;

  // Expose application settings to QML
  QQmlContext *rootContext = engine.rootContext();
  rootContext->setContextProperty("backendUrl", backendUrl);
  rootContext->setContextProperty("standalone", standalone);
  rootContext->setContextProperty("initialFile", filePath);

  // Load main QML file
  const QUrl url(u"qrc:/qt/qml/WizardMerge/main.qml"_qs);

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() {
        std::cerr << "Error: Failed to load QML" << std::endl;
        QCoreApplication::exit(-1);
      },
      Qt::QueuedConnection);

  engine.load(url);

  if (engine.rootObjects().isEmpty()) {
    std::cerr << "Error: No root objects loaded from QML" << std::endl;
    return -1;
  }

  std::cout << "WizardMerge Qt6 Frontend Started" << std::endl;
  std::cout << "Backend URL: " << backendUrl.toStdString() << std::endl;
  std::cout << "Standalone Mode: " << (standalone ? "Yes" : "No") << std::endl;
  if (!filePath.isEmpty()) {
    std::cout << "Opening file: " << filePath.toStdString() << std::endl;
  }

  return app.exec();
}
