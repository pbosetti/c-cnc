/******************************************************************************\
 File:         inic.h
 Timestamp:    2020-Jun-01
 Author:       Paolo Bosetti <paolo.bosetti@unitn.it>
 Organization: Università di Trento - https://unitn.it/dii
 LICENSE:      All rights reserved (C) 2020
               Contains some MIT-licensed code as prior-knowledge, see the
               header of relevant source files
\******************************************************************************/
/**
 * @file inic.h
 * @brief C wrapper to the `inipp.h` header-only library for managing INI files.
 * 
 * This is a C-wrapper to the `inipp.h` C++ library. The original library
 * is well apted to the flexibility of C++, simplifying the management
 * of different value types. In C, this is performed by a set of macros for 
 * creating type-mapped functions:
 * - in the header, the macro `declare_ini_get(type)` declares the function 
 *   `ini_get_type()`
 * - in the code file, the macro `ini_get(type)` defines the function 
 *   `ini_get_type()`
 * 
 * The `ini_get_<type>` functions are declared (and also defined) with
 * preprocessor macros. Their typical signature has the following arguments:
 * - `void *ini_p`: pointer to the ini object
 * - `char *section`: section name as a string
 * - `char *field`: field name as a astring
 * - `<type> *val`: pointer to output value
 * 
 * Each function returns 1 if the value has been found in the INI file, 0 
 * otherwise.
 * 
 * @see https://github.com/mcmtroffaes/inipp
 * @ingroup client
 * @ingroup server
 */

#include <stdint.h>
#include "defines.h"


#define declare_ini_get(t) \
int ini_get_##t(void *ini_p, const char *section, const char *field, t *val)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the INI object
 * 
 * @param path path to the INI file
 * @return void* opaque pointer to the INI object (a C++ object)
 */
void *ini_init(char const *path);

/**
 * @brief Delete object
 * 
 * @param ini an ini file object
 */
void ini_free(void *ini);

/**
 * @brief Construct `ini_get_int(void *ini_p, char *section, char *field, int *val)`
 */
declare_ini_get(int);

/**
 * @brief Construct `ini_get_double(void *ini_p, char *section, char *field, double *val)`
 */
declare_ini_get(double);

/**
 * @brief **Gets a char array**.
 * @param ini_p the ini object
 * @param section the name of the ini section
 * @param field the name of the desired field
 * @param val the pointer to the destination char array
 * @param len the size of the destination array (pre-allocated)
 */
int ini_get_char(void *ini_p, const char *section, const char *field, char *val, size_t len);

/**
 * @brief Construct `ini_get_uint64_t(void *ini_p, char *section, char *field, uint64_t *val)`
 */
declare_ini_get(uint64_t);

/**
 * @brief Construct `ini_get_uint32_t(void *ini_p, char *section, char *field, uint32_t *val)`
 */
declare_ini_get(uint32_t);

/**
 * @brief Construct `ini_get_long(void *ini_p, char *section, char *field, long *val)`
 */
declare_ini_get(long);

#ifdef __cplusplus
}
#endif
