#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QProgressBar>
#include <QThread>
#include "fileAnalyzer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

    ~MainWindow();

private slots:
    void chooseFiles();
    void onAnalysisFinished(const std::unordered_map<std::string, DirectoryStats*>& statsMap);

signals:
    void startAnalysis(const QStringList& files);
    void analysisFinished(const std::unordered_map<std::string, DirectoryStats*>& statsMap);

private:
    QThread analysisThread;
    FileAnalyzer* analyzer;
    QTextEdit* results;
    QProgressBar* progressBar;
    std::unordered_map<std::string, DirectoryStats*> statsMap;  // Add this line
};
#endif // MAINWINDOW_H
