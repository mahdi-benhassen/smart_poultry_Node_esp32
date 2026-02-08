https://github.com/mahdi-benhassen/smart_poultry_Node_esp32.git# Setup Libraries Script
# This script downloads necessary Arduino libraries into the components directory
# and adds CMakeLists.txt to them for ESP-IDF compatibility.

$componentsDir = "components"

if (!(Test-Path -Path $componentsDir)) {
    New-Item -ItemType Directory -Path $componentsDir
}

function Install-Lib {
    param (
        [string]$RepoUrl,
        [string]$Name,
        [string]$Requires = ""
    )
    $path = Join-Path $componentsDir $Name
    if (Test-Path $path) {
        Write-Host "Library $Name already exists."
    } else {
        Write-Host "Cloning $Name..."
        git clone $RepoUrl $path
        
        # Add CMakeLists.txt if not present
        $cmakePath = Join-Path $path "CMakeLists.txt"
        if (!(Test-Path $cmakePath)) {
            $reqString = "REQUIRES arduino-esp32"
            if ($Requires -ne "") {
                $reqString += " $Requires"
            }
            
            $cmakeContent = @"
idf_component_register(SRCS "*.cpp" "*.c" "*.S"
                       INCLUDE_DIRS "." "src"
                       $reqString)
"@
            Set-Content -Path $cmakePath -Value $cmakeContent
            Write-Host "Added CMakeLists.txt to $Name"
        }
    }
}

# Clone Libraries with Dependencies
Install-Lib "https://github.com/adafruit/Adafruit_Sensor.git" "Adafruit_Sensor"
Install-Lib "https://github.com/adafruit/DHT-sensor-library.git" "DHT-sensor-library" "Adafruit_Sensor"
Install-Lib "https://github.com/claws/BH1750.git" "BH1750"
Install-Lib "https://github.com/knolleary/pubsubclient.git" "pubsubclient"
Install-Lib "https://github.com/sandeepmistry/arduino-LoRa.git" "arduino-LoRa"
Install-Lib "https://github.com/bblanchon/ArduinoJson.git" "ArduinoJson"
Install-Lib "https://github.com/phoenix1747/MQ135.git" "MQ135"
Install-Lib "https://github.com/tzapu/WiFiManager.git" "WiFiManager"

Write-Host "All libraries set up."
