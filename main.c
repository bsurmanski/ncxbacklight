/*
 * NCXBacklight - Commandline ncurses interface to xrandr backlight capabilities
 * Copyright (C) 2014 Brandon Surmanski
 */

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ncurses.h>
#include <xcb/xcb.h>
#include <xcb/xcb_util.h>
#include <xcb/xproto.h>
#include <xcb/randr.h>
#include <sys/signal.h>
#include <unistd.h>

WINDOW *window;

static xcb_atom_t backlight;
static xcb_connection_t *conn;
static long value = 0;
static int width = 0;
static int height = 0;
static bool ncxb_clear = false;

static void ncxb_set(xcb_connection_t *conn, xcb_randr_output_t output, long value) {
    xcb_randr_change_output_property(conn, output, backlight, XCB_ATOM_INTEGER, 32,
            XCB_PROP_MODE_REPLACE, 1, (unsigned char*) &value);
}

static long ncxb_get(xcb_connection_t *conn, xcb_randr_output_t output) {
    xcb_generic_error_t *error;
    xcb_randr_get_output_property_reply_t *prop_reply = NULL;

    long value;
    return value;
}

void ncxb_exit(void);
void ncxb_handle_signal(int sig) {
    ncxb_exit();
    exit(0);
}

void ncxb_init_ncurses(void) {
    window = initscr();
    curs_set(0); // hide cursor
    leaveok(window, true);
    keypad(window, true);
    start_color();
    init_pair(1, COLOR_WHITE & 0xf, COLOR_BLACK * 0x0f);
    //attrset(COLOR_PAIR(1) | (COLOR_WHITE & 0xfffffff0));
    attron(COLOR_PAIR(1));

    getmaxyx(window, height, width);
}

void ncxb_init(void) {
    signal(SIGINT, ncxb_handle_signal);
    signal(SIGQUIT, ncxb_handle_signal);
    signal(SIGTERM, ncxb_handle_signal);
    // ncurses
    ncxb_init_ncurses();

    // xrandr
    conn = xcb_connect(NULL, NULL);
}

void ncxb_exit_ncurses(void) {
    refresh();
    curs_set(1); // show cursor
    leaveok(window, false);
    keypad(window, false);

    endwin();
    window = NULL;
}

void ncxb_exit(void) {
    // ncurses
    ncxb_exit_ncurses();

    //xrandr
    xcb_aux_sync(conn);
}

void update_values(void) {
}

void update(void) {
    int key = getch();
    switch(key) {
        case KEY_UP:
            value += 5;
            break;
        case KEY_DOWN:
            value -= 5;
            break;
        case '\014':
        case 'L':
        case 'l':
            ncxb_clear = true;
            break;
    }

    if(value > 100) value = 100;
    if(value < 0) value = 0;
}

void draw_value_bar(int x, int y, int h, long barval, char *barnm) {
    // draw bottom of bar
    mvaddch(y, x+2, ACS_LLCORNER);
    mvaddch(y, x+3, ACS_HLINE);
    mvaddch(y, x+4, ACS_HLINE);
    mvaddch(y, x+5, ACS_LRCORNER);

    // draw vertical sides of bar
    int i;
    for(i = 1; i < h; i++) {
        //mvaddstr(y-i, x, "         ");
        mvaddch(y-i, x + 2, ACS_VLINE);
        mvaddch(y-i, x + 5, ACS_VLINE);
    }

    // draw bar contents
    for(i = 1; i < h; i++) {
        int dc = barval > i * 100 / h ? ACS_CKBOARD : ' ';
        mvaddch(y-i, x + 3, dc); //left half of bar
        mvaddch(y-i, x + 4, dc); //right half of bar
    }

    // draw top of bar
    mvaddstr(y - h, x, "         ");
    mvaddch(y - h, x+2, ACS_ULCORNER);
    mvaddch(y - h, x+3, ACS_HLINE);
    mvaddch(y - h, x+4, ACS_HLINE);
    mvaddch(y - h, x+5, ACS_URCORNER);

    // write value of bar below
    char valuestr[128];
    snprintf(valuestr, sizeof(valuestr), "%ld", barval);
    mvaddstr(y+1, x+2, "        ");
    mvaddstr(y+1, x+3 - (barval >= 100), valuestr);

    // write name of bar
    if(barnm) {
        mvaddstr(y+2, x+2, "        ");
        mvaddstr(y+2, x+3 - strlen(barnm)/2, barnm);
    }
}

void draw_frame(int w, int h) {
    int i;

    // top/bottom lines
    for(i = 1; i < w; i++) {
        mvaddch(0, i, ACS_HLINE);
        mvaddch(h - 1, i, ACS_HLINE);
    }

    // side lines
    for(i = 1; i < h; i++) {
        mvaddch(i, 0, ACS_VLINE);
        mvaddch(i, w - 1, ACS_VLINE);
    }

    // corners
    mvaddch(0, 0, ACS_ULCORNER);
    mvaddch(0, w-1, ACS_URCORNER);
    mvaddch(h-1, 0, ACS_LLCORNER);
    mvaddch(h-1, w-1, ACS_LRCORNER);

    char title[128];
    int titlelen;
    sprintf(title, "%s v%s", "NCXBacklight", "0.1");
    titlelen = strlen(title);
    mvaddstr(0, w / 2 - titlelen / 2, title);
}

void draw(void) {
    //attrset(COLOR_PAIR(0)
    if(ncxb_clear) {
        clearok(window, true);
    }

    draw_value_bar(width / 2 - 2, height - 5, height - 8, value, "Main");
    draw_frame(width, height);
    refresh();
}

int main(int argc, char **argv) {
    ncxb_init();

    xcb_generic_error_t *error;

    xcb_intern_atom_cookie_t backlight_cookie = xcb_intern_atom(conn, 1, strlen("Backlight"), "Backlight");
    xcb_intern_atom_reply_t *backlight_reply = xcb_intern_atom_reply(conn, backlight_cookie, &error);
    backlight = backlight_reply->atom;
    free(backlight_reply);

    xcb_screen_iterator_t iter;
    iter = xcb_setup_roots_iterator(xcb_get_setup(conn));
    while(iter.rem) {
        //resources_cookie = xcb_randr_get_screen_resources(conn,
        xcb_screen_t *screen = iter.data;
        xcb_window_t root = screen->root;

        xcb_randr_get_screen_resources_cookie_t resources_cookie;
        xcb_randr_get_screen_resources_reply_t *resources_reply;

        resources_cookie = xcb_randr_get_screen_resources(conn, root);
        resources_reply = xcb_randr_get_screen_resources_reply(conn, resources_cookie, &error);
        if(error || !resources_reply) {
            int ec = error ? error->error_code : -1;
            fprintf(stderr, "RANDR Get Screen Resources returned error %d\n", ec);
            continue;
        }

        xcb_randr_output_t *outputs = xcb_randr_get_screen_resources_outputs(resources_reply);
        int i;
        for(i = 0; i < resources_reply->num_outputs; i++) {
            xcb_randr_output_t output = outputs[i];
            double cur;

            cur = ncxb_get(conn, output);
            xcb_aux_sync(conn);
            ncxb_set(conn, output, 500);
            xcb_flush(conn);
            usleep(200);
        }

        free(resources_reply);
        xcb_screen_next(&iter);
    }


    while(true) {
        draw();
        update();
    }

    ncxb_exit();

    return 0;
}
