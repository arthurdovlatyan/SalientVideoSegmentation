#include "MainWindow.h"
#include <QFileDialog>
#include <qerrormessage.h>

#include "Utils/ImageTools.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    
}

void MainWindow::initialize()
{
    m_orig_image.initialize(ui.label_orig_image);
    m_image.initialize(ui.label_image);

    connect(ui.pushButton_load_image, SIGNAL(clicked()), this, SLOT(on_push_button_clicked_load_image()));
    connect(ui.pushButton_undo, SIGNAL(clicked()), this, SLOT(on_push_button_clicked_undo()));

    connect(ui.pushButton_image_apply_Superpixels, SIGNAL(clicked()), this, SLOT(on_push_button_clicked_applySegmentation()));
    connect(ui.pushButton_image_color_superpixels, SIGNAL(clicked()), this, SLOT(on_push_button_clicked_colorSegments()));
    connect(ui.pushButton_image_ComputeUniqueness, SIGNAL(clicked()), this, SLOT(on_push_button_clicked_computeUniqueness()));
}

void MainWindow::on_push_button_clicked_undo()
{
    // TODO Refine the logic of the undo button
    if (!m_imageLib.empty())
    {
        QString filename = m_imageLib.top();
        m_imageLib.pop();
        loadImageFromPath(filename);
    }
    else
    {
        QMessageBox messageBox;
        messageBox.critical(0, "Error", "No images to display!");
    }
}

void MainWindow::on_push_button_clicked_applySegmentation()
{
    if (ui.comboBox_image_superpixel_variant->currentIndex() == 0)
    {
        m_detector.applySLIC();
    }
    
    
    m_current_image = m_detector.getResult().m_result;
    m_image.loadFromMat(m_current_image);
}

void MainWindow::on_push_button_clicked_colorSegments()
{
    m_current_image = m_detector.colorSupepixels();
    m_image.loadFromMat(m_current_image);
}

void MainWindow::on_push_button_clicked_computeUniqueness()
{
    m_current_image = m_detector.applySaliency();
    m_image.loadFromMat(m_current_image);
}

void MainWindow::showSegmentationResult()
{
    SalientDetector::SuperpixelAlgorithmResult a = m_detector.getResult();
    cv::imshow("mask", a.m_mask);
    cv::imshow("labels", a.m_labels);
    cv::imshow("result", a.m_result);
}

void MainWindow::loadImageFromPath(const QString& path)
{
    
    if (path.isNull() || path.isEmpty())
    {
        return;
    }
    m_current_image = cv::imread(path.toStdString());
    m_orig_image.loadFile(path);
    m_image.loadFile(path);
    m_detector.setMainImage(m_current_image);
}

void MainWindow::loadImage2_label_image(cv::Mat& img)
{
    m_image.loadFromMat(img);
}

void MainWindow::on_push_button_clicked_load_image()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select picture file", QString::fromStdString("C:\\Users\\dovla\\"),
        tr("Images (*.jpg *.jpeg *.jp2 *.png *.tif *.tiff)"));
    
    if (filename.isNull() || filename.isEmpty())
    {
        return;
    }

    loadImageFromPath(filename);

    if (m_currentImagePath != nullptr)
    {
        m_imageLib.push(m_currentImagePath);
    }
    m_currentImagePath = filename;
    
    ui.label_imge_width->setText(QString::number(m_current_image.size().width));
    ui.label_image_height->setText(QString::number(m_current_image.size().height));
}
