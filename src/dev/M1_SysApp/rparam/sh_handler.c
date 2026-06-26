/************************************************
 *  @file     : sh_handler.c
 *  @brief    : Shell-tunnel via RPARAM Table 1
 *
 *  Architecture:
 *    - RPARAM callback (CSP task) → posts cmd to queue → returns immediately
 *    - Dedicated worker task drains queue, runs command via embedded_cli,
 *      stores output in a 4 KB static buffer, updates table fields.
 *    - All large buffers are file-scope statics (never on any task stack).
 *    - Uses a private embedded_cli instance (independent of UART2 / RGOSH).
 ************************************************/

#include "sh_handler.h"
#include "exp_rparam_table.h"

#include "M1_SysApp/EmbeddedCLI/CLI_Setup/cli_setup.h"
#include "M1_SysApp/EmbeddedCLI/CLI_Command/cli_command.h"
#include "M1_SysApp/dmesg/dmesg.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* =========================================================================
 * Config
 * ========================================================================= */

#define SH_CLI_BUFFER_SIZE   2048
#define SH_WORKER_STACK_WORDS  384   /* 1.5 KB — enough for CLI dispatch */
#define SH_WORKER_PRIORITY   (configMAX_PRIORITIES - 5)
#define SH_QUEUE_DEPTH         1     /* one pending command at a time */

/* =========================================================================
 * Private CLI instance
 * ========================================================================= */

static CLI_UINT    s_cli_buf[BYTES_TO_CLI_UINTS(SH_CLI_BUFFER_SIZE)];
static EmbeddedCli *s_cli = NULL;

/* =========================================================================
 * Output buffer — 4 KB static, never on any stack
 * ========================================================================= */

static char   g_out[SH_MAX_OUTPUT + 1];
static size_t g_out_len      = 0;
static bool   g_capturing    = false;

static void sh_writeChar(EmbeddedCli *cli, char c)
{
    (void)cli;
    if (!g_capturing) return;
    if (g_out_len < SH_MAX_OUTPUT) {
        g_out[g_out_len++] = c;
    }
    /* Buffer full: silently discard — truncation marker appended after exec */
}

/* =========================================================================
 * Table 1 memory access
 * ========================================================================= */

static uint8_t *g_t1_mem      = NULL;
static size_t   g_t1_mem_size = 0;

static inline bool t1_ok(uint16_t off, size_t len)
{
    return g_t1_mem && ((size_t)off + len <= g_t1_mem_size);
}

static void t1_set_u8(uint16_t off, uint8_t v)
{
    if (t1_ok(off, 1)) g_t1_mem[off] = v;
}

static void t1_set_i8(uint16_t off, int8_t v)
{
    if (t1_ok(off, 1)) g_t1_mem[off] = (uint8_t)v;
}

static void t1_set_u16(uint16_t off, uint16_t v)
{
    if (t1_ok(off, sizeof(v))) memcpy(g_t1_mem + off, &v, sizeof(v));
}

static uint8_t t1_get_u8(uint16_t off)
{
    return t1_ok(off, 1) ? g_t1_mem[off] : 0;
}

static const char *t1_get_str(uint16_t off)
{
    return g_t1_mem ? (const char *)(g_t1_mem + off) : "";
}

static void t1_set_str(uint16_t off, const char *s, size_t maxlen)
{
    if (!t1_ok(off, maxlen)) return;
    char *p = (char *)(g_t1_mem + off);
    strncpy(p, s, maxlen - 1);
    p[maxlen - 1] = '\0';
}

/* =========================================================================
 * Output paging
 * ========================================================================= */

static uint8_t calc_pages(size_t len)
{
    if (len == 0) return 1;
    uint32_t p = (uint32_t)((len + SH_PAGE_SIZE - 1) / SH_PAGE_SIZE);
    return (p > 255u) ? 255u : (uint8_t)p;
}

static void load_page(uint8_t idx)
{
    if (g_out_len == 0) {
        t1_set_str(PARAM_T1_OUT, "", SH_PAGE_SIZE);
        return;
    }

    size_t offset = (size_t)idx * SH_PAGE_SIZE;
    if (offset >= g_out_len) {
        t1_set_str(PARAM_T1_OUT, "", SH_PAGE_SIZE);
        return;
    }

    size_t avail = g_out_len - offset;
    size_t copy  = (avail < (size_t)(SH_PAGE_SIZE - 1))
                   ? avail : (size_t)(SH_PAGE_SIZE - 1);

    char page_buf[SH_PAGE_SIZE];
    memset(page_buf, 0, sizeof(page_buf));
    memcpy(page_buf, g_out + offset, copy);
    page_buf[copy] = '\0';

    t1_set_str(PARAM_T1_OUT, page_buf, SH_PAGE_SIZE);
}

/* =========================================================================
 * Stop flag  (written from callback / CSP task, read from worker task)
 * ========================================================================= */

static volatile bool g_stop_req = false;

/* =========================================================================
 * Worker task
 * ========================================================================= */

typedef struct {
    char cmd[64];
} sh_cmd_msg_t;

static StackType_t  s_worker_stack[SH_WORKER_STACK_WORDS];
static StaticTask_t s_worker_tcb;
static TaskHandle_t s_worker_handle = NULL;

static StaticQueue_t s_queue_static;
static sh_cmd_msg_t  s_queue_storage[SH_QUEUE_DEPTH];
static QueueHandle_t g_queue = NULL;

static void do_exec(const char *cmd)
{
    if (!s_cli || !g_t1_mem) return;

    t1_set_u8(PARAM_T1_STATUS, SH_STATUS_RUNNING);

    /* Reset output */
    g_out_len    = 0;
    g_out[0]     = '\0';
    g_capturing  = true;

    /* Inject command char-by-char then send CR */
    for (const char *p = cmd; *p; p++)
        embeddedCliReceiveChar(s_cli, *p);
    embeddedCliReceiveChar(s_cli, '\r');

    /* Execute — synchronous, returns when command handler returns */
    embeddedCliProcess(s_cli);

    g_capturing = false;

    /* Check truncation */
    if (g_out_len >= SH_MAX_OUTPUT) {
        const char marker[] = "\n[...truncated]\n";
        size_t mlen = strlen(marker);
        if (g_out_len >= mlen)
            memcpy(g_out + g_out_len - mlen, marker, mlen);
    }
    g_out[g_out_len] = '\0';

    /* Stop requested while we were running? */
    if (g_stop_req) {
        g_stop_req = false;
        t1_set_u8(PARAM_T1_STATUS,  SH_STATUS_STOPPED);
        t1_set_u8(PARAM_T1_PAGES,   0);
        t1_set_u16(PARAM_T1_OUTLEN, 0);
        t1_set_str(PARAM_T1_OUT,    "", SH_PAGE_SIZE);
        return;
    }

    /* Update table */
    uint8_t  pages  = calc_pages(g_out_len);
    uint16_t outlen = (g_out_len > 0xFFFFu)
                      ? (uint16_t)0xFFFFu : (uint16_t)g_out_len;

    t1_set_i8 (PARAM_T1_RETCODE, 0);
    t1_set_u8 (PARAM_T1_PAGES,   pages);
    t1_set_u16(PARAM_T1_OUTLEN,  outlen);
    t1_set_u8 (PARAM_T1_PAGE,    0);   /* reset page cursor */
    load_page(0);                       /* pre-load page 0   */
    t1_set_u8 (PARAM_T1_STATUS,  SH_STATUS_DONE);

    char dbg[64];
    snprintf(dbg, sizeof(dbg),
             "[SH] done: %zu bytes, %u pages", g_out_len, pages);
    Dmesg_Write(dbg);
}

static void sh_worker_task(void *arg)
{
    (void)arg;
    Dmesg_Write("[SH] worker task started");

    sh_cmd_msg_t msg;
    for (;;) {
        if (xQueueReceive(g_queue, &msg, portMAX_DELAY) != pdTRUE)
            continue;

        /* Discard if stop was requested before we even start */
        if (g_stop_req) {
            g_stop_req = false;
            continue;
        }

        do_exec(msg.cmd);
    }
}

/* =========================================================================
 * Public API
 * ========================================================================= */

void sh_handler_set_table(gs_param_table_instance_t *table1)
{
    if (!table1 || !table1->memory) {
        Dmesg_Write("[SH] set_table: NULL");
        return;
    }

    g_t1_mem = (uint8_t *)table1->memory;

    /* Compute usable memory size from row definitions */
    size_t computed = 0;
    if (table1->rows && table1->row_count > 0) {
        for (uint16_t i = 0; i < table1->row_count; i++) {
            const gs_param_table_row_t *r = &table1->rows[i];
            uint32_t arr = (r->array_size > 0) ? r->array_size : 1u;
            size_t end = (size_t)r->addr + (size_t)r->size * arr;
            if (end > computed) computed = end;
        }
    }
    g_t1_mem_size = computed;

    char dbg[64];
    snprintf(dbg, sizeof(dbg),
             "[SH] table1 @ %p, %zu bytes", (void *)g_t1_mem, g_t1_mem_size);
    Dmesg_Write(dbg);
}

void sh_handler_start(void)
{
    /* Create private embedded_cli instance */
    EmbeddedCliConfig *cfg = embeddedCliDefaultConfig();
    cfg->cliBuffer          = s_cli_buf;
    cfg->cliBufferSize      = SH_CLI_BUFFER_SIZE;
    cfg->rxBufferSize       = 128;
    cfg->cmdBufferSize      = 64;
    cfg->historyBufferSize  = 0;
    cfg->maxBindingCount    = CLI_MAX_BINDING_COUNT;
    cfg->enableAutoComplete = 0;
    cfg->invitation         = "";
    cfg->staticBindings     = getCliStaticBindings();
    cfg->staticBindingCount = getCliStaticBindingCount();

    s_cli = embeddedCliNew(cfg);
    if (!s_cli) {
        Dmesg_Write("[SH] CLI init FAILED");
        return;
    }
    s_cli->writeChar = sh_writeChar;

    /* Static queue — no heap allocation */
    g_queue = xQueueCreateStatic(SH_QUEUE_DEPTH, sizeof(sh_cmd_msg_t),
                                 (uint8_t *)s_queue_storage, &s_queue_static);
    if (!g_queue) {
        Dmesg_Write("[SH] queue init FAILED");
        return;
    }

    /* Static task — no heap allocation */
    s_worker_handle = xTaskCreateStatic(
        sh_worker_task, "sh_work",
        SH_WORKER_STACK_WORDS, NULL,
        SH_WORKER_PRIORITY,
        s_worker_stack, &s_worker_tcb);

    if (!s_worker_handle) {
        Dmesg_Write("[SH] task create FAILED");
        return;
    }

    Dmesg_Write("[SH] handler ready");
}

/* =========================================================================
 * Callback — called from CSP task after SET writes to Table 1
 * ========================================================================= */

void sh_table1_callback(uint16_t addr, gs_param_table_instance_t *tinst)
{
    (void)tinst;

    /* --- page selector: load page immediately, no queue needed --- */
    if (addr == PARAM_T1_PAGE) {
        uint8_t pg = t1_get_u8(PARAM_T1_PAGE);
        uint8_t total = t1_get_u8(PARAM_T1_PAGES);

        if (total == 0 || pg >= total) {
            t1_set_str(PARAM_T1_OUT, "", SH_PAGE_SIZE);
            return;
        }
        load_page(pg);
        return;
    }

    /* --- command trigger --- */
    if (addr == PARAM_T1_EX) {
        const char *cmd = t1_get_str(PARAM_T1_EX);

        /* "stop": abort running command, reset to idle */
        if (strcmp(cmd, "stop") == 0) {
            g_stop_req = true;
            /* Immediately reset visible status so poll sees IDLE */
            t1_set_u8 (PARAM_T1_STATUS,  SH_STATUS_IDLE);
            t1_set_u8 (PARAM_T1_PAGES,   0);
            t1_set_u16(PARAM_T1_OUTLEN,  0);
            t1_set_str(PARAM_T1_OUT,     "", SH_PAGE_SIZE);
            Dmesg_Write("[SH] stop requested");
            return;
        }

        /* Empty string: no-op */
        if (cmd[0] == '\0') return;

        /* Reject if already running (guard against double-trigger) */
        if (t1_get_u8(PARAM_T1_STATUS) == SH_STATUS_RUNNING) {
            Dmesg_Write("[SH] busy, ignoring new cmd");
            return;
        }

        if (!g_queue) return;

        sh_cmd_msg_t msg;
        strncpy(msg.cmd, cmd, sizeof(msg.cmd) - 1);
        msg.cmd[sizeof(msg.cmd) - 1] = '\0';

        /* Non-blocking send — if queue full (previous cmd not yet consumed)
         * the new request is silently dropped. Depth=1 means at most one
         * command can be queued while the worker is busy. */
        BaseType_t sent = xQueueSend(g_queue, &msg, 0);
        if (sent != pdTRUE) {
            Dmesg_Write("[SH] queue full, cmd dropped");
        }
    }
}
