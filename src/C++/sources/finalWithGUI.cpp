#include <QApplication>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include <QThread>
#include <QMessageBox>
#include "mainWindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(new QLabel("Results:"));

    results = new QTextEdit;
    layout->addWidget(results);

    progressBar = new QProgressBar;
    layout->addWidget(progressBar);

    QPushButton* button = new QPushButton("Choose Files");
    connect(button, &QPushButton::clicked, this, &MainWindow::chooseFiles);

    layout->addWidget(button);

    QWidget* centralWidget = new QWidget;
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    analyzer = new FileAnalyzer;
    analyzer->moveToThread(&analysisThread);

    connect(&analysisThread, &QThread::finished, analyzer, &QObject::deleteLater);
    connect(this, &MainWindow::startAnalysis, analyzer, &FileAnalyzer::analyzeFiles);
    connect(analyzer, &FileAnalyzer::analysisFinished, this, &MainWindow::onAnalysisFinished);

    analysisThread.start();
}

MainWindow::~MainWindow()
{
    analysisThread.quit();
    analysisThread.wait();
}

void MainWindow::chooseFiles()
{
    QStringList files = QFileDialog::getOpenFileNames(this, "Select one or more files to open");
    if (files.isEmpty()) {
        QMessageBox::warning(this, "Warning", "No files selected");
        return;
    }

    progressBar->setRange(0, 0);
    emit startAnalysis(files);
}

void MainWindow::onAnalysisFinished(const std::unordered_map<std::string, DirectoryStats*>& statsMap) {
    progressBar->setRange(0, 1);

    // Process the statsMap
    uint64_t totalSize = 0;
    uint64_t largestFileSize = 0;
    std::string largestFilePath;
    std::string mostCommonFileType;
    size_t mostCommonFileTypeCount = 0;
    std::string leastCommonFileType;
    size_t leastCommonFileTypeCount = SIZE_MAX;
    size_t totalDirectories = 0;

    for (const auto& pair : statsMap) {
        DirectoryStats* stats = pair.second;
        totalSize += stats->total_size;
        totalDirectories += stats->directory_count;

        if (stats->largest_file && stats->largest_file->size > largestFileSize) {
            largestFileSize = stats->largest_file->size;
            largestFilePath = stats->largest_file->path;
        }

        if (stats->most_common_type && stats->most_common_type->count > mostCommonFileTypeCount) {
            mostCommonFileTypeCount = stats->most_common_type->count;
            mostCommonFileType = stats->most_common_type->type;
        }

        if (stats->least_common_type && stats->least_common_type->count < leastCommonFileTypeCount) {
            leastCommonFileTypeCount = stats->least_common_type->count;
            leastCommonFileType = stats->least_common_type->type;
        }

        // Update the results text edit with the analysis results
        QString resultsText;
        resultsText.append("Total size: " + QString::number(totalSize) + "\n");
        resultsText.append("Total directories: " + QString::number(totalDirectories) + "\n");
        resultsText.append("Largest file: " + QString::fromStdString(largestFilePath) + " (" + QString::number(largestFileSize) + " bytes)\n");
        resultsText.append("Most common file type: " + QString::fromStdString(mostCommonFileType) + " (" + QString::number(mostCommonFileTypeCount) + " occurrences)\n");
        resultsText.append("Least common file type: " + QString::fromStdString(leastCommonFileType) + " (" + QString::number(leastCommonFileTypeCount) + " occurrences)\n");
        resultsText.append("Path: " + QString::fromStdString(pair.first) + "\n");
        resultsText.append("Total size: " + QString::number(stats->total_size) + "\n");
        resultsText.append("Directory count: " + QString::number(stats->directory_count) + "\n");
        results->setText(resultsText);
        // Free the memory allocated by analyze_directory
        free_directory_stats(stats);
    }
}


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}
