#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <QDir>
#include <QList>
#include <QFile>
#include <QProcess>
#include <QShortcut>
#include <QFileInfo>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QMainWindow>
#include <QKeySequence>

#include "nmainwindow.h"
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
    void createShortcuts();

    bool hasChanged;

    QFile currentFile;
    QProcess buildProcess;
    QProcess executeProcess;
    QList<QShortcut *> shortCuts;

    Highlighter *highlighter;

private slots:
    void sourceChanged();

    void openFile();
    void saveFile();
    void newFile();

    void increaseFontSize();
    void reduceFontSize();

    void sendUserInput();

    void buildNRun();
    void abortProcess();
};

#endif // EDITORWINDOW_H
