use anyhow::{Context, Result};
use clap::{Parser, Subcommand};
use std::collections::HashSet;
use std::env;
use std::fs;
use std::process::Command;
use tcf_rs::Tcf;

#[derive(Parser)]
#[command(author, version, about = "TCF (Target Configuration File) Task Runner", long_about = None)]
struct Cli {
    #[arg(short, long, default_value = "build.tcf")]
    file: String,

    #[command(subcommand)]
    command: Commands,
}

#[derive(Subcommand)]
enum Commands {
    Run {
        task_name: String,
    },
    List,
    Get {
        key: String,
    },
}

fn main() -> Result<()> {
    let cli = Cli::parse();

    let content = fs::read_to_string(&cli.file)
        .with_context(|| format!("Failed to read {}", cli.file))?;
    
    let tcf = Tcf::parse(&content)
        .with_context(|| "Failed to parse .tcf file")?;

    match &cli.command {
        Commands::Run { task_name } => {
            let mut visited = HashSet::new();
            run_task(task_name, &tcf, &mut visited)?;
        }
        Commands::List => {
            println!("Project: {} v{}", tcf.project.name, tcf.project.version);
            println!("Available tasks:");
            for (name, task) in &tcf.tasks {
                let desc = task.description.as_deref().unwrap_or("No description");
                println!("  - {}: {}", name, desc);
            }
        }
        Commands::Get { key } => {
            let val = match key.as_str() {
                "project.name" => Some(tcf.project.name.clone()),
                "project.version" => Some(tcf.project.version.clone()),
                "project.language" => Some(tcf.project.language.clone()),
                k if k.starts_with("env.") => {
                    let env_key = &k[4..];
                    tcf.env.get(env_key).cloned()
                }
                _ => None,
            };
            
            if let Some(v) = val {
                print!("{}", v);
            } else {
                anyhow::bail!("Key '{}' not found", key);
            }
        }
    }

    Ok(())
}

fn run_task(task_name: &str, tcf: &Tcf, visited: &mut HashSet<String>) -> Result<()> {
    if visited.contains(task_name) {
        return Ok(());
    }
    visited.insert(task_name.to_string());

    let task = tcf.tasks.get(task_name)
        .with_context(|| format!("Task '{}' not found", task_name))?;

    for dep in &task.depends_on {
        run_task(dep, tcf, visited)?;
    }

    let os = env::consts::OS;
    let cmd_str = match os {
        "windows" => task.windows.as_ref().and_then(|o| o.command.as_deref()),
        "macos" => task.mac.as_ref().and_then(|o| o.command.as_deref()),
        "linux" => task.linux.as_ref().and_then(|o| o.command.as_deref()),
        _ => None,
    };
    
    let cmd_str = cmd_str.or(task.command.as_deref());

    if let Some(cmd) = cmd_str {
        println!("--> Running task '{}': {}", task_name, cmd);
        
        let mut process = if os == "windows" {
            let mut p = Command::new("cmd");
            p.arg("/C").arg(cmd);
            p
        } else {
            let mut p = Command::new("sh");
            p.arg("-c").arg(cmd);
            p
        };

        for (k, v) in &tcf.env {
            process.env(k, v);
        }

        let status = process.status()
            .with_context(|| format!("Failed to execute command: {}", cmd))?;

        if !status.success() {
            anyhow::bail!("Task '{}' failed with {}", task_name, status);
        }
    } else {
        println!("--> Task '{}' has no command for OS '{}'", task_name, os);
    }

    Ok(())
}

