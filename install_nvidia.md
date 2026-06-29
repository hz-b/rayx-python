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

## 3. Add CUDA to PATH

If the toolkit is installed but `nvcc` is still not found, its bin directory is not in your `PATH`.
Add it for the current shell:

```bash
export PATH=/usr/local/cuda/bin:$PATH
```

To make it permanent, add that line to your `~/.bashrc` (or `~/.zshrc`) and reload:

```bash
echo 'export PATH=/usr/local/cuda/bin:$PATH' >> ~/.bashrc
source ~/.bashrc
```

## 4. Install NVIDIA Driver and CUDA Toolkit
Both the NVIDIA driver and the CUDA toolkit can be installed from the official NVIDIA website:
https://developer.nvidia.com/cuda-downloads

