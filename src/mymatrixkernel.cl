__kernel void matrixVectorMul(__global int* matrixA,  __global int* vectorB, __global int* resultVector, int row,int col)
{
    int rowi = get_global_id(0);
    int index = (rowi * col);

    for(int j = 0;j < col;j++)
    {
    int value = 0;
    for (int k = 0; k < row;k++)
    {
        value += (matrixA[index + k] * vectorB[(k * row) + j]);
    }
     resultVector[index + j] = value;
    }
}