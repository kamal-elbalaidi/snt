#include <vte/vte.h>
#include <gtk/gtk.h>
#include "config.h"
#include "util.h"
#include <string.h>

static void adjust_font_size(VteTerminal *t, gboolean inc) {
    vte_terminal_set_font_scale(t, vte_terminal_get_font_scale(t) + (inc ? SCALE_FACTOR : -SCALE_FACTOR));
}

static void scroll_terminal(VteTerminal *t, gboolean up, gboolean page) {
    GtkAdjustment *adj = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(t));
    gdouble step = page ? vte_terminal_get_row_count(t) - 1 : SCROLL_LINE;
    gdouble val  = gtk_adjustment_get_value(adj) + (up ? -step : step);
    gdouble low  = gtk_adjustment_get_lower(adj);
    gdouble high = gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj);
    gtk_adjustment_set_value(adj, val < low ? low : val > high ? high : val);
}

static void quit(VteTerminal *t, gint s, gpointer u) {
    (void)t; (void)s; (void)u;
    gtk_main_quit();
}

static gboolean term_key_event(VteTerminal *t, GdkEventKey *event, gpointer user_data) {
    if ((event->state & gtk_accelerator_get_default_mod_mask()) != (MODIFIER)) return FALSE;
    switch (gdk_keyval_to_lower(event->keyval)) {
        case COPY:            vte_terminal_copy_clipboard_format(t, VTE_FORMAT_TEXT); break;
        case PASTE:           vte_terminal_paste_clipboard(t);                        break;
        case INCREASE_FONT:   adjust_font_size(t, TRUE);                              break;
        case DECREASE_FONT:   adjust_font_size(t, FALSE);                             break;
        case RESET_FONT:      vte_terminal_set_font_scale(t, FONT_SCALE);             break;
        case SCROLL_UP:       scroll_terminal(t, TRUE,  FALSE);                       break;
        case SCROLL_DOWN:     scroll_terminal(t, FALSE, FALSE);                       break;
        case SCROLL_PAGEUP:   scroll_terminal(t, TRUE,  TRUE);                        break;
        case SCROLL_PAGEDOWN: scroll_terminal(t, FALSE, TRUE);                        break;
        default: return FALSE;
    }
    return TRUE;
}

static void create_terminal(GtkWidget *window, char **command) {
    GtkWidget *terminal = vte_terminal_new();
    GtkWidget *box = gtk_event_box_new();

    PangoFontDescription *font = pango_font_description_new();
    pango_font_description_set_family(font, FONT);
    pango_font_description_set_size(font, FONT_SIZE * PANGO_SCALE);
    vte_terminal_set_font(VTE_TERMINAL(terminal), font);
    pango_font_description_free(font);


    GtkCssProvider *css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css, get_css_config(), -1, NULL);
    gtk_style_context_add_provider(
        gtk_widget_get_style_context(box),
        GTK_STYLE_PROVIDER(css),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(css);

    gtk_widget_set_margin_top(terminal,    BORDERBOX);
    gtk_widget_set_margin_bottom(terminal, BORDERBOX);
    gtk_widget_set_margin_start(terminal,  BORDERBOX);
    gtk_widget_set_margin_end(terminal,    BORDERBOX);

    gtk_container_add(GTK_CONTAINER(box), terminal);
    gtk_container_add(GTK_CONTAINER(window), box);

    vte_terminal_set_mouse_autohide(VTE_TERMINAL(terminal), CURSOR_AH);
    vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(terminal), cursor_blink());
    vte_terminal_set_cursor_shape(VTE_TERMINAL(terminal), cursorshape());
    vte_terminal_set_colors(VTE_TERMINAL(terminal), &CLR_GDK(CLR_7), NULL, PALETTE, PALETTE_SIZE);

    char *shell_cmd[2] = {NULL, NULL};
    if (!command) shell_cmd[0] = g_strdup(g_getenv("SHELL") ?: shell);

    vte_terminal_spawn_async(VTE_TERMINAL(terminal), VTE_PTY_DEFAULT,
        NULL, command ? command : shell_cmd, NULL, G_SPAWN_SEARCH_PATH,
        NULL, NULL, NULL, -1, NULL, NULL, NULL);

    if (!command) g_free(shell_cmd[0]);

    g_signal_connect(terminal, "child-exited",    G_CALLBACK(quit),           NULL);
    g_signal_connect(terminal, "key-press-event", G_CALLBACK(term_key_event), window);
    gtk_container_add(GTK_CONTAINER(window), terminal);
}

int main(int argc, char *argv[]) {
    char **command = NULL;
    const char *instance_name = termname, *class_name = termname;
    gtk_init(&argc, &argv);

    for (int i = 1; i < argc; i++) {
        if      (!strcmp(argv[i], "-e") && i+1 < argc) { command = &argv[i+1]; break; }
        else if (!strcmp(argv[i], "-n") && i+1 < argc)   instance_name = argv[++i];
        else if (!strcmp(argv[i], "-c") && i+1 < argc)   class_name    = argv[++i];
        else if (!strcmp(argv[i], "-w") && i+1 < argc)   width         = argv[++i];
        else if (!strcmp(argv[i], "-h") && i+1 < argc)   height        = argv[++i];
    }

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), TITLE);
    gtk_window_set_default_size(GTK_WINDOW(window), atoi(width), atoi(height));

    GdkPixbuf *icon = gdk_pixbuf_new_from_file(icon_path, NULL);
    if (icon) { gtk_window_set_icon(GTK_WINDOW(window), icon); g_object_unref(icon); }

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    gtk_window_set_wmclass(GTK_WINDOW(window), instance_name, class_name);
    #pragma GCC diagnostic pop
    g_set_prgname(instance_name);
    gtk_window_set_role(GTK_WINDOW(window), instance_name);
    g_set_application_name(class_name);

    create_terminal(window, command);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
