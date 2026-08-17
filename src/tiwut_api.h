#ifndef TIWUT_API_H
#define TIWUT_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
    double user_percent;
    double system_percent;
    double idle_percent;
    double total_percent;
    int physical_cores;
    int logical_cores;
    int perf_cores;
    int efficiency_cores;
    double load_avg_1m;
    double load_avg_5m;
    double load_avg_15m;
    uint64_t cpu_frequency_hz;
    char brand_string[128];
    int core_count;
    double core_usages[64];
} TiwutCpuStats;

typedef struct {
    uint64_t total_bytes;
    uint64_t free_bytes;
    uint64_t active_bytes;
    uint64_t inactive_bytes;
    uint64_t wired_bytes;
    uint64_t compressed_bytes;
    uint64_t purgeable_bytes;
    uint64_t swap_total_bytes;
    uint64_t swap_used_bytes;
    uint64_t swap_free_bytes;
    int memory_pressure_level;
    double used_percent;
} TiwutMemoryStats;

typedef struct {
    double cpu_temp_celsius;
    double gpu_temp_celsius;
    double battery_temp_celsius;
    int thermal_pressure_level;
    char thermal_state[32];
    int fan_count;
    int fan_speeds_rpm[8];
} TiwutThermalStats;

typedef struct {
    bool is_present;
    bool is_charging;
    bool is_charged;
    int current_capacity_percent;
    int current_capacity_mah;
    int max_capacity_mah;
    int design_capacity_mah;
    int cycle_count;
    int health_percent;
    int time_remaining_minutes;
    double voltage_volts;
    double amperage_amps;
    double temperature_celsius;
    char power_source[32];
} TiwutBatteryStats;

typedef struct {
    char bsd_name[32];
    char mount_point[256];
    char volume_name[128];
    char filesystem_type[32];
    uint64_t total_bytes;
    uint64_t free_bytes;
    uint64_t used_bytes;
    double used_percent;
    bool is_internal;
} TiwutStorageVolume;

typedef struct {
    int volume_count;
    TiwutStorageVolume volumes[16];
    uint64_t read_bytes_total;
    uint64_t write_bytes_total;
    uint64_t read_ops_total;
    uint64_t write_ops_total;
} TiwutStorageStats;

typedef struct {
    char interface_name[32];
    char ip_v4[48];
    char ip_v6[64];
    char mac_address[32];
    bool is_up;
    bool is_wifi;
    uint64_t rx_bytes;
    uint64_t tx_bytes;
    uint64_t rx_packets;
    uint64_t tx_packets;
} TiwutNetworkInterface;

typedef struct {
    int interface_count;
    TiwutNetworkInterface interfaces[16];
    char primary_interface[32];
    char gateway_ip[48];
    char dns_primary[48];
    double last_ping_ms;
} TiwutNetworkStats;

typedef struct {
    int pid;
    int ppid;
    int uid;
    char user[32];
    char name[128];
    char bundle_id[128];
    double cpu_percent;
    uint64_t memory_rss_bytes;
    uint64_t memory_vsz_bytes;
    int thread_count;
    int priority;
    char state[16];
} TiwutProcessInfo;

typedef struct {
    int process_count;
    TiwutProcessInfo processes[256];
} TiwutProcessList;

typedef struct {
    char bundle_id[128];
    char app_name[128];
    char path[256];
    int pid;
    bool is_active;
    bool is_hidden;
} TiwutAppInfo;

typedef struct {
    int app_count;
    TiwutAppInfo apps[128];
} TiwutAppList;

typedef struct {
    uint32_t display_id;
    int width;
    int height;
    double scale_factor;
    double refresh_rate_hz;
    int bit_depth;
    bool is_main;
    bool is_builtin;
    double brightness;
} TiwutDisplayInfo;

typedef struct {
    int display_count;
    TiwutDisplayInfo displays[8];
} TiwutDisplayStats;

typedef struct {
    char default_output_name[128];
    char default_input_name[128];
    float output_volume;
    bool is_output_muted;
    float input_volume;
    bool is_input_muted;
    int device_count;
} TiwutAudioStats;

typedef struct {
    bool is_caffeinated;
    uint32_t caffeinate_assertion_id;
    bool display_sleep_prevented;
    bool system_sleep_prevented;
    int sleep_timer_minutes;
} TiwutPowerStats;

typedef struct {
    char os_version[64];
    char os_build[32];
    char kernel_version[128];
    char hostname[128];
    char computer_name[128];
    char model_identifier[64];
    char model_name[128];
    char serial_number[64];
    char cpu_arch[32];
    uint64_t uptime_seconds;
    uint64_t boot_timestamp;
} TiwutSystemInfo;

void tiwut_api_init(void);
void tiwut_api_shutdown(void);

bool tiwut_get_cpu_stats(TiwutCpuStats *out_stats);
bool tiwut_get_memory_stats(TiwutMemoryStats *out_stats);
bool tiwut_get_thermal_stats(TiwutThermalStats *out_stats);
bool tiwut_get_battery_stats(TiwutBatteryStats *out_stats);
bool tiwut_get_storage_stats(TiwutStorageStats *out_stats);
bool tiwut_get_network_stats(TiwutNetworkStats *out_stats);
double tiwut_network_ping(const char *host);
bool tiwut_get_process_list(TiwutProcessList *out_list, int max_count);
bool tiwut_get_process_info(int pid, TiwutProcessInfo *out_info);
bool tiwut_kill_process(int pid, int signal_number);
bool tiwut_pause_process(int pid);
bool tiwut_resume_process(int pid);
bool tiwut_set_process_priority(int pid, int nice_val);

bool tiwut_get_app_list(TiwutAppList *out_list);
bool tiwut_launch_app(const char *app_identifier_or_path);
bool tiwut_quit_app(const char *bundle_id);
bool tiwut_focus_app(const char *bundle_id);

bool tiwut_get_display_stats(TiwutDisplayStats *out_stats);
bool tiwut_set_display_brightness(uint32_t display_id, double brightness);
bool tiwut_sleep_displays(void);

bool tiwut_get_audio_stats(TiwutAudioStats *out_stats);
bool tiwut_set_audio_volume(float volume);
bool tiwut_set_audio_mute(bool mute);

bool tiwut_get_power_stats(TiwutPowerStats *out_stats);
bool tiwut_system_sleep(void);
bool tiwut_system_restart(void);
bool tiwut_system_shutdown(void);
bool tiwut_caffeinate_start(bool prevent_display_sleep);
bool tiwut_caffeinate_stop(void);
bool tiwut_lock_screen(void);

bool tiwut_get_clipboard_text(char *out_text, size_t max_size);
bool tiwut_set_clipboard_text(const char *text);
bool tiwut_clear_clipboard(void);

bool tiwut_send_notification(const char *title, const char *subtitle, const char *message, const char *sound_name);
bool tiwut_get_system_info(TiwutSystemInfo *out_info);

void tiwut_json_cpu(char *out, size_t max_len);
void tiwut_json_memory(char *out, size_t max_len);
void tiwut_json_thermal(char *out, size_t max_len);
void tiwut_json_battery(char *out, size_t max_len);
void tiwut_json_storage(char *out, size_t max_len);
void tiwut_json_network(char *out, size_t max_len);
void tiwut_json_processes(char *out, size_t max_len);
void tiwut_json_apps(char *out, size_t max_len);
void tiwut_json_display(char *out, size_t max_len);
void tiwut_json_audio(char *out, size_t max_len);
void tiwut_json_power(char *out, size_t max_len);
void tiwut_json_clipboard(char *out, size_t max_len);
void tiwut_json_system(char *out, size_t max_len);
void tiwut_json_overview(char *out, size_t max_len);
void tiwut_prometheus_metrics(char *out, size_t max_len);

#ifdef __cplusplus
}
#endif

#endif
