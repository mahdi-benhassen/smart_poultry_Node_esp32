#!/bin/bash
# Setup Libraries Script (Linux/macOS)
# This script downloads necessary Arduino libraries into the components directory
# and adds CMakeLists.txt to them for ESP-IDF compatibility.

COMPONENTS_DIR="components"

mkdir -p "$COMPONENTS_DIR"

install_lib() {
    REPO_URL=$1
    NAME=$2
    REQUIRES=$3
    
    PATH_DIR="$COMPONENTS_DIR/$NAME"
    
    if [ -d "$PATH_DIR" ]; then
        echo "Library $NAME already exists."
    else
        echo "Cloning $NAME..."
        git clone "$REPO_URL" "$PATH_DIR"
        
        # Remove nested .git to allow main repo to track files
        rm -rf "$PATH_DIR/.git"
        
        # Add CMakeLists.txt if not present
        CMAKE_PATH="$PATH_DIR/CMakeLists.txt"
        if [ ! -f "$CMAKE_PATH" ]; then
            REQ_STRING="REQUIRES arduino-esp32"
            if [ -n "$REQUIRES" ]; then
                REQ_STRING="$REQ_STRING $REQUIRES"
            fi
            
            cat <<EOF > "$CMAKE_PATH"
idf_component_register(SRCS "*.cpp" "*.c" "*.S"
                       INCLUDE_DIRS "." "src"
                       $REQ_STRING)
EOF
            echo "Added CMakeLists.txt to $NAME"
        fi
    fi
}

# Clone Libraries with Dependencies
install_lib "https://github.com/adafruit/Adafruit_Sensor.git" "Adafruit_Sensor"
install_lib "https://github.com/adafruit/DHT-sensor-library.git" "DHT-sensor-library" "Adafruit_Sensor"
install_lib "https://github.com/claws/BH1750.git" "BH1750"
install_lib "https://github.com/knolleary/pubsubclient.git" "pubsubclient"
install_lib "https://github.com/sandeepmistry/arduino-LoRa.git" "arduino-LoRa"
install_lib "https://github.com/bblanchon/ArduinoJson.git" "ArduinoJson"
install_lib "https://github.com/phoenix1747/MQ135.git" "MQ135"
install_lib "https://github.com/tzapu/WiFiManager.git" "WiFiManager"

echo "All libraries set up."
