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