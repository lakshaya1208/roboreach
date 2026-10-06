# RoboReach

ESP32-based mobile robot with reach capability.

## Documentation & Specifications
- [`docs/FINAL_PROJECT_SPEC.md`](docs/FINAL_PROJECT_SPEC.md): **Authoritative Final Specification** based on the final BOM (`ROBOREACH BOM - Sheet1.pdf`).
- [`docs/SOFTWARE_ARCHITECTURE.md`](docs/SOFTWARE_ARCHITECTURE.md): Software architecture, data flow, and electrical dependencies.
- [`docs/PROTOTYPE_REFERENCE.md`](docs/PROTOTYPE_REFERENCE.md): Prototype test bench notes, architectural patterns, and deprecation details.


## Project Structure
- `docs/`: Authoritative hardware specifications and reference notes.
- `.agents/skills/roboreach-robot/`: Project-specific development guidelines and hardware constraints.
- `src/`: Application source files (firmware implementation pending TBD confirmations).
- `include/`: Header files, pinouts, and configurations.
- `lib/`: Modular drivers (motor drive, arm control, comms, safety).
- `tests/`: Unit and hardware verification tests:
  - [`tests/robolink_comm_test/`](tests/robolink_comm_test/README.md): RoboLink mobile app communication verification test.

