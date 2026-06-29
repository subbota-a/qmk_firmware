#!/bin/sh
# This script is intended for BUILDING firmware only.
# Flashing should be done separately (QMK Toolbox, dfu-util, etc).
# SKIP_FLASHING_SUPPORT=1 avoids the privileged container with /dev mount,
# which is not needed for compilation.
# QMK_USERSPACE= disables the "copy firmware to userspace folder" step, which
# otherwise tries to copy to /qmk_userspace (configured in the qmk_cli image but
# not mounted) and fails. The .bin still lands in the repo root.

SKIP_FLASHING_SUPPORT=1 ./util/docker_cmd.sh make QMK_USERSPACE= "$@"
