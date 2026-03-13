/**
 * @file main.c
 * @brief ELF App Template for base-firmware
 *
 * HOW TO USE THIS TEMPLATE
 * ========================
 * 1. Search-replace "my-app" with your app ID (max 15 chars, lowercase, no spaces)
 * 2. Search-replace "MY_APP" with the same in UPPER_SNAKE_CASE (for log tags)
 * 3. Fill in the manifest fields (name, version, icon, flow component)
 * 4. Implement on_app_start / on_app_stop / on_config_changed
 * 5. If your app exposes services for plugins, fill s_service_symbols[]
 * 6. Update CMakeLists.txt (project name) and sdkconfig.defaults (target)
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "app_api.h"   // app_manifest_register, nvs_proxy_*, flow_config_exists
#include "esp_elf.h"   // esp_elf_register_symbol / esp_elf_unregister_symbol

/* ── App identity ─────────────────────────────────────────────────────────── */

// TODO: max 15 chars, lowercase, no spaces — used as NVS namespace
#define APP_ID   "my-app"
#define APP_TAG  "[MY_APP]"

/* ── Default NVS config (JSON) ────────────────────────────────────────────── */

// TODO: replace with your app's configuration schema
static const char* s_default_config =
    "{"
    "\"configured\":false"
    "}";

/* ── Manifest (static, lives for the lifetime of the loaded ELF) ──────────── */

static app_manifest_t s_manifest;

/* ── Service symbols (optional — only needed if plugins call into this app) ── */

// TODO: replace with your real service functions, or remove the table entirely
// if this app does not expose services to plugins.
//
// static void my_service_fn(const uint8_t* data, size_t len) { ... }
//
// static const esp_elf_symbol_table_t s_service_symbols[] = {
//     ESP_ELFSYM_EXPORT(my_service_fn),
//     ESP_ELFSYM_END
// };
//
// static bool s_services_registered = false;

/* ── Forward declarations ─────────────────────────────────────────────────── */

static void on_app_start(void);
static void on_app_stop(void);
static void on_config_changed(void);
static int  on_command(const char* cmd, const char* payload,
                       char* response, size_t response_max);

/* ── Lifecycle callbacks ──────────────────────────────────────────────────── */

static void on_app_start(void)
{
    printf(APP_TAG " Starting...\n");

    // TODO: initialize hardware (GPIO, RMT, timers, sockets, …)

    // TODO: if this app exposes services for plugins, register them here:
    // esp_elf_register_symbol((esp_elf_symbol_table_t*)s_service_symbols);
    // s_services_registered = true;

    // TODO: start background tasks
    // xTaskCreate(my_task, "my_task", 4096, NULL, 5, &s_task_handle);

    s_manifest.is_running = true;
    printf(APP_TAG " Started\n");
}

static void on_app_stop(void)
{
    printf(APP_TAG " Stopping...\n");

    // TODO: stop background tasks first

    // TODO: unregister services before ELF is unloaded
    // if (s_services_registered) {
    //     esp_elf_unregister_symbol((esp_elf_symbol_table_t*)s_service_symbols);
    //     s_services_registered = false;
    // }

    // TODO: deinitialize hardware

    s_manifest.is_running = false;
    printf(APP_TAG " Stopped\n");
}

static void on_config_changed(void)
{
    printf(APP_TAG " Config changed — reloading\n");

    // TODO: re-read config from NVS and apply:
    //
    // char buf[512];
    // size_t len = sizeof(buf);
    // if (nvs_proxy_get_config(APP_ID, buf, &len) == 0) {
    //     // parse buf (JSON string) and apply
    // }
}

static int on_command(const char* cmd, const char* payload,
                      char* response, size_t response_max)
{
    // TODO: handle custom commands sent via POST /api/apps/<id>/command
    //
    // if (strcmp(cmd, "my-command") == 0) {
    //     snprintf(response, response_max, "{\"ok\":true}");
    //     return 0;
    // }

    (void)payload;
    snprintf(response, response_max, "{\"error\":\"unknown command: %s\"}", cmd);
    return -1;
}

/* ── Flow component (what appears in the Flow Editor palette) ─────────────── */

static void setup_flow_component(flow_component_t* c)
{
    memset(c, 0, sizeof(*c));

    // TODO: fill in your component's identity
    strncpy(c->name,     "my_service",  APP_MANIFEST_MAX_COMPONENT_NAME - 1);
    strncpy(c->label,    "My Service",  APP_MANIFEST_MAX_COMPONENT_NAME - 1);
    strncpy(c->category, "My App",      APP_MANIFEST_MAX_COMPONENT_NAME - 1);

    // FLOW_COMPONENT_OUTPUT  – plugins connect INTO this app (most common for hardware drivers)
    // FLOW_COMPONENT_INPUT   – this app produces data consumed by transforms/outputs
    // FLOW_COMPONENT_TRANSFORM – both in and out ports
    c->type = FLOW_COMPONENT_OUTPUT;

    // TODO: declare configuration parameters shown in the flow editor dialog
    // Example:
    // strncpy(c->params[0].name,          "gpio",   APP_MANIFEST_MAX_PARAM_NAME - 1);
    // strncpy(c->params[0].label,         "GPIO",   APP_MANIFEST_MAX_PARAM_NAME - 1);
    // strncpy(c->params[0].type,          "number", 15);
    // strncpy(c->params[0].default_value, "0",      APP_MANIFEST_MAX_PARAM_VALUE - 1);
    // c->param_count = 1;
}

/* ── Manifest registration ────────────────────────────────────────────────── */

static void register_manifest(void)
{
    memset(&s_manifest, 0, sizeof(s_manifest));

    // TODO: fill in display name, id, version
    strncpy(s_manifest.name,    "My App",  APP_MANIFEST_MAX_NAME_LEN - 1);
    strncpy(s_manifest.id,      APP_ID,    APP_MANIFEST_MAX_NAME_LEN - 1);
    strncpy(s_manifest.version, "1.0.0",   APP_MANIFEST_MAX_VERSION_LEN - 1);

    // TODO: replace with an inline SVG icon (24×24 viewBox recommended)
    strncpy(s_manifest.icon_svg,
        "<svg viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\">"
        "<circle cx=\"12\" cy=\"12\" r=\"10\"/>"
        "</svg>",
        APP_MANIFEST_MAX_ICON_LEN - 1);

    // Optional: web UI
    // s_manifest.html_content     = (char*)my_html;
    // s_manifest.html_content_len = sizeof(my_html) - 1;
    // s_manifest.has_ui           = true;

    // NVS config: set needs_config=true so the base firmware initialises
    // a default config blob on first boot (if no existing config found).
    s_manifest.needs_config      = true;
    s_manifest.default_config    = (char*)s_default_config;
    s_manifest.default_config_len = strlen(s_default_config);

    // Flow component (remove if app has no flow components)
    setup_flow_component(&s_manifest.flow_components[0]);
    s_manifest.flow_component_count = 1;

    // Callbacks
    s_manifest.on_start          = on_app_start;
    s_manifest.on_stop           = on_app_stop;
    s_manifest.on_config_changed = on_config_changed;
    s_manifest.on_command        = on_command;

    int ret = app_manifest_register(&s_manifest);
    printf(APP_TAG " Manifest register: %s\n", ret == 0 ? "OK" : "FAILED");
}

/* ── Entry point ──────────────────────────────────────────────────────────── */

void app_main(void)
{
    printf(APP_TAG " === " APP_ID " v1.0.0 ===\n");

    register_manifest();

    // TODO: any one-time initialisation that must happen at load time
    // (e.g. read + parse NVS config so it is ready before on_start is called)

    // DO NOT call on_app_start() here.
    // The base firmware calls it when the app is explicitly started.
}
