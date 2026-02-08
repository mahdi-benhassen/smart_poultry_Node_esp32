idf_component_register(SRCS "*.cpp" "*.c" "*.S"
                       INCLUDE_DIRS "." "src"
                       REQUIRES "arduino-esp32" Adafruit_Sensor)
