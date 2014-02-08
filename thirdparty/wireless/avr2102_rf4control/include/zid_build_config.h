/**
 * @file zid_build_config.h
 *
 * @brief This header file declares macros for various build configurations
 *
 * $Id: zid_build_config.h 33680 2012-11-30 12:26:17Z agasthian.s $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support email: avr@atmel.com
 */
/*
 * Copyright (c) 2014, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef ZID_BUILD_CONFIG_H
#define ZID_BUILD_CONFIG_H

/* === Includes ============================================================= */

/* === Macros =============================================================== */


/* NWK primitive configuration */

#ifdef ZID_USER_BUILD_CONFIG
#include "zid_user_build_config.h"
#else   /* Include all ZID primitives */
/* ZID primitive configuration */
#define ZID_SET									(1)
#define ZID_GET									(1)

#endif


/* === Types ================================================================ */

/* === Externals ============================================================ */

/* === Prototypes =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif



#endif  /* ZID_BUILD_CONFIG_H */
/* EOF */

