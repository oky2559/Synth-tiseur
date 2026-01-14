#!/bin/bash
# Script de démonstration du synthétiseur

echo "Building synthesizer..."
cd base-project
meson setup build
meson compile -C build

echo ""
echo "Running synthesizer..."
cd build/app
./base_project_app.exe

echo ""
echo "Success! Audio file generated: synthesizer_output.wav"
echo "Play it with your audio player (vlc, audacity, foobar2000, etc.)"
