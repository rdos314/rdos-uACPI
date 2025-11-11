@echo off

echo "Building Driver"
ide2make -p driver/acpi 1>nul
wmake -f driver/acpi.mk -h -e 1>nul

echo "Building Server"
ide2make -p rdos/uacpi 1>nul
wmake -f rdos/uacpi.mk -h -e 1>nul
