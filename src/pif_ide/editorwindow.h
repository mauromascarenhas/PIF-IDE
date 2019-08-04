#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QDir>
#include <QList>
#include <QFile>
#include <QProcess>
#include <QShortcut>
#include <QFileInfo>
#include <QSettings>
#include <QCheckBox>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QMainWindow>
#include <QKeySequence>
#include <QStandardPaths>

#include "nmainwindow.h"
#include "aboutwindow.h"
#include "settingswindow.h"
#include "highlighter/highlighter.h"

namespace Ui {
class EditorWindow;
}

class EditorWindow : public NMainWindow
{
    Q_OBJECT

public:
    explicit EditorWindow(QWidget *parent = nullptr);
    ~EditorWindow();

private:
    Ui::EditorWindow *ui;

private:
    bool compileProject();
    void runProject();

    void setupEditor();
    void setupEnvVars();
    void createShortcuts();

    bool hasChanged;

    QString pifcPath;
    QString cppPath;
    QString cPath;
    QString javaPath;
    QString javacPath;

    QFile currentFile;
    QProcess buildProcess;
    QProcess executeProcess;
    QList<QShortcut *> shortCuts;

    Highlighter *highlighter;

    AboutWindow *frmAbout;
    SettingsWindow *frmSettings;

private slots:
    void sourceChanged();

    void openFile();
    bool saveFile();
    void newFile();

    void increaseFontSize();
    void reduceFontSize();

    void sendUserInput();

    void buildNRun();
    void abortProcess();
};

#endif // EDITORWINDOW_H
