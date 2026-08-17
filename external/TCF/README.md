# TCF (Target Configuration File)

TCF is a modern, cross-platform task runner and configuration format designed to replace makefiles and scattered scripts across your repositories. It uses a clean, TOML-based syntax (`.tcf`) to define project metadata, environment variables, and execution tasks.

With TCF, you can define a single task that executes differently depending on the operating system (Windows, Mac, or Linux), and you can string tasks together using dependency chains.

## Features

- **Cross-Platform:** Write one config file. TCF automatically detects your OS and runs the correct command for Windows, macOS, or Linux.
- **Dependency Management:** Define `depends_on` to ensure prerequisites (like `clean` or `lint`) run before `build`.
- **Environment Variables:** Define global environment variables in `[env]` that are automatically injected into every task's process.
- **Language Agnostic:** Works for Rust, C++, Java, Node.js, Python, or any other buildable project.
- **Native SDKs:** Includes SDKs for Rust, Bash, and PowerShell so you can easily extract configuration values or invoke tasks programmatically.

## Installation

### Rust CLI (Core Executor)
Clone this repository and build the core CLI:
```bash
cargo build --release
```
The binary will be located at `target/release/tcf-rs` (you can rename it to `tcf` and add it to your PATH).

## Usage

### The CLI
```bash
# Run a specific task (and its dependencies)
tcf run build

# List all available tasks
tcf list

# Get a specific configuration value (great for scripts)
tcf get project.version
tcf get env.APP_ENV
```

### The SDKs

#### Bash SDK
Source the `tcf.sh` script in your bash files:
```bash
source sdk/bash/tcf.sh

# Extract a value
VERSION=$(tcf_get project.version)
echo "Building version $VERSION..."

# Run a task
tcf_run build
```

#### PowerShell SDK
Import the `tcf.psm1` module in your PowerShell scripts:
```powershell
Import-Module ./sdk/powershell/tcf.psm1

# Extract a value
$Version = Get-TcfValue project.version
Write-Host "Building version $Version..."

# Run a task
Invoke-TcfTask build
```

## The `.tcf` File Format

Here is a large, comprehensive example of a `build.tcf` file:

```toml
[project]
name = "enterprise_demo_app"
version = "2.5.0"
language = "rust"

[env]
APP_ENV = "production"
LOG_LEVEL = "info"
CARGO_TERM_COLOR = "always"
RUST_BACKTRACE = "1"
PORT = "8080"

[tasks.clean]
command = "cargo clean"

[tasks.clean.windows]
command = "cargo clean && del /Q /S target\\"

[tasks.clean.mac]
command = "cargo clean && rm -rf target/"

[tasks.clean.linux]
command = "cargo clean && rm -rf target/"

[tasks.lint]
depends_on = ["clean"]
command = "cargo clippy -- -D warnings"

[tasks.format]
depends_on = ["lint"]
command = "cargo fmt -- --check"

[tasks.build]
depends_on = ["format"]
command = "cargo build --release"

[tasks.build.windows]
command = "cargo build --release --target x86_64-pc-windows-msvc"

[tasks.build.mac]
command = "cargo build --release --target aarch64-apple-darwin"

[tasks.build.linux]
command = "cargo build --release --target x86_64-unknown-linux-gnu"

[tasks.test]
depends_on = ["build"]
command = "cargo test --release"

[tasks.test.windows]
command = "cargo test --release --target x86_64-pc-windows-msvc"

[tasks.package]
depends_on = ["test"]
command = "tar -czf release.tar.gz target/release/enterprise_demo_app"

[tasks.package.windows]
command = "powershell -Command \"Compress-Archive -Path target\\release\\enterprise_demo_app.exe -DestinationPath release.zip\""

[tasks.deploy]
depends_on = ["package"]
command = "echo Deploying $APP_ENV version $PORT..."

[tasks.deploy.windows]
command = "echo Deploying %APP_ENV% version on port %PORT%..."

[tasks.all]
depends_on = ["deploy"]
command = "echo All tasks completed successfully!"
```

### Explanation of the Format

1. **`[project]` block:** Metadata about your project. Accessible via `tcf get project.<key>`.
2. **`[env]` block:** Environment variables. These are automatically exported to the shell before running any task. Accessible via `tcf get env.<key>`.
3. **`[tasks.<name>]` block:** Defines a task.
   - `command`: The default shell command to run.
   - `depends_on`: An array of task names that must execute successfully before this task runs.
4. **`[tasks.<name>.<os>]` block:** OS-specific overrides. Replace `<os>` with `windows`, `mac`, or `linux`. If TCF detects it is running on that OS, it will use this command instead of the default `command`.
