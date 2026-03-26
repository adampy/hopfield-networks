# Hopfield Networks

## Installation
Tested on WSL2 (running Ubuntu 24.04 LTS)

Install required graphics libs
```bash
sudo apt-get update && sudo apt-get install -y libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev libgl-dev
```

## Run
When running through WSL terminal, the `$DISPLAY` env var should be set and windows will be forwarded into Windows using X11.
```bash
chmod +x build_and_run.sh
./build_and_run.sh
```

## Development
For new files (`.cpp` always, `.hpp` is not *required* but encouraged), make sure to put them in the `CMakeLists.txt` under `add_executable(hopfield_networks ...`