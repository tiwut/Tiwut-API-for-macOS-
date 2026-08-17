#include "server.h"
#include "tiwut_api.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>

static int g_server_fd = -1;
static volatile bool g_running = false;

static const char *get_mime_type(const char *path) {
    const char *dot = strrchr(path, '.');
    if (!dot) return "text/plain";
    if (strcasecmp(dot, ".html") == 0 || strcasecmp(dot, ".htm") == 0) return "text/html; charset=utf-8";
    if (strcasecmp(dot, ".css") == 0) return "text/css; charset=utf-8";
    if (strcasecmp(dot, ".js") == 0) return "application/javascript; charset=utf-8";
    if (strcasecmp(dot, ".json") == 0) return "application/json; charset=utf-8";
    if (strcasecmp(dot, ".svg") == 0) return "image/svg+xml";
    if (strcasecmp(dot, ".png") == 0) return "image/png";
    if (strcasecmp(dot, ".jpg") == 0 || strcasecmp(dot, ".jpeg") == 0) return "image/jpeg";
    if (strcasecmp(dot, ".ico") == 0) return "image/x-icon";
    if (strcasecmp(dot, ".txt") == 0) return "text/plain; charset=utf-8";
    return "application/octet-stream";
}

static void send_http_response(int client_fd, int status_code, const char *status_text, const char *content_type, const char *body, size_t body_len) {
    char header[1024];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Server: Tiwut-API/1.0 (macOS)\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "%s"
        "\r\n",
        status_code, status_text, content_type, body_len,
        g_config.server.cors_enabled ? "Access-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: GET, POST, OPTIONS\r\nAccess-Control-Allow-Headers: Content-Type, Authorization\r\n" : ""
    );

    write(client_fd, header, header_len);
    if (body && body_len > 0) {
        write(client_fd, body, body_len);
    }
}

static void extract_json_field(const char *json, const char *field, char *out_val, size_t max_len) {
    out_val[0] = 0;
    char search[128];
    snprintf(search, sizeof(search), "\"%s\"", field);
    char *pos = strstr(json, search);
    if (!pos) return;

    pos = strchr(pos, ':');
    if (!pos) return;
    pos++;

    while (*pos == ' ' || *pos == '\t' || *pos == '"') pos++;
    size_t i = 0;
    while (*pos && *pos != '"' && *pos != ',' && *pos != '}' && *pos != '\r' && *pos != '\n' && i < max_len - 1) {
        out_val[i++] = *pos++;
    }
    out_val[i] = 0;
}

static void serve_static_file(int client_fd, const char *req_path) {
    char clean_path[512] = "";
    if (strcmp(req_path, "/") == 0 || strcmp(req_path, "") == 0) {
        strncpy(clean_path, "/index.html", sizeof(clean_path) - 1);
    } else {
        strncpy(clean_path, req_path, sizeof(clean_path) - 1);
    }

    char *q = strchr(clean_path, '?');
    if (q) *q = 0;

    char file_path[1024];
    snprintf(file_path, sizeof(file_path), "%s%s", g_config.server.web_root, clean_path);

    struct stat st;
    if (stat(file_path, &st) != 0 || S_ISDIR(st.st_mode)) {
        const char *not_found = "{\"status\": \"error\", \"message\": \"404 Not Found\"}";
        send_http_response(client_fd, 404, "Not Found", "application/json", not_found, strlen(not_found));
        return;
    }

    FILE *f = fopen(file_path, "rb");
    if (!f) {
        const char *forbidden = "{\"status\": \"error\", \"message\": \"403 Forbidden\"}";
        send_http_response(client_fd, 403, "Forbidden", "application/json", forbidden, strlen(forbidden));
        return;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (fsize <= 0) {
        fclose(f);
        send_http_response(client_fd, 200, "OK", get_mime_type(file_path), "", 0);
        return;
    }

    char *buf = (char *)malloc(fsize);
    if (!buf) {
        fclose(f);
        const char *err = "{\"status\": \"error\", \"message\": \"Memory error\"}";
        send_http_response(client_fd, 500, "Internal Server Error", "application/json", err, strlen(err));
        return;
    }

    fread(buf, 1, fsize, f);
    fclose(f);

    send_http_response(client_fd, 200, "OK", get_mime_type(file_path), buf, (size_t)fsize);
    free(buf);
}

static void handle_client_request(int client_fd) {
    char request_buf[16384];
    ssize_t bytes_read = read(client_fd, request_buf, sizeof(request_buf) - 1);
    if (bytes_read <= 0) {
        close(client_fd);
        return;
    }
    request_buf[bytes_read] = 0;

    char method[16] = "";
    char path[512] = "";
    char protocol[32] = "";
    sscanf(request_buf, "%15s %511s %31s", method, path, protocol);

    if (strcasecmp(method, "OPTIONS") == 0) {
        send_http_response(client_fd, 204, "No Content", "text/plain", "", 0);
        close(client_fd);
        return;
    }

    if (g_config.server.auth_enabled) {
        char auth_header[256] = "";
        char *auth_pos = strcasestr(request_buf, "Authorization: Bearer ");
        if (auth_pos) {
            sscanf(auth_pos, "%*s %*s %255s", auth_header);
        }
        if (strcmp(auth_header, g_config.server.api_key) != 0) {
            const char *unauth = "{\"status\": \"error\", \"message\": \"Unauthorized. Valid Bearer API key required.\"}";
            send_http_response(client_fd, 401, "Unauthorized", "application/json", unauth, strlen(unauth));
            close(client_fd);
            return;
        }
    }

    char *body = strstr(request_buf, "\r\n\r\n");
    if (body) body += 4;
    else body = "";

    char res_buf[65536];

    if (strncmp(path, "/api/v1/health", 14) == 0) {
        snprintf(res_buf, sizeof(res_buf), "{\"status\": \"ok\", \"service\": \"Tiwut API\", \"os\": \"macOS\", \"version\": \"1.0.0\"}");
        send_http_response(client_fd, 200, "OK", "application/json", res_buf, strlen(res_buf));
    }
    else if (strncmp(path, "/api/v1/overview", 16) == 0) {
        tiwut_json_overview(res_buf, sizeof(res_buf));
        send_http_response(client_fd, 200, "OK", "application/json", res_buf, strlen(res_buf));
    }
    else if (strncmp(path, "/api/v1/metrics", 15) == 0) {
        tiwut_prometheus_metrics(res_buf, sizeof(res_buf));
        send_http_response(client_fd, 200, "OK", "text/plain; version=0.0.4", res_buf, strlen(res_buf));
    }
    else if (strncmp(path, "/api/v1/cpu", 11) == 0) {
        tiwut_json_cpu(res_buf, sizeof(res_buf));
        send_http_response(client_fd, 200, "OK", "application/json", res_buf, strlen(res_buf));
    }
    else if (strncmp(path, "/api/v1/memory", 14) == 0) {
        tiwut_json_memory(res_buf, sizeof(res_buf));
        send_http_response(client_fd, 200, "OK", "application/json", res_buf, strlen(res_buf));
    }
    else if (strncmp(path, "/api/v1/thermal", 15) == 0) {
        tiwut_json_thermal(res_buf, sizeof(res_buf));
        send_http_response(client_fd, 200, "OK", "application/json", res_buf, strlen(res_buf));
    }
    else if (strncmp(path, "/api/v1/battery", 15) == 0) {
        tiwut_json_battery(res_buf, sizeof(res_buf));
        send_http_response(client_fd, 200, "OK", "application/json", res_buf, strlen(res_buf));
    }
    else if (strncmp(path, "/api/v1/storage", 15) == 0) {
        tiwut_json_storage(res_buf, sizeof(res_buf));
        send_http_response(client_fd, 200, "OK", "application/json", res_buf, strlen(res_buf));
    }
    else if (strncmp(path, "/api/v1/network/ping", 20) == 0) {
        char host[128] = "1.1.1.1";
        char *q = strstr(path, "host=");
        if (q) {
            sscanf(q + 5, "%127[^& ]", host);
        }
        double latency = tiwut_network_ping(host);
        snprintf(res_buf, sizeof(res_buf), "{\"status\": \"ok\", \"host\": \"%s\", \"latency_ms\": %.2f}", host, latency);
        send_http_response(client_fd, 200, "OK", "application/json", res_buf, strlen(res_buf));
    }
    else if (strncmp(path, "/api/v1/network", 15) == 0) {
        tiwut_json_network(res_buf, sizeof(res_buf));
        send_http_response(client_fd, 200, "OK", "application/json", res_buf, strlen(res_buf));
    }
    else if (strncmp(path, "/api/v1/processes", 17) == 0) {
        if (strstr(path, "/kill") && strcasecmp(method, "POST") == 0) {
            int pid = 0;
            sscanf(path, "/api/v1/processes/%d/kill", &pid);
            bool ok = tiwut_kill_process(pid, SIGTERM);
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"pid\": %d, \"action\": \"kill\"}", ok ? "ok" : "error", pid);
            send_http_response(client_fd, ok ? 200 : 400, ok ? "OK" : "Bad Request", "application/json", res_buf, strlen(res_buf));
        } else if (strstr(path, "/pause") && strcasecmp(method, "POST") == 0) {
            int pid = 0;
            sscanf(path, "/api/v1/processes/%d/pause", &pid);
            bool ok = tiwut_pause_process(pid);
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"pid\": %d, \"action\": \"pause\"}", ok ? "ok" : "error", pid);
            send_http_response(client_fd, ok ? 200 : 400, ok ? "OK" : "Bad Request", "application/json", res_buf, strlen(res_buf));
        } else if (strstr(path, "/resume") && strcasecmp(method, "POST") == 0) {
            int pid = 0;
            sscanf(path, "/api/v1/processes/%d/resume", &pid);
            bool ok = tiwut_resume_process(pid);
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"pid\": %d, \"action\": \"resume\"}", ok ? "ok" : "error", pid);
            send_http_response(client_fd, ok ? 200 : 400, ok ? "OK" : "Bad Request", "application/json", res_buf, strlen(res_buf));
        } else if (strstr(path, "/priority") && strcasecmp(method, "POST") == 0) {
            int pid = 0;
            sscanf(path, "/api/v1/processes/%d/priority", &pid);
            char nice_str[16] = "0";
            extract_json_field(body, "nice", nice_str, sizeof(nice_str));
            int nice_val = atoi(nice_str);
            bool ok = tiwut_set_process_priority(pid, nice_val);
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"pid\": %d, \"nice\": %d}", ok ? "ok" : "error", pid, nice_val);
            send_http_response(client_fd, ok ? 200 : 400, ok ? "OK" : "Bad Request", "application/json", res_buf, strlen(res_buf));
        } else {
            int pid = 0;
            if (sscanf(path, "/api/v1/processes/%d", &pid) == 1 && pid > 0) {
                TiwutProcessInfo pi;
                if (tiwut_get_process_info(pid, &pi)) {
                    snprintf(res_buf, sizeof(res_buf),
                        "{\"status\": \"ok\", \"pid\": %d, \"ppid\": %d, \"user\": \"%s\", \"name\": \"%s\", \"rss_bytes\": %llu, \"threads\": %d, \"priority\": %d, \"state\": \"%s\"}",
                        pi.pid, pi.ppid, pi.user, pi.name, (unsigned long long)pi.memory_rss_bytes, pi.thread_count, pi.priority, pi.state
                    );
                    send_http_response(client_fd, 200, "OK", "application/json", res_buf, strlen(res_buf));
                } else {
                    const char *not_found = "{\"status\": \"error\", \"message\": \"Process not found\"}";
                    send_http_response(client_fd, 404, "Not Found", "application/json", not_found, strlen(not_found));
                }
            } else {
                tiwut_json_processes(res_buf, sizeof(res_buf));
                send_http_response(client_fd, 200, "OK", "application/json", res_buf, strlen(res_buf));
            }
        }
    }
    else if (strncmp(path, "/api/v1/apps", 12) == 0) {
        if (strstr(path, "/launch") && strcasecmp(method, "POST") == 0) {
            char target[256] = "";
            extract_json_field(body, "target", target, sizeof(target));
            bool ok = tiwut_launch_app(target);
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"target\": \"%s\"}", ok ? "ok" : "error", target);
            send_http_response(client_fd, ok ? 200 : 400, ok ? "OK" : "Bad Request", "application/json", res_buf, strlen(res_buf));
        } else if (strstr(path, "/quit") && strcasecmp(method, "POST") == 0) {
            char bundle_id[128] = "";
            extract_json_field(body, "bundle_id", bundle_id, sizeof(bundle_id));
            bool ok = tiwut_quit_app(bundle_id);
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"bundle_id\": \"%s\"}", ok ? "ok" : "error", bundle_id);
            send_http_response(client_fd, ok ? 200 : 400, ok ? "OK" : "Bad Request", "application/json", res_buf, strlen(res_buf));
        } else if (strstr(path, "/focus") && strcasecmp(method, "POST") == 0) {
            char bundle_id[128] = "";
            extract_json_field(body, "bundle_id", bundle_id, sizeof(bundle_id));
            bool ok = tiwut_focus_app(bundle_id);
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"bundle_id\": \"%s\"}", ok ? "ok" : "error", bundle_id);
            send_http_response(client_fd, ok ? 200 : 400, ok ? "OK" : "Bad Request", "application/json", res_buf, strlen(res_buf));
        } else {
            tiwut_json_apps(res_buf, sizeof(res_buf));
            send_http_response(client_fd, 200, "OK", "application/json", res_buf, strlen(res_buf));
        }
    }
    else if (strncmp(path, "/api/v1/display", 15) == 0) {
        if (strstr(path, "/sleep") && strcasecmp(method, "POST") == 0) {
            bool ok = tiwut_sleep_displays();
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"action\": \"sleep_displays\"}", ok ? "ok" : "error");
            send_http_response(client_fd, ok ? 200 : 500, ok ? "OK" : "Internal Server Error", "application/json", res_buf, strlen(res_buf));
        } else if (strstr(path, "/brightness") && strcasecmp(method, "POST") == 0) {
            char val_str[32] = "1.0";
            extract_json_field(body, "brightness", val_str, sizeof(val_str));
            double b = atof(val_str);
            bool ok = tiwut_set_display_brightness(0, b);
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"brightness\": %.2f}", ok ? "ok" : "error", b);
            send_http_response(client_fd, ok ? 200 : 400, ok ? "OK" : "Bad Request", "application/json", res_buf, strlen(res_buf));
        } else {
            tiwut_json_display(res_buf, sizeof(res_buf));
            send_http_response(client_fd, 200, "OK", "application/json", res_buf, strlen(res_buf));
        }
    }
    else if (strncmp(path, "/api/v1/audio", 13) == 0) {
        if (strstr(path, "/volume") && strcasecmp(method, "POST") == 0) {
            char vol_str[32] = "0.5";
            extract_json_field(body, "volume", vol_str, sizeof(vol_str));
            float vol = (float)atof(vol_str);
            bool ok = tiwut_set_audio_volume(vol);
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"volume\": %.2f}", ok ? "ok" : "error", vol);
            send_http_response(client_fd, ok ? 200 : 400, ok ? "OK" : "Bad Request", "application/json", res_buf, strlen(res_buf));
        } else if (strstr(path, "/mute") && strcasecmp(method, "POST") == 0) {
            char mute_str[32] = "true";
            extract_json_field(body, "mute", mute_str, sizeof(mute_str));
            bool mute = (strcmp(mute_str, "true") == 0 || strcmp(mute_str, "1") == 0);
            bool ok = tiwut_set_audio_mute(mute);
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"mute\": %s}", ok ? "ok" : "error", mute ? "true" : "false");
            send_http_response(client_fd, ok ? 200 : 400, ok ? "OK" : "Bad Request", "application/json", res_buf, strlen(res_buf));
        } else {
            tiwut_json_audio(res_buf, sizeof(res_buf));
            send_http_response(client_fd, 200, "OK", "application/json", res_buf, strlen(res_buf));
        }
    }
    else if (strncmp(path, "/api/v1/power", 13) == 0) {
        if (strstr(path, "/caffeinate") && strcasecmp(method, "POST") == 0) {
            bool ok = tiwut_caffeinate_start(true);
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"action\": \"caffeinate_started\"}", ok ? "ok" : "error");
            send_http_response(client_fd, ok ? 200 : 500, ok ? "OK" : "Internal Server Error", "application/json", res_buf, strlen(res_buf));
        } else if (strstr(path, "/decaffeinate") && strcasecmp(method, "POST") == 0) {
            bool ok = tiwut_caffeinate_stop();
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"action\": \"caffeinate_stopped\"}", ok ? "ok" : "error");
            send_http_response(client_fd, ok ? 200 : 500, ok ? "OK" : "Internal Server Error", "application/json", res_buf, strlen(res_buf));
        } else if (strstr(path, "/lock") && strcasecmp(method, "POST") == 0) {
            bool ok = tiwut_lock_screen();
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"action\": \"lock_screen\"}", ok ? "ok" : "error");
            send_http_response(client_fd, ok ? 200 : 500, ok ? "OK" : "Internal Server Error", "application/json", res_buf, strlen(res_buf));
        } else if (strstr(path, "/sleep") && strcasecmp(method, "POST") == 0) {
            bool ok = tiwut_system_sleep();
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"action\": \"sleep\"}", ok ? "ok" : "error");
            send_http_response(client_fd, ok ? 200 : 500, ok ? "OK" : "Internal Server Error", "application/json", res_buf, strlen(res_buf));
        } else if (strstr(path, "/restart") && strcasecmp(method, "POST") == 0) {
            bool ok = tiwut_system_restart();
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"action\": \"restart\"}", ok ? "ok" : "error");
            send_http_response(client_fd, ok ? 200 : 500, ok ? "OK" : "Internal Server Error", "application/json", res_buf, strlen(res_buf));
        } else if (strstr(path, "/shutdown") && strcasecmp(method, "POST") == 0) {
            bool ok = tiwut_system_shutdown();
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"action\": \"shutdown\"}", ok ? "ok" : "error");
            send_http_response(client_fd, ok ? 200 : 500, ok ? "OK" : "Internal Server Error", "application/json", res_buf, strlen(res_buf));
        } else {
            tiwut_json_power(res_buf, sizeof(res_buf));
            send_http_response(client_fd, 200, "OK", "application/json", res_buf, strlen(res_buf));
        }
    }
    else if (strncmp(path, "/api/v1/clipboard", 17) == 0) {
        if (strstr(path, "/clear") && strcasecmp(method, "POST") == 0) {
            bool ok = tiwut_clear_clipboard();
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"action\": \"clear_clipboard\"}", ok ? "ok" : "error");
            send_http_response(client_fd, ok ? 200 : 500, ok ? "OK" : "Internal Server Error", "application/json", res_buf, strlen(res_buf));
        } else if (strcasecmp(method, "POST") == 0) {
            char text[2048] = "";
            extract_json_field(body, "text", text, sizeof(text));
            bool ok = tiwut_set_clipboard_text(text);
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"action\": \"set_clipboard\"}", ok ? "ok" : "error");
            send_http_response(client_fd, ok ? 200 : 500, ok ? "OK" : "Internal Server Error", "application/json", res_buf, strlen(res_buf));
        } else {
            tiwut_json_clipboard(res_buf, sizeof(res_buf));
            send_http_response(client_fd, 200, "OK", "application/json", res_buf, strlen(res_buf));
        }
    }
    else if (strncmp(path, "/api/v1/notifications", 21) == 0 && strcasecmp(method, "POST") == 0) {
        char title[128] = "Tiwut Alert";
        char subtitle[128] = "";
        char message[256] = "Test notification";
        char sound[64] = "default";
        extract_json_field(body, "title", title, sizeof(title));
        extract_json_field(body, "subtitle", subtitle, sizeof(subtitle));
        extract_json_field(body, "message", message, sizeof(message));
        extract_json_field(body, "sound", sound, sizeof(sound));
        bool ok = tiwut_send_notification(title, subtitle, message, sound);
        snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"action\": \"notification_sent\"}", ok ? "ok" : "error");
        send_http_response(client_fd, ok ? 200 : 500, ok ? "OK" : "Internal Server Error", "application/json", res_buf, strlen(res_buf));
    }
    else if (strncmp(path, "/api/v1/system", 14) == 0) {
        tiwut_json_system(res_buf, sizeof(res_buf));
        send_http_response(client_fd, 200, "OK", "application/json", res_buf, strlen(res_buf));
    }
    else if (strncmp(path, "/api/v1/config", 14) == 0) {
        if (strstr(path, "/reload") && strcasecmp(method, "POST") == 0) {
            bool ok = tiwut_config_reload(NULL);
            snprintf(res_buf, sizeof(res_buf), "{\"status\": \"%s\", \"action\": \"config_reloaded\"}", ok ? "ok" : "error");
            send_http_response(client_fd, ok ? 200 : 500, ok ? "OK" : "Internal Server Error", "application/json", res_buf, strlen(res_buf));
        } else {
            tiwut_config_to_json(&g_config, res_buf, sizeof(res_buf));
            send_http_response(client_fd, 200, "OK", "application/json", res_buf, strlen(res_buf));
        }
    }
    else {
        serve_static_file(client_fd, path);
    }

    close(client_fd);
}

static void *server_worker_thread(void *arg) {
    int client_fd = (int)(intptr_t)arg;
    handle_client_request(client_fd);
    return NULL;
}

bool tiwut_server_start(int port, const char *host) {
    g_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_server_fd < 0) return false;

    int opt = 1;
    setsockopt(g_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (!host || strcmp(host, "0.0.0.0") == 0) {
        addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        inet_pton(AF_INET, host, &addr.sin_addr);
    }

    if (bind(g_server_fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        close(g_server_fd);
        g_server_fd = -1;
        return false;
    }

    if (listen(g_server_fd, 128) != 0) {
        close(g_server_fd);
        g_server_fd = -1;
        return false;
    }

    g_running = true;
    printf("[Tiwut API] Server listening on http://%s:%d\n", host ? host : "127.0.0.1", port);

    while (g_running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(g_server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            if (!g_running) break;
            continue;
        }

        pthread_t tid;
        pthread_create(&tid, NULL, server_worker_thread, (void *)(intptr_t)client_fd);
        pthread_detach(tid);
    }

    return true;
}

void tiwut_server_stop(void) {
    g_running = false;
    if (g_server_fd >= 0) {
        shutdown(g_server_fd, SHUT_RDWR);
        close(g_server_fd);
        g_server_fd = -1;
    }
}
