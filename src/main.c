#include "tiwut_api.h"
#include "config.h"
#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

static void handle_signal(int sig) {
    (void)sig;
    tiwut_server_stop();
    tiwut_api_shutdown();
    exit(0);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    const char *config_file = "config.yaml";
    int override_port = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--config") == 0 || strcmp(argv[i], "-c") == 0) {
            if (i + 1 < argc) config_file = argv[++i];
        } else if (strcmp(argv[i], "--port") == 0 || strcmp(argv[i], "-p") == 0) {
            if (i + 1 < argc) override_port = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            printf("Tiwut API - Powerful macOS System Control & Telemetry Service\n");
            printf("Usage: %s [options]\n", argv[0]);
            printf("Options:\n");
            printf("  -c, --config <file>  Path to config.yaml (default: config.yaml)\n");
            printf("  -p, --port <port>    Override HTTP port\n");
            printf("  -h, --help           Show this help message\n");
            return 0;
        }
    }

    tiwut_config_init_defaults(&g_config);
    tiwut_config_load_file(&g_config, config_file);

    if (override_port > 0) {
        g_config.server.port = override_port;
    }

    tiwut_api_init();

    printf("====================================================\n");
    printf("   TIWUT API SERVICE - MACOS EXCLUSIVE ENGINE       \n");
    printf("   Version: 1.0.0                                   \n");
    printf("   Config:  %s                                      \n", config_file);
    printf("   Port:    %d                                      \n", g_config.server.port);
    printf("   Host:    %s                                      \n", g_config.server.host);
    printf("   Web UI:  http://%s:%d/                           \n", g_config.server.host, g_config.server.port);
    printf("   API:     http://%s:%d/api/v1/overview            \n", g_config.server.host, g_config.server.port);
    printf("====================================================\n");

    tiwut_server_start(g_config.server.port, g_config.server.host);

    tiwut_api_shutdown();
    return 0;
}
