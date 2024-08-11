#include <vte/vte.h>
#include <gtk/gtk.h>
#include "config.h"
#include "util.h"
#include <string.h>

static gboolean window_key_event(GtkWindow *window, GdkEventKey *event);
static gboolean term_key_event(VteTerminal *terminal, GdkEventKey *event);
static void decrease_font(VteTerminal *terminal);
static void increase_font(VteTerminal *terminal);
static void create_terminal(char **command);
static void quit(VteTerminal *terminal, gint status, gpointer user_data);
int main(int argc, char *argv[]) {
  GtkWidget *window;
  char **command = NULL;
  const char *instance_name = NULL;
  const char *class_name = NULL;
  gtk_init(&argc, &argv);
  // Parse command line arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
      command = &argv[i + 1];
      break;  // Stop parsing after -e to allow multiple arguments for the command
    } else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
      instance_name = argv[i + 1];
      i++;
    } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
      class_name = argv[i + 1];
      i++;
    }else if (strcmp(argv[i], "-w") == 0 && i + 1 < argc) {
      width = argv[i + 1];
      i++;
    }else if (strcmp(argv[i], "-h") == 0 && i + 1 < argc) {
      height = argv[i + 1];
      i++;
    }
  }
  // Set WM_CLASS
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window),TITLE);
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
    if (class_name) {
      g_set_application_name(class_name);
    }
  }
  create_terminal(command);
  gtk_container_add(GTK_CONTAINER(window), term[0]);
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(window_key_event), NULL);
  gtk_widget_show_all(window);
  gtk_main();
  return 0;
}
static void create_terminal(char **command) {
  if (used >= MAX_TERM) return;
  GtkWidget *terminal = vte_terminal_new();
  PangoFontDescription *font_desc = pango_font_description_new();
  pango_font_description_set_family(font_desc, FONT);
  pango_font_description_set_size(font_desc, FONT_SIZE * PANGO_SCALE);
  vte_terminal_set_font(VTE_TERMINAL(terminal), font_desc);
  //Margin
  GtkCssProvider *provider = gtk_css_provider_new();
  gtk_css_provider_load_from_data(provider,
                                  "vte-terminal { padding: 10px; }",
                                  -1, NULL);
  gtk_style_context_add_provider(
    gtk_widget_get_style_context(GTK_WIDGET(terminal)),
    GTK_STYLE_PROVIDER(provider),
    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  pango_font_description_free(font_desc);
  vte_terminal_set_mouse_autohide(VTE_TERMINAL(terminal), CURSOR_AH);
  vte_terminal_set_cursor_blink_mode(VTE_TERMINAL(terminal), VTE_CURSOR_BLINK_OFF);
  vte_terminal_set_audible_bell(VTE_TERMINAL(terminal), BELL);
  vte_terminal_set_colors(VTE_TERMINAL(terminal), &CLR_GDK(CLR_7), NULL, PALETTE, PALETTE_SIZE);
  char **final_command;
  if (command == NULL) {
    const char *user_shell = g_getenv("SHELL");
    if (user_shell == NULL) user_shell = "/bin/sh";
    final_command = g_new(char *, 2);
    final_command[0] = g_strdup(user_shell);
    final_command[1] = NULL;
  } else {
    final_command = command;
  }
  vte_terminal_spawn_async(
    VTE_TERMINAL(terminal),
    VTE_PTY_DEFAULT,
    NULL,           // working directory
    final_command,  // command
    NULL,           // environment
    G_SPAWN_SEARCH_PATH,
    NULL, NULL,     // child setup
    NULL,           // child pid
    -1,             // timeout
    NULL,           // cancellable
    NULL, NULL      // callback
  );

  if (command == NULL) {
    g_strfreev(final_command);
  }
  g_signal_connect(terminal, "child-exited", G_CALLBACK(quit), NULL);
  g_signal_connect(terminal, "key-press-event", G_CALLBACK(term_key_event), NULL);
  term[used] = terminal;
  used++;
}
static void quit(VteTerminal *terminal, gint status, gpointer user_data) {
  (void)terminal;
  (void)status;
  (void)user_data;
  gtk_main_quit();
}
static void increase_font(VteTerminal *terminal) {
  vte_terminal_set_font_scale(VTE_TERMINAL(terminal), vte_terminal_get_font_scale(VTE_TERMINAL(terminal))+SCALE_FACTOR);
}
static void decrease_font(VteTerminal *terminal) {
  vte_terminal_set_font_scale(VTE_TERMINAL(terminal), vte_terminal_get_font_scale(VTE_TERMINAL(terminal))-SCALE_FACTOR);
}
//MYCOstom
static void scroll_up(VteTerminal *terminal,int i) {
  GtkAdjustment *adj = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(terminal));
  gdouble value = gtk_adjustment_get_value(adj);
  gdouble lower = gtk_adjustment_get_lower(adj);
  value = MAX(value - i, lower);
  gtk_adjustment_set_value(adj, value);
}
static void scroll_down(VteTerminal *terminal ,int i) {
  GtkAdjustment *adj = gtk_scrollable_get_vadjustment(GTK_SCROLLABLE(terminal));
  gdouble value = gtk_adjustment_get_value(adj);
  gdouble upper = gtk_adjustment_get_upper(adj);
  gdouble page_size = gtk_adjustment_get_page_size(adj);
  value = MIN(value + i, upper - page_size);
  gtk_adjustment_set_value(adj, value);
}
gboolean term_key_event(VteTerminal *terminal, GdkEventKey *event) {
  const guint modifiers = event->state & gtk_accelerator_get_default_mod_mask();
  if (modifiers == (MODIFIER)) {
    switch (gdk_keyval_to_lower(event->keyval)) {
      case COPY:               vte_terminal_copy_clipboard_format(terminal, VTE_FORMAT_TEXT); return TRUE;
      case PASTE:              vte_terminal_paste_clipboard(terminal); return TRUE;
      case INCREASE_FONT:      increase_font(terminal); return TRUE;
      case DECREASE_FONT:      decrease_font(terminal); return TRUE;
      case RESET_FONT:         vte_terminal_set_font_scale(terminal, FONT_SCALE); return TRUE;
      case SCROLL_UP:          scroll_up(terminal,1); return TRUE;
      case SCROLL_DOWN:        scroll_down(terminal,1); return TRUE;
      case SCROLL_PAGEUP:      scroll_up(terminal,33); return TRUE;
      case SCROLL_PAGEDOWN:    scroll_down(terminal,33); return TRUE;
      default:                return FALSE;
    } 
  }
  return FALSE;
}
static gboolean window_key_event(GtkWindow *window, GdkEventKey *event) {
  const guint modifiers = event->state & gtk_accelerator_get_default_mod_mask();
  if (modifiers == (MODIFIER)) {
    switch (gdk_keyval_to_lower(event->keyval)) {
      default: return FALSE;
    }
  }
  return FALSE;
}
