# Tiwut API (for macOS)

<p align="center">
  <b>A Powerful, Native System Telemetry & Hardware Control Engine Exclusively for macOS.</b><br>
  Direct Mach Kernel & IOKit Integration • Sub-millisecond Telemetry • Granular YAML Configuration • 50 Production Use Cases • CFeel & C++ SDKs • Tiwut Web SDK Control Center
</p>

---

## ⚡ Overview

**Tiwut API** is an exclusive macOS system management, observability, and control service. Unlike slow CLI-wrapping tools, Tiwut API connects directly to macOS kernel APIs (`mach_host`, `IOKit`, `CoreGraphics`, `AppKit`, `CoreAudio`, `AudioToolbox`, `SystemConfiguration`, and `proc_pidinfo`) to provide high-throughput, low-latency system control.

### Architecture

```
                                  +---------------------------------------+
                                  |            Client Surfaces            |
                                  |  - Web UI (Tiwut Web SDK 50 Themes)   |
                                  |  - CFeel Native Programs (.cfeel)     |
                                  |  - C++17 Client SDK (CPP-Headers)     |
                                  |  - REST API / Prometheus Exporter     |
                                  +-------------------+-------------------+
                                                      |
                                                      v
                                        +----------------------------+
                                        |    Tiwut HTTP REST Core    |
                                        | (Embedded Daemon on :8888) |
                                        +--------------+-------------+
                                                       |
                             +-------------------------+-------------------------+
                             |                         |                         |
                             v                         v                         v
                   +-------------------+     +-------------------+     +-------------------+
                   |   Mach VM Kernel  |     |   Apple IOKit     |     | CoreGraphics/AppKit|
                   |  (CPU/RAM/PIDs)   |     |  (Power/Thermals) |     |  (Displays/Audio) |
                   +-------------------+     +-------------------+     +-------------------+
```

---

## 🚀 Key Highlights

- **Exclusive macOS Native Engine**: Direct Mach kernel and IOKit bindings for sub-millisecond telemetry gathering without shell overhead.
- **Granular `config.yaml`**: Every single module and telemetry data field can be toggled on/off independently.
- **50 Production Use Cases**: Complete coverage of real-world scenarios across CI/CD, remote server fleets, media controls, automation, and DevOps.
- **CFeel Language Support**: Full FFI bindings and sample applications built with the **cfeel** compiler.
- **Tiwut Web SDK UI**: Interactive, glassmorphic Control Center dashboard with 50 dynamic themes from `tiwut-web-sdk-static` and custom SVG icons from `Icon-Library`.
- **TCF Build Orchestration**: Integrated with `TCF` (Target Configuration File) task runner (`build.tcf`).
- **C++17 SDK & Benchmarking**: Integrated with `CPP-Headers` (`tiwut_hpp_time.hpp`, `tiwut_hpp_random.hpp`, `tiwut_hpp_file.hpp`).
- **Prometheus / OpenMetrics Exporter**: `/api/v1/metrics` endpoint ready for Grafana and time-series monitoring.
- **Zero Comments in Code**: Clean, production-ready code without comments.

---

## 📁 Repository Structure

```
.
├── config.yaml               # Granular module & telemetry toggle configuration
├── build.tcf                 # TCF task configuration
├── Makefile                  # Clang build orchestration
├── README.md                 # Project documentation
├── docs/
│   └── USECASES.md           # 50 Detailed Use Cases & examples
├── src/                      # Core C / Objective-C engine
│   ├── tiwut_api.h           # Unified C header interface
│   ├── config.h & config.c   # YAML config loader
│   ├── system_macos.h & .m   # macOS native Mach & IOKit subsystem
│   ├── server.h & server.c   # Embedded HTTP REST & static file server
│   └── main.c               # Service entrypoint & CLI
├── sdk/
│   ├── cfeel/                # CFeel language SDK
│   │   └── tiwut_api.cfeel
│   └── cpp/                  # C++ client SDK
├── sample/
│   ├── web/                  # Interactive Dashboard (Tiwut Web SDK + Icons)
│   │   ├── index.html
│   │   ├── app.js
│   │   ├── app.css
│   │   ├── core/
│   │   ├── themes/
│   │   ├── components/
│   │   └── icons/
│   ├── cfeel/                # CFeel sample programs
│   │   ├── monitor.cfeel
│   │   ├── process_manager.cfeel
│   │   ├── power_control.cfeel
│   │   ├── notifier.cfeel
│   │   └── bench.cfeel
│   ├── cpp/                  # C++ sample client
│   │   └── main.cpp
│   └── bash/                 # Automated scripts
│       ├── 50_usecases.sh    # Executable 50 use case test suite
│       └── quickstart.sh
└── external/                 # Cloned Tiwut ecosystem repositories
    ├── cfeel/                # Minimalist compiled language & cfeelc compiler
    ├── tiwut-web-sdk-static/ # Modular HTML/CSS/JS Web SDK
    ├── TCF/                  # Target Configuration File task runner
    ├── CPP-Headers/          # High-performance C++17 utilities
    └── Icon-Library/         # Tiwut SVG Icon Library
```

---

## 🛠️ Installation & Building

### Prerequisites
- macOS (Apple Silicon or Intel)
- Command Line Tools (`clang`, `make`)
- Rust `cargo` (optional, for building `tcf-rs`)

### Build Everything
```bash
make all
make samples
```

Or using Tiwut's `tcf` task runner:
```bash
./external/TCF/target/release/tcf-rs run build
./external/TCF/target/release/tcf-rs run samples
```

---

## 🖥️ Running the Service

### 1. Launch the API Daemon
```bash
./bin/tiwut-api-server
```

CLI options:
- `-c, --config <file>` : Specify custom configuration path (default: `config.yaml`)
- `-p, --port <port>`   : Override HTTP port (e.g., `-p 9000`)
- `-h, --help`          : Display CLI usage help

### 2. Access the Web Control Center
Open your browser to:
```
http://127.0.0.1:8888/
```
The dashboard automatically loads with **Tiwut Web SDK** glassmorphism, responsive telemetry gauges, process controls, volume and power toggles, and a 50-theme switcher.

---

## ⚙️ Configuration (`config.yaml`)

Every single subsystem can be enabled or disabled on the fly without changing application code:

```yaml
server:
  port: 8888
  host: "127.0.0.1"
  api_key: "tiwut_secret_token"
  auth_enabled: false
  cors_enabled: true
  max_connections: 128
  timeout_ms: 5000
  web_root: "sample/web"

cpu:
  enabled: true
  per_core: true
  load_average: true
  frequency: true

memory:
  enabled: true
  swap: true
  pressure: true
  breakdown: true

thermal:
  enabled: true
  temperatures: true
  fans: true
  thermal_pressure: true

battery:
  enabled: true
  health: true
  cycles: true
  time_remaining: true
  capacity: true

storage:
  enabled: true
  volumes: true
  io_stats: true

network:
  enabled: true
  interfaces: true
  throughput: true
  ping: true

processes:
  enabled: true
  list: true
  kill: true
  pause_resume: true
  renice: true
  max_list_count: 100

apps:
  enabled: true
  list: true
  launch: true
  quit: true
  focus: true

display:
  enabled: true
  brightness: true
  sleep: true

audio:
  enabled: true
  volume_control: true
  mute_control: true

power:
  enabled: true
  sleep: true
  restart: true
  shutdown: true
  caffeinate: true
  lock: true

clipboard:
  enabled: true
  read: true
  write: true
  clear: true

notifications:
  enabled: true
  banner: true

system:
  enabled: true
  uptime: true
  os_version: true
  hardware_model: true
  serial_number: true

telemetry:
  enabled: true
  prometheus_metrics: true
  overview_endpoint: true
```

---

## 📡 REST API Reference

### Telemetry Endpoints
- `GET /api/v1/health` : Health status and service information
- `GET /api/v1/overview` : Complete consolidated snapshot (CPU, RAM, Disks, Network, Battery, Thermals, System)
- `GET /api/v1/cpu` : CPU usage (user, system, idle), load averages, frequency, per-core metrics
- `GET /api/v1/memory` : Unified Memory breakdown (active, wired, compressed, inactive, free, swap, pressure)
- `GET /api/v1/thermal` : Temperatures in Celsius, thermal pressure states, fan RPMs
- `GET /api/v1/battery` : Charge level, health %, cycle count, design/max capacity, power source
- `GET /api/v1/storage` : APFS and external volume mounts, total/used/free bytes
- `GET /api/v1/network` : Interfaces (en0, lo0), IPv4/IPv6, MAC, RX/TX bytes
- `GET /api/v1/network/ping?host=1.1.1.1` : Round-trip socket latency test
- `GET /api/v1/system` : Hardware model ID, serial number, kernel version, macOS build, uptime
- `GET /api/v1/metrics` : Prometheus / OpenMetrics scrape endpoint

### Process & Application Control
- `GET /api/v1/processes` : List running processes with PID, user, name, CPU %, RSS bytes, state
- `GET /api/v1/processes/:pid` : Single process detailed telemetry
- `POST /api/v1/processes/:pid/kill` : Terminate process (SIGTERM)
- `POST /api/v1/processes/:pid/pause` : Pause process (SIGSTOP)
- `POST /api/v1/processes/:pid/resume` : Resume process (SIGCONT)
- `POST /api/v1/processes/:pid/priority` : Adjust process priority (`{"nice": -5}`)
- `GET /api/v1/apps` : Running graphical macOS applications
- `POST /api/v1/apps/launch` : Launch app (`{"target": "com.apple.Safari"}`)
- `POST /api/v1/apps/quit` : Gracefully quit app (`{"bundle_id": "com.apple.Calculator"}`)
- `POST /api/v1/apps/focus` : Focus window (`{"bundle_id": "com.apple.Terminal"}`)

### System, Power & Media Control
- `GET /api/v1/display` : Connected display dimensions, refresh rates, brightness
- `POST /api/v1/display/brightness` : Set display brightness (`{"brightness": 0.85}`)
- `POST /api/v1/display/sleep` : Put display to sleep
- `GET /api/v1/audio` : Master output/input volume, mute state
- `POST /api/v1/audio/volume` : Set master audio volume (`{"volume": 0.65}`)
- `POST /api/v1/audio/mute` : Mute audio output (`{"mute": true}`)
- `GET /api/v1/power` : Active sleep assertions and keep-awake status
- `POST /api/v1/power/caffeinate` : Prevent system & display sleep (IOPMAssertion)
- `POST /api/v1/power/decaffeinate` : Release sleep assertion
- `POST /api/v1/power/lock` : Instantly lock macOS screen session
- `POST /api/v1/power/sleep` : Trigger system sleep
- `POST /api/v1/power/restart` : Reboot machine
- `POST /api/v1/power/shutdown` : Shut down machine
- `GET /api/v1/clipboard` : Read clipboard text
- `POST /api/v1/clipboard` : Write text to clipboard (`{"text": "Sample"}`)
- `POST /api/v1/clipboard/clear` : Clear clipboard
- `POST /api/v1/notifications` : Post native desktop banner notification (`{"title": "Alert", "message": "Done"}`)
- `GET /api/v1/config` : Current configuration state
- `POST /api/v1/config/reload` : Zero-downtime hot-reload of `config.yaml`

---

## 📖 50 Production Use Cases

See the full [docs/USECASES.md](docs/USECASES.md) for detailed workflows. To run the automated 50-case test suite:

```bash
bash sample/bash/50_usecases.sh
```

---

## 💻 Language SDKs & Samples

### CFeel Sample Program
Compiled using `external/cfeel/cfeelc`:
```feel
extern fn puts(str);
extern fn tiwut_api_init();
extern fn tiwut_api_shutdown();

fn main() {
    tiwut_api_init();
    let banner = "Tiwut API CFeel Monitor";
    puts(banner);
    tiwut_api_shutdown();
}
```
Run compiled samples:
```bash
./sample/bin/cfeel_monitor
./sample/bin/cfeel_process
./sample/bin/cfeel_power
./sample/bin/cfeel_notifier
./sample/bin/cfeel_bench
```

### C++17 Sample Program
Integrated with `CPP-Headers`:
```bash
./sample/bin/cpp_sample
```

---

## 📜 License

MIT License. Built exclusively for macOS.
