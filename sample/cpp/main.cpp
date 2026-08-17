#include <iostream>
#include <string>
#include "tiwut_api.h"
#include "tiwut_hpp_time.hpp"
#include "tiwut_hpp_random.hpp"
#include "tiwut_hpp_file.hpp"

void sample_system_query() {
    TiwutCpuStats cpu;
    tiwut_get_cpu_stats(&cpu);
    std::cout << "[CPP Client] CPU Logical Cores: " << cpu.logical_cores << ", Total Usage: " << cpu.total_percent << "%\n";

    TiwutMemoryStats mem;
    tiwut_get_memory_stats(&mem);
    std::cout << "[CPP Client] Memory Total: " << (mem.total_bytes / (1024 * 1024 * 1024)) << " GB, Used: " << mem.used_percent << "%\n";

    TiwutThermalStats therm;
    tiwut_get_thermal_stats(&therm);
    std::cout << "[CPP Client] Thermal State: " << therm.thermal_state << " (Level: " << therm.thermal_pressure_level << ")\n";

    TiwutBatteryStats bat;
    tiwut_get_battery_stats(&bat);
    std::cout << "[CPP Client] Battery Level: " << bat.current_capacity_percent << "%, Power Source: " << bat.power_source << "\n";
}

int main() {
    tiwut_api_init();

    std::cout << "====================================================\n";
    std::cout << "   TIWUT API - C++17 ADVANCED CLIENT & BENCHMARK    \n";
    std::cout << "   Timestamp: " << tiwut_hpp_get_local_timestamp() << "\n";
    std::cout << "====================================================\n\n";

    std::string session_uuid = tiwut_hpp_random_uuid_v4();
    std::cout << "[UUID Generated] Session ID: " << session_uuid << "\n\n";

    auto elapsed_us = tiwut_hpp_measure_execution(sample_system_query);
    std::cout << "\n[Benchmark] Full Mach telemetry fetch duration: " << (elapsed_us / 1000.0) << " ms\n\n";

    char json_overview[8192];
    tiwut_json_overview(json_overview, sizeof(json_overview));
    
    std::string snapshot_path = "sample_snapshot.json";
    if (tiwut_hpp_file_write_atomic(snapshot_path, std::string(json_overview))) {
        std::cout << "[File IO] Successfully persisted atomic system snapshot to " << snapshot_path << "\n";
        if (auto meta = tiwut_hpp_file_get_metadata(snapshot_path)) {
            std::cout << "[File IO] Snapshot size: " << meta->size_bytes << " bytes, Modified: " << meta->last_write_time << "\n";
        }
    }

    tiwut_api_shutdown();
    return 0;
}
