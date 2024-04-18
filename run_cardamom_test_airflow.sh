#!/bin/bash

set -e

# shellcheck disable=SC1091
conda activate cardamom

# Determine the directory of the script
basedir=$(cd "$(dirname "$0")" && pwd -P)
OUTPUTDIR="${PWD}/output"

mkdir -p "${OUTPUTDIR}"

"${basedir}/BASH/CARDAMOM_COMPILE.sh"