#!/bin/bash

# test script to launch locally built bpsend and bprecv executables in their own terminal windows

ROOT=$(pwd)

gnome-terminal --working-directory=$ROOT/build -- /bin/bash -c './bprecv ipn:5.1 data://127.0.0.1:37405 dacs://127.0.0.1:37406; exec /bin/bash'

sleep 1

gnome-terminal --working-directory=$ROOT/build -- /bin/bash -c './bpsend ipn:5.1 data://127.0.0.1:37405 dacs://127.0.0.1:37406; exec /bin/bash -i'

