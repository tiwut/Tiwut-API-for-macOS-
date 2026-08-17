#ifndef TIWUT_CONFIG_H
#define TIWUT_CONFIG_H

#include <stdbool.h>

typedef struct {
    int port;
    char host[64];
    char api_key[128];
    bool auth_enabled;
    bool cors_enabled;
    int max_connections;
    int timeout_ms;
    char web_root[256];
} ServerConfig;

typedef struct {
    bool enabled;
    bool per_core;
    bool load_average;
    bool frequency;
} CpuConfig;

typedef struct {
    bool enabled;
    bool swap;
    bool pressure;
    bool breakdown;
} MemoryConfig;

typedef struct {
    bool enabled;
    bool temperatures;
    bool fans;
    bool thermal_pressure;
} ThermalConfig;

typedef struct {
    bool enabled;
    bool health;
    bool cycles;
    bool time_remaining;
    bool capacity;
} BatteryConfig;

typedef struct {
    bool enabled;
    bool volumes;
    bool io_stats;
} StorageConfig;

typedef struct {
    bool enabled;
    bool interfaces;
    bool throughput;
    bool ping;
} NetworkConfig;

typedef struct {
    bool enabled;
    bool list;
    bool kill;
    bool pause_resume;
    bool renice;
    int max_list_count;
} ProcessConfig;

typedef struct {
    bool enabled;
    bool list;
    bool launch;
    bool quit;
    bool focus;
} AppConfig;

typedef struct {
    bool enabled;
    bool brightness;
    bool sleep;
} DisplayConfig;

typedef struct {
    bool enabled;
    bool volume_control;
    bool mute_control;
} AudioConfig;

typedef struct {
    bool enabled;
    bool sleep;
    bool restart;
    bool shutdown;
    bool caffeinate;
    bool lock;
} PowerConfig;

typedef struct {
    bool enabled;
    bool read;
    bool write;
    bool clear;
} ClipboardConfig;

typedef struct {
    bool enabled;
    bool banner;
} NotificationConfig;

typedef struct {
    bool enabled;
    bool uptime;
    bool os_version;
    bool hardware_model;
    bool serial_number;
} SystemConfig;

typedef struct {
    bool enabled;
    bool prometheus_metrics;
    bool overview_endpoint;
} TelemetryConfig;

typedef struct {
    ServerConfig server;
    CpuConfig cpu;
    MemoryConfig memory;
    ThermalConfig thermal;
    BatteryConfig battery;
    StorageConfig storage;
    NetworkConfig network;
    ProcessConfig processes;
    AppConfig apps;
    DisplayConfig display;
    AudioConfig audio;
    PowerConfig power;
    ClipboardConfig clipboard;
    NotificationConfig notifications;
    SystemConfig system;
    TelemetryConfig telemetry;
} TiwutConfig;

extern TiwutConfig g_config;

void tiwut_config_init_defaults(TiwutConfig *cfg);
bool tiwut_config_load_file(TiwutConfig *cfg, const char *filepath);
bool tiwut_config_reload(const char *filepath);
void tiwut_config_to_json(const TiwutConfig *cfg, char *out_json, int max_len);

#endif
