#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

#include "Utils/ImageViewer.h"

#include <stack>
#include <opencv2/opencv.hpp>
#include <opencv2/ximgproc.hpp>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
    void initialize();

public slots:
    void on_push_button_clicked_load_image();
    void on_push_button_clicked_undo();

protected:
    void loadImageFromPath(const QString& path);

private:
    Ui::MainWindowClass ui;
    cv::Mat m_current_image;
    VideoSegmentationUtils::ImageViewer m_orig_image;
    VideoSegmentationUtils::ImageViewer m_image;
    std::stack<QString> m_imageLib;
    QString m_currentImagePath;
    
};
