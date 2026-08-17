use serde::Deserialize;
use std::collections::HashMap;

#[derive(Debug, Deserialize)]
pub struct Tcf {
    pub project: Project,
    #[serde(default)]
    pub env: HashMap<String, String>,
    #[serde(default)]
    pub tasks: HashMap<String, Task>,
}

#[derive(Debug, Deserialize)]
pub struct Project {
    pub name: String,
    pub version: String,
    pub language: String,
}

#[derive(Debug, Deserialize, Default)]
pub struct Task {
    pub description: Option<String>,
    pub command: Option<String>,
    #[serde(default)]
    pub depends_on: Vec<String>,
    
    pub linux: Option<OsOverride>,
    pub windows: Option<OsOverride>,
    pub mac: Option<OsOverride>,
}

#[derive(Debug, Deserialize)]
pub struct OsOverride {
    pub command: Option<String>,
}

impl Tcf {
    pub fn parse(content: &str) -> Result<Self, toml::de::Error> {
        toml::from_str(content)
    }
}
