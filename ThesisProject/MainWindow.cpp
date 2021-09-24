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

void MainWindow::loadImageFromPath(const QString& path)
{
    
    if (path.isNull() || path.isEmpty())
    {
        return;
    }
    m_current_image = cv::imread(path.toStdString());
    m_orig_image.loadFile(path);
    m_image.loadFile(path);
    
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
    
}
