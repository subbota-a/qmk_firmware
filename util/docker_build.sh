#!/bin/sh
# This script is intended for BUILDING firmware only.
# Flashing should be done separately (QMK Toolbox, dfu-util, etc).
# SKIP_FLASHING_SUPPORT=1 avoids the privileged container with /dev mount,
# which is not needed for compilation.

SKIP_FLASHING_SUPPORT=1 ./util/docker_cmd.sh make "$@"
