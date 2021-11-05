#include "Detector.h"



SalientDetector::SalientDetector::SalientDetector(cv::Mat img)
	: m_orig_image(img)
{
	
}

void SalientDetector::SalientDetector::setMainImage(cv::Mat& img)
{
	m_orig_image = img;
}

void SalientDetector::SalientDetector::setSuperpixelAbstraction()
{
	m_superpixelAlgorithm->getResult().copyTo(m_superpixelAbstaction.m_result);
	m_superpixelAlgorithm->getLabels().copyTo(m_superpixelAbstaction.m_labels);
	m_superpixelAlgorithm->getMask().copyTo(m_superpixelAbstaction.m_mask);
}

void SalientDetector::SalientDetector::applySLICO(int region_size, 
	int ruler, 
	int min_element_size, 
	int num_iterations)
{
	m_superpixelAlgorithm = std::make_unique<Superpixels::SLICO>(m_orig_image, region_size, ruler, min_element_size, num_iterations);
	
	m_superpixelAlgorithm->calculateSuperpixels();

	setSuperpixelAbstraction();
}

void SalientDetector::SalientDetector::applySEEDS(int num_iterations, 
	int prior,
	bool double_step, 
	int num_superpixels, 
	int num_levels, 
	int num_histogram_bins)
{
	m_superpixelAlgorithm = std::make_unique<Superpixels::SEEDS>(m_orig_image,
		num_iterations,
		prior,
		double_step,
		num_superpixels,
		num_levels,
		num_histogram_bins);

	m_superpixelAlgorithm->calculateSuperpixels();

	setSuperpixelAbstraction();
}

void SalientDetector::SalientDetector::applySLIC()
{
	SLIC = true;
	m_superpixelAlgorithm = std::make_unique<Superpixels::SLIC>(m_orig_image);

	m_superpixelAlgorithm->calculateSuperpixels();

	setSuperpixelAbstraction();
}

SalientDetector::SuperpixelAlgorithmResult SalientDetector::SalientDetector::getResult()
{
	return m_superpixelAbstaction;
}

cv::Mat SalientDetector::SalientDetector::colorSupepixels()
{
	if (SLIC == true)
	{
		return m_superpixelAlgorithm->colorSuperpixels();
	}

	int nx = m_orig_image.cols, ny = m_orig_image.rows;
    int n = nx * ny;
    std::vector<cv::Vec3b> avg_colors(n);
    std::vector<int> num_pixels(n);

    std::vector<long> b(n), g(n), r(n);

    for (int y = 0; y < (int)m_superpixelAlgorithm->getLabels().rows; ++y) {
        for (int x = 0; x < (int)m_superpixelAlgorithm->getLabels().cols; ++x) {

            cv::Vec3b pix = m_orig_image.at<cv::Vec3b>(y, x);
            int lbl = m_superpixelAlgorithm->getLabels().at<int>(y, x);

            b[lbl] += (int)pix[0];
            g[lbl] += (int)pix[1];
            r[lbl] += (int)pix[2];

            ++num_pixels[lbl];
        }
    }

    for (int i = 0; i < n; ++i) {
			int num = num_pixels[i];
			avg_colors[i] = cv::Vec3b(b[i] / num, g[i] / num, r[i] / num);
    }

    cv::Mat output = this->m_orig_image.clone();
    for (int y = 0; y < (int)output.rows; ++y) {
        for (int x = 0; x < (int)output.cols; ++x) {
            int lbl = m_superpixelAlgorithm->getLabels().at<int>(y, x);
            if (num_pixels[lbl])
                output.at<cv::Vec3b>(y, x) = avg_colors[lbl];
        }
    }

    return output;
}

cv::Mat SalientDetector::SalientDetector::applySaliency(bool useGPU)
{
	GPUAcceleration::OpenCL opencl(useGPU);

	GPUAcceleration::Memory clusterCenters(opencl, CL_MEM_READ_WRITE, sizeof(float) * m_imageColored.size().width * m_imageColored.size().height * 5);
	GPUAcceleration::Memory saliencySP(opencl, CL_MEM_READ_WRITE, sizeof(float) * m_imageColored.size().width * m_imageColored.size().height);
	GPUAcceleration::Memory saliency(opencl, CL_MEM_READ_WRITE, sizeof(float) * m_imageColored.size().width * m_imageColored.size().height);
	GPUAcceleration::Memory clusterAssig(opencl, CL_MEM_READ_WRITE, sizeof(int) * m_imageColored.size().width * m_imageColored.size().height);

	GPUAcceleration::Memory imgRGB(opencl, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, m_imageColored.size().width * m_imageColored.size().height, &m_imageColored.at<float>(0, 0));
	/*GPUAcceleration::Memory imgLab(opencl, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, img.stride() * img.size().height, img(0, 0));*/

	float stdDevUniqueness = 0.25;
	float stdDevDistribution = 2;
	int k = 6;

	saliencyFiltersSP(opencl, m_imageColored.size(), clusterCenters, saliencySP, stdDevUniqueness, stdDevDistribution, k);
	//propagateSaliency(opencl, img.size(), img.stride(), spSize, (useLab) ? imgLab : imgRGB, clusterAssig, saliencySP, saliency, alpha, beta);
	return m_imageColored;
}

void SalientDetector::SalientDetector::saliencyFiltersSP(GPUAcceleration::OpenCL& opencl, const cv::Size& gridSize, GPUAcceleration::Memory& clusterCenters, GPUAcceleration::Memory& saliencySP, float stdDevUniqueness, float stdDevDistribution, float k)
{
	GPUAcceleration::Memory uniqueness(opencl, CL_MEM_READ_WRITE, sizeof(float) * gridSize.width * gridSize.height);
	GPUAcceleration::Memory distribution(opencl, CL_MEM_READ_WRITE, sizeof(float) * gridSize.width * gridSize.height);

	elementUniqueness(opencl, gridSize, clusterCenters, uniqueness, stdDevUniqueness);
	//elementDistribution(opencl, gridSize, clusterCenters, distribution, stdDevDistribution);
	//elementSaliency(opencl, gridSize, uniqueness, distribution, saliencySP, k);
}

void SalientDetector::SalientDetector::propagateSaliency(GPUAcceleration::OpenCL& opencl, const cv::Size& imgSize, int imgStride, const cv::Size& gridSize, GPUAcceleration::Memory& img, GPUAcceleration::Memory& clusterAssig, GPUAcceleration::Memory& saliencySP, GPUAcceleration::Memory& saliency, float alpha, float beta)
{
	GPUAcceleration::Kernel kernel(opencl, kernelSources, "propagateSaliency");

	int supportSize = 3 * sqrt(1.0 / (2.0 * beta));

	kernel.setArgument(0, &img.getMemory());
	kernel.setArgument(1, &clusterAssig.getMemory());
	kernel.setArgument(2, &saliencySP.getMemory());
	kernel.setArgument(3, &saliency.getMemory());
	kernel.setArgument(4, &imgSize.width);
	kernel.setArgument(5, &imgSize.height);
	kernel.setArgument(6, &imgStride);
	kernel.setArgument(7, &gridSize.width);
	kernel.setArgument(8, &gridSize.height);
	kernel.setArgument(9, &supportSize);
	kernel.setArgument(10, &alpha);
	kernel.setArgument(11, &beta);

	std::vector<size_t> tmpSize(2);
	tmpSize[0] = imgSize.height;
	tmpSize[1] = imgSize.width;

	executeKernel(opencl, kernel, tmpSize);
}

void SalientDetector::SalientDetector::elementUniqueness(GPUAcceleration::OpenCL& opencl, const cv::Size& gridSize, GPUAcceleration::Memory& clusterCenters, GPUAcceleration::Memory& uniqueness, float stdDevUniqueness)
{
	GPUAcceleration::Kernel kernel(opencl, kernelSources, "elementUniqueness");

	float stdDevUniqueness2 = stdDevUniqueness * stdDevUniqueness;

	kernel.setArgument(0, &clusterCenters.getMemory());
	kernel.setArgument(1, &uniqueness.getMemory());
	kernel.setArgument(2, &gridSize.width);
	kernel.setArgument(3, &gridSize.height);
	kernel.setArgument(4, &stdDevUniqueness2);

	std::vector<size_t> tmpSize(2);
	tmpSize[0] = gridSize.height;
	tmpSize[1] = gridSize.width;

	executeKernel(opencl,kernel,tmpSize);
}

void SalientDetector::SalientDetector::elementDistribution(GPUAcceleration::OpenCL& opencl, const cv::Size& gridSize, GPUAcceleration::Memory& clusterCenters, GPUAcceleration::Memory& distribution, float stdDevDistribution)
{
	GPUAcceleration::Kernel kernel(opencl, kernelSources, "elementDistribution");

	float stdDevDistribution2 = stdDevDistribution * stdDevDistribution;

	kernel.setArgument(0, &clusterCenters.getMemory());
	kernel.setArgument(1, &distribution.getMemory());
	kernel.setArgument(2, &gridSize.width);
	kernel.setArgument(3, &gridSize.height);
	kernel.setArgument(4, &stdDevDistribution2);

	std::vector<size_t> tmpSize(2);
	tmpSize[0] = gridSize.height;
	tmpSize[1] = gridSize.width;

	executeKernel(opencl, kernel, tmpSize);
}

void SalientDetector::SalientDetector::elementSaliency(GPUAcceleration::OpenCL& opencl, const cv::Size& gridSize, GPUAcceleration::Memory& uniqueness, GPUAcceleration::Memory& distribution, GPUAcceleration::Memory& saliency, float k)
{
	GPUAcceleration::Kernel kernel(opencl, kernelSources, "elementSaliency");

	kernel.setArgument(0, &uniqueness.getMemory());
	kernel.setArgument(1, &distribution.getMemory());
	kernel.setArgument(2, &saliency.getMemory());
	kernel.setArgument(3, &gridSize.width);
	kernel.setArgument(4, &gridSize.height);
	kernel.setArgument(5, &k);

	std::vector<size_t> tmpSize(2);
	tmpSize[0] = gridSize.height;
	tmpSize[1] = gridSize.width;

	executeKernel(opencl, kernel, tmpSize);
}




float eucDist2(float* v1, float* v2, int dim) {                         
   float dist = 0.0;                                                   
   for(int i = 0; i < dim; i++) dist += pow(v1[i] - v2[i], 2.0f);       
   return dist;                                                        
}                                                                      
                                                                      
float gauss(float d2, float sigma2) {                                  
   return exp((float)(-d2 / (2.0f * sigma2)));                         
}                                                                     
                                                                        
                                                                       
void elementUniqueness(                                       
   float* clusterCenters,                                     
   float* uniqueness,                                         
   const int clustersWidth, const int clustersHeight,                   
   float stdDevUniqueness2)                                            
{                                                                      
   int i = 0;                                            
   int j = 0;                                           
                                                                       
   if(i < clustersHeight && j < clustersWidth) {                        
      int idx = (i * clustersWidth + j) * 5;                           
      float currColor[3] = {clusterCenters[idx], clusterCenters[idx + 1], clusterCenters[idx + 2]}; 
      float currPos[2] = {clusterCenters[idx], clusterCenters[idx + 1]};
                                                                        
      float uniq = 0.0;                                                 
      float sumWp = 0.0;                                                
      for(int ii = 0; ii < clustersHeight; ii++) {                      
         for(int jj = 0; jj < clustersWidth; jj++) {                   
            int idxOther = (ii * clustersWidth + jj) * 5;               
            float otherColor[3] = {clusterCenters[idxOther], clusterCenters[idxOther + 1], clusterCenters[idxOther + 2]}; 
            float otherPos[2] = {clusterCenters[idxOther], clusterCenters[idxOther + 1]}; 
            float wp = gauss(eucDist2(currPos, otherPos, 2), stdDevUniqueness2);
            sumWp += wp;                                                
            uniq += wp * eucDist2(currColor, otherColor, 3);            
         }                                                              
      }                                                                 
      uniqueness[i * clustersWidth + j] = uniq / sumWp;                 
   }                                                                    
}                                                                       
                                                                        
                                                                       
void elementDistribution(                                      
   float* clusterCenters,                                      
   float* distribution,                                        
   const int clustersWidth, const int clustersHeight,                   
   float stdDevDistribution2)                                           
{                                                                       
   int i = 0;                                           
   int j = 0;                                            
                                                                        
   if(i < clustersHeight && j < clustersWidth) {                        
      int idx = (i * clustersWidth + j) * 5;                            
      float currColor[3] = {clusterCenters[idx], clusterCenters[idx + 1], clusterCenters[idx + 2]}; 
      float currPos[2] = {clusterCenters[idx], clusterCenters[idx + 1]}; 
                                                                        
      float blurP[2] = {0.0, 0.0};                                      
      float blurP2 = 0;                                                 
      float sumWc = 0.0;                                               
      for(int ii = 0; ii < clustersHeight; ii++) {                     
         for(int jj = 0; jj < clustersWidth; jj++) {                   
            int idxOther = (ii * clustersWidth + jj) * 5;               
            float otherColor[3] = {clusterCenters[idxOther], clusterCenters[idxOther + 1], clusterCenters[idxOther + 2]}; 
            float otherPos[2] = {clusterCenters[idxOther], clusterCenters[idxOther + 1]};
                                                                        
            float wc = gauss(eucDist2(currColor, otherColor, 2), stdDevDistribution2); 
            sumWc += wc;                                                
            blurP2 += wc * (otherPos[0] * otherPos[0] + otherPos[1] * otherPos[1]);  
            blurP[0] += wc * otherPos[0];                               
            blurP[1] += wc * otherPos[1];                               
         }                                                              
      }                                                                 
                                                                        
      blurP2 /= sumWc;                                                  
      blurP[0] /= sumWc;                                                
      blurP[1] /= sumWc;                                                
                                                                        
      distribution[i * clustersWidth + j] = blurP2 + pow(blurP[0], 2.0f) + pow(blurP[1], 2.0f); 
   }                                                                    
}                                                                       
                                                                        
                                                                        
void elementSaliency(                                          
   const float* uniqueness,                                    
   const float* distribution,                                  
   float* saliencySP,                                          
   const int clustersWidth, const int clustersHeight,                   
   float k)                                                             
{                                                                       
   int i = get_global_id(0);                                            
   int j = get_global_id(1);                                            
                                                                        
   if(i < clustersHeight && j < clustersWidth) {                        
      int idx = (i * clustersWidth + j);                                
      saliencySP[idx] = uniqueness[idx] * exp((float) -k * distribution[idx]);  
   }                                                                    
}                                                                       
                                                                        
void propagateSaliency(                                        
   const float* img,                                           
   const int* clusterAssig,                                    
   const float* saliencySP,                                    
   float* saliency,                                            
   const int imWidth, const int imHeight,                               
   const int imStride,                                                  
   const int clustersWidth, const int clustersHeight, int supportSize,  
   float alpha, float beta)                                             
{                                                                       
   int i = get_global_id(0);                                            
   int j = get_global_id(1);                                            
                                                                        
   if(i < imHeight && j < imWidth) {                                    
      int idx = (imStride/4) * i + 3 * j;                               
      float currColor[3] = {img[idx], img[idx + 1], img[idx + 2]};      
      float currPos[2] = {i, j};                                        
                                                                        
      float totalW = 0.0;                                               
      float totalS = 0.0;                                               
                                                                        
      for(int ii = i - supportSize; ii <= i + supportSize; ii++) {      
         if(ii < 0 || ii >= imHeight) continue;                         
         for(int jj = j - supportSize; jj <= j + supportSize; jj++) {   
            if(jj < 0 || jj >= imWidth) continue;                       
                                                                        
            int neighSpIdx = clusterAssig[ii * imWidth + jj];           
            float neighSal = saliencySP[neighSpIdx];                    
                                                                        
            int neighIdx = (imStride/4) * ii + 3 * jj;                  
            float neighColor[3] = {img[neighIdx], img[neighIdx + 1], img[neighIdx + 2]}; 
            float neighPos[2] = {ii, jj};                               
                                                                        
            float w = exp((float)-0.5 * ( alpha * eucDist2(currColor, neighColor, 3) + beta * eucDist2(currPos, neighPos, 2))); 
            totalW += w;                                                
                                                                        
            totalS += neighSal * w;                                     
        }                                                              
      }                                                                 
      totalS /= totalW;                                                 
      saliency[i * imWidth + j] = totalS;                               
   }
}                                                                    

// GPU CODE
const char* SalientDetector::SalientDetector::kernelSources =
"float eucDist2(float* v1, float* v2, int dim) {                         \n" \
"   float dist = 0.0;                                                    \n" \
"   for(int i = 0; i < dim; i++) dist += pow(v1[i] - v2[i], 2.0f);       \n" \
"   return dist;                                                         \n" \
"}                                                                       \n" \
"                                                                        \n" \
"float gauss(float d2, float sigma2) {                                   \n" \
"   return exp((float)(-d2 / (2.0f * sigma2)));                          \n" \
"}                                                                       \n" \
"                                                                        \n" \
"                                                                        \n" \
"__kernel void elementUniqueness(                                        \n" \
"   __global float* clusterCenters,                                      \n" \
"   __global float* uniqueness,                                          \n" \
"   const int clustersWidth, const int clustersHeight,                   \n" \
"   float stdDevUniqueness2)                                             \n" \
"{                                                                       \n" \
"   int i = get_global_id(0);                                            \n" \
"   int j = get_global_id(1);                                            \n" \
"                                                                        \n" \
"   if(i < clustersHeight && j < clustersWidth) {                        \n" \
"      int idx = (i * clustersWidth + j) * 5;                            \n" \
"      float currColor[3] = {clusterCenters[idx], clusterCenters[idx + 1], clusterCenters[idx + 2]}; \n" \
"      float currPos[2] = {clusterCenters[idx], clusterCenters[idx + 1]}; \n" \
"                                                                        \n" \
"      float uniq = 0.0;                                                 \n" \
"      float sumWp = 0.0;                                                \n" \
"      for(int ii = 0; ii < clustersHeight; ii++) {                      \n" \
"         for(int jj = 0; jj < clustersWidth; jj++) {                    \n" \
"            int idxOther = (ii * clustersWidth + jj) * 5;               \n" \
"            float otherColor[3] = {clusterCenters[idxOther], clusterCenters[idxOther + 1], clusterCenters[idxOther + 2]}; \n" \
"            float otherPos[2] = {clusterCenters[idxOther], clusterCenters[idxOther + 1]}; \n" \
"            float wp = gauss(eucDist2(currPos, otherPos, 2), stdDevUniqueness2);\n" \
"            sumWp += wp;                                                \n" \
"            uniq += wp * eucDist2(currColor, otherColor, 3);            \n" \
"         }                                                              \n" \
"      }                                                                 \n" \
"      uniqueness[i * clustersWidth + j] = uniq / sumWp;                 \n" \
"   }                                                                    \n" \
"}                                                                       \n" \
"                                                                        \n" \
"                                                                        \n" \
"__kernel void elementDistribution(                                      \n" \
"   __global float* clusterCenters,                                      \n" \
"   __global float* distribution,                                        \n" \
"   const int clustersWidth, const int clustersHeight,                   \n" \
"   float stdDevDistribution2)                                           \n" \
"{                                                                       \n" \
"   int i = get_global_id(0);                                            \n" \
"   int j = get_global_id(1);                                            \n" \
"                                                                        \n" \
"   if(i < clustersHeight && j < clustersWidth) {                        \n" \
"      int idx = (i * clustersWidth + j) * 5;                            \n" \
"      float currColor[3] = {clusterCenters[idx], clusterCenters[idx + 1], clusterCenters[idx + 2]}; \n" \
"      float currPos[2] = {clusterCenters[idx], clusterCenters[idx + 1]}; \n" \
"                                                                        \n" \
"      float blurP[2] = {0.0, 0.0};                                      \n" \
"      float blurP2 = 0;                                                 \n" \
"      float sumWc = 0.0;                                                \n" \
"      for(int ii = 0; ii < clustersHeight; ii++) {                      \n" \
"         for(int jj = 0; jj < clustersWidth; jj++) {                    \n" \
"            int idxOther = (ii * clustersWidth + jj) * 5;               \n" \
"            float otherColor[3] = {clusterCenters[idxOther], clusterCenters[idxOther + 1], clusterCenters[idxOther + 2]}; \n" \
"            float otherPos[2] = {clusterCenters[idxOther], clusterCenters[idxOther + 1]}; \n" \
"                                                                        \n" \
"            float wc = gauss(eucDist2(currColor, otherColor, 2), stdDevDistribution2); \n" \
"            sumWc += wc;                                                \n" \
"            blurP2 += wc * (otherPos[0] * otherPos[0] + otherPos[1] * otherPos[1]);  \n" \
"            blurP[0] += wc * otherPos[0];                               \n" \
"            blurP[1] += wc * otherPos[1];                               \n" \
"         }                                                              \n" \
"      }                                                                 \n" \
"                                                                        \n" \
"      blurP2 /= sumWc;                                                  \n" \
"      blurP[0] /= sumWc;                                                \n" \
"      blurP[1] /= sumWc;                                                \n" \
"                                                                        \n" \
"      distribution[i * clustersWidth + j] = blurP2 + pow(blurP[0], 2.0f) + pow(blurP[1], 2.0f); \n" \
"   }                                                                    \n" \
"}                                                                       \n" \
"                                                                        \n" \
"                                                                        \n" \
"__kernel void elementSaliency(                                          \n" \
"   __global const float* uniqueness,                                    \n" \
"   __global const float* distribution,                                  \n" \
"   __global float* saliencySP,                                          \n" \
"   const int clustersWidth, const int clustersHeight,                   \n" \
"   float k)                                                             \n" \
"{                                                                       \n" \
"   int i = get_global_id(0);                                            \n" \
"   int j = get_global_id(1);                                            \n" \
"                                                                        \n" \
"   if(i < clustersHeight && j < clustersWidth) {                        \n" \
"      int idx = (i * clustersWidth + j);                                \n" \
"      saliencySP[idx] = uniqueness[idx] * exp((float) -k * distribution[idx]);  \n" \
"   }                                                                    \n" \
"}                                                                       \n" \
"                                                                        \n" \
"__kernel void propagateSaliency(                                        \n" \
"   __global const float* img,                                           \n" \
"   __global const int* clusterAssig,                                    \n" \
"   __global const float* saliencySP,                                    \n" \
"   __global float* saliency,                                            \n" \
"   const int imWidth, const int imHeight,                               \n" \
"   const int imStride,                                                  \n" \
"   const int clustersWidth, const int clustersHeight, int supportSize,  \n" \
"   float alpha, float beta)                                             \n" \
"{                                                                       \n" \
"   int i = get_global_id(0);                                            \n" \
"   int j = get_global_id(1);                                            \n" \
"                                                                        \n" \
"   if(i < imHeight && j < imWidth) {                                    \n" \
"      int idx = (imStride/4) * i + 3 * j;                               \n" \
"      float currColor[3] = {img[idx], img[idx + 1], img[idx + 2]};      \n" \
"      float currPos[2] = {i, j};                                        \n" \
"                                                                        \n" \
"      float totalW = 0.0;                                               \n" \
"      float totalS = 0.0;                                               \n" \
"                                                                        \n" \
"      for(int ii = i - supportSize; ii <= i + supportSize; ii++) {      \n" \
"         if(ii < 0 || ii >= imHeight) continue;                         \n" \
"         for(int jj = j - supportSize; jj <= j + supportSize; jj++) {   \n" \
"            if(jj < 0 || jj >= imWidth) continue;                       \n" \
"                                                                        \n" \
"            int neighSpIdx = clusterAssig[ii * imWidth + jj];           \n" \
"            float neighSal = saliencySP[neighSpIdx];                    \n" \
"                                                                        \n" \
"            int neighIdx = (imStride/4) * ii + 3 * jj;                  \n" \
"            float neighColor[3] = {img[neighIdx], img[neighIdx + 1], img[neighIdx + 2]}; \n" \
"            float neighPos[2] = {ii, jj};                               \n" \
"                                                                        \n" \
"            float w = exp((float)-0.5 * ( alpha * eucDist2(currColor, neighColor, 3) + beta * eucDist2(currPos, neighPos, 2))); \n" \
"            totalW += w;                                                \n" \
"                                                                        \n" \
"            totalS += neighSal * w;                                     \n" \
"         }                                                              \n" \
"      }                                                                 \n" \
"      totalS /= totalW;                                                 \n" \
"      saliency[i * imWidth + j] = totalS;                               \n" \
"   }                                                                    \n" \
"}                                                                       \n" \
"\n";

