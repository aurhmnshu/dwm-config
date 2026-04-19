/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>
/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int gappx     = 4;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "JetBrainsMono Nerd Font:size=12","Apple Color Emoji:size=10" };
static const char col_bg[] = "#1a1612"; /* background */
static const char col_border[] = "#2e2519"; /* inactive border */
static const char col_fg[] = "#7a6040"; /* normal foreground */
static const char col_fgsel[] = "#e8d5a0"; /* selected foreground */
static const char col_accent[] = "#b89a6a"; /* accent (brass gold) */
static const char *colors[][3] = {
  /*             fg         bg         border   */
  [SchemeNorm] = { col_fg, col_bg, col_border },
  [SchemeSel]  = { col_fgsel, col_accent, col_accent },
};
/* tagging */
static const char *tags[] = { "1", "2", "3", "4" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */
static const int refreshrate = 60;  /* refresh rate (per second) for client move/resize */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "⌞⌝",      tile },    /* first entry is default */
	{ "{}",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "rofi", "-show", "drun", NULL };
static const char *termcmd[]  = { "st", NULL };

static const char *redshift_on[] = { "redshift", "-O", "4000", NULL };
static const char *redshift_off[] = { "redshift", "-x", NULL };

static const char *brightup[] = { "sh", "-c", "brightnessctl s +10%; pct=$(brightnessctl -m | cut -d, -f4 | tr -d '%'); killall herbe 2>/dev/null; herbe \"Brightness: $pct%\"", NULL };
static const char *brightdown[] = { "sh", "-c", "brightnessctl s 10%-; pct=$(brightnessctl -m | cut -d, -f4 | tr -d '%'); killall herbe 2>/dev/null; herbe \"Brightness: $pct%\"", NULL };

static const char *volup[] = { "sh", "-c", "pactl set-sink-volume @DEFAULT_SINK@ +5%; volume=$(pactl get-sink-volume @DEFAULT_SINK@ | awk '/Volume:/ {print $5}' | tr -d '%'); killall herbe 2>/dev/null; herbe \"Volume: $volume%\"", NULL };
static const char *voldown[] = { "sh", "-c", "pactl set-sink-volume @DEFAULT_SINK@ -5%; volume=$(pactl get-sink-volume @DEFAULT_SINK@ | awk '/Volume:/ {print $5}' | tr -d '%'); killall herbe 2>/dev/null; herbe \"Volume: $volume%\"", NULL };
static const char *volmute[] = { "sh", "-c", "pactl set-sink-mute @DEFAULT_SINK@ toggle; if pactl get-sink-mute @DEFAULT_SINK@ | grep -q 'yes'; then killall herbe 2>/dev/null; herbe 'Muted'; else volume=$(pactl get-sink-volume @DEFAULT_SINK@ | awk '/Volume:/ {print $5}' | tr -d '%'); killall herbe 2>/dev/null; herbe \"Volume: $volume%\"; fi", NULL };

/* Media keys with herbe notifications */
static const char *play_pause[] = { "sh", "-c", "playerctl play-pause && sleep 0.1 && herbe \"$(playerctl status)\"", NULL };
static const char *next_track[] = { "sh", "-c", "playerctl next && sleep 0.2 && herbe \"$(playerctl metadata --format 'Now: {{title}} - {{artist}}')\"", NULL };
static const char *prev_track[] = { "sh", "-c", "playerctl previous && sleep 0.2 && herbe \"$(playerctl metadata --format 'Now: {{title}} - {{artist}}')\"", NULL };

static const char *dolphin[] = { "dolphin", NULL };
static const char *flameshot_gui[] = { "flameshot", "gui", NULL };
static const char *firefox[] = { "firefox", NULL };
static const char *wificmd[] = { "iwmenu", "-l", "rofi", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_space,  spawn,          {.v = dmenucmd } },
 	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
 	{ 0,                            XK_Print,  spawn,          {.v = flameshot_gui } },
 	{ MODKEY,                       XK_e,      spawn,          {.v = dolphin } },
 	{ MODKEY,                       XK_f,      spawn,          {.v = firefox } },
 	{ MODKEY,                       XK_n,      spawn,          {.v = redshift_on } },
 	{ MODKEY|ShiftMask,             XK_n,      spawn,          {.v = redshift_off } },
 	{ MODKEY|ShiftMask,             XK_space,  spawn,          {.v = wificmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_w,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_v,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_s,      setlayout,          {0} },
	{ MODKEY|ShiftMask,             XK_f,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ 0, XF86XK_MonBrightnessUp,   spawn, {.v = brightup } },
	{ 0, XF86XK_MonBrightnessDown, spawn, {.v = brightdown } },
	{ 0, XF86XK_AudioRaiseVolume, spawn, {.v = volup } },
	{ 0, XF86XK_AudioLowerVolume, spawn, {.v = voldown } },
	{ 0, XF86XK_AudioMute,        spawn, {.v = volmute } },
	{ 0, XF86XK_AudioPlay,  spawn, {.v = play_pause } },
	{ 0, XF86XK_AudioNext,  spawn, {.v = next_track } },
	{ 0, XF86XK_AudioPrev,  spawn, {.v = prev_track } },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
    
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

