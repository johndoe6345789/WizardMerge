# WizardMerge


SEE ALSO: https://github.com/JohnDoe6345789/mergebot



PyQt6 + QML demo application that showcases a themed UI shell alongside simple
merge algorithm helpers. The project ships with a theming plugin system so you
can extend the UI palette without touching the core code.

## Features
- PyQt6 application bootstrapped from `wizardmerge.app`
- QML front-end that reads theme colors from the Python context
- Built-in light and dark themes plus an example warm plugin theme
- Simple merge algorithm utilities in `wizardmerge.algo`
- Helper scripts for environment setup and running the app

## Roadmap
See [ROADMAP.md](ROADMAP.md) for our vision and development plan to make resolving merge conflicts easier. The roadmap covers:
- Enhanced merge algorithms (three-way merge, conflict detection)
- Smart semantic merging for different file types
- Advanced visualization and UI improvements
- Git workflow integration
- AI-assisted conflict resolution

## Getting Started
1. Create a virtual environment and install dependencies:
   ```sh
   ./setup.sh
   ```

2. Launch the GUI (activates `.venv` automatically when present):
   ```sh
   ./run_app.sh
   ```

3. To install requirements into an existing environment instead of creating a
   new one:
   ```sh
   ./install_all_python.sh
   ```

## Theming
Themes live under `wizardmerge/themes`. Built-ins follow the `<name>_theme.py`
pattern. Plugin themes can be placed in `wizardmerge/themes/plugins` or in any
folder passed to `ThemeManager(extra_plugin_paths=[Path("/path/to/themes")])`.
Each theme module must expose a `Theme` instance named `theme` (or the
`warm_theme` example) with a palette mapping of color keys used by the QML UI.

## QML
The UI entry point is `wizardmerge/qml/main.qml`. It binds to a `theme` context
property injected from Python so you can use colors consistently across QML
components. Maintain two-space indentation when updating QML files.

## Algorithms
`wizardmerge/algo/merge.py` offers a deterministic `merge_pairs` function for
interleaving two sequences of lines and reporting their origins. The GUI can be
extended to call these helpers when you add inputs to the placeholder area in
the QML layout.
