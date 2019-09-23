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
    if (GlobalSettings::selectedTheme){
        NMainWindow::setTitlebarStylesheet(GlobalSettings::titlebarSSheet);
        ui->stackedSettings->setStyleSheet(GlobalSettings::settingsSSheet);
        ui->settingsPage1->setStyleSheet(GlobalSettings::settingsSSheet);
        ui->settingsPage2->setStyleSheet(GlobalSettings::settingsSSheet);
        ui->settingsPage3->setStyleSheet(GlobalSettings::settingsSSheet);
        ui->settingsPage4->setStyleSheet(GlobalSettings::settingsSSheet);
        ui->settingsPage5->setStyleSheet(GlobalSettings::settingsSSheet);
        ui->groupBox->setStyleSheet(GlobalSettings::settingsSSheet);
        ui->groupBox_2->setStyleSheet(GlobalSettings::settingsSSheet);
        ui->groupBox_3->setStyleSheet(GlobalSettings::settingsSSheet);
        ui->groupBox_4->setStyleSheet(GlobalSettings::settingsSSheet);
        ui->groupBox_5->setStyleSheet(GlobalSettings::settingsSSheet);
        ui->groupBox_6->setStyleSheet(GlobalSettings::settingsSSheet);
        this->setStyleSheet(GlobalSettings::settingsSSheet);
    }

    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                this->size(), QGuiApplication::primaryScreen()->availableGeometry()));

    connect(ui->btSave, SIGNAL(clicked(bool)), this, SLOT(saveSettings()));
    connect(ui->btCancel, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(ui->btReset, SIGNAL(clicked(bool)), this, SLOT(resetSettings()));
    connect(ui->cbUseCpp, SIGNAL(toggled(bool)), this, SLOT(useCppAsC(bool)));

    connect(ui->btSetC, SIGNAL(clicked(bool)), this, SLOT(getCPath()));
    connect(ui->btSetCpp, SIGNAL(clicked(bool)), this, SLOT(getCppPath()));
    connect(ui->btSetJava, SIGNAL(clicked(bool)), this, SLOT(getJavaPath()));
    connect(ui->btSetPifc, SIGNAL(clicked(bool)), this, SLOT(getPifcPath()));
    connect(ui->btSetJavac, SIGNAL(clicked(bool)), this, SLOT(getJavacPath()));

    connect(ui->btRestorePifcArgs, &QPushButton::clicked, [this]{
       ui->txtCmdPifc->setPlainText(tr("-f\n-l\n$lang\n$source\n-o\n$object"));
    });
    connect(ui->btRestoreCppArgs, &QPushButton::clicked, [this]{
       ui->txtCmdCpp->setPlainText(tr("$source\n-o\n$executable"));
    });
    connect(ui->btRestoreCArgs, &QPushButton::clicked, [this]{
       ui->txtCmdC->setPlainText(tr("$source\n-o\n$executable"));
    });
    connect(ui->btRestoreJavacArgs, &QPushButton::clicked, [this]{
       ui->txtCmdJavac->setPlainText(tr("$source"));
    });
    connect(ui->btRestoreJavaArgs, &QPushButton::clicked, [this]{
       ui->txtCmdJava->setPlainText(tr("-cp\n$wdir\n$executable"));
    });

    setupOptionPages();
    loadSettings();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::setupOptionPages(){
    ui->settingGroups->addItem(new QListWidgetItem(QIcon("://resources/images/items/code_pif.svg"), tr("PIF Compiler")));
    ui->settingGroups->addItem(new QListWidgetItem(QIcon("://resources/images/items/code_cpp.svg"), tr("C++ Compiler")));
    ui->settingGroups->addItem(new QListWidgetItem(QIcon("://resources/images/items/code.svg"), tr("C Compiler")));
    ui->settingGroups->addItem(new QListWidgetItem(QIcon("://resources/images/items/code_java.svg"), tr("Java JDK/JRE")));
    ui->settingGroups->addItem(new QListWidgetItem(QIcon("://resources/images/items/editor.svg"), tr("Editor")));
    ui->settingGroups->setCurrentRow(0);
    ui->settingGroups->setFocus();

    connect(ui->settingGroups, SIGNAL(currentRowChanged(int)), ui->stackedSettings, SLOT(setCurrentIndex(int)));
}

void SettingsWindow::loadSettings(){
    QSettings settings("Nintersoft", "PIF IDE");
    settings.beginGroup("environment variables");
    ui->edtC->setText(settings.value("c_path", "").toString());
    ui->edtCpp->setText(settings.value("cpp_path", "").toString());
    ui->edtJava->setText(settings.value("java_path", "").toString());
    ui->edtPifc->setText(settings.value("pifc_path", "").toString());
    ui->edtJavac->setText(settings.value("javac_path", "").toString());

    ui->txtCmdPifc->setPlainText(settings.value("pifc_args", tr("-f\n-l\n$lang\n$source\n-o\n$object")).toString());
    ui->txtCmdC->setPlainText(settings.value("c_args", tr("$source\n-o\n$executable")).toString());
    ui->txtCmdCpp->setPlainText(settings.value("cpp_args", tr("$source\n-o\n$executable")).toString());
    ui->txtCmdJavac->setPlainText(settings.value("javac_args", tr("$source")).toString());
    ui->txtCmdJava->setPlainText(settings.value("java_args", tr("-cp\n$wdir\n$executable")).toString());

    bool useCpp = settings.value("c_uses_cpp", false).toBool();
    ui->cbUseCpp->setChecked(useCpp);
    ui->edtC->setEnabled(!useCpp);
    ui->btSetC->setEnabled(!useCpp);
    ui->txtCmdC->setEnabled(!useCpp);
    ui->btRestoreCArgs->setEnabled(!useCpp);
    settings.endGroup();

    settings.beginGroup("editor");
    QFont font = ui->cbFont->font();
    font.setFamily(settings.value("font_family", "Segoe UI").toString());
    ui->cbFont->setFont(font);
    ui->sbFontSize->setValue(settings.value("font_size", 12).toInt());
    ui->cbTheme->setCurrentIndex(settings.value("colour_scheme", 0).toInt());
    settings.endGroup();
}

void SettingsWindow::saveSettings(){
    QSettings settings("Nintersoft", "PIF IDE");
    settings.beginGroup("environment variables");
    settings.setValue("c_path", ui->edtC->text());
    settings.setValue("cpp_path", ui->edtCpp->text());
    settings.setValue("java_path", ui->edtJava->text());
    settings.setValue("pifc_path", ui->edtPifc->text());
    settings.setValue("javac_path", ui->edtJavac->text());
    settings.setValue("c_uses_cpp", ui->cbUseCpp->isChecked());

    settings.setValue("c_args", ui->txtCmdC->toPlainText());
    settings.setValue("cpp_args", ui->txtCmdCpp->toPlainText());
    settings.setValue("java_args", ui->txtCmdJava->toPlainText());
    settings.setValue("javac_args", ui->txtCmdJavac->toPlainText());
    settings.setValue("pifc_args", ui->txtCmdPifc->toPlainText());
    settings.endGroup();

    settings.beginGroup("editor");
    settings.setValue("font_family", ui->cbFont->font().family());
    settings.setValue("font_size", ui->sbFontSize->value());
    settings.setValue("colour_scheme", ui->cbTheme->currentIndex());
    settings.endGroup();

    QMessageBox::information(nullptr, tr("Info | PIF IDE"),
                             tr("Changes are going to take effect as soon as you restart PIF IDE."),
                             QMessageBox::Ok);
    this->close();
}

void SettingsWindow::resetSettings(){
    switch (QMessageBox::question(nullptr, tr("Reset Settings | PIF IDE"),
                                  tr("Would you like to reset settings to the last saved satus"
                                     " or to restore defaults?"),
                                  QMessageBox::Reset | QMessageBox::RestoreDefaults | QMessageBox::Cancel)) {
    case QMessageBox::Reset:
        loadSettings();
        break;
    case QMessageBox::RestoreDefaults:
        QSettings settings("Nintersoft", "PIF IDE");
        settings.clear();
        QMessageBox::information(nullptr, tr("Info | PIF IDE"),
                                 tr("Changes are going to take effect as soon as you restart PIF IDE."),
                                 QMessageBox::Ok);
        this->close();
        break;
    }
}

void SettingsWindow::useCppAsC(bool checked){
    ui->edtC->setEnabled(!checked);
    ui->btSetC->setEnabled(!checked);
    ui->txtCmdC->setEnabled(!checked);
    ui->btRestoreCArgs->setEnabled(!checked);
}

void SettingsWindow::getCPath(){
    QFileDialog fileDlg;
    fileDlg.setWindowTitle(tr("Get %1 Compiler | PIF IDE").arg("C"));
    fileDlg.setAcceptMode(QFileDialog::AcceptOpen);
    fileDlg.setFileMode(QFileDialog::ExistingFile);
    fileDlg.setDirectory(QFileInfo(ui->edtC->text()).absolutePath());
    if (fileDlg.exec()) ui->edtC->setText(fileDlg.selectedFiles().at(0));
}

void SettingsWindow::getCppPath(){
    QFileDialog fileDlg;
    fileDlg.setWindowTitle(tr("Get %1 Compiler | PIF IDE").arg("CPP"));
    fileDlg.setAcceptMode(QFileDialog::AcceptOpen);
    fileDlg.setFileMode(QFileDialog::ExistingFile);
    fileDlg.setDirectory(QFileInfo(ui->edtCpp->text()).absolutePath());
    if (fileDlg.exec()) ui->edtCpp->setText(fileDlg.selectedFiles().at(0));
}

void SettingsWindow::getJavaPath(){
    QFileDialog fileDlg;
    fileDlg.setWindowTitle(tr("Get %1 Executioner | PIF IDE").arg("JAVA"));
    fileDlg.setAcceptMode(QFileDialog::AcceptOpen);
    fileDlg.setFileMode(QFileDialog::ExistingFile);
    fileDlg.setDirectory(QFileInfo(ui->edtJava->text()).absolutePath());
    if (fileDlg.exec()) ui->edtJava->setText(fileDlg.selectedFiles().at(0));
}

void SettingsWindow::getPifcPath(){
    QFileDialog fileDlg;
    fileDlg.setWindowTitle(tr("Get %1 Compiler | PIF IDE").arg("PIF"));
    fileDlg.setAcceptMode(QFileDialog::AcceptOpen);
    fileDlg.setFileMode(QFileDialog::ExistingFile);
    fileDlg.setDirectory(QFileInfo(ui->edtPifc->text()).absolutePath());
    if (fileDlg.exec()) ui->edtPifc->setText(fileDlg.selectedFiles().at(0));
}

void SettingsWindow::getJavacPath(){
    QFileDialog fileDlg;
    fileDlg.setWindowTitle(tr("Get %1 Compiler | PIF IDE").arg("JAVA"));
    fileDlg.setAcceptMode(QFileDialog::AcceptOpen);
    fileDlg.setFileMode(QFileDialog::ExistingFile);
    fileDlg.setDirectory(QFileInfo(ui->edtJavac->text()).absolutePath());
    if (fileDlg.exec()) ui->edtJavac->setText(fileDlg.selectedFiles().at(0));
}
