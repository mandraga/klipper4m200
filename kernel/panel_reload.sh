#!/bin/bash
# When not booting in fel mode, the panel driver does not work at start.
# The reason is unknown, however restarting the driver helps.

set -eu

# small delay to let boot settle
sleep 1

echo "panel reload workaround starting" | tee /dev/kmsg
# panel reload workaround
modprobe -r panel-mtf0397swi 2>/dev/null || true
modprobe panel-mtf0397swi
