#define WIDTH        "800"
#define HEIGHT        "400"
#define R_FACTOR        2                 // window resize factor
#define TITLE         "Simple New Terminal"
#define FONT          "JetBrainsMono NFM Medium ,DejaVuSansM Nerd Font Propo"
#define FONT_SIZE     11
#define FONT_SCALE    1                  // Leave this like this
#define SCALE_FACTOR  0.1                // Leave this like this
#define CURSOR_AH     FALSE              // auto hide the cursor while typing
#define REWRAP        FALSE              // rewrap terminal based on content size
#define BELL          TRUE              // no bell sound
#define MAX_TERM      10
// colors
#define CLR_0  0x0c0c0c                  //black 
#define CLR_1  0x80202D                  //red
#define CLR_2  0x6aa847                  //grean
#define CLR_3  0x847e2b                  //yellow
#define CLR_4  0x5f5788                  //blue 
#define CLR_5  0x793d75                  //pink
#define CLR_6  0x5d9e79                  //cyin  
#define CLR_7  0xcccccc                  //white
#define CLR_8  0x313131                  //black 
#define CLR_9  0x80202D                  //red
#define CLR_10 0x6aa847                  //grean
#define CLR_11 0x847e2b                  //yello
#define CLR_12 0x5f5788                  //blue 
#define CLR_13 0x793d75                  //pink
#define CLR_14 0x5d9e79                  //cyin 
#define CLR_15 0xcccccc                  //white
#define PALETTE_SIZE  16
// Modifier
#define MODIFIER GDK_CONTROL_MASK|GDK_SHIFT_MASK 
#define COPY                GDK_KEY_y
#define PASTE               GDK_KEY_p
#define INCREASE_FONT       GDK_KEY_Page_Up
#define DECREASE_FONT       GDK_KEY_Page_Down
#define RESET_FONT          GDK_KEY_Home
#define SCROLL_UP           GDK_KEY_k
#define SCROLL_DOWN         GDK_KEY_j
#define SCROLL_PAGEUP       GDK_KEY_u
#define SCROLL_PAGEDOWN     GDK_KEY_d
