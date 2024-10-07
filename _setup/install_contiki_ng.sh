#!/bin/bash

# Clone the Contiki-NG repository
echo "Cloning the Contiki-NG repository as a submodule..."
cd ../
git submodule add https://github.com/contiki-ng/contiki-ng.git main_contiki

# Check if the clone was successful
if [ $? -eq 0 ]; then
  echo "Successfully cloned Contiki-NG."
else
  echo "Failed to clone Contiki-NG repository."
  exit 1
fi

# Change to the Contiki-NG directory
cd main_contiki || { echo "Failed to enter the main contiki-ng directory."; exit 1; }

# Initialize and update submodules
echo "Updating submodules..."
git submodule update --init --recursive

# Check if the submodule update was successful
if [ $? -eq 0 ]; then
  echo "Submodules updated successfully."
else
  echo "Failed to update submodules."
  exit 1
fi
