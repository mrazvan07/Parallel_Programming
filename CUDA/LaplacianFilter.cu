__global__ void LaplacianFilterKernel(unsigned char *d_img, int width, int height)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    int index = x + y * width;

    if(x > 0 && x < width-1 && y > 0 && y < height-1)
    {
        int laplacian = 4*d_img[index] - d_img[index-1] - d_img[index+1] - d_img[index-width] - d_img[index+width];
        d_img[index] = min(255, max(0, laplacian));
    }
}

void LaplacianFilter(unsigned char *h_img, int width, int height)
{
    dim3 block(16, 16);
    dim3 grid((width + block.x - 1) / block.x, (height + block.y - 1) / block.y);

    unsigned char *d_img;
    cudaMalloc((void **)&d_img, width*height*sizeof(unsigned char));
    cudaMemcpy(d_img, h_img, width*height*sizeof(unsigned char), cudaMemcpyHostToDevice);

    LaplacianFilterKernel<<<grid, block>>>(d_img, width, height);

    cudaMemcpy(h_img, d_img, width*height*sizeof(unsigned char), cudaMemcpyDeviceToHost);
    cudaFree(d_img);
}