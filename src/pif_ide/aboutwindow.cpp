#include "aboutwindow.h"
#include "ui_aboutwindow.h"

AboutWindow::AboutWindow(QWidget *parent) :
    NMainWindow(parent),
    ui(new Ui::AboutWindow)
{
    ui->setupUi(this);

    // Sets the custom Widgets on the parent Class
    // Otherwise, the window resizing feature will not work
    NMainWindow::setCustomWidgets(ui->centralWidget, ui->statusBar);
    NMainWindow::setMaximizeButtonEnabled(false);

    loadCompilerData();

    connect(ui->btOHelp, SIGNAL(clicked(bool)), this, SLOT(openOnlineHelp()));
    connect(ui->btOSamples, SIGNAL(clicked(bool)), this, SLOT(openOnlineSamples()));
    connect(ui->btSamples, SIGNAL(clicked(bool)), this, SLOT(openSourceSamples()));
    connect(ui->btNSOSL, SIGNAL(clicked(bool)), this, SLOT(openNSOSL()));
}

AboutWindow::~AboutWindow()
{
    delete ui;
}

void AboutWindow::loadCompilerData(){
    QSettings settings("Nintersoft", "PIF IDE");
    settings.beginGroup("environment variables");
    QString cPath = settings.value("c_path", "").toString(),
            cppPath = settings.value("cpp_path", "").toString(),
            pifcPath = settings.value("pifc_path", "").toString(),
            javaPath = settings.value("java_path", "").toString(),
            javacPath = settings.value("javac_path", "").toString();
    settings.endGroup();

    if (!cPath.isEmpty())
        ui->lblInfoC->setText(tr("C compiler located at \"%1\"").arg(cPath));

    if (!cppPath.isEmpty())
        ui->lblInfoCpp->setText(tr("C++ compiler located at \"%1\"").arg(cppPath));

    if (!pifcPath.isEmpty())
        ui->lblInfoPifc->setText(tr("PIF compiler located at \"%1\"").arg(pifcPath));

    if (!javaPath.isEmpty() && !javacPath.isEmpty())
        ui->lblInfoJava->setText(tr("Java compiler located at \"%1\"\n"
                                    "JVM located at \"%2\"").arg(javacPath, javaPath));
    else if (!javaPath.isEmpty())
        ui->lblInfoJava->setText(tr("JVM located at \"%2\"").arg(javaPath));
    else if (!javacPath.isEmpty())
        ui->lblInfoJava->setText(tr("Java compiler located at \"%1\"").arg(javacPath));
}

void AboutWindow::openOnlineHelp(){
    QDesktopServices::openUrl(QUrl("https://github.com/mauromascarenhas/PIF/tree/master/docs"));
}

void AboutWindow::openOnlineSamples(){
    QDesktopServices::openUrl(QUrl("https://github.com/mauromascarenhas/PIF/tree/master/exemplos"));
}

void AboutWindow::openSourceSamples(){
    QDesktopServices::openUrl(QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + "/exemplos/"));
}

void AboutWindow::openNSOSL(){
    QDesktopServices::openUrl(QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + "/Licença de Código Aberto Nintersoft rev1.pdf"));
}
