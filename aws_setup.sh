#!/bin/bash

if [ "$#" -lt 1 ]; then
  echo "Usage: $0 <IC type>"
  echo "IC type: amebaz2"
  exit 1
fi

AMEBA="$1"

if [ "$AMEBA" != "amebaz2" ]; then
  echo "Invalid IC type. Expected: amebaz2"
  exit 1
fi

PROJECT_DIR="$PWD/project/realtek_${AMEBA}_v0_example"

AMAZON_FREERTOS_DIR="$PWD/component/common/application/amazon-freertos"
AMAZON_FREERTOS_REPO_URL="https://github.com/Ameba-AIoT/ameba-amazon-freertos.git"
AMAZON_FREERTOS_REPO_BRANCH="FreeRTOS-LTS-202406.xx"

AMAZON_FREERTOS_OS_DIR="$PWD/component/os/freertos/freertos_v10.4.3"
AMAZON_FREERTOS_OS_REPO_URL="https://github.com/FreeRTOS/FreeRTOS-Kernel.git"
AMAZON_FREERTOS_OS_REPO_BRANCH="V10.4.3-LTS-Patch-2"

# --- 1: Cloning Amazon FreeRTOS submodule ---
if [ ! -d "$AMAZON_FREERTOS_DIR" ]; then
  echo "Cloning AWS repository..."
  git clone --recurse-submodules -b "$AMAZON_FREERTOS_REPO_BRANCH" "$AMAZON_FREERTOS_REPO_URL" "$AMAZON_FREERTOS_DIR"
fi

# --- 2: Cloning FreeRTOS v10.4.3 submodule ---
if [ ! -d "$AMAZON_FREERTOS_OS_DIR" ] || [ -z "$(find "$AMAZON_FREERTOS_OS_DIR" -mindepth 1)" ]; then
    mkdir -p "$AMAZON_FREERTOS_OS_DIR"
    git clone --branch "$AMAZON_FREERTOS_OS_REPO_BRANCH" --depth 1 "$AMAZON_FREERTOS_OS_REPO_URL" "$AMAZON_FREERTOS_OS_DIR"
fi

# --- Helper Functions ---
modify_makefile() {
  find "$BASE_DIR" -type f -name "Makefile" | while read -r FILE; do
    if grep -q "ENABLE_AWS_FREERTOS = 0" "$FILE"; then
      echo "Enabling AWS FreeRTOS in: $FILE"
      sed -i 's/^ENABLE_AWS_FREERTOS = 0/ENABLE_AWS_FREERTOS = 1/' "$FILE"
    fi
  done
}

# --- 3: Configuring Makefile ---
case "$AMEBA" in
  amebaz2)
    BASE_DIR="$PROJECT_DIR"
    echo "Configuring for $AMEBA..."
    modify_makefile
    ;;
  *)
    echo "Invalid IC type. Expected: amebaz2 or amebaz2plus."
    exit 1
    ;;
esac

echo "Amazon FreeRTOS setup complete"
