# Tiwut API - 50 Comprehensive macOS Use Cases

This document details 50 real-world production use cases, system administration workflows, developer integrations, and automation tasks powered exclusively by the **Tiwut API for macOS**.

---

### Table of Contents
1. [Core Telemetry & Infrastructure (1-10)](#1-core-telemetry--infrastructure)
2. [Process & Workload Management (11-20)](#2-process--workload-management)
3. [Power, Energy & Thermal Optimization (21-30)](#3-power-energy--thermal-optimization)
4. [Desktop, Media & Hardware Controls (31-40)](#4-desktop-media--hardware-controls)
5. [Developer Workflows, Automation & DevOps (41-50)](#5-developer-workflows-automation--devops)

---

## 1. Core Telemetry & Infrastructure

### Use Case 1: Remote Mac Mini Fleet Health Monitoring
- **Description**: Query real-time aggregated hardware stats across a cluster of CI/CD build nodes.
- **Endpoint**: `GET /api/v1/overview`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/overview
  ```
- **Outcome**: Returns unified JSON containing CPU, memory, thermal state, power, storage, and network stats in a single network round-trip.

### Use Case 2: Granular Per-Core CPU Load Balancing
- **Description**: Inspect individual Performance and Efficiency core usage on Apple Silicon to identify thread bottlenecks.
- **Endpoint**: `GET /api/v1/cpu`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/cpu
  ```
- **Outcome**: Returns `core_usages` array with percentage utilization per core alongside 1m/5m/15m load averages.

### Use Case 3: Memory Pressure & Swap Exhaustion Alerting
- **Description**: Monitor macOS Mach VM statistics to trigger proactive warnings before apps face Out-Of-Memory termination.
- **Endpoint**: `GET /api/v1/memory`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/memory
  ```
- **Outcome**: Provides exact breakdown of active, wired, compressed, inactive, and swap bytes, with `memory_pressure_level`.

### Use Case 4: Prometheus & Grafana Metric Ingestion
- **Description**: Ingest native macOS hardware performance counters into enterprise Grafana dashboards.
- **Endpoint**: `GET /api/v1/metrics`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/metrics
  ```
- **Outcome**: Outputs OpenMetrics/Prometheus formatted gauges for CPU, memory, temperatures, battery, and uptime.

### Use Case 5: APFS Disk Volume Capacity Watchdog
- **Description**: Detect low storage space on internal APFS containers and mounted external NVMe scratch disks.
- **Endpoint**: `GET /api/v1/storage`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/storage
  ```
- **Outcome**: Enumerates all mounted volumes with total, free, and used byte counts and disk I/O metrics.

### Use Case 6: Network Interface Status & IP Address Lookup
- **Description**: Check if primary Wi-Fi (`en0`) or Thunderbolt Ethernet (`en5`) is active and retrieve IP/MAC addresses.
- **Endpoint**: `GET /api/v1/network`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/network
  ```
- **Outcome**: Lists all network adapters with IPv4, IPv6, MAC address, link state, and cumulative RX/TX bytes.

### Use Case 7: Network Latency & Gateway Reachability Verification
- **Description**: Perform low-overhead round-trip network latency tests directly from the macOS kernel socket layer.
- **Endpoint**: `GET /api/v1/network/ping?host=1.1.1.1`
- **cURL Command**:
  ```bash
  curl -s "http://127.0.0.1:8888/api/v1/network/ping?host=1.1.1.1"
  ```
- **Outcome**: Returns millisecond latency response indicating network connectivity health.

### Use Case 8: Hardware Inventory & Serial Number Audit
- **Description**: Retrieve hardware serial numbers, chip architecture, and model IDs for IT asset management.
- **Endpoint**: `GET /api/v1/system`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/system
  ```
- **Outcome**: Returns `serial_number`, `model_identifier`, `cpu_arch`, `os_version`, and `boot_timestamp`.

### Use Case 9: Service Liveness & Health Check
- **Description**: Kubernetes / load balancer health probe for containerized and bare-metal macOS runners.
- **Endpoint**: `GET /api/v1/health`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/health
  ```
- **Outcome**: Returns `{"status": "ok", "service": "Tiwut API"}`.

### Use Case 10: Dynamic Config Reloading without Restarts
- **Description**: Update telemetry module configurations on disk and trigger zero-downtime hot-reload.
- **Endpoint**: `POST /api/v1/config/reload`
- **cURL Command**:
  ```bash
  curl -s -X POST http://127.0.0.1:8888/api/v1/config/reload
  ```
- **Outcome**: Re-reads `config.yaml` and applies enabled/disabled module switches instantly.

---

## 2. Process & Workload Management

### Use Case 11: Real-time Process Listing & Resource Attribution
- **Description**: Enumerate top active processes by CPU and memory consumption.
- **Endpoint**: `GET /api/v1/processes`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/processes
  ```
- **Outcome**: Returns JSON array of processes with PID, user, process name, RSS bytes, CPU %, and thread count.

### Use Case 12: Targeted Rogue Process Termination
- **Description**: Gracefully terminate runaway or unresponsive processes using standard SIGTERM.
- **Endpoint**: `POST /api/v1/processes/:pid/kill`
- **cURL Command**:
  ```bash
  curl -s -X POST http://127.0.0.1:8888/api/v1/processes/1234/kill
  ```
- **Outcome**: Dispatches signal to PID and confirms process termination.

### Use Case 13: Temporary Process Freezing (SIGSTOP)
- **Description**: Freeze background compiler or render tasks when interactive foreground apps need full CPU.
- **Endpoint**: `POST /api/v1/processes/:pid/pause`
- **cURL Command**:
  ```bash
  curl -s -X POST http://127.0.0.1:8888/api/v1/processes/1234/pause
  ```
- **Outcome**: Sends SIGSTOP, placing the target process into a halted state without losing state.

### Use Case 14: Paused Process Resumption (SIGCONT)
- **Description**: Unfreeze previously paused tasks to resume computational workloads.
- **Endpoint**: `POST /api/v1/processes/:pid/resume`
- **cURL Command**:
  ```bash
  curl -s -X POST http://127.0.0.1:8888/api/v1/processes/1234/resume
  ```
- **Outcome**: Sends SIGCONT to resume execution.

### Use Case 15: Process Priority Tuning (Renice)
- **Description**: Elevate priority for real-time audio/video processing or deprioritize background batch tasks.
- **Endpoint**: `POST /api/v1/processes/:pid/priority`
- **cURL Command**:
  ```bash
  curl -s -X POST -H "Content-Type: application/json" -d '{"nice": -5}' http://127.0.0.1:8888/api/v1/processes/1234/priority
  ```
- **Outcome**: Adjusts process nice value via macOS `setpriority` syscall.

### Use Case 16: Single Process Detailed Inspection
- **Description**: Query memory mapping, thread count, and parent PID for a specific process ID.
- **Endpoint**: `GET /api/v1/processes/:pid`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/processes/1
  ```
- **Outcome**: Returns specific telemetry for the target process.

### Use Case 17: Running GUI Application Enumeration
- **Description**: List all running macOS graphical applications with bundle IDs and visibility flags.
- **Endpoint**: `GET /api/v1/apps`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/apps
  ```
- **Outcome**: Lists apps with `bundle_id`, `app_name`, `pid`, `is_active`, and `is_hidden`.

### Use Case 18: Headless Application Launching
- **Description**: Launch apps remotely via Bundle ID (e.g. `com.apple.Safari`) or absolute filesystem path.
- **Endpoint**: `POST /api/v1/apps/launch`
- **cURL Command**:
  ```bash
  curl -s -X POST -H "Content-Type: application/json" -d '{"target": "com.apple.Safari"}' http://127.0.0.1:8888/api/v1/apps/launch
  ```
- **Outcome**: Launches application via NSWorkspace.

### Use Case 19: Clean Application Quit
- **Description**: Instruct running GUI apps to exit cleanly, prompting save dialogs if necessary.
- **Endpoint**: `POST /api/v1/apps/quit`
- **cURL Command**:
  ```bash
  curl -s -X POST -H "Content-Type: application/json" -d '{"bundle_id": "com.apple.Calculator"}' http://127.0.0.1:8888/api/v1/apps/quit
  ```
- **Outcome**: Sends clean termination request to application bundle.

### Use Case 20: Window Focus & Application Activation
- **Description**: Bring a specific application window to the foreground programmatically.
- **Endpoint**: `POST /api/v1/apps/focus`
- **cURL Command**:
  ```bash
  curl -s -X POST -H "Content-Type: application/json" -d '{"bundle_id": "com.apple.Terminal"}' http://127.0.0.1:8888/api/v1/apps/focus
  ```
- **Outcome**: Activates target application and brings its windows to front.

---

## 3. Power, Energy & Thermal Optimization

### Use Case 21: Thermal Throttling & Fan Speed Monitoring
- **Description**: Check Apple Silicon thermal pressure level (Nominal, Moderate, Heavy, Trapping) and fan RPMs.
- **Endpoint**: `GET /api/v1/thermal`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/thermal
  ```
- **Outcome**: Provides temperatures in Celsius and thermal pressure state to prevent performance throttling.

### Use Case 22: Battery Capacity, Health & Degradation Tracker
- **Description**: Monitor MacBook battery health percentage, cycle counts, charging status, and remaining runtime.
- **Endpoint**: `GET /api/v1/battery`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/battery
  ```
- **Outcome**: Returns battery charge %, health %, design vs current capacity mAh, voltage, and power source.

### Use Case 23: Long-running Job Keep-Awake (Caffeinate Controller)
- **Description**: Prevent system and display sleep during heavy compilation, machine learning training, or rendering.
- **Endpoint**: `POST /api/v1/power/caffeinate`
- **cURL Command**:
  ```bash
  curl -s -X POST http://127.0.0.1:8888/api/v1/power/caffeinate
  ```
- **Outcome**: Creates an active `IOPMAssertion` preventing idle system sleep.

### Use Case 24: Sleep Assertion Release (Decaffeinate)
- **Description**: Release keep-awake assertions when batch jobs finish to allow power-saving sleep.
- **Endpoint**: `POST /api/v1/power/decaffeinate`
- **cURL Command**:
  ```bash
  curl -s -X POST http://127.0.0.1:8888/api/v1/power/decaffeinate
  ```
- **Outcome**: Releases `IOPMAssertion` and restores normal energy-saving policies.

### Use Case 25: Instant Screen Lock (Panic / Security Button)
- **Description**: Instantly lock the macOS session when walking away or receiving security trigger.
- **Endpoint**: `POST /api/v1/power/lock`
- **cURL Command**:
  ```bash
  curl -s -X POST http://127.0.0.1:8888/api/v1/power/lock
  ```
- **Outcome**: Calls native login framework to lock display session immediately.

### Use Case 26: Energy Saving Display Sleep Trigger
- **Description**: Put connected monitors to sleep without putting the host machine or background jobs to sleep.
- **Endpoint**: `POST /api/v1/display/sleep`
- **cURL Command**:
  ```bash
  curl -s -X POST http://127.0.0.1:8888/api/v1/display/sleep
  ```
- **Outcome**: Dispatches idle request to `IODisplayWrangler`.

### Use Case 27: Scheduled Overnight System Sleep
- **Description**: Put entire macOS workstation to low-power system sleep after work hours.
- **Endpoint**: `POST /api/v1/power/sleep`
- **cURL Command**:
  ```bash
  curl -s -X POST http://127.0.0.1:8888/api/v1/power/sleep
  ```
- **Outcome**: Puts machine to sleep via IOKit power management port.

### Use Case 28: Remote Scheduled Host Reboot
- **Description**: Reboot machine after kernel updates or maintenance cycles.
- **Endpoint**: `POST /api/v1/power/restart`
- **cURL Command**:
  ```bash
  curl -s -X POST http://127.0.0.1:8888/api/v1/power/restart
  ```
- **Outcome**: Triggers clean system reboot.

### Use Case 29: Controlled System Shutdown
- **Description**: Safely shut down headless server racks before UPS power depletion.
- **Endpoint**: `POST /api/v1/power/shutdown`
- **cURL Command**:
  ```bash
  curl -s -X POST http://127.0.0.1:8888/api/v1/power/shutdown
  ```
- **Outcome**: Triggers safe shutdown sequence.

### Use Case 30: Power Management Assertion Status Inspection
- **Description**: Inspect whether any processes are currently inhibiting sleep.
- **Endpoint**: `GET /api/v1/power`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/power
  ```
- **Outcome**: Returns `is_caffeinated`, assertion IDs, and sleep prevention flags.

---

## 4. Desktop, Media & Hardware Controls

### Use Case 31: Multi-Monitor Topology & Display Metrics
- **Description**: Query connected displays, native resolutions, refresh rates (ProMotion 120Hz), and scale factors.
- **Endpoint**: `GET /api/v1/display`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/display
  ```
- **Outcome**: Returns array of connected displays with dimensions, main screen flags, and brightness levels.

### Use Case 32: Display Brightness Level Adjustment
- **Description**: Synchronize external display brightness with time of day or ambient light levels.
- **Endpoint**: `POST /api/v1/display/brightness`
- **cURL Command**:
  ```bash
  curl -s -X POST -H "Content-Type: application/json" -d '{"brightness": 0.75}' http://127.0.0.1:8888/api/v1/display/brightness
  ```
- **Outcome**: Sets brightness level between 0.0 and 1.0.

### Use Case 33: CoreAudio Master Volume & Device Telemetry
- **Description**: Inspect default input/output audio devices and current volume/mute settings.
- **Endpoint**: `GET /api/v1/audio`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/audio
  ```
- **Outcome**: Returns default audio device names, volume percentage, and mute state.

### Use Case 34: Remote Master Audio Volume Adjustment
- **Description**: Adjust speaker volume programmatically from automation scripts or Stream Deck.
- **Endpoint**: `POST /api/v1/audio/volume`
- **cURL Command**:
  ```bash
  curl -s -X POST -H "Content-Type: application/json" -d '{"volume": 0.50}' http://127.0.0.1:8888/api/v1/audio/volume
  ```
- **Outcome**: Updates CoreAudio master scalar volume to 50%.

### Use Case 35: One-Click Meeting Mute Button
- **Description**: Instantly mute microphone or speakers during conference calls.
- **Endpoint**: `POST /api/v1/audio/mute`
- **cURL Command**:
  ```bash
  curl -s -X POST -H "Content-Type: application/json" -d '{"mute": true}' http://127.0.0.1:8888/api/v1/audio/mute
  ```
- **Outcome**: Sets master audio mute state.

### Use Case 36: System Clipboard Inspection
- **Description**: Read current text in NSPasteboard for developer clipboard managers.
- **Endpoint**: `GET /api/v1/clipboard`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/clipboard
  ```
- **Outcome**: Returns current clipboard string and byte length.

### Use Case 37: Programmatic Clipboard Injection
- **Description**: Push tokens, URLs, or code snippets into the macOS clipboard from external tools.
- **Endpoint**: `POST /api/v1/clipboard`
- **cURL Command**:
  ```bash
  curl -s -X POST -H "Content-Type: application/json" -d '{"text": "https://tiwut.com"}' http://127.0.0.1:8888/api/v1/clipboard
  ```
- **Outcome**: Writes text to `NSPasteboard` general pasteboard.

### Use Case 38: Sensitive Clipboard Wiper
- **Description**: Clear clipboard contents after copying passwords or credentials.
- **Endpoint**: `POST /api/v1/clipboard/clear`
- **cURL Command**:
  ```bash
  curl -s -X POST http://127.0.0.1:8888/api/v1/clipboard/clear
  ```
- **Outcome**: Empties pasteboard contents.

### Use Case 39: macOS Native Banner Notification Dispatcher
- **Description**: Post native macOS desktop notification banner from remote build servers or webhooks.
- **Endpoint**: `POST /api/v1/notifications`
- **cURL Command**:
  ```bash
  curl -s -X POST -H "Content-Type: application/json" -d '{"title": "CI/CD Pipeline", "subtitle": "Build Succeeded", "message": "All unit tests passed", "sound": "Hero"}' http://127.0.0.1:8888/api/v1/notifications
  ```
- **Outcome**: Displays native notification with sound and title.

### Use Case 40: Active Configuration Schema Query
- **Description**: Query which modules and sub-features are enabled in `config.yaml`.
- **Endpoint**: `GET /api/v1/config`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/config
  ```
- **Outcome**: Returns complete configuration tree.

---

## 5. Developer Workflows, Automation & DevOps

### Use Case 41: Local LLM & AI Model Inference Capacity Balancer
- **Description**: Check available GPU thermal budget and RAM before spinning up heavy local LLMs (e.g. Ollama/Llama.cpp).
- **Endpoint**: `GET /api/v1/overview`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/overview
  ```
- **Outcome**: Verifies memory and thermals before launching resource-heavy AI processes.

### Use Case 42: CI/CD Build Keep-Awake Hook
- **Description**: Automatically trigger caffeinate during GitHub Actions / Xcode Cloud builds and release on finish.
- **Endpoint**: `POST /api/v1/power/caffeinate`
- **cURL Command**:
  ```bash
  curl -s -X POST http://127.0.0.1:8888/api/v1/power/caffeinate
  ```
- **Outcome**: Guarantees zero sleep during long compilation tasks.

### Use Case 43: Home Assistant Mac Workstation Integration
- **Description**: Expose Mac sensors (battery, volume, caffeinate, display) to Home Assistant smart home automations.
- **Endpoint**: `GET /api/v1/overview`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/overview
  ```
- **Outcome**: Bridges Mac sensors into IoT automation pipelines.

### Use Case 44: Stream Deck Custom Control Interface
- **Description**: Bind physical hardware keys to toggle mute, lock screen, sleep displays, or trigger caffeinate.
- **Endpoint**: `POST /api/v1/audio/mute`
- **cURL Command**:
  ```bash
  curl -s -X POST -H "Content-Type: application/json" -d '{"mute": true}' http://127.0.0.1:8888/api/v1/audio/mute
  ```
- **Outcome**: Immediate hardware-triggered state toggle.

### Use Case 45: Raycast & Alfred Workflow Extensions
- **Description**: Build ultra-fast Raycast script commands using Tiwut API's local REST endpoints.
- **Endpoint**: `GET /api/v1/processes`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/processes
  ```
- **Outcome**: Feeds data into interactive search interfaces.

### Use Case 46: Docker for Mac Host Resource Tracker
- **Description**: Monitor host CPU and RAM usage to auto-scale Docker container limits dynamically.
- **Endpoint**: `GET /api/v1/memory`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/memory
  ```
- **Outcome**: Prevents VM thrashing and swap ballooning.

### Use Case 47: Zombie & Stalled Process Cleaner
- **Description**: Identify processes in Zombie state (`Z`) and terminate parent process.
- **Endpoint**: `GET /api/v1/processes`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/processes
  ```
- **Outcome**: Detects dead process trees and frees kernel process table entries.

### Use Case 48: Presentation Mode Automation
- **Description**: Trigger Do Not Disturb, prevent sleep, and set brightness to maximum for keynotes.
- **Endpoint**: `POST /api/v1/power/caffeinate`
- **cURL Command**:
  ```bash
  curl -s -X POST http://127.0.0.1:8888/api/v1/power/caffeinate
  ```
- **Outcome**: Ensures smooth presentations without interruptions.

### Use Case 49: Cross-Device Text Synchronization Hub
- **Description**: Read text from Mac clipboard and synchronize with remote iOS or Linux devices.
- **Endpoint**: `GET /api/v1/clipboard`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/clipboard
  ```
- **Outcome**: Allows cross-device clipboard sync tools.

### Use Case 50: Multi-Mac Render Farm Centralized Dashboard
- **Description**: Poll fleet of Apple Silicon Macs to distribute 3D rendering jobs (Blender / Maya) to idle machines.
- **Endpoint**: `GET /api/v1/overview`
- **cURL Command**:
  ```bash
  curl -s http://127.0.0.1:8888/api/v1/overview
  ```
- **Outcome**: Optimal job scheduling based on real-time per-node CPU/GPU and thermal load.
