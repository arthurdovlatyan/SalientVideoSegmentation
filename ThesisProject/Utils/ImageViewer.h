#pragma once

#include <string>

#include <qimage.h>
#include <qlabel.h>
#include <qscrollarea.h>

namespace VideoSegmentationUtils
{

class ImageViewer 
{
private:

    QImage m_image;
    QLabel* m_label;
    QScrollArea m_scrollArea;
    double m_scaleFactor;
    bool m_loaded = false;
    std::string m_fileDir;
    std::string m_fileName;

public:
    ImageViewer() = default;
    
    bool loadFile(const QString&);

    void initialize(QLabel* label);

    // getters
    bool isLoaded();
    QImage getImage();
    std::string getFileName();
    std::string getFileDir();

    //functionality
    /*void zoomIn();
    void zoomOut();
    void normalSize();*/


protected:
    void setImage(const QImage& newImage);

};

}