#include "opencl.h"
#include <iostream>
#include "CL\cl.h"

/*******************************************************************************
 * OpenCL wrappers
 *******************************************************************************/

std::string
GPUAcceleration::openCLErrorString(int err)
{
    switch (err) {
    case CL_SUCCESS:
        return "Success!";
    case CL_DEVICE_NOT_FOUND:
        return "Device not found.";
    case CL_DEVICE_NOT_AVAILABLE:
        return "Device not available";
    case CL_COMPILER_NOT_AVAILABLE:
        return "Compiler not available";
    case CL_MEM_OBJECT_ALLOCATION_FAILURE:
        return "Memory object allocation failure";
    case CL_OUT_OF_RESOURCES:
        return "Out of resources";
    case CL_OUT_OF_HOST_MEMORY:
        return "Out of host memory";
    case CL_PROFILING_INFO_NOT_AVAILABLE:
        return "Profiling information not available";
    case CL_MEM_COPY_OVERLAP:
        return "Memory copy overlap";
    case CL_IMAGE_FORMAT_MISMATCH:
        return "Image format mismatch";
    case CL_IMAGE_FORMAT_NOT_SUPPORTED:
        return "Image format not supported";
    case CL_BUILD_PROGRAM_FAILURE:
        return "Program build failure";
    case CL_MAP_FAILURE:
        return "Map failure";
    case CL_INVALID_VALUE:
        return "Invalid value";
    case CL_INVALID_DEVICE_TYPE:
        return "Invalid device type";
    case CL_INVALID_PLATFORM:
        return "Invalid platform";
    case CL_INVALID_DEVICE:
        return "Invalid device";
    case CL_INVALID_CONTEXT:
        return "Invalid context";
    case CL_INVALID_QUEUE_PROPERTIES:
        return "Invalid queue properties";
    case CL_INVALID_COMMAND_QUEUE:
        return "Invalid command queue";
    case CL_INVALID_HOST_PTR:
        return "Invalid host pointer";
    case CL_INVALID_MEM_OBJECT:
        return "Invalid memory object";
    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
        return "Invalid image format descriptor";
    case CL_INVALID_IMAGE_SIZE:
        return "Invalid image size";
    case CL_INVALID_SAMPLER:
        return "Invalid sampler";
    case CL_INVALID_BINARY:
        return "Invalid binary";
    case CL_INVALID_BUILD_OPTIONS:
        return "Invalid build options";
    case CL_INVALID_PROGRAM:
        return "Invalid program";
    case CL_INVALID_PROGRAM_EXECUTABLE:
        return "Invalid program executable";
    case CL_INVALID_KERNEL_NAME:
        return "Invalid kernel name";
    case CL_INVALID_KERNEL_DEFINITION:
        return "Invalid kernel definition";
    case CL_INVALID_KERNEL:
        return "Invalid kernel";
    case CL_INVALID_ARG_INDEX:
        return "Invalid argument index";
    case CL_INVALID_ARG_VALUE:
        return "Invalid argument value";
    case CL_INVALID_ARG_SIZE:
        return "Invalid argument size";
    case CL_INVALID_KERNEL_ARGS:
        return "Invalid kernel arguments";
    case CL_INVALID_WORK_DIMENSION:
        return "Invalid work dimension";
    case CL_INVALID_WORK_GROUP_SIZE:
        return "Invalid work group size";
    case CL_INVALID_WORK_ITEM_SIZE:
        return "Invalid work item size";
    case CL_INVALID_GLOBAL_OFFSET:
        return "Invalid global offset";
    case CL_INVALID_EVENT_WAIT_LIST:
        return "Invalid event wait list";
    case CL_INVALID_EVENT:
        return "Invalid event";
    case CL_INVALID_OPERATION:
        return "Invalid operation";
    case CL_INVALID_GL_OBJECT:
        return "Invalid OpenGL object";
    case CL_INVALID_BUFFER_SIZE:
        return "Invalid buffer size";
    case CL_INVALID_MIP_LEVEL:
        return "Invalid mip-map level";
    default:
        return "Unknown";
    }
}


GPUAcceleration::OpenCL::OpenCL(bool useGPU)
{
    //cl_int ret;
    //cl_uint ret_num_platforms;
    cl_platform_id platform_id = NULL;
    //cl_uint ret_num_devices;

    cl_device_type deviceType;
    if (useGPU)
    {
        deviceType = CL_DEVICE_TYPE_GPU;
    }
    else
    {
        deviceType = CL_DEVICE_TYPE_CPU;
    }

    cl_uint numPlatforms = 0;
    int err = clGetPlatformIDs(0, NULL, &numPlatforms);
    // Get (in numPlatforms) the number of OpenCL platforms available
    // No platform ID will be return, since platforms is NULL
    if (CL_SUCCESS != err)
    {
        std::cerr << "Error: clGetplatform_ids() to get num platforms returned %s.\n" << std::endl;
        std::exit(0);
    }
    if (0 == numPlatforms)
    {
        std::cerr << "Error: No platforms found!\n" << std::endl;
        std::exit(0);
    }

    std::vector<cl_platform_id> platforms(numPlatforms);

    // Now, obtains a list of numPlatforms OpenCL platforms available
    // The list of platforms available will be returned in platforms
    err = clGetPlatformIDs(numPlatforms, &platforms[0], NULL);
    if (CL_SUCCESS != err)
    {
        std::cerr << "Error: clGetplatform_ids() to get platforms returned %s.\n" << std::endl;
        std::exit(0);
    }

    // Check if one of the available platform matches the preferred requirements
    for (cl_uint i = 0; i < numPlatforms; i++)
    {
        bool match = true;
        cl_uint numDevices = 0;

        

        // match is true if the platform's name is the required one or don't care (NULL)
        
        // Obtains the number of deviceType devices available on platform
        // When the function failed we expect numDevices to be zero.
        // We ignore the function return value since a non-zero error code
        // could happen if this platform doesn't support the specified device type.
        err = clGetDeviceIDs(platforms[i], deviceType, 0, NULL, &numDevices);
        if (CL_SUCCESS != err)
        {
            std::cout << "   Required device was not found on this platform.\n" << std::endl;
        }

        if (0 != numDevices)
        {
            // There is at list one device that answer the requirements
            std::cout << "   Required device was found.\n" << std::endl;
            platform_id = platforms[i];
            break;
        }
        
    }
    struct ocl_args_d_t
    {

        // Regular OpenCL objects:
        cl_context       context;           // hold the context handler
        cl_device_id     device;            // hold the selected device handler
        cl_command_queue commandQueue;      // hold the commands-queue handler
        cl_program       program;           // hold the program handler
        cl_kernel        kernel;            // hold the kernel handler
        float            platformVersion;   // hold the OpenCL platform version (default 1.2)
        float            deviceVersion;     // hold the OpenCL device version (default. 1.2)
        float            compilerVersion;   // hold the device OpenCL C version (default. 1.2)

        // Objects that are specific for algorithm implemented in this sample
        cl_mem           srcA;              // hold first source buffer
        cl_mem           srcB;              // hold second source buffer
        cl_mem           dstMem;            // hold destination buffer
    };

    ocl_args_d_t ocl;
    cl_context_properties contextProperties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platform_id, 0 };
    _context = clCreateContextFromType(contextProperties, deviceType, NULL, NULL, &err);

    if ((CL_SUCCESS != err) || (NULL == _context))
    {
        std::cerr << "Couldn't create a context, clCreateContextFromType() returned '%s'.\n" << std::endl;
        std::exit(0);
    }

    err = clGetContextInfo(_context, CL_CONTEXT_DEVICES, sizeof(cl_device_id), &ocl.device, NULL);
#ifdef CL_VERSION_2_0
    if (CL_VERSION_2_0 == ocl.deviceVersion)
    {
        const cl_command_queue_properties properties[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0 };
        ocl.commandQueue = clCreateCommandQueueWithProperties(_context, ocl.device, properties, &err);
    }
    else {
        // default behavior: OpenCL 1.2
        cl_command_queue_properties properties = CL_QUEUE_PROFILING_ENABLE;
        ocl.commandQueue = clCreateCommandQueue(_context, ocl.device, properties, &err);
    }
#else
    // default behavior: OpenCL 1.2
    cl_command_queue_properties properties = CL_QUEUE_PROFILING_ENABLE;
    ocl.commandQueue = clCreateCommandQueue(_context, ocl.device, properties, &err);
#endif
    if (CL_SUCCESS != err)
    {
        std::cerr << "Error: clCreateCommandQueue() returned %s.\n" << std::endl;
        std::exit(0);
    }

    //ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    //// Creating device ID group
    //int err = clGetDeviceIDs(platform_id, useGPU ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU, 1, &_deviceId, NULL);
    ////if (err != CL_SUCCESS) /*LOG_FATAL("Failed to create a device group!")*/ return;

    //// Create a compute context
    //_context = clCreateContext(0, 1, &_deviceId, NULL, NULL, &err);
    ////if (!_context) /*LOG_FATAL("Failed to create a compute context!")*/ return;

    //// Create a command queue
    //_commandsQueue = clCreateCommandQueue(_context, _deviceId, 0, &err);
    ////if (!_commandsQueue) /*LOG_FATAL("Failed to create a command queue!")*/ return;
}

GPUAcceleration::OpenCL::~OpenCL()
{
    clReleaseCommandQueue(_commandsQueue);
    clReleaseContext(_context);
}


GPUAcceleration::Kernel::Kernel(const OpenCL& opencl, const std::string& source, const std::string& kernelName)
{
    int err;

    // Create the compute program from the source buffer
    const char* src = source.c_str();
    _program = clCreateProgramWithSource(opencl.getContext(), 1, (const char**)&src, NULL, &err);
    if (!_program) /*LOG_FATAL("Failed to create compute program!")*/ return;

    // Build the program executable
    err = clBuildProgram(_program, 0, NULL, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        size_t len;
        char buffer[2048];

        /*LOG_ERROR("Failed to build program executable! " << openCLErrorString(err))*/;
        clGetProgramBuildInfo(_program, opencl.getDeviceID(), CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        exit(EXIT_FAILURE);
    }

    // Create the compute kernel in the program we wish to run
    _kernel = clCreateKernel(_program, kernelName.c_str(), &err);
    if (!_kernel || err != CL_SUCCESS)/* LOG_FATAL("Error: Failed to create compute kernel! " << openCLErrorString(err))*/ return;
}

GPUAcceleration::Kernel::~Kernel()
{
    clReleaseProgram(_program);
    clReleaseKernel(_kernel);
}

size_t
GPUAcceleration::Kernel::getWorkGroupSize(const OpenCL& opencl)
{
    size_t workgroupSize;

    int err = clGetKernelWorkGroupInfo(_kernel, opencl.getDeviceID(), CL_KERNEL_WORK_GROUP_SIZE, sizeof(workgroupSize), &workgroupSize, NULL);
    if (err != CL_SUCCESS) /*LOG_FATAL("Error: Failed to retrieve kernel work group info! " << err)*/ return 0;

    return workgroupSize;
}

void
GPUAcceleration::executeKernel(const OpenCL& opencl, const Kernel& kernel, size_t domainSize)
{
    std::vector<size_t> tmp(1, domainSize);
    executeKernel(opencl, kernel, tmp);
}

void
GPUAcceleration::executeKernel(const OpenCL& opencl, const Kernel& kernel, const std::vector<size_t>& domainSize)
{
    int err = clEnqueueNDRangeKernel(opencl.getCommandsQueue(), kernel.getKernel(), domainSize.size(), NULL, &domainSize[0], NULL, 0, NULL, NULL);
    if (err) /*LOG_FATAL("Failed to execute kernel: " << openCLErrorString(err))*/ return;

    clFinish(opencl.getCommandsQueue());
}

GPUAcceleration::Memory::Memory(const OpenCL& opencl, cl_mem_flags flags, size_t size, void* host_ptr) :
    _size(size)
{
    _mem = clCreateBuffer(opencl.getContext(), flags, size, host_ptr, NULL);
    if (!_mem) /*LOG_FATAL("Failed to allocate device memory!")*/ return;
}

GPUAcceleration::Memory::~Memory()
{
    clReleaseMemObject(_mem);
}

void
GPUAcceleration::Memory::readBuffer(const OpenCL& opencl, void* buffer)
{
    size_t offset = 0;

    int err = clEnqueueReadBuffer(opencl.getCommandsQueue(), _mem, CL_TRUE, offset, _size, buffer, 0, NULL, NULL);
    if (err != CL_SUCCESS) /*LOG_FATAL("Error: Failed to read output array! " << openCLErrorString(err))*/ return;
}
