#include "toolkeys.h"

/*
keycode SDLK_FIRST = 0
keycode SDLK_BACKSPACE = 8
keycode SDLK_TAB = 9
keycode SDLK_CLEAR = 12
keycode SDLK_RETURN = 13
keycode SDLK_PAUSE = 19
keycode SDLK_ESCAPE = 27
keycode SDLK_SPACE = 32
keycode SDLK_EXCLAIM = 33
keycode SDLK_QUOTEDBL = 34
keycode SDLK_HASH = 35
keycode SDLK_DOLLAR = 36
keycode SDLK_AMPERSAND = 38
keycode SDLK_QUOTE = 39
keycode SDLK_LEFTPAREN = 40
keycode SDLK_RIGHTPAREN = 41
keycode SDLK_ASTERISK = 42
keycode SDLK_PLUS = 43
keycode SDLK_COMMA = 44
keycode SDLK_MINUS = 45
keycode SDLK_PERIOD = 46
keycode SDLK_SLASH = 47

keycode SDLK_0 = 48
keycode SDLK_1 = 49
keycode SDLK_2 = 50
keycode SDLK_3 = 51
keycode SDLK_4 = 52
keycode SDLK_5 = 53
keycode SDLK_6 = 54
keycode SDLK_7 = 55
keycode SDLK_8 = 56
keycode SDLK_9 = 57


keycode SDLK_COLON = 58
keycode SDLK_SEMICOLON = 59
keycode SDLK_LESS = 60
keycode SDLK_EQUALS = 61
keycode SDLK_GREATER = 62
keycode SDLK_QUESTION = 63
keycode SDLK_AT = 64


keycode SDLK_LEFTBRACKET = 91
keycode SDLK_BACKSLASH = 92
keycode SDLK_RIGHTBRACKET = 93
keycode SDLK_CARET = 94
keycode SDLK_UNDERSCORE = 95
keycode SDLK_BACKQUOTE = 96


keycode SDLK_a = 97
keycode SDLK_b = 98
keycode SDLK_c = 99
keycode SDLK_d = 100
keycode SDLK_e = 101
keycode SDLK_f = 102
keycode SDLK_g = 103
keycode SDLK_h = 104
keycode SDLK_i = 105
keycode SDLK_j = 106
keycode SDLK_k = 107
keycode SDLK_l = 108
keycode SDLK_m = 1
keycode SDLK_n = 110
keycode SDLK_o = 111
keycode SDLK_p = 112
keycode SDLK_q = 113
keycode SDLK_r = 114
keycode SDLK_s = 115
keycode SDLK_t = 116
keycode SDLK_u = 117
keycode SDLK_v = 118
keycode SDLK_w = 119
keycode SDLK_x = 120
keycode SDLK_y = 121
keycode SDLK_z = 122
keycode SDLK_DELETE = 127


keycode SDLK_WORLD_0 = 160
keycode SDLK_WORLD_1 = 161
keycode SDLK_WORLD_2 = 162
keycode SDLK_WORLD_3 = 163
keycode SDLK_WORLD_4 = 164
keycode SDLK_WORLD_5 = 165
keycode SDLK_WORLD_6 = 166
keycode SDLK_WORLD_7 = 167
keycode SDLK_WORLD_8 = 168
keycode SDLK_WORLD_9 = 169
keycode SDLK_WORLD_10 = 170
keycode SDLK_WORLD_11 = 171
keycode SDLK_WORLD_12 = 172
keycode SDLK_WORLD_13 = 173
keycode SDLK_WORLD_14 = 174
keycode SDLK_WORLD_15 = 175
keycode SDLK_WORLD_16 = 176
keycode SDLK_WORLD_17 = 177
keycode SDLK_WORLD_18 = 178
keycode SDLK_WORLD_19 = 179
keycode SDLK_WORLD_20 = 180
keycode SDLK_WORLD_21 = 181
keycode SDLK_WORLD_22 = 182
keycode SDLK_WORLD_23 = 183
keycode SDLK_WORLD_24 = 184
keycode SDLK_WORLD_25 = 185
keycode SDLK_WORLD_26 = 186
keycode SDLK_WORLD_27 = 187
keycode SDLK_WORLD_28 = 188
keycode SDLK_WORLD_29 = 189
keycode SDLK_WORLD_30 = 190
keycode SDLK_WORLD_31 = 191
keycode SDLK_WORLD_32 = 192
keycode SDLK_WORLD_33 = 193
keycode SDLK_WORLD_34 = 194
keycode SDLK_WORLD_35 = 195
keycode SDLK_WORLD_36 = 196
keycode SDLK_WORLD_37 = 197
keycode SDLK_WORLD_38 = 198
keycode SDLK_WORLD_39 = 199
keycode SDLK_WORLD_40 = 200
keycode SDLK_WORLD_41 = 201
keycode SDLK_WORLD_42 = 202
keycode SDLK_WORLD_43 = 203
keycode SDLK_WORLD_44 = 204
keycode SDLK_WORLD_45 = 205
keycode SDLK_WORLD_46 = 206
keycode SDLK_WORLD_47 = 207
keycode SDLK_WORLD_48 = 208
keycode SDLK_WORLD_49 = 2
keycode SDLK_WORLD_50 = 210
keycode SDLK_WORLD_51 = 211
keycode SDLK_WORLD_52 = 212
keycode SDLK_WORLD_53 = 213
keycode SDLK_WORLD_54 = 214
keycode SDLK_WORLD_55 = 215
keycode SDLK_WORLD_56 = 216
keycode SDLK_WORLD_57 = 217
keycode SDLK_WORLD_58 = 218
keycode SDLK_WORLD_59 = 219
keycode SDLK_WORLD_60 = 220
keycode SDLK_WORLD_61 = 221
keycode SDLK_WORLD_62 = 222
keycode SDLK_WORLD_63 = 223
keycode SDLK_WORLD_64 = 224
keycode SDLK_WORLD_65 = 225
keycode SDLK_WORLD_66 = 226
keycode SDLK_WORLD_67 = 227
keycode SDLK_WORLD_68 = 228
keycode SDLK_WORLD_69 = 229
keycode SDLK_WORLD_70 = 230
keycode SDLK_WORLD_71 = 231
keycode SDLK_WORLD_72 = 232
keycode SDLK_WORLD_73 = 233
keycode SDLK_WORLD_74 = 234
keycode SDLK_WORLD_75 = 235
keycode SDLK_WORLD_76 = 236
keycode SDLK_WORLD_77 = 237
keycode SDLK_WORLD_78 = 238
keycode SDLK_WORLD_79 = 239
keycode SDLK_WORLD_80 = 240
keycode SDLK_WORLD_81 = 241
keycode SDLK_WORLD_82 = 242
keycode SDLK_WORLD_83 = 243
keycode SDLK_WORLD_84 = 244
keycode SDLK_WORLD_85 = 245
keycode SDLK_WORLD_86 = 246
keycode SDLK_WORLD_87 = 247
keycode SDLK_WORLD_88 = 248
keycode SDLK_WORLD_89 = 249
keycode SDLK_WORLD_90 = 250
keycode SDLK_WORLD_91 = 251
keycode SDLK_WORLD_92 = 252
keycode SDLK_WORLD_93 = 253
keycode SDLK_WORLD_94 = 254
keycode SDLK_WORLD_95 = 255


keycode SDLK_KP0 = 256
keycode SDLK_KP1 = 257
keycode SDLK_KP2 = 258
keycode SDLK_KP3 = 259
keycode SDLK_KP4 = 260
keycode SDLK_KP5 = 261
keycode SDLK_KP6 = 262
keycode SDLK_KP7 = 263
keycode SDLK_KP8 = 264
keycode SDLK_KP9 = 265
keycode SDLK_KP_PERIOD = 266
keycode SDLK_KP_DIVIDE = 267
keycode SDLK_KP_MULTIPLY = 268
keycode SDLK_KP_MINUS = 269
keycode SDLK_KP_PLUS = 270
keycode SDLK_KP_ENTER = 271
keycode SDLK_KP_EQUALS = 272


keycode SDLK_UP = 273
keycode SDLK_DOWN = 274
keycode SDLK_RIGHT = 275
keycode SDLK_LEFT = 276
keycode SDLK_INSERT = 277
keycode SDLK_HOME = 278
keycode SDLK_END = 279
keycode SDLK_PAGEUP = 280
keycode SDLK_PAGEDOWN = 281


keycode SDLK_F1 = 282
keycode SDLK_F2 = 283
keycode SDLK_F3 = 284
keycode SDLK_F4 = 285
keycode SDLK_F5 = 286
keycode SDLK_F6 = 287
keycode SDLK_F7 = 288
keycode SDLK_F8 = 289
keycode SDLK_F9 = 290
keycode SDLK_F10 = 291
keycode SDLK_F11 = 292
keycode SDLK_F12 = 293
keycode SDLK_F13 = 294
keycode SDLK_F14 = 295
keycode SDLK_F15 = 296


keycode SDLK_NUMLOCK = 300
keycode SDLK_CAPSLOCK = 301
keycode SDLK_SCROLLOCK = 302
keycode SDLK_RSHIFT = 303
keycode SDLK_LSHIFT = 304
keycode SDLK_RCTRL = 305
keycode SDLK_LCTRL = 306
keycode SDLK_RALT = 307
keycode SDLK_LALT = 308
keycode SDLK_RMETA = 3
keycode SDLK_LMETA = 310
keycode SDLK_LSUPER = 311
keycode SDLK_RSUPER = 312
keycode SDLK_MODE = 313
keycode SDLK_COMPOSE = 314


keycode SDLK_HELP = 315
keycode SDLK_PRINT = 316
keycode SDLK_SYSREQ = 317
keycode SDLK_BREAK = 318
keycode SDLK_MENU = 319
keycode SDLK_POWER = 320
keycode SDLK_EURO = 321
keycode SDLK_UNDO = 322
*/













