# TAMProxy Compiler Setup Script (Ubuntu only)

################################
# DEFINE INSTALLATION ROUTINES #
################################

InstallMake () {
    echo "Installing make..."
    sudo apt -y install make
    echo ""
}

InstallArduino () {
    echo "Installing Arduino 2.2.1..."
    curl https://downloads.arduino.cc/arduino-ide/arduino-ide_2.2.1_Linux_64bit.zip --output ~/arduino-2.2.1.zip
    unzip ~/arduino-2.2.1.zip
    mv arduino-ide_2.2.1_Linux_64bit ~/arduino-2.2.1
    echo ""
}

InstallUdev () {
    echo "Installing udev rules..."
    curl https://www.pjrc.com/teensy/00-teensy.rules --output teensy.rules
    sudo mv teensy.rules /etc/udev/rules.d/00-teensy.rules
    echo ""
}

#####################################
# ENSURE DEPENDENCIES ARE INSTALLED #
#####################################

echo "Checking dependencies..."

# Check if `make` is installed
make --version > /dev/null || InstallMake

# Check if Arduino 2.x is installed
ARDUINO_IDE=~/arduino-2.2.1/arduino-ide
[ -f $ARDUINO_IDE ] || InstallArduino

# Direct the user to install Teensyduino 1.58
$ARDUINO_IDE &> /dev/null &
echo ""
echo "INSTALL TEENSYDUINO"
echo "1. Go to File > Preferences."
echo "2. Copy the following URL into 'Additional Boards Manager URLs'."
echo ""
echo "   https://www.pjrc.com/teensy/package_teensy_index.json"
echo ""
echo "3. Click 'OK'."
echo "4. Go to Tools > Board > Boards Manager."
echo "5. Search 'teensy'."
echo "6. Select 'Teensy by Paul Stoffregen' > Install."
echo "7. Close Arduino IDE."
echo ""
read -p "Press Enter to continue setup >> "
echo ""

###################################
# ENSURE SUBMODULES ARE INSTALLED #
###################################

echo "Checking submodules..."

# Initialize and update the submodules
git submodule init
git submodule update

echo ""

#######################
# DOWNLOAD UDEV RULES #
#######################

# Download udev rules
echo "Updating udev rules..."
[ -f /etc/udev/rules.d/00-teensy.rules ] || InstallUdev
echo ""

#################################
# COPY MAKEFILE AND CONFIG FILE #
#################################

cp Makefile_example Makefile
cp src/config_example_teensy41.h src/config.h

#################
# MAKE TAMPROXY #
#################

make