<<<<<<< Updated upstream
//
//#include "../../../UsefullLibs/FreeImage/Source/FreeImage.h"
//#include "Image.h"
//
//FIBITMAP*
//convertRGBFtoRGB(const Image& img)
//{
//    FIBITMAP* rgb = FreeImage_Allocate(img.size().width, img.size().height, 24);
//
//    for (int i = 0; i < img.size().height; i++) {
//        const float* rowf = img.scanLine(img.size().height - 1 - i);
//        BYTE* row = (BYTE*)FreeImage_GetScanLine(rgb, i);
//        for (int j = 0; j < img.size().width; j++, rowf += 3, row += 3) {
//            row[FI_RGBA_RED] = rowf[0] * 255.0;
//            row[FI_RGBA_GREEN] = rowf[1] * 255.0;
//            row[FI_RGBA_BLUE] = rowf[2] * 255.0;
//        }
//    }
//
//    return rgb;
//}
//
//Image::Image(const std::string& fname) :
//    _img(NULL),
//    _size(0, 0)
//{
//    int flag = 0;
//    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
//
//    // check the file signature and deduce its format
//    // (the second argument is currently not used by FreeImage)
//    fif = FreeImage_GetFileType(fname.c_str(), 0);
//    if (fif == FIF_UNKNOWN) {
//        // no signature ?
//        // try to guess the file format from the file extension
//        fif = FreeImage_GetFIFFromFilename(fname.c_str());
//    }
//
//    // check that the plugin has reading capabilities ...
//    if ((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
//        FIBITMAP* tmp8 = FreeImage_Load(fif, fname.c_str(), flag);
//        FIBITMAP* tmp32 = FreeImage_ConvertToRGBF(tmp8);
//        FreeImage_Unload(tmp8);
//
//        _size.width = FreeImage_GetWidth(tmp32);
//        _size.height = FreeImage_GetHeight(tmp32);
//        _stride = FreeImage_GetPitch(tmp32);
//        _nChannels = FreeImage_GetLine(tmp32) / (_size.width * sizeof(float));
//
//        _img = new char[_stride * _size.height];
//
//        for (int i = 0; i < _size.height; i++) {
//            memcpy(_img + i * _stride, FreeImage_GetScanLine(tmp32, _size.height - 1 - i), _stride);
//        }
//
//        FreeImage_Unload(tmp32);
//    }
//}
//
//Image::Image(const cv::Size& size) :
//    _size(size)
//{
//    _nChannels = 3;
//    _stride = size.width * sizeof(float) * _nChannels;
//    _img = new char[_stride * _size.height];
//}
//
//Image::Image(const Image& other) :
//    _img(NULL)
//{
//    _size = other._size;
//    _stride = other._stride;
//    _nChannels = other._nChannels;
//
//    _img = new char[_stride * _size.height];
//
//    memcpy(other._img, _img, _size.height * _stride);
//}
//
//Image::~Image()
//{
//    if (_img) delete[] _img;
//}
//
//bool
//Image::writeToFile(const std::string& fname) const
//{
//    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
//    BOOL status = FALSE;
//
//    int flag = 0;
//    if (_img) {
//        // try to guess the file format from the file extension
//        fif = FreeImage_GetFIFFromFilename(fname.c_str());
//        if (fif != FIF_UNKNOWN) {
//            // FIBITMAP *tmp = FreeImage_ConvertTo24Bits(_data);
//            FIBITMAP* tmp = convertRGBFtoRGB(*this);
//
//            // check that the plugin has sufficient writing and export capabilities ...
//            WORD bpp = FreeImage_GetBPP(tmp);
//            if (FreeImage_FIFSupportsWriting(fif) && FreeImage_FIFSupportsExportBPP(fif, bpp)) {
//                status = FreeImage_Save(fif, tmp, fname.c_str(), flag);
//            }
//
//            FreeImage_Unload(tmp);
//        }
//    }
//
//    return (status == TRUE);
//}
//
=======


#include "Image.h"

FIBITMAP*
convertRGBFtoRGB(const Image& img)
{
    FIBITMAP* rgb = FreeImage_Allocate(img.size().width, img.size().height, 24);

    for (int i = 0; i < img.size().height; i++) {
        const float* rowf = img.scanLine(img.size().height - 1 - i);
        BYTE* row = (BYTE*)FreeImage_GetScanLine(rgb, i);
        for (int j = 0; j < img.size().width; j++, rowf += 3, row += 3) {
            row[FI_RGBA_RED] = rowf[0] * 255.0;
            row[FI_RGBA_GREEN] = rowf[1] * 255.0;
            row[FI_RGBA_BLUE] = rowf[2] * 255.0;
        }
    }

    return rgb;
}

Image::Image(const std::string& fname) :
    _img(NULL),
    _size(0, 0)
{
    int flag = 0;
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

    // check the file signature and deduce its format
    // (the second argument is currently not used by FreeImage)
    fif = FreeImage_GetFileType(fname.c_str(), 0);
    if (fif == FIF_UNKNOWN) {
        // no signature ?
        // try to guess the file format from the file extension
        fif = FreeImage_GetFIFFromFilename(fname.c_str());
    }

    // check that the plugin has reading capabilities ...
    if ((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
        FIBITMAP* tmp8 = FreeImage_Load(fif, fname.c_str(), flag);
        FIBITMAP* tmp32 = FreeImage_ConvertToRGBF(tmp8);
        FreeImage_Unload(tmp8);

        _size.width = FreeImage_GetWidth(tmp32);
        _size.height = FreeImage_GetHeight(tmp32);
        _stride = FreeImage_GetPitch(tmp32);
        _nChannels = FreeImage_GetLine(tmp32) / (_size.width * sizeof(float));

        _img = new char[_stride * _size.height];

        for (int i = 0; i < _size.height; i++) {
            memcpy(_img + i * _stride, FreeImage_GetScanLine(tmp32, _size.height - 1 - i), _stride);
        }

        FreeImage_Unload(tmp32);
    }
}

Image::Image(const Size& size) :
    _size(size)
{
    _nChannels = 3;
    _stride = size.width * sizeof(float) * _nChannels;
    _img = new char[_stride * _size.height];
}

Image::Image(const Image& other) :
    _img(NULL)
{
    _size = other._size;
    _stride = other._stride;
    _nChannels = other._nChannels;

    _img = new char[_stride * _size.height];

    memcpy(other._img, _img, _size.height * _stride);
}

Image::~Image()
{
    if (_img) delete[] _img;
}

bool
Image::writeToFile(const std::string& fname) const
{
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    BOOL status = FALSE;

    int flag = 0;
    if (_img) {
        // try to guess the file format from the file extension
        fif = FreeImage_GetFIFFromFilename(fname.c_str());
        if (fif != FIF_UNKNOWN) {
            // FIBITMAP *tmp = FreeImage_ConvertTo24Bits(_data);
            FIBITMAP* tmp = convertRGBFtoRGB(*this);

            // check that the plugin has sufficient writing and export capabilities ...
            WORD bpp = FreeImage_GetBPP(tmp);
            if (FreeImage_FIFSupportsWriting(fif) && FreeImage_FIFSupportsExportBPP(fif, bpp)) {
                status = FreeImage_Save(fif, tmp, fname.c_str(), flag);
            }

            FreeImage_Unload(tmp);
        }
    }

    return (status == TRUE);
}



void
writePgm(const std::string& outFName, float* data, const Size& size)
{
    //int tmp[size.width * size.height];
    std::vector<int> tmp(size.width * size.height);
    float maxVal = data[0], minVal = data[0];
    for (int i = 1; i < size.width * size.height; i++) {
        minVal = (minVal > data[i]) ? data[i] : minVal;
        maxVal = (maxVal < data[i]) ? data[i] : maxVal;
    }

    float range = maxVal - minVal;

    for (int i = 0; i < size.width * size.height; i++) {
        tmp[i] = 255.0 * (data[i] - minVal) / range;
    }

    writePgm(outFName, tmp.data(), size);
}

void
writePgm(const std::string& outFName, int* data, const Size& size)
{
    std::ofstream f(outFName.c_str());

    int maxVal = data[0];
    for (int i = 1; i < size.width * size.height; i++) {
        maxVal = (maxVal > data[i]) ? maxVal : data[i];
    }

    f << "P2\n"
        << size.width << " " << size.height << "\n"
        << maxVal << "\n";

    for (int i = 0, k = 0; i < size.height; i++) {
        std::string sep = "";
        for (int j = 0; j < size.width; j++, k++) {
            f << sep << data[k];
            sep = " ";
        }
        f << "\n";
    }
}

Size superPixelGridSize(const Size& imageSize, int superPixelSpacing)
{
    return Size(imageSize.width / superPixelSpacing, imageSize.height / superPixelSpacing);
}

static
const char* kernelSources =
"void                                                                    \n" \
"matmul(const float m[3][3], const float v[3], float w[3])               \n" \
"{                                                                       \n" \
"    for(int i = 0; i < 3; i++) {                                        \n" \
"        w[i] = 0.0;                                                     \n" \
"        for(int j = 0; j < 3; j++) {                                    \n" \
"            w[i] += m[i][j] * v[j];                                     \n" \
"        }                                                               \n" \
"    }                                                                   \n" \
"}                                                                       \n" \
"                                                                        \n" \
"__kernel void rgb2lab(                                                  \n" \
"    __global float *rgbIm,                                              \n" \
"    __global float *labIm,                                              \n" \
"    const int width, const int height, const int stride)                \n" \
"{                                                                       \n" \
"    // rgb to xyz transform                                             \n" \
"    const float m[3][3] = {                                             \n" \
"        {0.412453, 0.357580, 0.180423},                                 \n" \
"        {0.212671, 0.715160, 0.072169},                                 \n" \
"        {0.019334, 0.119193, 0.950227}                                  \n" \
"    };                                                                  \n" \
"                                                                        \n" \
"    int i = get_global_id(0);                                           \n" \
"    int j = get_global_id(1);                                           \n" \
"                                                                        \n" \
"    if(i < height && j < width) {                                       \n" \
"        int idx = (stride / 4) * i + 3 * j;                             \n" \
"        float rgb[3] = {rgbIm[idx], rgbIm[idx + 1], rgbIm[idx + 2]};    \n" \
"                                                                        \n" \
"        float xyz[3];                                                   \n" \
"        matmul(m, rgb, xyz);                                            \n" \
"                                                                        \n" \
"        xyz[0] /= 0.950456;                                             \n" \
"        xyz[2] /= 1.088754;                                             \n" \
"                                                                        \n" \
"        float thresh = 0.008856;                                        \n" \
"        int xyzThresh[3];                                               \n" \
"        for(int ii = 0; ii < 3; ii++) xyzThresh[ii] = xyz[ii] > thresh; \n" \
"                                                                        \n" \
"        float xyzF[3];                                                  \n" \
"        for(int ii = 0; ii < 3; ii++) {                                 \n" \
"            if(xyzThresh[ii]) xyzF[ii] =  pow(xyz[ii], (float)(1.0f / 3.0f));\n" \
"            else xyzF[ii] = 7.787 * xyz[ii] + 16.0 / 116.0;             \n" \
"        }                                                               \n" \
"                                                                        \n" \
"        float Lab[3];                                                   \n" \
"        if(xyzThresh[1]) Lab[0] = ((float)116.0 * pow(xyz[1], (float) (1.0f / 3.0f)) - 16.0); \n" \
"        else Lab[1] = 903.3 * xyz[1];                                   \n" \
"        Lab[1] = 500 * (xyzF[0] - xyzF[1]);                             \n" \
"        Lab[2] = 200 * (xyzF[1] - xyzF[2]);                             \n" \
"                                                                        \n" \
"        for(int ii = 0; ii < 3; ii++) {                                 \n" \
"            labIm[idx + ii] = Lab[ii];                                  \n" \
"        }                                                               \n" \
"    }                                                                   \n" \
"}                                                                       \n" \
"\n";

void
rgb2lab(GPUAcceleration::OpenCL& opencl, const Size& size, int stride, GPUAcceleration::Memory& rgb, GPUAcceleration::Memory& lab)
{
    GPUAcceleration::Kernel kernel(opencl, kernelSources, "rgb2lab");

    kernel.setArgument(0, &rgb.getMemory());
    kernel.setArgument(1, &lab.getMemory());
    kernel.setArgument(2, &size.width);
    kernel.setArgument(3, &size.height);
    kernel.setArgument(4, &stride);

    std::vector<size_t> tmpSize(2);
    tmpSize[0] = size.height;
    tmpSize[1] = size.width;
    executeKernel(opencl, kernel, tmpSize);
}
>>>>>>> Stashed changes
