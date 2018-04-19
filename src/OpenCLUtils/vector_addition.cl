/*
Kernel from https://www.eriksmistad.no/
*/

__kernel void vector_add(__global const int *A, __global const int *B, __global int *C) {
    // Index of current processing element
    int i = get_global_id(0);
    // Perform the addition
    C[i] = A[i] + B[i];
}
