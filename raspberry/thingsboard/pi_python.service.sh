#!/bin/bash

PYTHON="python3"
SCRIPT_PATH=`dirname "$0"`
SCRIPT_PATH=`realpath $SCRIPT_PATH`

${PYTHON} -m venv ${SCRIPT_PATH}/env

echo "Call source ${SCRIPT_PATH}/env/bin/activate"
source "${SCRIPT_PATH}/env/bin/activate"

# install requirements
echo "Call ${PYTHON} -m pip install -r \"${SCRIPT_PATH}/env/requirements.txt\""
${PYTHON} -m pip install -r "${SCRIPT_PATH}/env/requirements.txt"

# auto send raspberry pi performance to server
${PYTHON} "${SCRIPT_PATH}/send_pi_performance.py"
