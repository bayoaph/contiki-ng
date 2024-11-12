#!/bin/bash

# Update package list
echo "Updating package list..."
sudo apt update

# Install necessary packages
echo "Installing required packages..."
sudo apt install -y build-essential doxygen git git-lfs curl wireshark python3-serial srecord rlwrap

# Check installation status
if [ $? -eq 0 ]; then
  echo "All packages installed successfully."
else
  echo "There was an error installing the packages."
  exit 1
fi

# Add the current user to the wireshark group
USER=$(whoami)
echo "Adding user '$USER' to the 'wireshark' group..."
sudo usermod -a -G wireshark $USER

# Check if the user was added successfully
if [ $? -eq 0 ]; then
  echo "User '$USER' successfully added to the 'wireshark' group."
else
  echo "Failed to add user '$USER' to the 'wireshark' group."
fi

# Download GCC ARM toolchain
TOOLCHAIN_URL="https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/9-2020q2/gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2"
TOOLCHAIN_ARCHIVE="gcc-arm-none-eabi-9-2020-q2-update-x86_64-linux.tar.bz2"

echo "Downloading GCC ARM toolchain from $TOOLCHAIN_URL..."
wget $TOOLCHAIN_URL

# Check if the download was successful
if [ $? -eq 0 ]; then
  echo "Download complete. Extracting the toolchain..."
  tar -xjf $TOOLCHAIN_ARCHIVE

  if [ $? -eq 0 ]; then
    echo "Toolchain extracted successfully."
  else
    echo "Failed to extract the toolchain."
  fi
else
  echo "Failed to download the toolchain."
fi

# Download and install MSPGCC 4.7.2
MSPGCC_URL="http://simonduq.github.io/resources/mspgcc-4.7.2-compiled.tar.bz2"
echo "Downloading MSPGCC 4.7.2 from $MSPGCC_URL..."
wget -nv $MSPGCC_URL

# Check if the download was successful
if [ $? -eq 0 ]; then
  echo "Download complete. Extracting MSPGCC and copying files to /usr/local/..."
  tar xjf mspgcc*.tar.bz2 -C /tmp/ && \
  sudo cp -f -r /tmp/msp430/* /usr/local/ && \
  rm -rf /tmp/msp430 mspgcc*.tar.bz2

  if [ $? -eq 0 ]; then
    echo "MSPGCC installed successfully."
  else
    echo "Failed to install MSPGCC."
  fi
else
  echo "Failed to download MSPGCC."
fi

# Install Java and Ant
echo "Installing default JDK and Apache Ant..."
sudo apt install -y default-jdk ant

# Check installation status for JDK and Ant
if [ $? -eq 0 ]; then
  echo "JDK and Ant installed successfully."
else
  echo "Failed to install JDK and Ant."
fi

# Configure Java alternatives
echo "Configuring Java alternatives..."
sudo update-alternatives --config java

# Inform the user
if [ $? -eq 0 ]; then
  echo "Java alternatives configured successfully."
else
  echo "There was an issue configuring Java alternatives."
fi

# Add the current user to the plugdev group and dialout group
# This will allow access to USB without sudo
USER=$(whoami)
echo "Adding user '$USER' to the 'plugdev' and 'dialout' groups..."
sudo usermod -a -G plugdev $USER
sudo usermod -a -G dialout $USER

echo "All configurations have been made. You need to reboot your system."
