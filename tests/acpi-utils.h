/*
 * Utilities for working with ACPI tables
 *
 * Copyright (c) 2013 Red Hat Inc.
 *
 * Authors:
 *  Michael S. Tsirkin <mst@redhat.com>,
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#ifndef TEST_ACPI_UTILS_H
#define TEST_ACPI_UTILS_H

#include "hw/acpi/acpi-defs.h"
#include "libqtest.h"

/* DSDT and SSDTs format */
typedef struct {
    union {
        AcpiTableHeader *header;
        uint8_t *aml;            /* aml bytecode from guest */
    };
    uint32_t aml_len;
    gchar *aml_file;
    gchar *asl;            /* asl code generated from aml */
    gsize asl_len;
    gchar *asl_file;
    bool tmp_files_retain;   /* do not delete the temp asl/aml */
} AcpiSdtTable;

#define ACPI_READ_FIELD(qts, field, addr)                \
    do {                                                 \
        qtest_memread(qts, addr, &field, sizeof(field)); \
        addr += sizeof(field);                           \
    } while (0)

#define ACPI_READ_ARRAY_PTR(qts, arr, length, addr)      \
    do {                                                 \
        int idx;                                         \
        for (idx = 0; idx < length; ++idx) {             \
            ACPI_READ_FIELD(qts, arr[idx], addr);        \
        }                                                \
    } while (0)

#define ACPI_READ_ARRAY(qts, arr, addr)                                 \
    ACPI_READ_ARRAY_PTR(qts, arr, sizeof(arr) / sizeof(arr[0]), addr)

#define ACPI_ASSERT_CMP(actual, expected) do { \
    char ACPI_ASSERT_CMP_str[5] = {}; \
    memcpy(ACPI_ASSERT_CMP_str, &actual, 4); \
    g_assert_cmpstr(ACPI_ASSERT_CMP_str, ==, expected); \
} while (0)

#define ACPI_ASSERT_CMP64(actual, expected) do { \
    char ACPI_ASSERT_CMP_str[9] = {}; \
    memcpy(ACPI_ASSERT_CMP_str, &actual, 8); \
    g_assert_cmpstr(ACPI_ASSERT_CMP_str, ==, expected); \
} while (0)


#define ACPI_FOREACH_RSDT_ENTRY(table, table_len, entry_ptr, entry_size) \
    for (entry_ptr = table + 36 /* 1st Entry */;                         \
         entry_ptr < table + table_len;                                  \
         entry_ptr += entry_size)

uint8_t acpi_calc_checksum(const uint8_t *data, int len);
uint32_t acpi_find_rsdp_address(QTestState *qts);
uint64_t acpi_get_xsdt_address(uint8_t *rsdp_table);
void acpi_parse_rsdp_table(QTestState *qts, uint32_t addr, uint8_t *rsdp_table);
void acpi_fetch_table(QTestState *qts, uint8_t **aml, uint32_t *aml_len,
                      const uint8_t *addr_ptr, const char *sig,
                      bool verify_checksum);

#endif  /* TEST_ACPI_UTILS_H */
