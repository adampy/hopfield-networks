# Evaluation of Storage Prescriptions in Hopfield Networks
A C++ application for visualising and experimenting with Hopfield networks, built using GLFW and OpenGL.

## Authors
- Adam Crawley (201490505)
- William Kingdon (201501499)
- Rahim Karim (201519128)

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

## Usage
General workflow:
- Start by creating a network in **Network** (set `Rows`/`Columns`, then `Create Network`)
- If necessary, use **Display** to change visual scale.
- Add memories in **Activations / Memories** using `Save Activations as New Memory (and relearn)`
- Stored memories, and current overlaps, can be viewed in the **Activations / Memories** panel.
- Use **Learning** to switch storage prescriptions.
- Use **Inference** to test recall (optionally infuse noise first) by running inference passes of different types.
- Use **Experiments** to generate CSV outputs under `data/experiment*/...`. While an experiment is running, a separate `Experiment Progress` window shows a progress bar and the controls are disabled.

In the visualisation, left clicking sets activations to 1, and right clicking sets them to 0.

### `state.hpf`
This file persists a network and its memories for later experimentation.
- `Load from state.hpf` loads a previously saved network state from the working directory.
- `Save to state.hpf` overwrites the current `state.hpf` with the current network size, prescription, and memories.
- A typical workflow is:
  Create a network -> add memories -> save to `state.hpf` -> later reload and continue experimenting.

## Debugging
If running through VSCode, the automatic build system may cause the solution not to run and give an error such as
```
CMake Error: Error: generator : Ninja
Does not match the generator used previously: Unix Makefiles
```
If this is the case, `rm build -rf`, and rerun the build script.

## Development
For new files (`.cpp` always, `.hpp` is not *required* but encouraged), make sure to put them in the `CMakeLists.txt` under `add_executable(hopfield_networks ...`

## Experiments
Experiment data used within the final report:
- Experiment 1 `data/experiment1/Exp1_Sizes6_Trials20_Theta0.90`
- Experiment 1 (second diagram): `data/experiment1/Exp1_Sizes6_Trials20_Theta0.90 NO NOISE`
- Experiment 2: `data/experiment2/Exp2_N100_Trials50_Theta0.95`
- Experiment 3: `data/experiment3/N100_Trials50_MaxP70_MaxNoise0.405_NoiseRes0.010000_Theta0.95`

To visualise experiments:
- Experiment 1: `python experiment1_viewer.py <FOLDER>`
- Experiment 2: `python experiment2_viewer.py <FOLDER>`
- Experiment 3: `python heatmap_viewer.py data/experiment3/<RUN_DIR>`
Additional arguments can be supplied for formatting purposes.

## References
ImGUI for visualisation of settings: https://github.com/ocornut/imgui
Eigen for matrix operations: https://libeigen.gitlab.io/
OpenGL / GLFW for rendering: https://www.opengl.org/