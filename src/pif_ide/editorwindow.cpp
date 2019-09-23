#include "editorwindow.h"
#include "ui_editorwindow.h"

EditorWindow::EditorWindow(QWidget *parent) :
    NMainWindow(parent),
    ui(new Ui::EditorWindow)
{
    ui->setupUi(this);

    // Sets the custom Widgets on the parent Class
    // Otherwise, the window resizing feature will not work
    NMainWindow::setCustomWidgets(ui->centralWidget, ui->statusBar);

    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                this->size(), QGuiApplication::primaryScreen()->availableGeometry()));

    hasChanged = false;

    frmAbout = nullptr;
    frmSettings = nullptr;

    buildProcess = nullptr;
    compileProcess = nullptr;
    executeProcess = nullptr;

    // Detects system language
    QString locale = QLocale::system().name();
    locale.truncate(locale.indexOf("_"));

    if (locale == "pt") changeLanguage("pt");

    // Initial setup
    setupEditor();
    setupEnvVars();
    createShortcuts();

    // Connects signals and slots
    connect(ui->btNewFile, SIGNAL(clicked()), this, SLOT(newFile()));
    connect(ui->btOpenFile, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->btSaveFile, SIGNAL(clicked()), this, SLOT(saveFile()));
    connect(ui->btSaveFileAs, SIGNAL(clicked()), this, SLOT(saveFileAs()));

    connect(ui->btConsoleView, SIGNAL(toggled(bool)), ui->consoleWidget, SLOT(setVisible(bool)));

    connect(ui->cIn, SIGNAL(returnPressed()), this, SLOT(sendUserInput()));

    connect(ui->btChangeExec, SIGNAL(clicked()), this, SLOT(changeExec()));

    connect(ui->btCompileNRun, SIGNAL(clicked()), this, SLOT(buildNRun()));
    connect(ui->btAbort, SIGNAL(clicked()), this, SLOT(abortProcess()));

    connect(ui->sourceEditor, SIGNAL(textChanged()), this, SLOT(sourceChanged()));

    connect(ui->btAbout, SIGNAL(clicked(bool)), this, SLOT(openAboutForm()));
    connect(ui->btSettings, SIGNAL(clicked(bool)), this, SLOT(openSettingsForm()));

    // Hides output console
    ui->btConsoleView->setChecked(false);

    // Sets focus on sourceEditor
    ui->sourceEditor->setFocus();

    // Enables text completion
    QStringList words = QStringList() << "enquanto" << "faça" << "senão"
                                      << "literal" << "numérico" << "booleano"
                                      << "fim-se" << "fim-enquanto" << "leia"
                                      << "escreva" << "programa" << "falso"
                                      << "verdadeiro";
    ui->sourceEditor->setCompleter(new QCompleter(words, this));
}

EditorWindow::~EditorWindow()
{
    abortProcess();

    delete highlighter;

    if (currentFile.isOpen()) currentFile.close();

    for (QShortcut *shortcut : shortCuts) delete shortcut;
    shortCuts.clear();

    delete ui;
}

void EditorWindow::changeEvent(QEvent *event){
    if (event && event->type() == QEvent::LanguageChange) ui->retranslateUi(this);
    NMainWindow::changeEvent(event);
}

void EditorWindow::closeEvent(QCloseEvent *event){
    int alt = -1;
    if (hasChanged) alt = QMessageBox::question(nullptr, tr("Save current | PIF IDE"),
                                                tr("The current project has unsaved changes. Would you like to save them"
                                                   " before continuing?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (alt > 0 && alt == QMessageBox::Yes)
        while (!saveFile() &&
                (alt = QMessageBox::question(nullptr, tr("Confirmation | PIF IDE"),
                                      tr("Seems that you have aborted the operation of saving the current project."
                                         " Would you like to reconsider and try to save it again?"),
                                      QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
                    == QMessageBox::Yes);
    if (alt == QMessageBox::Cancel) event->ignore();

    NMainWindow::closeEvent(event);
}

void EditorWindow::openAboutForm(){
    if (frmAbout) delete frmAbout;

    frmAbout = new AboutWindow();
    frmAbout->show();
}

void EditorWindow::openSettingsForm(){
    if (frmSettings) delete frmSettings;

    frmSettings = new SettingsWindow();
    frmSettings->show();
}

void EditorWindow::setupEditor(){
    QFont font;
    QSettings settings("Nintersoft", "PIF IDE");
    if (settings.childGroups().contains("editor")){
        settings.beginGroup("editor");
        font.setFamily(settings.value("font_family", "Segoe UI").toString());
        font.setPointSize(settings.value("font_size", 12).toInt());
        GlobalSettings::selectedTheme = settings.value("colour_scheme", 0).toInt();
        settings.endGroup();
    }
    else {
        font.setFamily("Segoe UI");
        font.setPointSize(12);
    }
    font.setFixedPitch(true);

    ui->sourceEditor->setFont(font);
    highlighter = new Highlighter(ui->sourceEditor->document());

    if (GlobalSettings::selectedTheme){
        QStringList paths = QStringList() << "://resources/themes/dark/about.css"
                                          << "://resources/themes/dark/editor.css"
                                          << "://resources/themes/dark/settings.css"
                                          << "://resources/themes/dark/titlebar.css";

        QVector<QString *> stylesheets = QVector<QString *>() << &(GlobalSettings::aboutSSheet)
                                                            << &(GlobalSettings::editorSSheet)
                                                            << &(GlobalSettings::settingsSSheet)
                                                            << &(GlobalSettings::titlebarSSheet);

        for (int i = 0; i < paths.size(); ++i){
            QFile current(paths.at(i));
            if (current.open(QIODevice::ReadOnly | QIODevice::Text)){
                *(stylesheets[i]) = current.readAll();
                current.close();
            }
        }

        NMainWindow::setTitlebarStylesheet(GlobalSettings::titlebarSSheet);
        this->setStyleSheet(GlobalSettings::editorSSheet);

        ui->btOpenFile->setIcon(QIcon("://resources/images/open_w.svg"));
        ui->btSaveFile->setIcon(QIcon("://resources/images/save_w.svg"));
        ui->btSaveFileAs->setIcon(QIcon("://resources/images/save_as_w.svg"));
        ui->btNewFile->setIcon(QIcon("://resources/images/new_w.svg"));
        ui->btSettings->setIcon(QIcon("://resources/images/settings_alt_w.svg"));
        ui->btChangeExec->setIcon(QIcon("://resources/images/change_exec_w.svg"));
        ui->btConsoleView->setIcon(QIcon("://resources/images/console_w.svg"));
        ui->btAbort->setIcon(QIcon("://resources/images/abort_run_w.svg"));
        ui->btCompileNRun->setIcon(QIcon("://resources/images/run_w.svg"));
        ui->btAbout->setIcon(QIcon("://resources/images/about_w.svg"));

        highlighter->createRules(true);
    }

    inputColour = GlobalSettings::selectedTheme ? "magenta" : "red";
    outputColour = GlobalSettings::selectedTheme ? "white" : "black";
}

void EditorWindow::setupEnvVars(){
    QSettings settings("Nintersoft", "PIF IDE");
    if (settings.childGroups().contains("environment variables")){
        settings.beginGroup("environment variables");

        bool cUsesCpp = settings.value("c_uses_cpp", false).toBool();
        cPath = settings.value(cUsesCpp ? "cpp_path" : "c_path", "").toString();
        cppPath = settings.value("cpp_path", "").toString();
        javaPath = settings.value("java_path", "").toString();
        pifcPath = settings.value("pifc_path", "").toString();
        javacPath = settings.value("javac_path", "").toString();

#ifdef Q_OS_WINDOWS
        if (!(pifcPath = QStandardPaths::findExecutable("pifc")).isEmpty())
            settings.setValue("pifc_path", pifcPath);

        if (pifcPath.isEmpty()){
            QDir appDir(QApplication::applicationDirPath());
            if (appDir.cdUp() && appDir.cdUp()){
                QString aPath = appDir.absolutePath() + QDir::separator();
                pifcPath = QStandardPaths::findExecutable("pifc", QStringList() << aPath + "Nintersoft" + QDir::separator() + "PIF" + QDir::separator()
                                                                                << aPath + QDir::separator() + "Projeto PIF" + QDir::separator() + "PIF" + QDir::separator()
                                                                                << aPath + QDir::separator() + "PIF" + QDir::separator());
                if (!pifcPath.isEmpty()) settings.setValue("pifc_path", pifcPath);
            }
        }
#endif

        GlobalSettings::cmdlPifc = settings.value("pifc_args", tr("-f\n-l\n$lang\n$source\n-o\n$object")).toString().split("\n", QString::SkipEmptyParts);
        GlobalSettings::cmdlCppComp = settings.value("cpp_args", tr("$source\n-o\n$executable")).toString().split("\n", QString::SkipEmptyParts);
        GlobalSettings::cmdlCComp = cUsesCpp ? GlobalSettings::cmdlCppComp : settings.value("c_args", tr("$source\n-o\n$executable")).toString().split("\n", QString::SkipEmptyParts);
        GlobalSettings::cmdlJavaComp = settings.value("javac_args", tr("$source")).toString().split("\n", QString::SkipEmptyParts);
        GlobalSettings::cmdlJavaExec = settings.value("java_args", tr("-cp\n$wdir\n$executable")).toString().split("\n", QString::SkipEmptyParts);
    }
    else {
        settings.beginGroup("environment variables");

        cPath = QStandardPaths::findExecutable("gcc");
        cppPath = QStandardPaths::findExecutable("g++");
        javaPath = QStandardPaths::findExecutable("java");
        pifcPath = QStandardPaths::findExecutable("pifc");
        javacPath = QStandardPaths::findExecutable("javac");

#ifdef Q_OS_WINDOWS
        if (pifcPath.isEmpty()){
            QDir appDir(QApplication::applicationDirPath());
            if (appDir.cdUp() && appDir.cdUp()){
                QString aPath = appDir.absolutePath() + QDir::separator();
                pifcPath = QStandardPaths::findExecutable("pifc", QStringList() << aPath + "Nintersoft" + QDir::separator() + "PIF" + QDir::separator()
                                                                                << aPath + QDir::separator() + "Projeto PIF" + QDir::separator() + "PIF" + QDir::separator()
                                                                                << aPath + QDir::separator() + "PIF" + QDir::separator());
            }
        }
#endif

        settings.setValue("c_path", cPath);
        settings.setValue("cpp_path", cppPath);
        settings.setValue("java_path", javaPath);
        settings.setValue("pifc_path", pifcPath);
        settings.setValue("javac_path", javacPath);

        if (cPath.isEmpty() && !cppPath.isEmpty()){
            cPath = cppPath;
            settings.setValue("c_uses_cpp", true);
        }
    }
    settings.endGroup();

    if (pifcPath.isEmpty()){
        ui->btCompileNRun->setEnabled(false);

        settings.beginGroup("warnings");
        if (!settings.value("no_pifc", false).toBool()){
            QCheckBox *cb = new QCheckBox(this);
            cb->setText(tr("Do not show this message again!"));

            QMessageBox message;
            message.setIcon(QMessageBox::Warning);
            message.setText(tr("It was not possible to find a PIF compiler in this machine."
                               " Any execution operation will be available until a valid path to a PIF compiler is set."));
            message.setStandardButtons(QMessageBox::Ok);
            message.setCheckBox(cb);

            connect(cb, &QCheckBox::toggled, [](bool checked){
                QSettings settings("Nintersoft", "PIF IDE");
                settings.beginGroup("warnings");
                settings.setValue("no_pifc", checked);
                settings.endGroup();
            });

            message.exec();
            delete cb;
        }
        settings.endGroup();
    }

    if (settings.childGroups().contains("execution")){
        settings.beginGroup("execution");
        curExec = static_cast<Executor>(settings.value("standard exec", Executor::NONE).toInt());
        settings.endGroup();
    }
    else {
        if (!cPath.isEmpty()) curExec = Executor::C;
        else if (!cppPath.isEmpty()) curExec = Executor::CPP;
        else if (!(javaPath.isEmpty() || javacPath.isEmpty())) curExec = Executor::JAVA;
        else curExec = Executor::NONE;

        settings.beginGroup("execution");
        settings.setValue("standard exec", curExec);
        settings.endGroup();
    }

    ui->btChangeExec->setEnabled(!(javaPath.isEmpty() || javacPath.isEmpty())
                                 || !cPath.isEmpty() || !cppPath.isEmpty());
}

void EditorWindow::createShortcuts(){
    shortCuts.append(new QShortcut(QKeySequence(tr("Ctrl+O", "File|Open")), this));
    shortCuts.append(new QShortcut(QKeySequence(tr("Ctrl+S", "File|Save")), this));
    shortCuts.append(new QShortcut(QKeySequence(tr("F12", "File|Save As")), this));
    shortCuts.append(new QShortcut(QKeySequence(tr("Ctrl+Shift+S", "File|Save As")), this));
    shortCuts.append(new QShortcut(QKeySequence(tr("Ctrl+N", "File|New")), this));
    shortCuts.append(new QShortcut(QKeySequence(tr("Ctrl++", "Zoom|In")), this));
    shortCuts.append(new QShortcut(QKeySequence(tr("Ctrl+-", "Zoom|Out")), this));
    shortCuts.append(new QShortcut(QKeySequence(tr("Ctrl+Shift+C", "Console|Toggle")), this));
    shortCuts.append(new QShortcut(QKeySequence(tr("Ctrl+R", "Project|Run")), this));
    shortCuts.append(new QShortcut(QKeySequence(tr("Ctrl+Shift+E", "Project|Abort")), this));

    connect(shortCuts[0], SIGNAL(activated()), this, SLOT(openFile()));
    connect(shortCuts[1], SIGNAL(activated()), this, SLOT(saveFile()));
    connect(shortCuts[2], SIGNAL(activated()), this, SLOT(saveFileAs()));
    connect(shortCuts[3], SIGNAL(activated()), this, SLOT(saveFileAs()));
    connect(shortCuts[4], SIGNAL(activated()), this, SLOT(newFile()));

    connect(shortCuts[5], SIGNAL(activated()), this, SLOT(increaseFontSize()));
    connect(shortCuts[6], SIGNAL(activated()), this, SLOT(reduceFontSize()));

    connect(shortCuts[7], SIGNAL(activated()), ui->btConsoleView, SLOT(toggle()));

    if (!pifcPath.isEmpty()){
        connect(shortCuts[8], SIGNAL(activated()), this, SLOT(buildNRun()));
        connect(shortCuts[9], SIGNAL(activated()), this, SLOT(abortProcess()));
    }
}

void EditorWindow::changeLanguage(const QString &slug){
    changeTranslator(translator, QApplication::applicationDirPath().append("/lang/pif_ide_%1.qm").arg(slug));
    changeTranslator(qtTranslator, QApplication::applicationDirPath().append("/translations/qt_%1.qm").arg(slug));
}

void EditorWindow::changeTranslator(QTranslator &translator, const QString &filePath){
    QApplication::removeTranslator(&translator);
    if (translator.load(filePath)) QApplication::installTranslator(&translator);
}

void EditorWindow::sourceChanged(){
    hasChanged = true;
    if (currentFile.isOpen())
        this->setWindowTitle(tr("%1.pifc* | PIF IDE").arg(QFileInfo(currentFile).baseName()));
    else this->setWindowTitle(tr("NewFile.pifc* | PIF IDE"));

    disconnect(ui->sourceEditor, SIGNAL(textChanged()), this, SLOT(sourceChanged()));
}

void EditorWindow::openFile(){
    int alt = -1;
    if (hasChanged && (alt = QMessageBox::question(nullptr, tr("Save current | PIF IDE"),
                                     tr("The current project has unsaved changes. Would you like to save them"
                                        " before continuing?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
                        == QMessageBox::Cancel) return;

    if (alt > 0 && alt == QMessageBox::Yes)
        while (!saveFile() &&
                QMessageBox::question(nullptr, tr("Confirmation | PIF IDE"),
                                      tr("Seems that you have aborted the operation of saving the current project."
                                         " Would you like to reconsider and try to save it again?"),
                                      QMessageBox::Yes, QMessageBox::No)
                    == QMessageBox::Yes);

    if (currentFile.isOpen()) currentFile.close();

    QFileDialog openDialog(nullptr, tr("Open PIF file | PIF IDE"), QDir::homePath(), tr("PIF source file (%1)").arg("*.pifc"));
    openDialog.setFileMode(QFileDialog::ExistingFile);
    openDialog.setAcceptMode(QFileDialog::AcceptOpen);
    openDialog.setDefaultSuffix(".pifc");
    if (openDialog.exec()){
        currentFile.setFileName(openDialog.selectedFiles()[0]);
        if (!currentFile.open(QIODevice::ReadWrite | QIODevice::Text)){
            QMessageBox::critical(nullptr, tr("Error | PIF IDE"),
                                  tr("It was not possible to open the specified file. Please, make sure the file %1 "
                                     "exists and is readable/writeable.").arg(currentFile.fileName()),
                                  QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }

        hasChanged = false;
        disconnect(ui->sourceEditor, SIGNAL(textChanged()), this, SLOT(sourceChanged()));
        this->setWindowTitle(tr("%1.pifc | PIF IDE").arg(QFileInfo(currentFile).baseName()));
        ui->sourceEditor->setText(currentFile.readAll());
        connect(ui->sourceEditor, SIGNAL(textChanged()), this, SLOT(sourceChanged()));
    }
}

bool EditorWindow::saveFile(){
    if (!hasChanged) return true;

    if (currentFile.isOpen()){
        currentFile.close();
        if (currentFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){
            QTextStream stream(&currentFile);
            stream.setCodec("UTF-8");
            stream.setGenerateByteOrderMark(true);
            stream << ui->sourceEditor->toPlainText().toUtf8();
            currentFile.close();

            while (!currentFile.open(QIODevice::ReadWrite | QIODevice::Text) &&
                QMessageBox::warning(nullptr, tr("Warning | PIF IDE"),
                                     tr("It was not possible to stablish a file lock (open with read/write permissions)."
                                        " This may cause your current project to be overwritten by other programs."
                                        " Would you like us to try again?"), QMessageBox::Yes, QMessageBox::No)
                   == QMessageBox::Yes);

            hasChanged = false;
            this->setWindowTitle(tr("%1.pifc | PIF IDE").arg(QFileInfo(currentFile).baseName()));
            connect(ui->sourceEditor, SIGNAL(textChanged()), this, SLOT(sourceChanged()));
            return true;
        }
        else {
            QMessageBox::critical(nullptr, tr("Error | PIF IDE"),
                                  tr("It was not possible to truncate the specified filename so as to save the"
                                     " current state of the project (It was not possible to stablish the file"
                                     " lock as well). Please, try again later."), QMessageBox::Ok);
            return false;
        }
    }

    QFileDialog saveDialog(nullptr, tr("Save PIF file | PIF IDE"), QDir::homePath(), tr("PIF source file (%1)").arg("*.pifc"));
    saveDialog.setAcceptMode(QFileDialog::AcceptSave);
    saveDialog.setDefaultSuffix(".pifc");
    if (saveDialog.exec()){
        currentFile.setFileName(saveDialog.selectedFiles()[0]);
        if (!currentFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){
            QMessageBox::critical(nullptr, tr("Error | PIF IDE"),
                                  tr("It was not possible to create the specified file. Please, make sure the file %1 "
                                     "is readable/writeable.").arg(currentFile.fileName()),
                                  QMessageBox::Ok, QMessageBox::NoButton);
            return false;
        }

        QTextStream stream(&currentFile);
        stream.setCodec("UTF-8");
        stream.setGenerateByteOrderMark(true);
        stream << ui->sourceEditor->toPlainText().toUtf8();
        currentFile.close();

        while (!currentFile.open(QIODevice::ReadWrite | QIODevice::Text) &&
            QMessageBox::warning(nullptr, tr("Warning | PIF IDE"),
                                 tr("It was not possible to stablish a file lock (open with read/write permissions)."
                                    " This may cause your current project to be overwritten by other programs."
                                    " Would you like us to try again?"), QMessageBox::Yes, QMessageBox::No)
               == QMessageBox::Yes);

        hasChanged = false;
        this->setWindowTitle(tr("%1.pifc | PIF IDE").arg(QFileInfo(currentFile).baseName()));
        connect(ui->sourceEditor, SIGNAL(textChanged()), this, SLOT(sourceChanged()));
        return true;
    }
    else return false;
}

bool EditorWindow::saveFileAs(){
    QFileDialog saveDialog(nullptr, tr("Save PIF file as... | PIF IDE"), QDir::homePath(), tr("PIF source file (%1)").arg("*.pifc"));
    saveDialog.setAcceptMode(QFileDialog::AcceptSave);
    saveDialog.setDefaultSuffix(".pifc");
    if (saveDialog.exec()){
        if (currentFile.isOpen()) currentFile.close();

        currentFile.setFileName(saveDialog.selectedFiles()[0]);
        if (!currentFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){
            QMessageBox::critical(nullptr, tr("Error | PIF IDE"),
                                  tr("It was not possible to create the specified file. Please, make sure the file %1 "
                                     "is readable/writeable.").arg(currentFile.fileName()),
                                  QMessageBox::Ok, QMessageBox::NoButton);
            return false;
        }

        QTextStream stream(&currentFile);
        stream.setCodec("UTF-8");
        stream.setGenerateByteOrderMark(true);
        stream << ui->sourceEditor->toPlainText().toUtf8();
        currentFile.close();

        while (!currentFile.open(QIODevice::ReadWrite | QIODevice::Text) &&
            QMessageBox::warning(nullptr, tr("Warning | PIF IDE"),
                                 tr("It was not possible to stablish a file lock (open with read/write permissions)."
                                    " This may cause your current project to be overwritten by other programs."
                                    " Would you like us to try again?"), QMessageBox::Yes, QMessageBox::No)
               == QMessageBox::Yes);

        hasChanged = false;
        this->setWindowTitle(tr("%1.pifc | PIF IDE").arg(QFileInfo(currentFile).baseName()));
        connect(ui->sourceEditor, SIGNAL(textChanged()), this, SLOT(sourceChanged()));
        return true;
    }
    else return false;
}

void EditorWindow::newFile(){
    int alt = -1;
    if (hasChanged && (alt = QMessageBox::question(nullptr, tr("Save current | PIF IDE"),
                                     tr("The current project has unsaved changes. Would you like to save them"
                                        " before continuing?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
                        == QMessageBox::Cancel) return;

    if (alt > 0 && alt == QMessageBox::Yes)
        while (!saveFile() &&
                QMessageBox::question(nullptr, tr("Confirmation | PIF IDE"),
                                      tr("Seems that you have aborted the operation of saving the current project."
                                         " Would you like to reconsider and try to save it again?"),
                                      QMessageBox::Yes, QMessageBox::No)
                    == QMessageBox::Yes);

    if (currentFile.isOpen()) currentFile.close();

    hasChanged = false;
    disconnect(ui->sourceEditor, SIGNAL(textChanged()), this, SLOT(sourceChanged()));
    this->setWindowTitle(tr("PIF IDE"));
    ui->sourceEditor->clear();
    connect(ui->sourceEditor, SIGNAL(textChanged()), this, SLOT(sourceChanged()));
}

void EditorWindow::compileProject(){
    int alt = -1;
    if (hasChanged && (alt = QMessageBox::question(nullptr, tr("Save current | PIF IDE"),
                                     tr("The current project has unsaved changes. Would you like to save them"
                                        " before continuing? (if you choose 'no', the project is not going to be built)"),
                                                   QMessageBox::Yes | QMessageBox::No))
                        == QMessageBox::Cancel) return;
    if (alt > 0 && alt == QMessageBox::Yes)
        while (!saveFile() &&
                (alt = QMessageBox::question(nullptr, tr("Confirmation | PIF IDE"),
                                      tr("Seems that you have aborted the operation of saving the current project."
                                         " Would you like to reconsider and try to save it again?"),
                                      QMessageBox::Yes, QMessageBox::No))
                    == QMessageBox::Yes);
    if (alt == QMessageBox::No) return;

    ui->btAbort->setEnabled(true);
    ui->btCompileNRun->setEnabled(false);
    if (!ui->btConsoleView->isChecked()) ui->btConsoleView->toggle();

    ui->cOut->append(QString("<font color=\"%1\"><strong>[ %2 ] :</strong></font>").arg(outputColour, tr("PIF COMPILER OUTPUT")));

    if (buildProcess){
        disconnect(buildProcess, SIGNAL(readyReadStandardError()), this, SLOT(builderError()));
        disconnect(buildProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(builderOutput()));
        disconnect(buildProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(builderExited(int,QProcess::ExitStatus)));
        delete buildProcess;
    }

    buildProcess = new QProcess(this);
    buildProcess->setProgram(pifcPath);
    buildProcess->setProcessChannelMode(QProcess::SeparateChannels);

    connect(buildProcess, SIGNAL(readyReadStandardError()), this, SLOT(builderError()));
    connect(buildProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(builderOutput()));
    connect(buildProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(builderExited(int,QProcess::ExitStatus)));

    QFileInfo currentFileInfo(currentFile.fileName());
    QString objectPath = currentFileInfo.absolutePath() + QDir::separator() + currentFileInfo.baseName();

    QStringList cLineArgs = GlobalSettings::cmdlPifc;
    cLineArgs.replaceInStrings("$source", currentFile.fileName());
    switch (curExec) {
        case C:
            cLineArgs.replaceInStrings("$lang", "c")
                    .replaceInStrings("$object", (objectPath + ".c"));
            break;
        case CPP:
            cLineArgs.replaceInStrings("$lang", "cpp")
                    .replaceInStrings("$object", (objectPath + ".cpp"));
            break;
        default:
            cLineArgs.replaceInStrings("$lang", "java")
                    .replaceInStrings("$object", (objectPath + ".java"));
            break;
    }
    buildProcess->setArguments(cLineArgs);
    buildProcess->start();
}

void EditorWindow::compileObject(){
    ui->cOut->append(QString("<font color=\"%1\"><strong>[ %2 ] :</strong></font>").arg(outputColour, tr("COMPILER OUTPUT")));

    QFileInfo currentFileInfo(currentFile.fileName());
    QString objectPath = currentFileInfo.absolutePath() + QDir::separator() + currentFileInfo.baseName();
#ifdef Q_OS_WINDOWS
    QString compileExt = ".exe";
#else
    QString compileExt = "";
#endif

    if (compileProcess){
        disconnect(compileProcess, SIGNAL(readyReadStandardError()), this, SLOT(compilerError()));
        disconnect(compileProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(compilerOutput()));
        disconnect(compileProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(compilerExited(int,QProcess::ExitStatus)));
        delete compileProcess;
    }

    compileProcess = new QProcess(this);
    compileProcess->setProcessChannelMode(QProcess::SeparateChannels);

    connect(compileProcess, SIGNAL(readyReadStandardError()), this, SLOT(compilerError()));
    connect(compileProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(compilerOutput()));
    connect(compileProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(compilerExited(int,QProcess::ExitStatus)));

    QStringList cLineArgs;
    switch (curExec) {
        case C:
            if (cPath.isEmpty()) {
                if (!ui->btConsoleView->isChecked()) ui->btConsoleView->toggle();
                ui->cOut->append(QString("<font color=\"red\"><strong>%1</strong></font>").arg(tr("%1 compiler path is not set.").arg("C")));
                abortProcess();
                return;
            }
            compileProcess->setProgram(cPath);
            cLineArgs = GlobalSettings::cmdlCComp;
            cLineArgs.replaceInStrings("$source", (objectPath + ".c"))
                    .replaceInStrings("$executable", (objectPath + compileExt));
            break;
        case CPP:
            if (cppPath.isEmpty()) {
                if (!ui->btConsoleView->isChecked()) ui->btConsoleView->toggle();
                ui->cOut->append(QString("<font color=\"red\"><strong>%1</strong></font>").arg(tr("%1 compiler path is not set.").arg("C++")));
                abortProcess();
                return;
            }
            compileProcess->setProgram(cppPath);
            cLineArgs = GlobalSettings::cmdlCppComp;
            cLineArgs.replaceInStrings("$source", (objectPath + ".cpp"))
                    .replaceInStrings("$executable", (objectPath + compileExt));
            break;
        case JAVA:
            if (javacPath.isEmpty()) {
                if (!ui->btConsoleView->isChecked()) ui->btConsoleView->toggle();
                ui->cOut->append(QString("<font color=\"orange\"></strong>%1<strong></font>").arg(tr("%1 compiler path is not set.").arg("Java")));
                abortProcess();
                return;
            }
            compileProcess->setProgram(javacPath);
            cLineArgs = GlobalSettings::cmdlJavaComp;
            cLineArgs.replaceInStrings("$source", (objectPath + ".java"));
            break;
        default:
            if (javacPath.isEmpty()) {
                if (!ui->btConsoleView->isChecked()) ui->btConsoleView->toggle();
                ui->cOut->append(QString("<font color=\"orange\"><strong>%1</strong></font>").arg(tr("There is no final compiler defined!")));
                abortProcess();
                return;
            }
    }
    compileProcess->setArguments(cLineArgs);
    compileProcess->start();
}

void EditorWindow::runProject(){
    ui->cOut->append(QString("<font color=\"%1\"><strong>[ %2 ] :</strong></font>").arg(outputColour, tr("APPLICATION OUTPUT")));
    ui->cOut->append("");

    ui->btAbort->setEnabled(true);
    ui->btCompileNRun->setEnabled(false);
    if (!ui->btConsoleView->isChecked()) ui->btConsoleView->toggle();

    QFileInfo currentFileInfo(currentFile.fileName());
    QString objectPath = currentFileInfo.absolutePath() + QDir::separator() + currentFileInfo.baseName();
#ifdef Q_OS_WINDOWS
    objectPath += ".exe";
#endif

    if (!QFile::exists(objectPath)) {
        if (!ui->btConsoleView->isChecked()) ui->btConsoleView->toggle();
        ui->cOut->append(QString("<font color=\"orange\"></strong>%1</strong></font>").arg(tr("There is no output executable to run. Expected : '%1'").arg(objectPath)));
        abortProcess();
        return;
    }

    if (executeProcess){
        disconnect(executeProcess, SIGNAL(readyReadStandardError()), this, SLOT(executionError()));
        disconnect(executeProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(executionOutput()));
        disconnect(executeProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(executionExited(int,QProcess::ExitStatus)));
        delete executeProcess;
    }

    executeProcess = new QProcess(this);
    executeProcess->setProcessChannelMode(QProcess::SeparateChannels);

    connect(executeProcess, SIGNAL(readyReadStandardError()), this, SLOT(executionError()));
    connect(executeProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(executionOutput()));
    connect(executeProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(executionExited(int,QProcess::ExitStatus)));

    QStringList cLineArgs;
    switch (curExec) {
        case JAVA:
            if (javaPath.isEmpty()) {
                if (!ui->btConsoleView->isChecked()) ui->btConsoleView->toggle();
                ui->cOut->append(QString("<font color=\"orange\"><strong>%1</strong></font>").arg(tr("There is no JVM set. Execution aborted.")));
                abortProcess();
                return;
            }
            executeProcess->setProgram(javaPath);
            cLineArgs = GlobalSettings::cmdlJavaExec;
            cLineArgs.replaceInStrings("$wdir", currentFileInfo.absolutePath())
                    .replaceInStrings("$executable", currentFileInfo.baseName());
            break;
        default:
            executeProcess->setProgram(objectPath);
            break;
    }
    executeProcess->setArguments(cLineArgs);
    executeProcess->start(QIODevice::ReadWrite | QIODevice::Unbuffered);
}

void EditorWindow::reduceFontSize(){
    QFont oldF = ui->sourceEditor->font();
    int old = oldF.pointSize();
    oldF.setPointSize(old > 10 ? --old : old);

    ui->sourceEditor->setFont(oldF);
    ui->statusBar->showMessage(tr("Font size set : %1").arg(old), 5000);
}

void EditorWindow::increaseFontSize(){
    QFont oldF = ui->sourceEditor->font();
    int old = oldF.pointSize();
    oldF.setPointSize(++old);

    ui->sourceEditor->setFont(oldF);
    ui->statusBar->showMessage(tr("Font size set : %1").arg(old), 5000);
}

void EditorWindow::sendUserInput(){
    if (!executeProcess || executeProcess->state() == QProcess::NotRunning)
        ui->cOut->append(QString("<font color=\"%1\">%2</font>").arg(inputColour, ui->cIn->text()));
    else {
        ui->cOut->append(QString("<font color=\"blue\">%1</font>").arg(ui->cIn->text()));
        executeProcess->write(ui->cIn->text().toUtf8() + "\n");
    }
    ui->cIn->clear();
}

void EditorWindow::changeExec(){
    QString c = tr("C compiler (executable)"),
            cpp = tr("C++ compiler (executable)"),
            java = tr("Java (Java binary)");

    int checked = 0;
    QStringList itens;
    if (!cPath.isEmpty()) itens << c;
    if (!cppPath.isEmpty()){
        itens << cpp;
        if (curExec == Executor::CPP) checked = itens.size() - 1;
    }
    if (!(javaPath.isEmpty() || javacPath.isEmpty())){
        itens << java;
        if (curExec == Executor::JAVA) checked = itens.size() - 1;
    }

    QString selected = QInputDialog::getItem(nullptr, tr("Choose executor | PIF IDE"),
                                            tr("Please, choose the final compiler in which the application will"
                                               " be build with."),
                                            itens, checked, false);

    if (selected == c) curExec = Executor::C;
    else if (selected == cpp) curExec = Executor::CPP;
    else if (selected == java) curExec = Executor::JAVA;
    else curExec = Executor::NONE;

    QSettings settings("Nintersoft", "PIF IDE");
    settings.beginGroup("execution");
    settings.setValue("standard exec", curExec);
    settings.endGroup();
}

void EditorWindow::buildNRun(){
    ui->cOut->clear();
    compileProject();
}

void EditorWindow::abortProcess(){
    ui->btAbort->setEnabled(false);
    ui->btCompileNRun->setEnabled(true);

    if (buildProcess && buildProcess->state() != QProcess::NotRunning) buildProcess->kill();
    if (compileProcess && compileProcess->state() != QProcess::NotRunning) compileProcess->kill();
    if (executeProcess && executeProcess->state() != QProcess::NotRunning) executeProcess->kill();
}

void EditorWindow::builderError(){
    if (!ui->btConsoleView->isChecked()) ui->btConsoleView->toggle();
    QStringList lines = QString::fromUtf8(buildProcess->readAllStandardError()).split(QRegularExpression("(\\n|\\r\\n)"));
    if (lines.size() > 1 && lines.last().isEmpty()) lines.removeLast();
    for (const QString &current : lines)
        ui->cOut->append(QString("<font color=\"orange\">%1</font>").arg(current));
}

void EditorWindow::builderOutput(){
    if (!ui->btConsoleView->isChecked()) ui->btConsoleView->toggle();
    QStringList lines = QString::fromUtf8(buildProcess->readAllStandardOutput()).split(QRegularExpression("(\\n|\\r\\n)"));
    if (lines.size() > 1 && lines.last().isEmpty()) lines.removeLast();
    for (const QString &current : lines)
        ui->cOut->append(QString("<font color=\"%1\">%2</font>").arg(outputColour, current));
}

void EditorWindow::builderExited(int exitCode, QProcess::ExitStatus status){
    if (exitCode || status == QProcess::CrashExit){
        ui->cOut->append(QString("<font color=\"orange\">%1</font>").arg(tr("PIF Compiler exited with code %1.").arg(exitCode)));
        abortProcess();
        return;
    }
    ui->cOut->append(QString("<font color=\"green\">%1</font>").arg(tr("PIF Compiler exited with code %1.").arg(exitCode)));
    ui->cOut->append("");
    compileObject();
}

void EditorWindow::compilerError(){
    if (!ui->btConsoleView->isChecked()) ui->btConsoleView->toggle();
    QStringList lines = QString::fromUtf8(compileProcess->readAllStandardError()).split(QRegularExpression("(\\n|\\r\\n)"));
    if (lines.size() > 1 && lines.last().isEmpty()) lines.removeLast();
    for (const QString &current : lines)
        ui->cOut->append(QString("<font color=\"orange\">%1</font>").arg(current));
}

void EditorWindow::compilerOutput(){
    if (!ui->btConsoleView->isChecked()) ui->btConsoleView->toggle();
    QStringList lines = QString::fromUtf8(compileProcess->readAllStandardOutput()).split(QRegularExpression("(\\n|\\r\\n)"));
    if (lines.size() > 1 && lines.last().isEmpty()) lines.removeLast();
    for (const QString &current : lines)
        ui->cOut->append(QString("<font color=\"%1\">%2</font>").arg(outputColour, current));
}

void EditorWindow::compilerExited(int exitCode, QProcess::ExitStatus status){
    if (exitCode || status == QProcess::CrashExit){
        ui->cOut->append(QString("<font color=\"orange\">%1</font>").arg(tr("Compiler exited with code %1.").arg(exitCode)));
        abortProcess();
        return;
    }
    ui->cOut->append(QString("<font color=\"green\">%1</font>").arg(tr("Compiler exited with code %1.").arg(exitCode)));
    ui->cOut->append("");
    runProject();
}

void EditorWindow::executionError(){
    if (!ui->btConsoleView->isChecked()) ui->btConsoleView->toggle();
    QStringList lines = QString::fromUtf8(executeProcess->readAllStandardError()).split(QRegularExpression("(\\n|\\r\\n)"));
    if (lines.size() > 1 && lines.last().isEmpty()) lines.removeLast();
    for (const QString &current : lines)
        ui->cOut->append(QString("<font color=\"orange\">%1</font>").arg(current));
}

void EditorWindow::executionOutput(){
    if (!ui->btConsoleView->isChecked()) ui->btConsoleView->toggle();
    QStringList lines = QString::fromUtf8(executeProcess->readAllStandardOutput()).split(QRegularExpression("(\\n|\\r\\n)"));
    if (lines.size() > 1 && lines.last().isEmpty()) lines.removeLast();
    for (const QString &current : lines)
        ui->cOut->append(QString("<font color=\"%1\">%2</font>").arg(outputColour, current));
}

void EditorWindow::executionExited(int exitCode, QProcess::ExitStatus){
    ui->cOut->append("");
    ui->cOut->append(QString("<font color=\"magenta\">%1</font>").arg(tr("\nProcess exited with code %1.").arg(exitCode)));
    ui->btAbort->setEnabled(false);
    ui->btCompileNRun->setEnabled(true);
}
