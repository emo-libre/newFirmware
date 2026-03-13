/**
 * @file app_manifest.h
 * @brief App Manifest System for ELF Applications
 *
 * This header defines the manifest structure and functions for ELF apps
 * to register their UI components, API endpoints, and metadata.
 *
 * IMPORTANT: The struct layout MUST match the base firmware's app_manifest.h
 * exactly. Field order, sizes, and alignment are part of the binary ABI.
 *
 * Workflow:
 * 1. ELF app calls manifest_local_receive() or app_manifest_register()
 * 2. System calls get_node_type() to check role
 * 3. manifest_routing() decides: store locally (leader) or forward (slave)
 * 4. manifest_store() stores on leader node
 * 5. If needs_config=true, default_config is saved to NVS (if not exists)
 */

#ifndef APP_MANIFEST_H
#define APP_MANIFEST_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Maximum sizes for manifest fields
#define APP_MANIFEST_MAX_NAME_LEN        32
#define APP_MANIFEST_MAX_VERSION_LEN     16
#define APP_MANIFEST_MAX_ICON_LEN        256
#define APP_MANIFEST_MAX_HTML_LEN        32768  // 32KB max HTML content
#define APP_MANIFEST_MAX_CONFIG_LEN      8192   // 8KB max config JSON
#define APP_MANIFEST_MAX_ENDPOINTS       16
#define APP_MANIFEST_MAX_ENDPOINT_PATH   64

// Flow component constants (must match base firmware)
#define APP_MANIFEST_MAX_COMPONENT_NAME   64
#define APP_MANIFEST_MAX_COMPONENT_PARAMS 8
#define APP_MANIFEST_MAX_PARAM_NAME       32
#define APP_MANIFEST_MAX_PARAM_VALUE      64
#define APP_MANIFEST_MAX_PARAMS           8     // alias for compat
#define APP_MANIFEST_MAX_FLOW_COMPONENTS  4

/**
 * @brief API endpoint definition
 */
typedef struct {
    char path[APP_MANIFEST_MAX_ENDPOINT_PATH];  // e.g., "/api/artnet/config"
    uint8_t method;                              // HTTP method (GET=0, POST=1, PUT=2, DELETE=3)
    void* handler;                               // Function pointer to handler
} app_endpoint_t;

/**
 * @brief Flow component types
 */
typedef enum {
    FLOW_COMPONENT_INPUT = 0,      // Data source (e.g., Art-Net receiver)
    FLOW_COMPONENT_OUTPUT = 1,     // Data sink (e.g., LED driver)
    FLOW_COMPONENT_TRANSFORM = 2   // Data processor (e.g., color mapper)
} flow_component_type_t;

/**
 * @brief Flow component parameter definition
 *
 * Binary layout: name[32] + label[32] + type[16] + default_value[64] + options[256] = 400 bytes
 */
typedef struct {
    char name[APP_MANIFEST_MAX_PARAM_NAME];           // Parameter name (e.g., "gpio")
    char label[APP_MANIFEST_MAX_PARAM_NAME];          // Display label (e.g., "GPIO Pin")
    char type[16];                                     // Type: "number", "string", "bool", "select"
    char default_value[APP_MANIFEST_MAX_PARAM_VALUE]; // Default value
    char options[256];                                 // For select type: comma-separated options
} flow_component_param_t;

/**
 * @brief Flow component definition
 *
 * Binary layout: name[64] + label[64] + type(4) + category[64] + params[8×400] + param_count(1)
 */
typedef struct {
    char name[APP_MANIFEST_MAX_COMPONENT_NAME];       // Component name (e.g., "artnet_receiver")
    char label[APP_MANIFEST_MAX_COMPONENT_NAME];      // Display label (e.g., "Art-Net Receiver")
    flow_component_type_t type;                        // Component type
    char category[APP_MANIFEST_MAX_COMPONENT_NAME];   // Category (e.g., "Art-Net Controller")
    flow_component_param_t params[APP_MANIFEST_MAX_COMPONENT_PARAMS]; // Parameters
    uint8_t param_count;                               // Number of parameters
} flow_component_t;

/**
 * @brief App manifest structure
 *
 * ELF apps fill this structure and register it with the base firmware
 * to expose their UI and API to the system.
 *
 * IMPORTANT: Field order must match base firmware exactly (binary ABI).
 */
typedef struct {
    // App identification
    char name[APP_MANIFEST_MAX_NAME_LEN];           // Display name
    char id[APP_MANIFEST_MAX_NAME_LEN];             // Unique identifier (max 15 chars for NVS)
    char version[APP_MANIFEST_MAX_VERSION_LEN];     // Semantic version

    // UI components
    char icon_svg[APP_MANIFEST_MAX_ICON_LEN];       // SVG icon data (inline SVG)
    char* html_content;                              // Pointer to HTML content
    uint32_t html_content_len;                       // Length of HTML content

    // Configuration
    bool needs_config;                               // Whether app needs NVS config storage
    char* default_config;                            // Default JSON config
    uint32_t default_config_len;                     // Length of default_config

    // API endpoints
    app_endpoint_t endpoints[APP_MANIFEST_MAX_ENDPOINTS];
    uint8_t endpoint_count;

    // App state
    bool is_running;
    bool has_ui;                                     // Whether app has UI to display

    // Callbacks — MUST come before flow_components (binary layout)
    void (*on_start)(void);                            // Called when app is started
    void (*on_stop)(void);                             // Called when app is stopped
    void (*on_config_changed)(void);                   // Called when config changes
    int (*on_command)(const char* command, const char* payload, char* response, size_t response_max); // Called for routed commands

    // Flow components
    flow_component_t flow_components[APP_MANIFEST_MAX_FLOW_COMPONENTS];
    uint8_t flow_component_count;
} app_manifest_t;

/**
 * @brief HTTP methods for API endpoints
 */
typedef enum {
    APP_HTTP_GET = 0,
    APP_HTTP_POST = 1,
    APP_HTTP_PUT = 2,
    APP_HTTP_DELETE = 3
} app_http_method_t;

//=============================================================================
// Core Manifest Functions (resolved from base firmware symbol table)
//=============================================================================

int manifest_local_receive(app_manifest_t* manifest);
const char* get_node_type(void);
int manifest_routing(app_manifest_t* manifest);
int manifest_store(app_manifest_t* manifest);
void manifest_clear_leader_cache(void);

//=============================================================================
// Legacy/Compatibility Functions
//=============================================================================

int app_manifest_register(app_manifest_t* manifest);
int app_manifest_unregister(const char* app_id);
app_manifest_t* app_manifest_get(const char* app_id);
app_manifest_t** app_manifest_get_all(int* count);

#ifdef __cplusplus
}
#endif

#endif // APP_MANIFEST_H
