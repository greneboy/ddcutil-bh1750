#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ddcutil_c_api.h>

#define DEVICE "/dev/ttyACM0" // Serial from Arduino
#define MIN_BRIGHTNESS 60
#define MAX_BRIGHTNESS 100

// Map lux to brightness (simple linear scale)
int map_lux_to_brightness(float lux) {
    if (lux < 10) return MAX_BRIGHTNESS;
    if (lux > 200) return MIN_BRIGHTNESS;
    return MAX_BRIGHTNESS - (int)((lux - 10) * (MAX_BRIGHTNESS - MIN_BRIGHTNESS) / 190.0);
}

int main() {
    int fd = open(DEVICE, O_RDONLY | O_NOCTTY);
    if (fd < 0) {
        perror("Cannot open serial device");
        return 1;
    }

    ddca_enable_debug_trace(0);
    DDCA_Display_Info_List* dlist;
    ddca_get_display_info_list2(&dlist);
    
    if (dlist->ct == 0) {
        fprintf(stderr, "No monitors found via DDC\n");
        return 1;
    }

    DDCA_Display_Handle disp;
    ddca_open_display2(dlist->info[0].handle, &disp); // adjust if multiple monitors

    char buf[32];
    while (1) {
        int len = read(fd, buf, sizeof(buf) - 1);
        if (len > 0) {
            buf[len] = '\0';
            float lux = atof(buf);

            int brightness = map_lux_to_brightness(lux);
            ddca_set_any_vcp_value(disp, 0x10, brightness); // set the brightness
        }
        usleep(500000); // 0.5 sec
    }

    ddca_close_display(disp);
    ddca_free_display_info_list(dlist);
    close(fd);
    return 0;
}

