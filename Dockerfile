FROM quay.io/pypa/manylinux_2_28

RUN dnf config-manager --add-repo https://developer.download.nvidia.com/compute/cuda/repos/rhel8/x86_64/cuda-rhel8.repo

RUN dnf install -y cuda-toolkit
RUN dnf install -y nvidia-gds
RUN dnf install -y hdf5-devel wget
RUN dnf install -y boost-devel

ENV CUDA_HOME="/usr/local/cuda"
ENV PATH="$PATH:$CUDA_HOME/bin"
ENV LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$CUDA_HOME/lib64"