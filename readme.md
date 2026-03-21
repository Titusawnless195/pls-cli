# 🍺 pls - Blazingly Fast Context-Aware AI CLI

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![License](https://img.shields.io/badge/License-MIT-green.svg)
![Ollama](https://img.shields.io/badge/Powered%20by-Ollama-black.svg)

`pls` is a minimalist, blazingly fast Linux command-line utility written in C++. It translates natural language requests into ready-to-use bash/zsh commands using local LLMs via **Ollama**.

Unlike hundreds of bloated Python wrappers, `pls` runs instantly (zero-overhead), has no third-party dependencies, and is completely context-aware of your system environment.

## ✨ Killer Features
* ⚡ **Zero-Overhead:** Written in raw C++ using `libcurl`. Manual JSON parsing ensures microsecond execution times.
* 🐧 **OS Context-Aware:** Automatically reads `/etc/os-release` to detect your Linux distribution and preferred package manager (zypper, apt, pacman, dnf).
* 👁️ **Directory Context-Aware:** The AI "sees" the files in your current directory. Asking to "delete log files" will generate a command specifically for the files in your folder.
* 🚰 **UNIX Pipes (`stdin`) Support:** Feed logs or errors directly into the AI: `cat error.log | pls "find the issue and fix it"`.
* 📋 **Wayland Native:** Out-of-the-box clipboard integration via `wl-copy`.
* 🔒 **100% Private:** Everything runs locally on your hardware. No API keys, no data harvesting.

## 🛠️ Installation

**Dependencies:** `libcurl` and `cmake`.
(e.g., for openSUSE: `sudo zypper in libcurl-devel cmake gcc-c++`)

```bash
git clone https://github.com/YOUR_USERNAME/pls-ai-cli.git
cd pls-ai-cli
mkdir build && cd build
cmake ..
cmake --build .
sudo cp pls /usr/local/bin/

🚀 Usage

First, tell pls which local Ollama model to use (we highly recommend qwen2.5-coder:3b for the best speed/intelligence ratio):

pls --set qwen2.5-coder:3b

(you can see ollama active models via pls --list)

Basic translation:

pls update my system
# Output: zypper refresh && zypper dup (if you are on openSUSE)

Execution (-e) and Clipboard (-c) flags:

pls -e clear docker cache
# Generates the command and asks: 🚀 Execute? [y/N]:

Pipe Magic (stdin):

ps aux | pls -e "find the command to kill the process consuming the most memory"
# Output: kill -9 $(ps aux | sort -k4,4nr | head -n 1 | awk '{print $2}')

⚙️ Under the Hood

LLM generation temperature is hardcoded to 0.0 for maximum logical accuracy and zero hallucinations.
Active model config is safely stored in ~/.config/pls_model.txt.
Directory and Pipe context sizes are strictly limited to prevent memory overflow and VRAM crashes.
