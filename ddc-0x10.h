#ifndef DDC_0X10_H
#define DDC_0X10_H

#include <stdint.h>
#include <ddcutil_c_api.h>
#include <stdio.h>

static inline void set_brightness_all_displays(uint16_t target_brightness) {
    DDCA_Status rc;
    DDCA_Display_Handle dh = NULL;
    DDCA_Display_Ref dref;
    DDCA_Display_Info_List* dlist = NULL;

    rc = ddca_get_display_info_list2(false, &dlist);
    if (rc != 0 || dlist == NULL) {
        fprintf(stderr, "Failed to get display list: %s\n", ddca_rc_desc(rc));
        return;
    }

    for (int i = 0; i < dlist->ct; ++i) {
        dref = dlist->info[i].dref;

        rc = ddca_open_display2(dref, false, &dh);
        if (rc != 0 || dh == NULL) {
            fprintf(stderr, "Failed to open display #%d: %s\n", i, ddca_rc_desc(rc));
            continue;
        }

        // Try to read current brightness
        DDCA_Non_Table_Vcp_Value valrec = {0};
        rc = ddca_get_non_table_vcp_value(dh, 0x10, &valrec);
        const char *repr = ddca_dh_repr(dh);
        if (rc != 0) {
            fprintf(stderr, "Skipping %s — brightness not readable: %s\n",
                    repr ? repr : "(null)", ddca_rc_desc(rc));
            ddca_close_display(dh);
            continue;
        }

        uint16_t current_value = valrec.sl;

        if (current_value == target_brightness) {
            fprintf(stderr, "Brightness already %d on %s, skipping.\n",
                    current_value, repr ? repr : "(null)");
            ddca_close_display(dh);
            continue;
        }

        // Apply new brightness
        uint8_t hi = target_brightness >> 8;
        uint8_t lo = target_brightness & 0xFF;
        rc = ddca_set_non_table_vcp_value(dh, 0x10, hi, lo);
        if (rc != 0) {
            fprintf(stderr, "Failed to set brightness to %d on %s: %s\n",
                    target_brightness, repr ? repr : "(null)", ddca_rc_desc(rc));
        } else {
            printf("Set brightness to %d on display %s\n",
                   target_brightness, repr ? repr : "(null)");
        }

        ddca_close_display(dh);
        dh = NULL;
    }

    ddca_free_display_info_list(dlist);
}

#endif
