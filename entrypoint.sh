#!/usr/bin/env bash

set -euxo pipefail

make && ./build/kilo.exe
