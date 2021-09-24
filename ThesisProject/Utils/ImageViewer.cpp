#include "ImageViewer.h"

#include <QApplication>
#include <QClipboard>
#include <QColorSpace>
#include <QDir>
#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QMimeData>
#include <QPainter>
#include <QScreen>
#include <QScrollArea>
#include <QScrollBar>
#include <QStandardPaths>
#include <QStatusBar>  

bool VideoSegmentationUtils::ImageViewer::isLoaded()
{
    return m_loaded;
}

QImage VideoSegmentationUtils::ImageViewer::getImage()
{
    return m_image;
}

void VideoSegmentationUtils::ImageViewer::initialize(QLabel* label)
{
    m_scaleFactor = 1.0;
    m_loaded = false;
   
    m_label = label;
    

    //m_label->setBackgroundRole(QPalette::Base);
    m_label->setScaledContents(true);
    m_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    /*m_scrollArea.setWidget(m_label);
    m_scrollArea.setVisible(false);*/
}

std::string VideoSegmentationUtils::ImageViewer::getFileName()
{
    return m_fileName;
}

std::string VideoSegmentationUtils::ImageViewer::getFileDir()
{
    return m_fileDir;
}

void VideoSegmentationUtils::ImageViewer::setImage(const QImage& newImage)
{
    m_image = newImage;
    if (m_image.colorSpace().isValid())
    {
        m_image.convertToColorSpace(QColorSpace::SRgb);
    }
    
    m_label->setPixmap(QPixmap::fromImage(m_image).scaled(m_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_scaleFactor = 1.0;

}

bool VideoSegmentationUtils::ImageViewer::loadFile(const QString& fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull())
    {
        return false;
    }
    setImage(newImage);

    m_loaded = true;
    m_fileName = fileName.toUtf8().constData();

    // strip the file extension
    m_fileName = m_fileName.substr(0, m_fileName.size() - 4); 
    m_fileDir = m_fileName;
    size_t found = m_fileName.find_last_of("/"); // find last directory
    m_fileDir = m_fileDir.substr(0, found) + "/"; // extract file location
    m_fileName = m_fileName.substr(found + 1); // delete ending slash
    
    return true;
}
