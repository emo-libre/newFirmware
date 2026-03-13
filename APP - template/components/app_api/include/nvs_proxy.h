/**
 * @file nvs_proxy.h
 * @brief NVS Proxy for ELF Applications
 *
 * Provides NVS storage access to ELF apps without requiring them to link nvs_flash.
 * Each ELF app gets its own NVS namespace based on its app_id.
 * Configuration is stored as JSON blobs.
 *
 * Workflow:
 * 1. ELF app registers manifest with needs_config=true and default_config JSON
 * 2. Base firmware creates NVS namespace for app if not exists
 * 3. Base firmware saves default_config if no existing config
 * 4. ELF app calls nvs_proxy_get_config() to read config
 * 5. Web UI can update config via nvs_proxy_set_config()
 */

#ifndef NVS_PROXY_H
#define NVS_PROXY_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Maximum config blob size (16KB - increased for larger app configs)
// Note: NVS blob max is ~508KB per entry, but we limit to reasonable size
#define NVS_PROXY_MAX_CONFIG_SIZE 16384

// Fixed key name for config blob
#define NVS_PROXY_CONFIG_KEY "config"

/**
 * @brief Read config blob from NVS for an ELF app
 *
 * The app_id is used as the NVS namespace. Each app has isolated storage.
 *
 * @param app_id Unique identifier of the ELF app (max 15 chars, used as NVS namespace)
 * @param out_value Pointer to buffer to receive the config JSON data
 * @param length Pointer to size variable:
 *               - Input: size of out_value buffer
 *               - Output: actual size of the config
 * @return 0 on success, negative error code on failure
 *         -1: Invalid parameters
 *         -2: NVS open failed
 *         -3: Config not found
 *         -4: Buffer too small (length will contain required size)
 */
int nvs_proxy_get_config(const char* app_id, void* out_value, size_t* length);

/**
 * @brief Write config blob to NVS for an ELF app
 *
 * The app_id is used as the NVS namespace. Each app has isolated storage.
 *
 * @param app_id Unique identifier of the ELF app (max 15 chars)
 * @param value Pointer to the config JSON data to write
 * @param length Size of the config data in bytes (max NVS_PROXY_MAX_CONFIG_SIZE)
 * @return 0 on success, negative error code on failure
 *         -1: Invalid parameters
 *         -2: NVS open failed
 *         -3: NVS write failed
 *         -4: NVS commit failed
 *         -5: Config too large
 */
int nvs_proxy_set_config(const char* app_id, const void* value, size_t length);

/**
 * @brief Check if config exists for an ELF app
 *
 * @param app_id Unique identifier of the ELF app
 * @return 1 if config exists, 0 if not, negative on error
 */
int nvs_proxy_config_exists(const char* app_id);

/**
 * @brief Erase config for an ELF app
 *
 * @param app_id Unique identifier of the ELF app
 * @return 0 on success, negative error code on failure
 */
int nvs_proxy_erase_config(const char* app_id);

/**
 * @brief Initialize default config for an app if not exists
 *
 * Called by app_manifest when an app with needs_config=true registers.
 * Only writes default_config if no existing config is found.
 *
 * @param app_id Unique identifier of the ELF app
 * @param default_config Default JSON config string
 * @param length Length of default_config
 * @return 0 on success (config initialized or already exists), negative on error
 */
int nvs_proxy_init_default_config(const char* app_id, const char* default_config, size_t length);

/**
 * @brief Initialize NVS proxy system
 *
 * Called by base firmware during startup.
 * Ensures NVS flash is initialized.
 *
 * @return 0 on success, negative on failure
 */
int nvs_proxy_init(void);

#ifdef __cplusplus
}
#endif

#endif // NVS_PROXY_H
