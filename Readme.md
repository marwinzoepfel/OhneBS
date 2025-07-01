# OhneBS: A Bare-Metal Learning Operating System

A small, 64-bit operating system for the Raspberry Pi 400, written from scratch in C and AArch64 Assembly. This project is a hands-on journey to explore the fundamental concepts of operating system architecture and low-level hardware programming.

## About This Project

**Disclaimer:** I am not a professional software developer. I am a student at a German university, pursuing a degree in **Electrical Engineering and Information Technology**.

This project was born out of a desire to bridge the gap between theory and practice. After designing some simple **8-bit chips in the style of the old Intel 8080**, I wanted to truly understand how a modern computer works at its core. This OS is my attempt to do just that: to peel back the layers of abstraction and interact directly with the hardware.Project Goals

The primary goal of this project is not to create a feature-complete, daily-driver OS, but to learn from the challenges involved in its creation. The core objectives are:

- **Hardware Control:** To directly control the Raspberry Pi's peripherals (GPIO, UART, etc.) without the safety net of a host OS.
- **Architectural Understanding:** To gain a deep, practical understanding of the ARMv8-A (AArch64) architecture.
- **OS Concepts in Practice:** To implement foundational operating system concepts like interrupts, virtual memory, and process scheduling from the ground up.

## Current Status & Implemented Features

- I will update this as soon, as the first alpha is finished

## Planned Roadmap (Next Steps)

- I will update this as soon, as the first alpha is finished

## Hardware Setup

This project is developed for a **Raspberry Pi 400**. The following hardware is used for I/O:

- **Output:** A **USB to TTL Serial Adapter** (connected to UART pins GPIO 14/15) serves as the primary debug console.
- **Input:** A **PS/2 Keyboard**, connected via a **bidirectional logic level converter** (5V ↔ 3.3V) to GPIO pins **23 (CLK)** and **24 (DATA)**.

## Build Instructions

#### Prerequisites

- A Linux environment (e.g., Ubuntu or WSL).
- The `aarch64-none-elf` cross-compiler toolchain.

#### Compiling

Navigate to the project's root directory and run the following command:

```bash
make -f Makefile.gcc
```

This will compile the kernel and produce the final `kernel8.img` binary, which can then be copied to the boot partition of an SD card.

## License

This project is licensed under the **GNU General Public License v2.0 (GPLv2)**.

#### What does this mean?

In simple terms, the GPLv2 gives you the freedom to:

- **Use:** You are free to use this software for any purpose.
- **Study & Modify:** You have full access to the source code and are free to change it to suit your needs.
- **Share:** You are free to share and distribute copies of the original software.

In return, it includes a "copyleft" provision, which means:

- **Share Alike:** If you modify the code and distribute your modified version, you **must** also make your version available under the same GPLv2 license. This ensures that the software and its derivatives will always remain free and open-source for everyone to use and learn from.

## Resources & Acknowledgements

This project would not be possible without the excellent documentation and work of others in the community.

- **BCM2711 Peripherals Datasheet:** The ultimate source of truth for the Raspberry Pi's hardware.
- **ARMv8-A Architecture Reference Manual:** The official documentation for the CPU architecture.
- **isometimes' rpi4-osdev Tutorial:** An invaluable reference for bare-metal programming on the RPi 4.
- **Cambridge "Baking Pi" Tutorial:** The classic and inspirational guide for OS development on the Pi.
