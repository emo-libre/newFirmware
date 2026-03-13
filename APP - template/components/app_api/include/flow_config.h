/**
 * @file flow_config.h
 * @brief Flow Configuration API for ELF Apps (read-only)
 *
 * Flow configuration is managed by the base firmware.
 * ELF apps only need flow_config_exists() to check if a config is set.
 * Plugin loading and lifecycle is handled entirely by the base firmware.
 */

#ifndef FLOW_CONFIG_H
#define FLOW_CONFIG_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Check if flow configuration exists for an app
 *
 * @param app_id App ID to check
 * @return true if configuration exists, false otherwise
 */
bool flow_config_exists(const char* app_id);

#ifdef __cplusplus
}
#endif

#endif // FLOW_CONFIG_H
