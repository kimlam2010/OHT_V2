#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>

#include "hal_common.h"
#include "communication_manager.h"
#include "module_manager.h"

typedef struct {
    uint32_t timeouts;
    uint32_t crc_errors;
    uint32_t flapping;
    uint32_t health_checks;
    uint32_t duration_s;
    uint32_t rss_kb;
} soak_metrics_t;

static uint32_t read_rss_kb(void) {
    FILE *f = fopen("/proc/self/status", "r");
    if (!f) return 0;
    char line[256];
    uint32_t rss = 0;
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, "VmRSS:", 6) == 0) {
            unsigned v = 0; if (sscanf(line+6, "%u", &v) == 1) rss = v; break;
        }
    }
    fclose(f);
    return rss; // kB
}

static void ensure_dir(const char *path) {
    struct stat st; if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) return;
    (void)mkdir(path, 0755);
}

static void write_report(const soak_metrics_t *m, const char *path) {
    if (!m || !path) return;
    FILE *f = fopen(path, "w");
    if (!f) return;
    time_t now = time(NULL);
    char ts[64]; strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S", localtime(&now));
    fprintf(f, "# Soak Test Report\n\n");
    fprintf(f, "- Timestamp: %s\n", ts);
    fprintf(f, "- Duration: %u s\n", m->duration_s);
    fprintf(f, "- Timeouts: %u\n", m->timeouts);
    fprintf(f, "- CRC errors: %u\n", m->crc_errors);
    fprintf(f, "- Flapping transitions (online↔offline): %u\n", m->flapping);
    fprintf(f, "- Health checks: %u\n", m->health_checks);
    fprintf(f, "- RSS memory: %u kB\n", m->rss_kb);
    fprintf(f, "\n## Notes\n- Test loops scan/health/polling and records counters.\n");
    fclose(f);
}

int main(int argc, char **argv) {
    uint32_t duration_s = 60; // default
    for (int i = 1; i < argc; ++i) {
        if ((strcmp(argv[i], "--minutes") == 0 || strcmp(argv[i], "-m") == 0) && i+1 < argc) {
            duration_s = (uint32_t)strtoul(argv[++i], NULL, 10) * 60U;
        } else if ((strcmp(argv[i], "--seconds") == 0 || strcmp(argv[i], "-s") == 0) && i+1 < argc) {
            duration_s = (uint32_t)strtoul(argv[++i], NULL, 10);
        }
    }

    printf("🚀 Starting Soak Test: duration=%u s\n", duration_s);

    // Initialize Communication Manager for real RS485 (if available)
    comm_mgr_config_t cm_cfg = {0};
    cm_cfg.baud_rate = 115200;
    cm_cfg.data_bits = 8;
    cm_cfg.stop_bits = 1;
    cm_cfg.parity = 0; // none
    cm_cfg.timeout_ms = 100;
    cm_cfg.retry_count = 2;
    cm_cfg.retry_delay_ms = 10;
    cm_cfg.modbus_slave_id = 1;
    cm_cfg.enable_crc_check = true;
    cm_cfg.enable_echo_suppression = true;
    cm_cfg.buffer_size = 512;
    hal_status_t cm_init_status = comm_manager_init(&cm_cfg);
    printf("[SOAK] comm_manager_init => %d\n", cm_init_status);

    // Initialize Module Manager and load optional YAML config
    (void)module_manager_init();
    (void)module_manager_load_config_from_yaml("/etc/oht50/modules.yaml");
    (void)module_manager_discover_modules(); // best-effort

    // Force a few Modbus reads to generate TX on the bus (0x02..0x07)
    for (uint8_t addr = 0x02; addr <= 0x07; ++addr) {
        uint16_t tmp = 0;
        hal_status_t r = comm_manager_modbus_read_holding_registers(addr, 0x0000, 1, &tmp);
        printf("[SOAK] Probe addr=0x%02X read 0x0000 => status=%d val=%u\n", addr, r, tmp);
    }

    // Track previous statuses for flapping count
    module_info_t prev[MODULE_REGISTRY_MAX_MODULES];
    size_t prev_count = 0; memset(prev, 0, sizeof(prev));

    soak_metrics_t metrics = {0};
    metrics.duration_s = duration_s;

    uint64_t end_time = hal_get_timestamp_ms() + (uint64_t)duration_s * 1000ULL;
    uint32_t last_log_s = 0;

    while (hal_get_timestamp_ms() < end_time) {
        (void)module_manager_update();
        (void)comm_manager_update();

        // Gather registry and flapping
        module_info_t cur[MODULE_REGISTRY_MAX_MODULES]; size_t cur_count = 0;
        (void)registry_get_all(cur, MODULE_REGISTRY_MAX_MODULES, &cur_count);
        for (size_t i = 0; i < cur_count; ++i) {
            // search in prev
            module_status_t prev_st = MODULE_STATUS_UNKNOWN;
            for (size_t j = 0; j < prev_count; ++j) {
                if (prev[j].address == cur[i].address) { prev_st = prev[j].status; break; }
            }
            if (prev_st != MODULE_STATUS_UNKNOWN && prev_st != cur[i].status) {
                metrics.flapping++;
            }
        }
        // copy cur to prev
        memcpy(prev, cur, cur_count * sizeof(module_info_t));
        prev_count = cur_count;

        // Read communication stats (thread-safe getters could be added; here read directly if available)
        comm_mgr_stats_t s = {0};
        (void)comm_manager_get_statistics(&s);
        metrics.timeouts = s.timeout_count;
        metrics.crc_errors = s.crc_error_count;

        // Health checks from module_manager statistics
        module_stats_t ms = {0};
        (void)module_manager_get_statistics(&ms);
        metrics.health_checks = ms.health_checks;

        // Memory
        metrics.rss_kb = read_rss_kb();

        uint32_t elapsed_s = (uint32_t)((duration_s * 1000ULL - (end_time - hal_get_timestamp_ms())) / 1000ULL);
        if (elapsed_s != last_log_s) {
            last_log_s = elapsed_s;
            printf("[SOAK] t=%us timeouts=%u crc=%u flap=%u rss=%u kB\n",
                   elapsed_s, metrics.timeouts, metrics.crc_errors, metrics.flapping, metrics.rss_kb);
            fflush(stdout);
        }

        hal_sleep_ms(200);
    }

    // Ensure output dir exists and write report
    ensure_dir("../profiling_results");
    const char *report_path = getenv("SOAK_REPORT_PATH");
    char default_path[256];
    if (!report_path) {
        snprintf(default_path, sizeof(default_path), "../profiling_results/soak_report.md");
        report_path = default_path;
    }
    write_report(&metrics, report_path);
    printf("✅ Soak test completed. Report: %s\n", report_path);
    return 0;
}


