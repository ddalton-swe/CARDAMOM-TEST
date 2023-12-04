#!/bin/bash

# Create and activate a Conda environment
echo "Creating and activating Conda environment"
conda create --name myenv python=3.7
eval "$(conda shell.bash hook)"

# Install required Conda packages
echo "Installing Conda packages"
conda install -c conda-forge libnetcdf gcc parallel