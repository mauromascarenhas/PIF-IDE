#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
    NMainWindow(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    // Sets the custom Widgets on the parent Class
    // Otherwise, the window resizing feature will not work
    NMainWindow::setCustomWidgets(ui->centralWidget, ui->statusBar);
    NMainWindow::setMaximizeButtonEnabled(false);

    connect(ui->btCancel, SIGNAL(clicked(bool)), this, SLOT(close()));
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::showEvent(QShowEvent *event){
    loadSettings();

    NMainWindow::showEvent(event);
}

void SettingsWindow::loadSettings(){
    QSettings settings("Nintersoft", "PIF IDE");
    settings.beginGroup("environment variables");
    ui->edtC->setText(settings.value("c_path", "").toString());
    ui->edtCpp->setText(settings.value("cpp_path", "").toString());
    ui->edtJava->setText(settings.value("java_path", "").toString());
    ui->edtPifc->setText(settings.value("pifc_path", "").toString());
    ui->edtJavac->setText(settings.value("javac_path", "").toString());
    settings.endGroup();
}
