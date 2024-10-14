#include <vte/vte.h>
#include <gtk/gtk.h>
#include "config.h"
#include "util.h"
#include <string.h>

static gboolean term_key_event(VteTerminal *terminal, GdkEventKey *event, gpointer user_data);
static void create_terminal(GtkWidget *window, char **command);
static void quit(VteTerminal *terminal, gint status, gpointer user_data);

// Font size 
static void adjust_font_size(VteTerminal *terminal, gboolean increase) {
    double scale = vte_terminal_get_font_scale(terminal);
    scale += increase ? SCALE_FACTOR : -SCALE_FACTOR;
    vte_terminal_set_font_scale(terminal, scale);
}

// Scroll terminal
static void scroll_terminal(VteTerminal *terminal, gboolean up, gboolean page) {
    GtkAdjustment *adj = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(terminal));
    gdouble value = gtk_adjustment_get_value(adj);
    gdouble step = page ? vte_terminal_get_row_count(terminal) - 1 : SCROLL_LINE;
    if (up) {
        value = MAX(value - step, gtk_adjustment_get_lower(adj));
    } else {
        gdouble upper = gtk_adjustment_get_upper(adj);
        gdouble page_size = gtk_adjustment_get_page_size(adj);
        value = MIN(value + step, upper - page_size);
    }
    gtk_adjustment_set_value(adj, value);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GdkPixbuf *icon;
    static char **command = NULL;
    const char *instance_name = termname;
    const char *class_name = termname;

    gtk_init(&argc, &argv);

    // the command-line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
            command = &argv[i + 1];
            break;
        } else if (i + 1 < argc) {
            if (strcmp(argv[i], "-n") == 0) instance_name = argv[++i];
            else if (strcmp(argv[i], "-c") == 0) class_name = argv[++i];
            else if (strcmp(argv[i], "-w") == 0) width = argv[++i];
            else if (strcmp(argv[i], "-h") == 0) height = argv[++i];
        }
    }
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    // Set icon
    icon = gdk_pixbuf_new_from_file("/usr/share/icons/hicolor/512x512/apps/snt.png" ,NULL);
    if (icon){
      gtk_window_set_icon(GTK_WINDOW(window),icon);
      g_object_unref(icon);
    }else {
      g_print("Feild Icon");
    }
    gtk_window_set_title(GTK_WINDOW(window), TITLE);
    gtk_window_set_default_size(GTK_WINDOW(window), atoi(width), atoi(height));

    if (instance_name || class_name) {
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
        gtk_window_set_wmclass(GTK_WINDOW(window), instance_name, class_name);
        #pragma GCC diagnostic pop
        if (instance_name) {
            g_set_prgname(instance_name);
            gtk_window_set_role(GTK_WINDOW(window), instance_name);
        }
        if (class_name) g_set_application_name(class_name);
    }

    create_terminal(window, command);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}

static void create_terminal(GtkWidget *window, char **command) {
    GtkWidget *terminal = vte_terminal_new();
    PangoFontDescription *font_desc = pango_font_description_new();
    pango_font_description_set_family(font_desc, FONT);
    pango_font_description_set_size(font_desc, FONT_SIZE * PANGO_SCALE);
    vte_terminal_set_font(VTE_TERMINAL(terminal), font_desc);

    // Css syntax for padding effect
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, "vte-terminal { padding: 10px}", -1, NULL);
    gtk_style_context_add_provider(
        gtk_widget_get_style_context(GTK_WIDGET(terminal)),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    pango_font_description_free(font_desc);
    g_object_unref(provider);

    vte_terminal_set_mouse_autohide(VTE_TERMINAL(terminal), CURSOR_AH);
    vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(terminal), cursor_blink());
    vte_terminal_set_cursor_shape(VTE_TERMINAL(terminal), cursorshape());
    vte_terminal_set_colors(VTE_TERMINAL(terminal), &CLR_GDK(CLR_7), NULL, PALETTE, PALETTE_SIZE);

    // Final Command 
    char *final_command[2] = {NULL, NULL};
    if (command == NULL) {
        final_command[0] = g_strdup(g_getenv("SHELL") ?: shell);
    }

    vte_terminal_spawn_async(
        VTE_TERMINAL(terminal),
        VTE_PTY_DEFAULT,
        NULL, command ? command : final_command, NULL,
        G_SPAWN_SEARCH_PATH,
        NULL, NULL, NULL, -1, NULL, NULL, NULL
    );

    if (command == NULL) g_free(final_command[0]);

    g_signal_connect(terminal, "child-exited", G_CALLBACK(quit), NULL);
    g_signal_connect(terminal, "key-press-event", G_CALLBACK(term_key_event), window);
    gtk_container_add(GTK_CONTAINER(window), terminal);
}

static void quit(VteTerminal *terminal, gint status, gpointer user_data) {
    (void)terminal; (void)status; (void)user_data;
    gtk_main_quit();
}

// Key events
static gboolean term_key_event(VteTerminal *terminal, GdkEventKey *event, gpointer user_data) {
    const guint modifiers = event->state & gtk_accelerator_get_default_mod_mask();
    if (modifiers == (MODIFIER)) {
        switch (gdk_keyval_to_lower(event->keyval)) {
            case COPY:
                vte_terminal_copy_clipboard_format(terminal, VTE_FORMAT_TEXT);
                return TRUE;
            case PASTE:
                vte_terminal_paste_clipboard(terminal);
                return TRUE;
            case INCREASE_FONT:
                adjust_font_size(terminal, TRUE);
                return TRUE;
            case DECREASE_FONT:
                adjust_font_size(terminal, FALSE);
                return TRUE;
            case RESET_FONT:
                vte_terminal_set_font_scale(terminal, FONT_SCALE);
                return TRUE;
            case SCROLL_UP:
                scroll_terminal(terminal, TRUE, FALSE);
                return TRUE;
            case SCROLL_DOWN:
                scroll_terminal(terminal, FALSE, FALSE);
                return TRUE;
            case SCROLL_PAGEUP:
                scroll_terminal(terminal, TRUE, TRUE);
                return TRUE;
            case SCROLL_PAGEDOWN:
                scroll_terminal(terminal, FALSE, TRUE);
                return TRUE;
        }
    }
    return FALSE;
}
