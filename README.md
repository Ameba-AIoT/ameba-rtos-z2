# ameba-rtos-z2
GitHub `ameba-rtos-z2` repository is the development framework for AmebaZ2 SOC.

# Documentation
* :books: [Application Notes](https://github.com/Ameba-AIoT/ameba-rtos-z2/blob/main/doc/AN0500_Realtek_Ameba-ZII_application_note.en_v1.40.pdf)
* :books: [Datasheet](https://github.com/Ameba-AIoT/ameba-rtos-z2/blob/main/doc/RTL8720Cx-VH2_Datasheet_V1.0_20230224.pdf)

# Setup Build Environments

## GCC Environment on Windows
On Windows, you can use Cygwin as the GCC environment. Cygwin is a large collection of GNU and Open Source tools which provide the similar functionality as a Linux distribution on Windows. Please visit the official website of Cygwin and install the software.

Note:
* Please use Cygwin 32-bit version.
* During the Cygwin installation, please install "math -> bc" and "devel -> make".

## GCC Environment on Linux
On Linux, please install packages for the GCC environment.
* Use command `apt-get install make` to install "make".
* Use command `apt-get install bc` to install "bc".

# Configure the Project
* For general configurations, you can configure the options in "platform_opts.h".
* For BT configurations, you can configure the options in "platform_opts_bt.h".

# Compile the Project
1) Open the Cygwin Terminal or Linux terminal.
2) Direct to compile path. Execute command `cd /project/realtek_amebaz2_v0_example/GCC-RELEASE`.
3) Clean up pervious compilation files. Execute command `make clean`.
4) Build the SDK. Execute command `make all`.
5) Make sure there is no error after compilation.

# Download image
After a successfully compilation, the images `partition.bin`, `bootloader.bin`, `firmware_is.bin` and `flash_is.bin` can be seen in the folder/sub-folder of /GCC-RELEASE.
* `partition.bin` stores partition table, recording the address of Boot image and firmware image;
* `bootloader.bin` is bootloader image;
* `firmware_is.bin` is application image;
* `flash_is.bin` links `partition.bin`, `bootloader.bin` and `firmware_is.bin`.

To download image to board, you can either
* Directly download the image binary to board from GCC (J-Link debugger is required), please check the ApplicationNote chapter **SDK Build Environment Setup** for more details.
* Or using the PG tool for Ameba-ZII (in /tools/AmebaZ2), please check the ApplicationNote chapter **Image Tool** for more details.

# Release Notes

## Vesion Sync
GitHub `ameba-rtos-z2` is currently synchronized with 7.1 GIT 240412_b9f8a996.
