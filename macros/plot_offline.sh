#!/bin/bash

# Check if the correct number of arguments is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <basepath>"
    exit 1
fi

basepath=$1

# Check if the basepath exists
if [ ! -d "$basepath" ]; then
    echo "Error: The specified basepath '$basepath' does not exist."
    exit 1
fi

# Navigate to the UNPACKING_ROOT/bin directory
cd $UNPACKING_ROOT/bin || exit 1

# Run the unpacking tool
./unpack -d $basepath

# Run the printEvents tool
./printEvents -d $basepath

# Run the dataCheck tool
./dataCheck -d $basepath

# Navigate to the UNPACKING_ROOT/macros directory
cd $UNPACKING_ROOT/macros || exit 1

# Run the ROOT script
root -l -q "plot_events_with_crates.C(\"$basepath/Events.root\",\"$basepath\")"
