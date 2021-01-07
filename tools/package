#!/bin/bash

# Move to the current working directory.
cd $(dirname ${BASH_SOURCE[ ${#BASH_SOURCE[@]} - 1 ]})

# Archive the repository, excluding build artifacts.
cd ../..
tar cvzf vcdashboard-src.tar.gz --exclude=ViceCityDashboard/build --exclude=ViceCityDashboard/bin ViceCityDashboard

