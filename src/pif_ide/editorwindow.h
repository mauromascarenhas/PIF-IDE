#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QDir>
#include <QList>
#include <QFile>
#include <QEvent>
#include <QScreen>
#include <QLocale>
#include <QVector>
#include <QProcess>
#include <QShortcut>
#include <QFileInfo>
#include <QSettings>
#include <QCheckBox>
#include <QCompleter>
#include <QTranslator>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QMainWindow>
#include <QInputDialog>
#include <QKeySequence>
#include <QStandardPaths>

#include "pifeditor.h"
#include "nmainwindow.h"
#include "aboutwindow.h"
#include "globalsettings.h"
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

    enum Executor{
        NONE,
        C,
        CPP,
        JAVA
    } curExec;

    bool hasChanged;

    QString pifcPath;
    QString cppPath;
    QString cPath;
    QString javaPath;
    QString javacPath;

    QString inputColour;
    QString outputColour;

    QFile currentFile;
    QProcess *buildProcess;
    QProcess *compileProcess;
    QProcess *executeProcess;
    QList<QShortcut *> shortCuts;

    Highlighter *highlighter;

    AboutWindow *frmAbout;
    SettingsWindow *frmSettings;

    QTranslator translator;
    QTranslator qtTranslator;

    void compileProject();
    void compileObject();
    void runProject();

    void setupEditor();
    void setupEnvVars();
    void createShortcuts();

    void changeLanguage(const QString &slug);
    void changeTranslator(QTranslator &translator, const QString &filePath);

protected:
    void changeEvent(QEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void openAboutForm();
    void openSettingsForm();

    void sourceChanged();

    void openFile();
    bool saveFile();
    bool saveFileAs();
    void newFile();

    void increaseFontSize();
    void reduceFontSize();

    void sendUserInput();

    void changeExec();

    void buildNRun();
    void abortProcess();

    void builderError();
    void builderOutput();
    void builderExited(int exitCode, QProcess::ExitStatus status);

    void compilerError();
    void compilerOutput();
    void compilerExited(int exitCode, QProcess::ExitStatus status);

    void executionError();
    void executionOutput();
    void executionExited(int exitCode, QProcess::ExitStatus status);
};

#endif // EDITORWINDOW_H
