#!/bin/bash

archive_filename=vicecitydashboard-src.tar.gz

# Move to the repository root.
cd "$(dirname "$0")/.."
checkout_dir="$(basename $(pwd))"

# Archive the repository, excluding build artifacts.
cd ..
tar cvzf ${archive_filename} --exclude="${checkout_dir}/build" --exclude="${checkout_dir}/bin" "${checkout_dir}"
mv -v ${archive_filename} "${checkout_dir}"
