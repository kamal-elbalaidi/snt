// Used to parse colors
#define TITLE       "Simple new terminal"
#define CLR_R(x)    (((x) & 0xff0000) >> 16)     
#define CLR_G(x)    (((x) & 0x00ff00) >>  8)
#define CLR_B(x)    (((x) & 0x0000ff) >>  0)
#define CLR_16(x)   ((double)(x) / 0xff)
#define CLR_GDK(x)  (const GdkRGBA){ .red   = CLR_16(CLR_R(x)), \
                                     .green = CLR_16(CLR_G(x)), \
                                     .blue  = CLR_16(CLR_B(x)) }

// Palette color converted from config file
static const GdkRGBA PALETTE[] = {                        
    CLR_GDK(CLR_0),  CLR_GDK(CLR_1),  CLR_GDK(CLR_2),
    CLR_GDK(CLR_3),  CLR_GDK(CLR_4),  CLR_GDK(CLR_5),
    CLR_GDK(CLR_6),  CLR_GDK(CLR_7),  CLR_GDK(CLR_8),
    CLR_GDK(CLR_9),  CLR_GDK(CLR_10), CLR_GDK(CLR_11),
    CLR_GDK(CLR_12), CLR_GDK(CLR_13), CLR_GDK(CLR_14),
    CLR_GDK(CLR_15)  };

GtkWidget * term[1];

// Functions
static const char cursor_blink(){
  if(!CURSOR_BLINK){
    return VTE_CURSOR_BLINK_OFF;
  }
  return VTE_CURSOR_BLINK_ON;
}
static const char cursorshape(){
  switch (CURSORSHAPE) {
    case 2 :    return VTE_CURSOR_SHAPE_BLOCK;         return TRUE;
    case 4 :    return VTE_CURSOR_SHAPE_UNDERLINE;     return TRUE;
    case 6 :    return VTE_CURSOR_SHAPE_IBEAM;         return TRUE;
    case 7 :    return VTE_CURSOR_SHAPE_IBEAM;         return TRUE;
    default:    return FALSE;  
  }
}

// Variables
static const char *termname           = "snt";
static const char *shell              = "/bin/sh";             // shell environment variable
static const char *height             = HEIGHT;                // height size  window
static const char *width              = WIDTH;                 // width size  window
