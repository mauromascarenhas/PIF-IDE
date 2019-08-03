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

    hasChanged = false;

    setupEditor();
    createShortcuts();

    // Connects signals and slots
    connect(ui->btNewFile, SIGNAL(clicked()), this, SLOT(newFile()));
    connect(ui->btOpenFile, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->btSaveFile, SIGNAL(clicked()), this, SLOT(saveFile()));

    connect(ui->btConsoleView, SIGNAL(toggled(bool)), ui->consoleWidget, SLOT(setVisible(bool)));

    connect(ui->cIn, SIGNAL(returnPressed()), this, SLOT(sendUserInput()));

    connect(ui->btCompileNRun, SIGNAL(clicked()), this, SLOT(buildNRun()));
    connect(ui->btAbort, SIGNAL(clicked()), this, SLOT(abortProcess()));

    connect(ui->sourceEditor, SIGNAL(textChanged()), this, SLOT(sourceChanged()));

    // TODO: Info and Settings window
    // TODO: Load settings (and store them as well)

    // Hides output console
    ui->btConsoleView->setChecked(false);
}

EditorWindow::~EditorWindow()
{
    delete highlighter;

    for (QShortcut *shortcut : shortCuts) delete shortcut;
    shortCuts.clear();

    delete ui;
}

void EditorWindow::setupEditor(){
    QFont font;
//    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(12);

    ui->sourceEditor->setFont(font);
    highlighter = new Highlighter(ui->sourceEditor->document());
}

void EditorWindow::createShortcuts(){
    shortCuts.append(new QShortcut(QKeySequence(tr("Ctrl+O", "File|Open")), this));
    shortCuts.append(new QShortcut(QKeySequence(tr("Ctrl+S", "File|Save")), this));
    shortCuts.append(new QShortcut(QKeySequence(tr("Ctrl+N", "File|New")), this));
    shortCuts.append(new QShortcut(QKeySequence(tr("Ctrl++", "Zoom|In")), this));
    shortCuts.append(new QShortcut(QKeySequence(tr("Ctrl+-", "Zoom|Out")), this));
    shortCuts.append(new QShortcut(QKeySequence(tr("Ctrl+Shift+C", "Console|Toggle")), this));
    shortCuts.append(new QShortcut(QKeySequence(tr("Ctrl+R", "Project|Run")), this));
    shortCuts.append(new QShortcut(QKeySequence(tr("Ctrl+Shift+E", "Project|Abort")), this));

    connect(shortCuts[0], SIGNAL(activated()), this, SLOT(openFile()));
    connect(shortCuts[1], SIGNAL(activated()), this, SLOT(saveFile()));
    connect(shortCuts[2], SIGNAL(activated()), this, SLOT(newFile()));

    connect(shortCuts[3], SIGNAL(activated()), this, SLOT(increaseFontSize()));
    connect(shortCuts[4], SIGNAL(activated()), this, SLOT(reduceFontSize()));

    connect(shortCuts[5], SIGNAL(activated()), ui->btConsoleView, SLOT(toggle()));

    connect(shortCuts[6], SIGNAL(activated()), this, SLOT(buildNRun()));
    connect(shortCuts[7], SIGNAL(activated()), this, SLOT(abortProcess()));
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
    if (hasChanged && (alt = QMessageBox::question(this, tr("Save current | PIF IDE"),
                                     tr("The current project has unsaved changed. Would you like to save them"
                                        " before continuing?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
                        == QMessageBox::Cancel) return;

    // WARNING: It is not checking if file has been saved!
    if (alt > 0 && alt == QMessageBox::Yes) saveFile();

    if (currentFile.isOpen()) currentFile.close();

    QFileDialog openDialog(this, tr("Open PIF file - PIF IDE"), QDir::homePath(), tr("PIF source file (%1)").arg("*.pifc"));
    openDialog.setFileMode(QFileDialog::ExistingFile);
    openDialog.setAcceptMode(QFileDialog::AcceptOpen);
    openDialog.setDefaultSuffix(".pifc");
    if (openDialog.exec()){
        currentFile.setFileName(openDialog.selectedFiles()[0]);
        if (!currentFile.open(QIODevice::ReadWrite | QIODevice::Text)){
            QMessageBox::critical(this, tr("Error | PIF IDE"),
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

void EditorWindow::saveFile(){
    if (!hasChanged) return;

    if (currentFile.isOpen()){
        currentFile.close();
        if (currentFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){
            QTextStream stream(&currentFile);
            stream.setCodec("UTF-8");
            stream.setGenerateByteOrderMark(true);
            stream << ui->sourceEditor->toPlainText().toUtf8();
            currentFile.close();

            while (!currentFile.open(QIODevice::ReadWrite | QIODevice::Text) &&
                QMessageBox::warning(this, tr("Warning | PIF IDE"),
                                     tr("It was not possible to stablish a file lock (open with read/write permissions)."
                                        " This may cause your current project to be overwritten by other programs."
                                        " Would you like us to try again?"), QMessageBox::Yes, QMessageBox::No)
                   == QMessageBox::Yes);
            return;

            hasChanged = false;
            this->setWindowTitle(tr("%1.pifc | PIF IDE").arg(QFileInfo(currentFile).baseName()));
            connect(ui->sourceEditor, SIGNAL(textChanged()), this, SLOT(sourceChanged()));
        }
        else {
            QMessageBox::critical(this, tr("Error | PIF IDE"),
                                  tr("It was not possible to truncate the specified filename so as to save the"
                                     " current state of the project (It was not possible to stablish the file"
                                     " lock as well). Please, try again later."), QMessageBox::Ok);
            return;
        }
    }

    QFileDialog saveDialog(this, tr("Save PIF file - PIF IDE"), QDir::homePath(), tr("PIF source file (%1)").arg("*.pifc"));
    saveDialog.setAcceptMode(QFileDialog::AcceptSave);
    saveDialog.setDefaultSuffix(".pifc");
    if (saveDialog.exec()){
        currentFile.setFileName(saveDialog.selectedFiles()[0]);
        if (!currentFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){
            QMessageBox::critical(this, tr("Error | PIF IDE"),
                                  tr("It was not possible to create the specified file. Please, make sure the file %1 "
                                     "is readable/writeable.").arg(currentFile.fileName()),
                                  QMessageBox::Ok, QMessageBox::NoButton);
            return;
        }

        QTextStream stream(&currentFile);
        stream.setCodec("UTF-8");
        stream.setGenerateByteOrderMark(true);
        stream << ui->sourceEditor->toPlainText().toUtf8();
        currentFile.close();

        while (!currentFile.open(QIODevice::ReadWrite | QIODevice::Text) &&
            QMessageBox::warning(this, tr("Warning | PIF IDE"),
                                 tr("It was not possible to stablish a file lock (open with read/write permissions)."
                                    " This may cause your current project to be overwritten by other programs."
                                    " Would you like us to try again?"), QMessageBox::Yes, QMessageBox::No)
               == QMessageBox::Yes);

        hasChanged = false;
        this->setWindowTitle(tr("%1.pifc | PIF IDE").arg(QFileInfo(currentFile).baseName()));
        connect(ui->sourceEditor, SIGNAL(textChanged()), this, SLOT(sourceChanged()));
    }
}

void EditorWindow::newFile(){
    int alt = -1;
    if (hasChanged && (alt = QMessageBox::question(this, tr("Save current | PIF IDE"),
                                     tr("The current project has unsaved changed. Would you like to save them"
                                        " before continuing?"), QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
                        == QMessageBox::Cancel) return;

    // WARNING: It is not checking if file has been saved!
    if (alt > 0 && alt == QMessageBox::Yes) saveFile();

    if (currentFile.isOpen()) currentFile.close();

    hasChanged = false;
    disconnect(ui->sourceEditor, SIGNAL(textChanged()), this, SLOT(sourceChanged()));
    this->setWindowTitle(tr("PIF IDE"));
    ui->sourceEditor->clear();
    connect(ui->sourceEditor, SIGNAL(textChanged()), this, SLOT(sourceChanged()));
}

bool EditorWindow::compileProject(){
    //WARNING: Has to check if saved

    ui->btAbort->setEnabled(true);
    ui->btCompileNRun->setEnabled(false);

    //TODO: Implement

    //TODO: Show console on error
    return true;
}

void EditorWindow::runProject(){
    //WARNING: Has to check for output

    ui->btAbort->setEnabled(true);
    ui->btCompileNRun->setEnabled(false);
    if (!ui->btConsoleView->isChecked()) ui->btConsoleView->toggle();

    //TODO: Implement
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
    ui->cOut->append(QString("%1").arg(ui->cIn->text()));
    ui->cIn->clear();
}

void EditorWindow::buildNRun(){
    if (compileProject())
        runProject();
}

void EditorWindow::abortProcess(){
    ui->btAbort->setEnabled(false);
    ui->btCompileNRun->setEnabled(true);

    // TODO: Implement
}
