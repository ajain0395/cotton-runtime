__kernel void matrixVectorMul(__global float* matrixA,  __global float* vectorB, __global float* resultVector, int row,int col)
{
    int rowi = get_global_id(0);
    int index = (rowi * col);

    for(int j = 0;j < col;j++)
    {
    float value = 0;
    for (int k = 0; k < row;k++)
    {
        value += (matrixA[index + k] * vectorB[(k * row) + j]);
    }
     resultVector[index + j] = value;
    }
}