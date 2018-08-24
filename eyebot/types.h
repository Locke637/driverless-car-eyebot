/**
 * \file		types.h
 * \brief		Defines types used for the EyeBot 7
 * \author      Remi Keat & Marcus Pham
 */

#ifndef TYPES_H_
#define TYPES_H_

/**
 * \cond
 * Ignore this section in Doxygen
 */
#include <X11/Xlib.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>

#include <wiringPi.h>
#include <softPwm.h>

#include <pthread.h>
#include <semaphore.h>


#define NUMBER_TRY 10
#define HDT_MAX_NAMECHAR 80
#define LCD_MENU_STRLENGTH 32 /* for storage declaration */
#define LCD_LIST_STRLENGTH 64 /* for storage declaration */
#define MENU_HEIGHT 38
#define KEYTM_MAX_REGIONS 32
#define VERSION "1.0"
#define MACHINE_SPEED 700000000
#define MACHINE_TYPE VEHICLE
#define MACHINE_NAME "EyeBot"
#define LIBM6OS_VERSION "1.0"
#define MACHINE_ID 1

#define TIMEOUT 1000

#define HDT_MAX_PATHCHAR 256
#define HDT_MAX_FILECHAR 40
#define HDT_MAX_READBUFF 128

/* constants for text colorflags */
#define LCD_BGCOL_TRANSPARENT         0x01
#define LCD_BGCOL_NOTRANSPARENT       0x10

#define LCD_BGCOL_INVERSE             0x02
#define LCD_BGCOL_NOINVERSE           0x20

#define LCD_FGCOL_INVERSE             0x04
#define LCD_FGCOL_NOINVERSE           0x40

/* constants for lcd modes */
#define LCD_AUTOREFRESH               0x0001
#define LCD_NOAUTOREFRESH             0x0100

#define LCD_SCROLLING                 0x0002
#define LCD_NOSCROLLING               0x0200

#define LCD_LINEFEED                  0x0004
#define LCD_NOLINEFEED                0x0400

#define LCD_SHOWMENU                  0x0008
#define LCD_HIDEMENU                  0x0800

#define LCD_LISTMENU				  0x0010
#define LCD_CLASSICMENU				  0x1000

#define LCD_FB_ROTATE                 0x0080
#define LCD_FB_NOROTATION             0x8000

typedef char* DeviceSemantics;

typedef sem_t SEMA;

extern int IPInitialised;

extern int CAMSIZE;
extern int CAMPIXELS;
extern int CAMWIDTH;
extern int CAMHEIGHT;
extern int CAMMODE;
extern int CAM_INITIAL;

extern int IP_WIDTH, IP_HEIGHT, IP_PIXELS, IP_SIZE;

extern int LCD_INITIAL;
extern int IM_TYPE;
extern int IM_LEFT;
extern int IM_TOP;
extern int IM_WIDTH;
extern int IM_LENGTH;

/** Success code **/
#define CAM_SUCCESS 0
/** Failure code **/
#define CAM_FAILURE 1

//the font name length
#define FONTNAMELEN 200

// PSD position constants
#define PSD_LEFT  1
#define PSD_FRONT 2
#define PSD_RIGHT 3
#define PSD_BACK  4


/**
 * Stop ignoring
 * \endcond
 */

/**
 * \brief Structure defining a TASK item
 */
typedef struct
{
    pthread_t thread;
    int identifier;
} TASK;

#define IOBOARD 1

#define VEHICLE 1
#define PLATFORM 2
#define WALKER 3

#define HDT_FILE "/home/pi/eyebot/bin/hdt.txt"

typedef int COLOR;
typedef unsigned char BYTE;

typedef int TIMER;

/** \name Resolution
 * Available resolutions for CAM and IP functions
 */
///@{
#define QQVGA 0
#define QVGA  1
#define VGA   2
#define CAM1MP 3
#define CAMHD 4
#define CAM5MP 5
#define CUSTOM 10
///@}

/** \name QQVGA
 * Constants defined for QQVGA (160x120)
 */
///@{
#define QQVGA_X 160
#define QQVGA_Y 120
#define QQVGA_SIZE (QQVGA_X*QQVGA_Y*3)
#define QQVGA_PIXELS (QQVGA_X*QQVGA_Y)

typedef BYTE QQVGAcol[QQVGA_SIZE];
typedef BYTE QQVGAgray[QQVGA_PIXELS];
///@}

/** \name QVGA
 * Constants defined for QVGA (320x240)
 */
///@{
#define QVGA_X 320
#define QVGA_Y 240
#define QVGA_SIZE (QVGA_X*QVGA_Y*3)
#define QVGA_PIXELS (QVGA_X*QVGA_Y)

typedef BYTE QVGAcol[QVGA_SIZE];
typedef BYTE QVGAgray[QVGA_PIXELS];
///@}

/** \name VGA
 * Constants defined for VGA (640x480)
 */
///@{
#define VGA_X 640
#define VGA_Y 480
#define VGA_SIZE (VGA_X*VGA_Y*3)
#define VGA_PIXELS (VGA_X*VGA_Y)

typedef BYTE VGAcol[VGA_SIZE];
typedef BYTE VGAgray[VGA_PIXELS];
///@}

/** \name CAM1MP
 * Constants defined for CAM1MP (1296x730)
 */
///@{
#define CAM1MP_X 1296
#define CAM1MP_Y 730
#define CAM1MP_SIZE (CAM1MP_X*CAM1MP_Y*3)
#define CAM1MP_PIXELS (CAM1MP_X*CAM1MP_Y)

typedef BYTE CAM1MPcol[CAM1MP_SIZE];
typedef BYTE CAM1MPgray[CAM1MP_PIXELS];
///@}

/** \name CAMHD
 * Constants defined for CAMHD (1920x1080)
 */
///@{
#define CAMHD_X 1920
#define CAMHD_Y 1080
#define CAMHD_SIZE (CAMHD_X*CAMHD_Y*3)
#define CAMHD_PIXELS (CAMHD_X*CAMHD_Y)

typedef BYTE CAMHDcol[CAMHD_SIZE];
typedef BYTE CAMHDgray[CAMHD_PIXELS];
///@}

/** \name CAM5MP
 * Constants defined for CAM5MP (2592x1944)
 */
///@{
#define CAM5MP_X 2592
#define CAM5MP_Y 1944
#define CAM5MP_SIZE (CAM5MP_X*CAM5MP_Y*3)
#define CAM5MP_PIXELS (CAM5MP_X*CAM5MP_Y)

typedef BYTE CAM5MPcol[CAM5MP_SIZE];
typedef BYTE CAM5MPgray[CAM5MP_PIXELS];
///@}

/** \name Colours
 * The available predefined RGB colours
 */
///@{
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define WHITE 0xFFFFFF
#define GRAY 0x808080
#define BLACK 0x000000
#define SILVER 0xC0C0C0
#define LIGHTGRAY 0xD3D3D3
#define DARKGRAY 0xA9A9A9
#define NAVY 0x000080
#define CYAN 0x00EEEE
#define TEAL 0x008080
#define MAGENTA 0xFF00FF
#define PURPLE 0x800080
#define MAROON 0x800000
#define YELLOW 0xFFFF00
#define OLIVE 0x9ACD32
#define ORANGE 0xFFA500
///@}

/** \name Keys
 * Pre-defined key constants - usable for standard & region map
 */
///@{
#define ANYKEY 0xFFFFFFFF
#define NOKEY 0x00000000
#define KEY1 0x00000001
#define KEY2 0x00000002
#define KEY3 0x00000004
#define KEY4 0x00000008

#define KEY_ESCAPE 0x80000000
#define KEY_LISTTL 0x40000000
#define KEY_LISTUP 0x20000000
#define KEY_LISTDN 0x10000000

#define KEY_LIST1 0x00000010
#define KEY_LIST2 0x00000020
#define KEY_LIST3 0x00000040
#define KEY_LIST4 0x00000080
#define KEY_LIST5 0x00000100
#define KEY_LIST6 0x00000200
#define KEY_LIST7 0x00000400
#define KEY_LIST8 0x00000800

#define KEY_INVALID 0xFFFFFFFF

#define KEY_GOIDLE 1
#define KEY_NOIDLE 0
#define KEY_STATE -1

#define KEY_GOIDLE 1
#define KEY_NOIDLE 0
#define KEY_STATE -1

//Keymodes
#define KEYTM_UNKNOWN  0x00
#define KEYTM_CLASSIC  0x01
#define KEYTM_STANDARD 0x02
#define KEYTM_REGIONS  0x03
#define KEYTM_LISTMENU 0x04


///@}

/** \name IR_Keys
 * Pre-defined IR key constants
 */
///@{
//			Define name			char			config file name
#define		IRTV_1			'1'				//KEY_1
#define		IRTV_2			'2'				//KEY_2
#define		IRTV_3			'3'				//KEY_3
#define		IRTV_4			'4'				//KEY_4
#define		IRTV_5			'5'				//KEY_5
#define		IRTV_6			'6'				//KEY_6
#define		IRTV_7			'7'				//KEY_7
#define		IRTV_8			'8'				//KEY_8
#define		IRTV_9			'9'				//KEY_9
#define		IRTV_0			'0'				//KEY_0
#define		IRTV_RED		'R'				//KEY_RED
#define		IRTV_GREEN		'G'				//KEY_GREEN
#define		IRTV_YELLOW		'Y'				//KEY_YELLOW
#define		IRTV_BLUE		'B'				//KEY_BLUE
#define		IRTV_LEFT		'L'				//KEY_LEFT
#define		IRTV_RIGHT		'>'				//KEY_RIGHT
#define		IRTV_UP			'U'				//KEY_UP
#define		IRTV_DOWN		'D'				//KEY_DOWN
#define		IRTV_OK			'O'				//KEY_OK
#define		IRTV_POWER		'P'				//KEY_POWER
///@}

/** \name Fonts
 * Pre-defined Fonts available
 */
///@{
#define HELVETICA 0
#define TIMES 1
#define COURIER 2
///@}

/** \name Styles
 * Pre-defined Font styles available
 */
///@{
#define NORMAL 0
#define BOLD 1
#define ITALICS 2
///@}

/**
 * \cond
 * Ignore the Structs
 */

/**
 * \brief Structure defining a Hints for the LCD
 */
typedef struct
{
	unsigned long flags;
	unsigned long functions;
	unsigned long decorations;
	long inputMode;
	unsigned long status;
} HINTS;

/**
 * \brief Structure defining a Screen for the LCD
 */
typedef struct
{
	int xres, yres;
	int bpp;
} SCREEN;

/**
 * \brief Structure defining the cursor for the LCD
 */
typedef struct
{
	int x, y;
	int xmax, ymax;
} CURSOR;

/**
 * \brief Structure defining the Framebuffer info for the LCD
 */
typedef struct
{
	SCREEN screen;
	CURSOR cursor; /* x & y holds font width & height! */
} FBINFO;

/**
 * \brief Structure defining the CPU info
 */
typedef struct
{
	char name[50];
	char mhz[50];
	char arch[50];
	char bogomips[50];
} INFO_CPU;

/**
 * \brief Structure defining the memory info
 */
typedef struct
{
	char procnum[20];
	char total[40];
	char free[40];
} INFO_MEM;

/**
 * \brief Structure defining the processor info
 */
typedef struct
{
	char num[20];
} INFO_PROC;

/**
 * \brief Structure defining battery info
 */
typedef struct
{
	char uptime[20];
	char vbatt[20];
	int vbatt_8;
} INFO_MISC;

/**
 * \brief Structure representing the coordinates of a point
 */
typedef struct
{
	int x, y;
} COORD_PAIR;

/**
 * \brief Structure defining a rectangular box on the LCD
 */
typedef struct
{
	int active;
	COORD_PAIR tl;       //top left
	COORD_PAIR br;       //bottom right
} BOX;

/**
 * \brief Structure defining boxes for touchscreen use
 */
typedef struct
{
	int mode;
	BOX rect[KEYTM_MAX_REGIONS];
} TOUCH_MAP;

/**
 * \brief Structure defining touchscreen presses
 */
typedef struct
{
	COORD_PAIR point, value;
	int sync, status;
} TOUCH_EVENT;

/**
 * \brief Structure defining a menu item
 */
typedef struct
{
	char label[LCD_MENU_STRLENGTH];
	XColor fgcol, bgcol;
	void *plink; /* link to user data/function! */
} MENU_ITEM;

/**
 * \brief Structure defining a menu
 */
typedef struct
{
	char title[LCD_LIST_STRLENGTH];
	XColor fgcol, bgcol;
	int size, start, width, left, scroll; /* configure these! */
	int index/*, count*/; /* the library will set & manage these! */
	MENU_ITEM *pitems; /* pointer to array of 'size' menuitems */
	int no_empty;
} LIST_MENU;

/**
 * \brief Structure defining an LCD
 */
typedef struct
{
	int lcdNum;
	Display* d;
	int s;
	Window w;
	Colormap colormap;
	GC gc;
	XFontStruct* fontstruct;
    int fontType;
    int fontVariation;
    int fontSize;
	int fontHeight;
	int fontWidth;
	int height;
	int width;
	int startCurPosX;
	int startCurPosY;
	XColor fgTextColor;
	XColor bgTextColor;
	char colorflag;
	int mode;
	MENU_ITEM menuItems[4];
	LIST_MENU* listMenu;
	int fd;
	bool X11Error;
	Screen *scr;
} LCD_HANDLE;

/**
 * \brief Structure representing a rectangle
 */
typedef struct
{
	int x, y;
	int height, width;
} RECT;

/**
 * \brief Structure defining a HDT entry
 */
typedef struct HDT_ENTRY
{
	int length;
	char *buffer;
}
HDT_ENTRY;

/**
 * \brief Structure defining a HDT Table
 */
typedef struct HDT_TABLE
{
	struct HDT_TABLE *pnext;
	char name[HDT_MAX_NAMECHAR];
	int size;
	int *data;
}
HDT_TABLE;

/**
 * \brief Structure defining a HDT Device
 */
typedef struct HDT_DEVICE
{
	struct HDT_DEVICE *pnext;
	char name[HDT_MAX_NAMECHAR];
	char tabname[HDT_MAX_NAMECHAR];
	HDT_TABLE *ptable; /* pointer to a table */
}
HDT_DEVICE;

/**
 * \brief Structure defining a HDT Camera
 */
typedef struct HDT_CAM
{
	struct HDT_CAM *pnext;
	char name[HDT_MAX_NAMECHAR];
	int regaddr, ucb1400io, width, height;
}
HDT_CAM;

/**
 * \brief Structure defining a HDT Motor
 */
typedef struct HDT_MOTOR
{
	struct HDT_MOTOR *pnext;
	char name[HDT_MAX_NAMECHAR];
	char tabname[HDT_MAX_NAMECHAR];
	HDT_TABLE *ptable; /* pointer to a table */
    int regaddr;
}
HDT_MOTOR;

/**
 * \brief Structure defining a HDT encoder
 */
typedef struct HDT_ENCODER
{
	struct HDT_ENCODER *pnext;
	char name[HDT_MAX_NAMECHAR];
	char motorname[HDT_MAX_NAMECHAR];
	HDT_MOTOR *pmotor;
	int regaddr, clickspm;
	int maxspeed;
}
HDT_ENCODER;

/**
 * \brief Structure defining a HDT servo
 */
typedef struct HDT_SERVO
{
	struct HDT_SERVO *pnext;
	char name[HDT_MAX_NAMECHAR];
    char tabname[HDT_MAX_NAMECHAR];
    HDT_TABLE *ptable; /* pointer to a table */
	int regaddr, freq;
	int mintime, maxtime;
    int low, high;
}
HDT_SERVO;

/**
 * \brief Structure defining a HDT psd
 */
typedef struct HDT_PSD
{
	struct HDT_PSD *pnext;
	char name[HDT_MAX_NAMECHAR];
	char tabname[HDT_MAX_NAMECHAR];
	HDT_TABLE *ptable; /* pointer to a table */
	int regaddr;
}
HDT_PSD;

/**
 * \brief Structure defining a HDT drive
 */
typedef struct HDT_DRIVE
{
    struct HDT_DRIVE *pnext;
    char name[HDT_MAX_NAMECHAR];
    char encname1[HDT_MAX_NAMECHAR],encname2[HDT_MAX_NAMECHAR];
    char encname3[HDT_MAX_NAMECHAR],encname4[HDT_MAX_NAMECHAR];
    HDT_ENCODER *penc1, *penc2, *penc3, *penc4;
    int drivetype;
    int wheeldist1, axesdist, wheeldist2;
}
HDT_DRIVE;

/**
 * \brief Structure defining a HDT IRTV
 */
typedef struct HDT_IRTV
{
	struct HDT_IRTV *pnext;
	char name[HDT_MAX_NAMECHAR];
	int type, length, togmask, invmask, mode, buffsize, delay;
}
HDT_IRTV;

/**
 * \brief Structure defining a HDT ADC
 */
typedef struct HDT_ADC
{
	struct HDT_ADC *pnext;
	char name[HDT_MAX_NAMECHAR];
	char procname[HDT_MAX_NAMECHAR];
	int denom;
}
HDT_ADC;

/**
 * \brief Structure defining a HDT COM
 */
typedef struct HDT_COM
{
	struct HDT_COM *pnext;
	char name[HDT_MAX_NAMECHAR];
	char devname[HDT_MAX_NAMECHAR];
}
HDT_COM;

/**
 * \brief Structure defining a LCD fontcolour/name
 */
typedef struct 
{
	XColor fontColour;
	char fontName[FONTNAMELEN];
} EYEFONT;

/**
 * End Ignore
 * \endcond
 */
#endif /* TYPES_H_ */
