#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

TiwutConfig g_config;
static char g_config_path[512] = "config.yaml";

static void trim_string(char *str) {
    char *start = str;
    while (isspace((unsigned char)*start)) start++;
    if (*start == 0) {
        *str = 0;
        return;
    }
    char *end = start + strlen(start) - 1;
    while (end > start && (isspace((unsigned char)*end) || *end == '\r' || *end == '\n' || *end == '"' || *end == '\'')) {
        *end = 0;
        end--;
    }
    if (*start == '"' || *start == '\'') start++;
    memmove(str, start, strlen(start) + 1);
}

static bool parse_bool(const char *val) {
    if (!val) return false;
    if (strcasecmp(val, "true") == 0 || strcasecmp(val, "yes") == 0 || strcasecmp(val, "1") == 0 || strcasecmp(val, "on") == 0) {
        return true;
    }
    return false;
}

void tiwut_config_init_defaults(TiwutConfig *cfg) {
    memset(cfg, 0, sizeof(TiwutConfig));

    cfg->server.port = 8888;
    strncpy(cfg->server.host, "127.0.0.1", sizeof(cfg->server.host) - 1);
    strncpy(cfg->server.api_key, "tiwut_secret_token", sizeof(cfg->server.api_key) - 1);
    cfg->server.auth_enabled = false;
    cfg->server.cors_enabled = true;
    cfg->server.max_connections = 128;
    cfg->server.timeout_ms = 5000;
    strncpy(cfg->server.web_root, "sample/web", sizeof(cfg->server.web_root) - 1);

    cfg->cpu.enabled = true;
    cfg->cpu.per_core = true;
    cfg->cpu.load_average = true;
    cfg->cpu.frequency = true;

    cfg->memory.enabled = true;
    cfg->memory.swap = true;
    cfg->memory.pressure = true;
    cfg->memory.breakdown = true;

    cfg->thermal.enabled = true;
    cfg->thermal.temperatures = true;
    cfg->thermal.fans = true;
    cfg->thermal.thermal_pressure = true;

    cfg->battery.enabled = true;
    cfg->battery.health = true;
    cfg->battery.cycles = true;
    cfg->battery.time_remaining = true;
    cfg->battery.capacity = true;

    cfg->storage.enabled = true;
    cfg->storage.volumes = true;
    cfg->storage.io_stats = true;

    cfg->network.enabled = true;
    cfg->network.interfaces = true;
    cfg->network.throughput = true;
    cfg->network.ping = true;

    cfg->processes.enabled = true;
    cfg->processes.list = true;
    cfg->processes.kill = true;
    cfg->processes.pause_resume = true;
    cfg->processes.renice = true;
    cfg->processes.max_list_count = 100;

    cfg->apps.enabled = true;
    cfg->apps.list = true;
    cfg->apps.launch = true;
    cfg->apps.quit = true;
    cfg->apps.focus = true;

    cfg->display.enabled = true;
    cfg->display.brightness = true;
    cfg->display.sleep = true;

    cfg->audio.enabled = true;
    cfg->audio.volume_control = true;
    cfg->audio.mute_control = true;

    cfg->power.enabled = true;
    cfg->power.sleep = true;
    cfg->power.restart = true;
    cfg->power.shutdown = true;
    cfg->power.caffeinate = true;
    cfg->power.lock = true;

    cfg->clipboard.enabled = true;
    cfg->clipboard.read = true;
    cfg->clipboard.write = true;
    cfg->clipboard.clear = true;

    cfg->notifications.enabled = true;
    cfg->notifications.banner = true;

    cfg->system.enabled = true;
    cfg->system.uptime = true;
    cfg->system.os_version = true;
    cfg->system.hardware_model = true;
    cfg->system.serial_number = true;

    cfg->telemetry.enabled = true;
    cfg->telemetry.prometheus_metrics = true;
    cfg->telemetry.overview_endpoint = true;
}

bool tiwut_config_load_file(TiwutConfig *cfg, const char *filepath) {
    if (filepath) {
        strncpy(g_config_path, filepath, sizeof(g_config_path) - 1);
    }
    FILE *f = fopen(g_config_path, "r");
    if (!f) {
        return false;
    }

    char line[512];
    char section[128] = "";

    while (fgets(line, sizeof(line), f)) {
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '#' || *p == '\r' || *p == '\n' || *p == 0) {
            continue;
        }

        int indent = (int)(p - line);
        char *colon = strchr(p, ':');
        if (!colon) continue;

        *colon = 0;
        char key[128];
        char val[256];
        strncpy(key, p, sizeof(key) - 1);
        key[sizeof(key) - 1] = 0;
        trim_string(key);

        char *val_start = colon + 1;
        while (isspace((unsigned char)*val_start)) val_start++;
        char *val_comment = strchr(val_start, '#');
        if (val_comment) *val_comment = 0;
        strncpy(val, val_start, sizeof(val) - 1);
        val[sizeof(val) - 1] = 0;
        trim_string(val);

        if (indent == 0 && strlen(val) == 0) {
            strncpy(section, key, sizeof(section) - 1);
            continue;
        }

        char full_key[256];
        if (indent > 0 && strlen(section) > 0) {
            snprintf(full_key, sizeof(full_key), "%s.%s", section, key);
        } else {
            strncpy(full_key, key, sizeof(full_key) - 1);
        }

        if (strcmp(full_key, "server.port") == 0) {
            cfg->server.port = atoi(val);
        } else if (strcmp(full_key, "server.host") == 0) {
            strncpy(cfg->server.host, val, sizeof(cfg->server.host) - 1);
        } else if (strcmp(full_key, "server.api_key") == 0) {
            strncpy(cfg->server.api_key, val, sizeof(cfg->server.api_key) - 1);
        } else if (strcmp(full_key, "server.auth_enabled") == 0) {
            cfg->server.auth_enabled = parse_bool(val);
        } else if (strcmp(full_key, "server.cors_enabled") == 0) {
            cfg->server.cors_enabled = parse_bool(val);
        } else if (strcmp(full_key, "server.max_connections") == 0) {
            cfg->server.max_connections = atoi(val);
        } else if (strcmp(full_key, "server.timeout_ms") == 0) {
            cfg->server.timeout_ms = atoi(val);
        } else if (strcmp(full_key, "server.web_root") == 0) {
            strncpy(cfg->server.web_root, val, sizeof(cfg->server.web_root) - 1);
        } else if (strcmp(full_key, "cpu.enabled") == 0) {
            cfg->cpu.enabled = parse_bool(val);
        } else if (strcmp(full_key, "cpu.per_core") == 0) {
            cfg->cpu.per_core = parse_bool(val);
        } else if (strcmp(full_key, "cpu.load_average") == 0) {
            cfg->cpu.load_average = parse_bool(val);
        } else if (strcmp(full_key, "cpu.frequency") == 0) {
            cfg->cpu.frequency = parse_bool(val);
        } else if (strcmp(full_key, "memory.enabled") == 0) {
            cfg->memory.enabled = parse_bool(val);
        } else if (strcmp(full_key, "memory.swap") == 0) {
            cfg->memory.swap = parse_bool(val);
        } else if (strcmp(full_key, "memory.pressure") == 0) {
            cfg->memory.pressure = parse_bool(val);
        } else if (strcmp(full_key, "memory.breakdown") == 0) {
            cfg->memory.breakdown = parse_bool(val);
        } else if (strcmp(full_key, "thermal.enabled") == 0) {
            cfg->thermal.enabled = parse_bool(val);
        } else if (strcmp(full_key, "thermal.temperatures") == 0) {
            cfg->thermal.temperatures = parse_bool(val);
        } else if (strcmp(full_key, "thermal.fans") == 0) {
            cfg->thermal.fans = parse_bool(val);
        } else if (strcmp(full_key, "thermal.thermal_pressure") == 0) {
            cfg->thermal.thermal_pressure = parse_bool(val);
        } else if (strcmp(full_key, "battery.enabled") == 0) {
            cfg->battery.enabled = parse_bool(val);
        } else if (strcmp(full_key, "battery.health") == 0) {
            cfg->battery.health = parse_bool(val);
        } else if (strcmp(full_key, "battery.cycles") == 0) {
            cfg->battery.cycles = parse_bool(val);
        } else if (strcmp(full_key, "battery.time_remaining") == 0) {
            cfg->battery.time_remaining = parse_bool(val);
        } else if (strcmp(full_key, "battery.capacity") == 0) {
            cfg->battery.capacity = parse_bool(val);
        } else if (strcmp(full_key, "storage.enabled") == 0) {
            cfg->storage.enabled = parse_bool(val);
        } else if (strcmp(full_key, "storage.volumes") == 0) {
            cfg->storage.volumes = parse_bool(val);
        } else if (strcmp(full_key, "storage.io_stats") == 0) {
            cfg->storage.io_stats = parse_bool(val);
        } else if (strcmp(full_key, "network.enabled") == 0) {
            cfg->network.enabled = parse_bool(val);
        } else if (strcmp(full_key, "network.interfaces") == 0) {
            cfg->network.interfaces = parse_bool(val);
        } else if (strcmp(full_key, "network.throughput") == 0) {
            cfg->network.throughput = parse_bool(val);
        } else if (strcmp(full_key, "network.ping") == 0) {
            cfg->network.ping = parse_bool(val);
        } else if (strcmp(full_key, "processes.enabled") == 0) {
            cfg->processes.enabled = parse_bool(val);
        } else if (strcmp(full_key, "processes.list") == 0) {
            cfg->processes.list = parse_bool(val);
        } else if (strcmp(full_key, "processes.kill") == 0) {
            cfg->processes.kill = parse_bool(val);
        } else if (strcmp(full_key, "processes.pause_resume") == 0) {
            cfg->processes.pause_resume = parse_bool(val);
        } else if (strcmp(full_key, "processes.renice") == 0) {
            cfg->processes.renice = parse_bool(val);
        } else if (strcmp(full_key, "processes.max_list_count") == 0) {
            cfg->processes.max_list_count = atoi(val);
        } else if (strcmp(full_key, "apps.enabled") == 0) {
            cfg->apps.enabled = parse_bool(val);
        } else if (strcmp(full_key, "apps.list") == 0) {
            cfg->apps.list = parse_bool(val);
        } else if (strcmp(full_key, "apps.launch") == 0) {
            cfg->apps.launch = parse_bool(val);
        } else if (strcmp(full_key, "apps.quit") == 0) {
            cfg->apps.quit = parse_bool(val);
        } else if (strcmp(full_key, "apps.focus") == 0) {
            cfg->apps.focus = parse_bool(val);
        } else if (strcmp(full_key, "display.enabled") == 0) {
            cfg->display.enabled = parse_bool(val);
        } else if (strcmp(full_key, "display.brightness") == 0) {
            cfg->display.brightness = parse_bool(val);
        } else if (strcmp(full_key, "display.sleep") == 0) {
            cfg->display.sleep = parse_bool(val);
        } else if (strcmp(full_key, "audio.enabled") == 0) {
            cfg->audio.enabled = parse_bool(val);
        } else if (strcmp(full_key, "audio.volume_control") == 0) {
            cfg->audio.volume_control = parse_bool(val);
        } else if (strcmp(full_key, "audio.mute_control") == 0) {
            cfg->audio.mute_control = parse_bool(val);
        } else if (strcmp(full_key, "power.enabled") == 0) {
            cfg->power.enabled = parse_bool(val);
        } else if (strcmp(full_key, "power.sleep") == 0) {
            cfg->power.sleep = parse_bool(val);
        } else if (strcmp(full_key, "power.restart") == 0) {
            cfg->power.restart = parse_bool(val);
        } else if (strcmp(full_key, "power.shutdown") == 0) {
            cfg->power.shutdown = parse_bool(val);
        } else if (strcmp(full_key, "power.caffeinate") == 0) {
            cfg->power.caffeinate = parse_bool(val);
        } else if (strcmp(full_key, "power.lock") == 0) {
            cfg->power.lock = parse_bool(val);
        } else if (strcmp(full_key, "clipboard.enabled") == 0) {
            cfg->clipboard.enabled = parse_bool(val);
        } else if (strcmp(full_key, "clipboard.read") == 0) {
            cfg->clipboard.read = parse_bool(val);
        } else if (strcmp(full_key, "clipboard.write") == 0) {
            cfg->clipboard.write = parse_bool(val);
        } else if (strcmp(full_key, "clipboard.clear") == 0) {
            cfg->clipboard.clear = parse_bool(val);
        } else if (strcmp(full_key, "notifications.enabled") == 0) {
            cfg->notifications.enabled = parse_bool(val);
        } else if (strcmp(full_key, "notifications.banner") == 0) {
            cfg->notifications.banner = parse_bool(val);
        } else if (strcmp(full_key, "system.enabled") == 0) {
            cfg->system.enabled = parse_bool(val);
        } else if (strcmp(full_key, "system.uptime") == 0) {
            cfg->system.uptime = parse_bool(val);
        } else if (strcmp(full_key, "system.os_version") == 0) {
            cfg->system.os_version = parse_bool(val);
        } else if (strcmp(full_key, "system.hardware_model") == 0) {
            cfg->system.hardware_model = parse_bool(val);
        } else if (strcmp(full_key, "system.serial_number") == 0) {
            cfg->system.serial_number = parse_bool(val);
        } else if (strcmp(full_key, "telemetry.enabled") == 0) {
            cfg->telemetry.enabled = parse_bool(val);
        } else if (strcmp(full_key, "telemetry.prometheus_metrics") == 0) {
            cfg->telemetry.prometheus_metrics = parse_bool(val);
        } else if (strcmp(full_key, "telemetry.overview_endpoint") == 0) {
            cfg->telemetry.overview_endpoint = parse_bool(val);
        }
    }

    fclose(f);
    return true;
}

bool tiwut_config_reload(const char *filepath) {
    TiwutConfig new_cfg;
    tiwut_config_init_defaults(&new_cfg);
    if (tiwut_config_load_file(&new_cfg, filepath ? filepath : g_config_path)) {
        g_config = new_cfg;
        return true;
    }
    return false;
}

void tiwut_config_to_json(const TiwutConfig *cfg, char *out_json, int max_len) {
    snprintf(out_json, max_len,
        "{\n"
        "  \"server\": {\"port\": %d, \"host\": \"%s\", \"auth_enabled\": %s, \"cors_enabled\": %s},\n"
        "  \"modules\": {\n"
        "    \"cpu\": {\"enabled\": %s, \"per_core\": %s, \"load_average\": %s, \"frequency\": %s},\n"
        "    \"memory\": {\"enabled\": %s, \"swap\": %s, \"pressure\": %s, \"breakdown\": %s},\n"
        "    \"thermal\": {\"enabled\": %s, \"temperatures\": %s, \"fans\": %s, \"thermal_pressure\": %s},\n"
        "    \"battery\": {\"enabled\": %s, \"health\": %s, \"cycles\": %s, \"time_remaining\": %s, \"capacity\": %s},\n"
        "    \"storage\": {\"enabled\": %s, \"volumes\": %s, \"io_stats\": %s},\n"
        "    \"network\": {\"enabled\": %s, \"interfaces\": %s, \"throughput\": %s, \"ping\": %s},\n"
        "    \"processes\": {\"enabled\": %s, \"list\": %s, \"kill\": %s, \"pause_resume\": %s, \"renice\": %s, \"max_list_count\": %d},\n"
        "    \"apps\": {\"enabled\": %s, \"list\": %s, \"launch\": %s, \"quit\": %s, \"focus\": %s},\n"
        "    \"display\": {\"enabled\": %s, \"brightness\": %s, \"sleep\": %s},\n"
        "    \"audio\": {\"enabled\": %s, \"volume_control\": %s, \"mute_control\": %s},\n"
        "    \"power\": {\"enabled\": %s, \"sleep\": %s, \"restart\": %s, \"shutdown\": %s, \"caffeinate\": %s, \"lock\": %s},\n"
        "    \"clipboard\": {\"enabled\": %s, \"read\": %s, \"write\": %s, \"clear\": %s},\n"
        "    \"notifications\": {\"enabled\": %s, \"banner\": %s},\n"
        "    \"system\": {\"enabled\": %s, \"uptime\": %s, \"os_version\": %s, \"hardware_model\": %s, \"serial_number\": %s},\n"
        "    \"telemetry\": {\"enabled\": %s, \"prometheus_metrics\": %s, \"overview_endpoint\": %s}\n"
        "  }\n"
        "}",
        cfg->server.port, cfg->server.host, cfg->server.auth_enabled ? "true" : "false", cfg->server.cors_enabled ? "true" : "false",
        cfg->cpu.enabled ? "true" : "false", cfg->cpu.per_core ? "true" : "false", cfg->cpu.load_average ? "true" : "false", cfg->cpu.frequency ? "true" : "false",
        cfg->memory.enabled ? "true" : "false", cfg->memory.swap ? "true" : "false", cfg->memory.pressure ? "true" : "false", cfg->memory.breakdown ? "true" : "false",
        cfg->thermal.enabled ? "true" : "false", cfg->thermal.temperatures ? "true" : "false", cfg->thermal.fans ? "true" : "false", cfg->thermal.thermal_pressure ? "true" : "false",
        cfg->battery.enabled ? "true" : "false", cfg->battery.health ? "true" : "false", cfg->battery.cycles ? "true" : "false", cfg->battery.time_remaining ? "true" : "false", cfg->battery.capacity ? "true" : "false",
        cfg->storage.enabled ? "true" : "false", cfg->storage.volumes ? "true" : "false", cfg->storage.io_stats ? "true" : "false",
        cfg->network.enabled ? "true" : "false", cfg->network.interfaces ? "true" : "false", cfg->network.throughput ? "true" : "false", cfg->network.ping ? "true" : "false",
        cfg->processes.enabled ? "true" : "false", cfg->processes.list ? "true" : "false", cfg->processes.kill ? "true" : "false", cfg->processes.pause_resume ? "true" : "false", cfg->processes.renice ? "true" : "false", cfg->processes.max_list_count,
        cfg->apps.enabled ? "true" : "false", cfg->apps.list ? "true" : "false", cfg->apps.launch ? "true" : "false", cfg->apps.quit ? "true" : "false", cfg->apps.focus ? "true" : "false",
        cfg->display.enabled ? "true" : "false", cfg->display.brightness ? "true" : "false", cfg->display.sleep ? "true" : "false",
        cfg->audio.enabled ? "true" : "false", cfg->audio.volume_control ? "true" : "false", cfg->audio.mute_control ? "true" : "false",
        cfg->power.enabled ? "true" : "false", cfg->power.sleep ? "true" : "false", cfg->power.restart ? "true" : "false", cfg->power.shutdown ? "true" : "false", cfg->power.caffeinate ? "true" : "false", cfg->power.lock ? "true" : "false",
        cfg->clipboard.enabled ? "true" : "false", cfg->clipboard.read ? "true" : "false", cfg->clipboard.write ? "true" : "false", cfg->clipboard.clear ? "true" : "false",
        cfg->notifications.enabled ? "true" : "false", cfg->notifications.banner ? "true" : "false",
        cfg->system.enabled ? "true" : "false", cfg->system.uptime ? "true" : "false", cfg->system.os_version ? "true" : "false", cfg->system.hardware_model ? "true" : "false", cfg->system.serial_number ? "true" : "false",
        cfg->telemetry.enabled ? "true" : "false", cfg->telemetry.prometheus_metrics ? "true" : "false", cfg->telemetry.overview_endpoint ? "true" : "false"
    );
}
