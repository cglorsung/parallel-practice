/*
Author: Conor Lorsung following the example found at https://www.eriksmistad.no/
*/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <CL/cl.h>
#include "MainConfig.h"

#define MAX_SOURCE_SIZE (0x100000)

int hello() {
    printf("Hello, world!\n");
    int x = 0, y = 1;
    int *z;

    z = &x;
    printf("VALOF z = &x: %d | Expecting integer memory address\n", z);

    y = *z;
    printf("VALOF y = *z: %d | Expecting 0\n", y);

    x = (int) z;
    printf("VALOF x = z : %d | Expecting integer memory address\n", x);

    *z = 3;
    printf("VALOF *z = 3: x=%d ; y=%d ; z = %d\n", x, y, z);

    //Print expected values for last assignment
    printf("Expecting x, y, z = 3, 0, intMemAddr");

    return 0;
}

int main(void) {
    hello();

    int i;
    const int LIST_SIZE = 1024;
    int *A = (int*) malloc(sizeof(int)*LIST_SIZE);
    int *B = (int*) malloc(sizeof(int)*LIST_SIZE);
    for(i = 0; i < LIST_SIZE; i++) {
        A[i] = i;
        B[i] = LIST_SIZE - i;
    }

    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen("C:/Users/Conor/Desktop/CSCI/parallel-practice/src/OpenCLUtils/vector_addition.cl", "r");
    if(!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }

    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int), NULL, &ret);
    cl_mem b_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int), NULL, &ret);
    cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, LIST_SIZE * sizeof(int), NULL, &ret);

    ret = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), A, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, b_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), B, 0, NULL, NULL);

    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);

    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    cl_kernel kernel = clCreateKernel(program, "vector_addition", &ret);

    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&c_mem_obj);

    size_t global_item_size = LIST_SIZE;
    size_t local_item_size = 64;
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

    int *C = (int*)malloc(sizeof(int)*LIST_SIZE);
    ret = clEnqueueReadBuffer(command_queue, c_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), C, 0, NULL, NULL);

    for(i = 0; i < LIST_SIZE; i++) {
        printf("%d + %d = %d\n", A[i], B[i], C[i]);
    }

    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(a_mem_obj);
    ret = clReleaseMemObject(b_mem_obj);
    ret = clReleaseMemObject(c_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(A);
    free(B);
    free(C);

    return 0;
}
