# NVIDIA Driver and CUDA Toolkit
Both the NVIDIA driver and the CUDA toolkit are required to run RayX on a GPU. 

## 1. NVIDIA Driver

First, check that the NVIDIA driver is correctly installed:

```bash
nvidia-smi
```
If this command fails, install the NVIDIA driver before proceeding.

## 2. CUDA Toolkit

First, check that the CUDA toolkit is correctly installed:

```bash
which nvcc
nvcc --version
```

## 3. Install NVIDIA Driver and CUDA Toolkit
Both the NVIDIA driver and the CUDA toolkit can be installed from the official NVIDIA website:
https://developer.nvidia.com/cuda-downloads

