#include "MainWindow.h"
#include <QFileDialog>
#include <qerrormessage.h>
#include "MBS.h"
#include "MBSSaliency.h"
#include "Utils/ImageTools.h"
#include <QMessageBox>
#include <fstream>

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
    connect(ui.pushButton_cimputeSaliencyGPU, SIGNAL(clicked()), this, SLOT(on_push_button_clicked_computeSuperpixelsGPU()));
    connect(ui.pushButton_image_apply_Superpixels, SIGNAL(clicked()), this, SLOT(on_push_button_clicked_applySegmentation()));
    connect(ui.pushButton_image_color_superpixels, SIGNAL(clicked()), this, SLOT(on_push_button_clicked_colorSegments()));
    connect(ui.pushButton_image_ComputeUniqueness, SIGNAL(clicked()), this, SLOT(on_push_button_clicked_computeUniqueness()));
    connect(ui.pushButton_MBS, SIGNAL(clicked()), this, SLOT(on_push_button_clicked_MBS()));
    connect(ui.pushButton_computeWholeDir, SIGNAL(clicked()), this, SLOT(on_push_button_compute_whole_dir()));
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

void MainWindow::on_push_button_clicked_computeSuperpixelsGPU()
{
    if (ui.comboBox_image_superpixel_variant->currentIndex() == 0)
    {
        m_detector.applySLIC(true);
    }


    m_current_image = m_detector.getResult().m_result;
    m_image.loadFromMat(m_current_image);
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

void MainWindow::on_push_button_clicked_MBS()
{
    using namespace cv;
    Mat im = m_current_image;
    Mat MBmap, morphmat, smallmat, smallres, res;
    int radius(0);
    Scalar s;
    double start, t1, t2, t3, t4;
    double scale = 200.0 / ((im.rows > im.cols) ? im.rows : im.cols);
    cv::resize(im, smallmat, cv::Size(im.cols * scale, im.rows * scale));


    start = clock();
    MBmap = doWork(smallmat, true, true, false);
    t1 = clock();
    s = mean(MBmap);
    radius = floor(50 * sqrt(s.val[0]));
    radius = (radius > 3) ? radius : 3;
    morphmat = morpySmooth(MBmap, radius);
    smallres = enhanceConstrast(morphmat);


    t4 = clock();
    cv::resize(smallres, res, cv::Size(im.cols, im.rows));

    if (m_analyze == true) {
        //cout << "computing MBD map:  " << (t1 - start) / CLOCKS_PER_SEC << " s" << endl;
        //cout << "postprocessing :  " << (t4 - t1) / CLOCKS_PER_SEC << " s" << endl;
        //cout << "total :  " << (t4 - start) / CLOCKS_PER_SEC << " s" << endl;
    }
    
    m_current_image = res; 
    imwrite("../result.png", res);
    m_image.loadFile("../result.png");
    return;
}

void MainWindow::on_push_button_clicked_load_image()
{
    QString filename = QFileDialog::getOpenFileName(this, "Select picture file", QString::fromStdString("C:\\Users\\dovla\\Documents\\Personal\\AUA\\Thesis\\Evaluation\\GrabCut\\data_GT"),
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

void MainWindow::on_push_button_compute_whole_dir()
{
    QDir directory("C:\\Users\\dovla\\Documents\\Personal\\AUA\\Thesis\\Evaluation\\GrabCut\\data_GT");
    QStringList images = directory.entryList(QStringList() << "*.jpg" << "*.JPG" <<"*.bmp", QDir::Files);
    ofstream MyFile("results.txt");
    foreach(QString filename, images) {
        m_currentImagePath = "C:\\Users\\dovla\\Documents\\Personal\\AUA\\Thesis\\Evaluation\\GrabCut\\data_GT\\" + filename;
        loadImageFromPath(m_currentImagePath);
        using namespace cv;
        Mat im = m_current_image;
        Mat MBmap, morphmat, smallmat, smallres, res;
        int radius(0);
        Scalar s;
        double start, t1, t2, t3, t4;
        double scale = 200.0 / ((im.rows > im.cols) ? im.rows : im.cols);
        cv::resize(im, smallmat, cv::Size(im.cols * scale, im.rows * scale));
        start = clock();
        MBmap = doWork(smallmat, true, true, false);
        t1 = clock();
        s = mean(MBmap);
        radius = floor(50 * sqrt(s.val[0]));
        radius = (radius > 3) ? radius : 3;
        morphmat = morpySmooth(MBmap, radius);
        smallres = enhanceConstrast(morphmat);


        t4 = clock();
        cv::resize(smallres, res, cv::Size(im.cols, im.rows));
        
        if (m_analyze == true) {
            MyFile << "computing MBD map:  " << (t1 - start) / CLOCKS_PER_SEC << " s" << endl;
            MyFile << "postprocessing :  " << (t4 - t1) / CLOCKS_PER_SEC << " s" << endl;
            MyFile << "total :  " << (t4 - start) / CLOCKS_PER_SEC << " s" << endl;
        }
        //imwrite("../result.png", res);
        //cout << "The result has been written into result.png..." << endl;
        m_current_image = res;
        cv::String newFileName = filename.toStdString();
        imwrite(newFileName, res);
    }
    MyFile.close();
}

