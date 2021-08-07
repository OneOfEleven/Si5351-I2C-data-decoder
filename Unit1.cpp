
// Si5351 I2C data decoder
//
// This was written and compiled using the old Borland Builder C++ v6 dev system (because its easy and quick to use)
// 
// Written by Cathy G6AMU
// August 2021

#include <vcl.h>
#include <inifiles.hpp>

#include <stdio.h>
#include <math.h>

#pragma hdrstop

#include "Unit1.h"

#pragma package(smart_init)
#pragma resource "*.dfm"

#define ARRAY_SIZE(array)       (sizeof(array) / sizeof(array[0]))
#define SQR(x)                  ((x) * (x))
#define IROUND(x)               ((int)floor((x) + 0.5))
#define I64ROUND(x)             ((int64_t)floor((x) + 0.5))
#define MIN(a, b)               (((a) < (b)) ? (a) : (b))
#define MAX(a, b)               (((a) > (b)) ? (a) : (b))
#define ABS(x)                  (((x) >= 0) ? (x) : -(x))

// ****************************************************************

typedef struct
{
	int     addr;
	uint8_t reset_value;
	char    *name;
} t_si5351_reg_list;

typedef enum
{
	SI5351_REG_DEVICE_STATUS                     =   0,
	SI5351_REG_INTERRUPT_STATUS_STICKY           =   1,
	SI5351_REG_INTERRUPT_STATUS_MASK             =   2,

	SI5351_REG_OUTPUT_ENABLE_CONTROL             =   3,
	SI5351_REG_OEB_PIN_ENABLE_CONTROL            =   9,

	SI5351_REG_PLL_INPUT_SOURCE                  =  15,

	SI5351_REG_CLK0_CONTROL                      =  16,
	SI5351_REG_CLK1_CONTROL                      =  17,
	SI5351_REG_CLK2_CONTROL                      =  18,
	SI5351_REG_CLK3_CONTROL                      =  19,
	SI5351_REG_CLK4_CONTROL                      =  20,
	SI5351_REG_CLK5_CONTROL                      =  21,
	SI5351_REG_CLK6_CONTROL                      =  22,
	SI5351_REG_CLK7_CONTROL                      =  23,

	SI5351_REG_CLK3_0_DISABLE_STATE              =  24,
	SI5351_REG_CLK7_4_DISABLE_STATE              =  25,

	SI5351_REG_PLLA_PARAMETERS_0                 =  26,
	SI5351_REG_PLLA_PARAMETERS_1                 =  27,
	SI5351_REG_PLLA_PARAMETERS_2                 =  28,
	SI5351_REG_PLLA_PARAMETERS_3                 =  29,
	SI5351_REG_PLLA_PARAMETERS_4                 =  30,
	SI5351_REG_PLLA_PARAMETERS_5                 =  31,
	SI5351_REG_PLLA_PARAMETERS_6                 =  32,
	SI5351_REG_PLLA_PARAMETERS_7                 =  33,

	SI5351_REG_PLLB_PARAMETERS_0                 =  34,
	SI5351_REG_PLLB_PARAMETERS_1                 =  35,
	SI5351_REG_PLLB_PARAMETERS_2                 =  36,
	SI5351_REG_PLLB_PARAMETERS_3                 =  37,
	SI5351_REG_PLLB_PARAMETERS_4                 =  38,
	SI5351_REG_PLLB_PARAMETERS_5                 =  39,
	SI5351_REG_PLLB_PARAMETERS_6                 =  40,
	SI5351_REG_PLLB_PARAMETERS_7                 =  41,

	SI5351_REG_MULTISYNTH0_PARAMETERS_1          =  42,
	SI5351_REG_MULTISYNTH0_PARAMETERS_2          =  43,
	SI5351_REG_MULTISYNTH0_PARAMETERS_3          =  44,
	SI5351_REG_MULTISYNTH0_PARAMETERS_4          =  45,
	SI5351_REG_MULTISYNTH0_PARAMETERS_5          =  46,
	SI5351_REG_MULTISYNTH0_PARAMETERS_6          =  47,
	SI5351_REG_MULTISYNTH0_PARAMETERS_7          =  48,
	SI5351_REG_MULTISYNTH0_PARAMETERS_8          =  49,

	SI5351_REG_MULTISYNTH1_PARAMETERS_1          =  50,
	SI5351_REG_MULTISYNTH1_PARAMETERS_2          =  51,
	SI5351_REG_MULTISYNTH1_PARAMETERS_3          =  52,
	SI5351_REG_MULTISYNTH1_PARAMETERS_4          =  53,
	SI5351_REG_MULTISYNTH1_PARAMETERS_5          =  54,
	SI5351_REG_MULTISYNTH1_PARAMETERS_6          =  55,
	SI5351_REG_MULTISYNTH1_PARAMETERS_7          =  56,
	SI5351_REG_MULTISYNTH1_PARAMETERS_8          =  57,

	SI5351_REG_MULTISYNTH2_PARAMETERS_1          =  58,
	SI5351_REG_MULTISYNTH2_PARAMETERS_2          =  59,
	SI5351_REG_MULTISYNTH2_PARAMETERS_3          =  60,
	SI5351_REG_MULTISYNTH2_PARAMETERS_4          =  61,
	SI5351_REG_MULTISYNTH2_PARAMETERS_5          =  62,
	SI5351_REG_MULTISYNTH2_PARAMETERS_6          =  63,
	SI5351_REG_MULTISYNTH2_PARAMETERS_7          =  64,
	SI5351_REG_MULTISYNTH2_PARAMETERS_8          =  65,

	SI5351_REG_MULTISYNTH3_PARAMETERS_1          =  66,
	SI5351_REG_MULTISYNTH3_PARAMETERS_2          =  67,
	SI5351_REG_MULTISYNTH3_PARAMETERS_3          =  68,
	SI5351_REG_MULTISYNTH3_PARAMETERS_4          =  69,
	SI5351_REG_MULTISYNTH3_PARAMETERS_5          =  70,
	SI5351_REG_MULTISYNTH3_PARAMETERS_6          =  71,
	SI5351_REG_MULTISYNTH3_PARAMETERS_7          =  72,
	SI5351_REG_MULTISYNTH3_PARAMETERS_8          =  73,

	SI5351_REG_MULTISYNTH4_PARAMETERS_1          =  74,
	SI5351_REG_MULTISYNTH4_PARAMETERS_2          =  75,
	SI5351_REG_MULTISYNTH4_PARAMETERS_3          =  76,
	SI5351_REG_MULTISYNTH4_PARAMETERS_4          =  77,
	SI5351_REG_MULTISYNTH4_PARAMETERS_5          =  78,
	SI5351_REG_MULTISYNTH4_PARAMETERS_6          =  79,
	SI5351_REG_MULTISYNTH4_PARAMETERS_7          =  80,
	SI5351_REG_MULTISYNTH4_PARAMETERS_8          =  81,

	SI5351_REG_MULTISYNTH5_PARAMETERS_1          =  82,
	SI5351_REG_MULTISYNTH5_PARAMETERS_2          =  83,
	SI5351_REG_MULTISYNTH5_PARAMETERS_3          =  84,
	SI5351_REG_MULTISYNTH5_PARAMETERS_4          =  85,
	SI5351_REG_MULTISYNTH5_PARAMETERS_5          =  86,
	SI5351_REG_MULTISYNTH5_PARAMETERS_6          =  87,
	SI5351_REG_MULTISYNTH5_PARAMETERS_7          =  88,
	SI5351_REG_MULTISYNTH5_PARAMETERS_8          =  89,

	SI5351_REG_MULTISYNTH6_PARAMETERS            =  90,

	SI5351_REG_MULTISYNTH7_PARAMETERS            =  91,

	SI5351_REG_CLOCK_6_7_OUTPUT_DIVIDER          =  92,

	SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_0      = 149,
	SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_1      = 150,
	SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_2      = 151,
	SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_3      = 152,
	SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_4      = 153,
	SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_5      = 154,
	SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_6      = 155,
	SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_7      = 156,
	SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_8      = 157,
	SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_9      = 158,
	SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_10     = 159,
	SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_11     = 160,
	SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_12     = 161,

	SI5351_REG_VCXO_PARAMTER_0                   = 162,
	SI5351_REG_VCXO_PARAMTER_1                   = 163,
	SI5351_REG_VCXO_PARAMTER_2                   = 164,

	SI5351_REG_CLK0_INITIAL_PHASE_OFFSET         = 165,
	SI5351_REG_CLK1_INITIAL_PHASE_OFFSET         = 166,
	SI5351_REG_CLK2_INITIAL_PHASE_OFFSET         = 167,
	SI5351_REG_CLK3_INITIAL_PHASE_OFFSET         = 168,
	SI5351_REG_CLK4_INITIAL_PHASE_OFFSET         = 169,
	SI5351_REG_CLK5_INITIAL_PHASE_OFFSET         = 170,

	SI5351_REG_PLL_RESET                         = 177,
	SI5351_REG_CRYSTAL_INTERNAL_LOAD_CAPACITANCE = 183,
	SI5351_REG_FANOUT                            = 187
} t_si5351_register;

const t_si5351_reg_list si5351_reg_list[] =
{
	{SI5351_REG_DEVICE_STATUS                    , 0x00, "DEVICE STATUS                    "},
	{SI5351_REG_INTERRUPT_STATUS_STICKY          , 0x00, "INTERRUPT STATUS STICKY          "},
	{SI5351_REG_INTERRUPT_STATUS_MASK            , 0x00, "INTERRUPT STATUS MASK            "},

	{SI5351_REG_OUTPUT_ENABLE_CONTROL            , 0x00, "OUTPUT ENABLE CONTROL            "},
	{SI5351_REG_OEB_PIN_ENABLE_CONTROL           , 0x00, "OEB PIN ENABLE CONTROL           "},

	{SI5351_REG_PLL_INPUT_SOURCE                 , 0x00, "PLL INPUT SOURCE                 "},

	{SI5351_REG_CLK0_CONTROL                     , 0x00, "CLK 0 CONTROL                    "},
	{SI5351_REG_CLK1_CONTROL                     , 0x00, "CLK 1 CONTROL                    "},
	{SI5351_REG_CLK2_CONTROL                     , 0x00, "CLK 2 CONTROL                    "},
	{SI5351_REG_CLK3_CONTROL                     , 0x00, "CLK 3 CONTROL                    "},
	{SI5351_REG_CLK4_CONTROL                     , 0x00, "CLK 4 CONTROL                    "},
	{SI5351_REG_CLK5_CONTROL                     , 0x00, "CLK 5 CONTROL                    "},
	{SI5351_REG_CLK6_CONTROL                     , 0x00, "CLK 6 CONTROL                    "},
	{SI5351_REG_CLK7_CONTROL                     , 0x00, "CLK 7 CONTROL                    "},

	{SI5351_REG_CLK3_0_DISABLE_STATE             , 0x00, "CLK 3 to 0 DISABLE STATE         "},
	{SI5351_REG_CLK7_4_DISABLE_STATE             , 0x00, "CLK 7 to 4 DISABLE STATE         "},

	{SI5351_REG_PLLA_PARAMETERS_0                , 0x00, "PLL A PARAMETERS 0               "},
	{SI5351_REG_PLLA_PARAMETERS_1                , 0x00, "PLL A PARAMETERS 1               "},
	{SI5351_REG_PLLA_PARAMETERS_2                , 0x00, "PLL A PARAMETERS 2               "},
	{SI5351_REG_PLLA_PARAMETERS_3                , 0x00, "PLL A PARAMETERS 3               "},
	{SI5351_REG_PLLA_PARAMETERS_4                , 0x00, "PLL A PARAMETERS 4               "},
	{SI5351_REG_PLLA_PARAMETERS_5                , 0x00, "PLL A PARAMETERS 5               "},
	{SI5351_REG_PLLA_PARAMETERS_6                , 0x00, "PLL A PARAMETERS 6               "},
	{SI5351_REG_PLLA_PARAMETERS_7                , 0x00, "PLL A PARAMETERS 7               "},

	{SI5351_REG_PLLB_PARAMETERS_0                , 0x00, "PLL B PARAMETERS 0               "},
	{SI5351_REG_PLLB_PARAMETERS_1                , 0x00, "PLL B PARAMETERS 1               "},
	{SI5351_REG_PLLB_PARAMETERS_2                , 0x00, "PLL B PARAMETERS 2               "},
	{SI5351_REG_PLLB_PARAMETERS_3                , 0x00, "PLL B PARAMETERS 3               "},
	{SI5351_REG_PLLB_PARAMETERS_4                , 0x00, "PLL B PARAMETERS 4               "},
	{SI5351_REG_PLLB_PARAMETERS_5                , 0x00, "PLL B PARAMETERS 5               "},
	{SI5351_REG_PLLB_PARAMETERS_6                , 0x00, "PLL B PARAMETERS 6               "},
	{SI5351_REG_PLLB_PARAMETERS_7                , 0x00, "PLL B PARAMETERS 7               "},

	{SI5351_REG_MULTISYNTH0_PARAMETERS_1         , 0x00, "MULTISYNTH 0 PARAMETERS 1        "},
	{SI5351_REG_MULTISYNTH0_PARAMETERS_2         , 0x00, "MULTISYNTH 0 PARAMETERS 2        "},
	{SI5351_REG_MULTISYNTH0_PARAMETERS_3         , 0x00, "MULTISYNTH 0 PARAMETERS 3        "},
	{SI5351_REG_MULTISYNTH0_PARAMETERS_4         , 0x00, "MULTISYNTH 0 PARAMETERS 4        "},
	{SI5351_REG_MULTISYNTH0_PARAMETERS_5         , 0x00, "MULTISYNTH 0 PARAMETERS 5        "},
	{SI5351_REG_MULTISYNTH0_PARAMETERS_6         , 0x00, "MULTISYNTH 0 PARAMETERS 6        "},
	{SI5351_REG_MULTISYNTH0_PARAMETERS_7         , 0x00, "MULTISYNTH 0 PARAMETERS 7        "},
	{SI5351_REG_MULTISYNTH0_PARAMETERS_8         , 0x00, "MULTISYNTH 0 PARAMETERS 8        "},

	{SI5351_REG_MULTISYNTH1_PARAMETERS_1         , 0x00, "MULTISYNTH 1 PARAMETERS 1        "},
	{SI5351_REG_MULTISYNTH1_PARAMETERS_2         , 0x00, "MULTISYNTH 1 PARAMETERS 2        "},
	{SI5351_REG_MULTISYNTH1_PARAMETERS_3         , 0x00, "MULTISYNTH 1 PARAMETERS 3        "},
	{SI5351_REG_MULTISYNTH1_PARAMETERS_4         , 0x00, "MULTISYNTH 1 PARAMETERS 4        "},
	{SI5351_REG_MULTISYNTH1_PARAMETERS_5         , 0x00, "MULTISYNTH 1 PARAMETERS 5        "},
	{SI5351_REG_MULTISYNTH1_PARAMETERS_6         , 0x00, "MULTISYNTH 1 PARAMETERS 6        "},
	{SI5351_REG_MULTISYNTH1_PARAMETERS_7         , 0x00, "MULTISYNTH 1 PARAMETERS 7        "},
	{SI5351_REG_MULTISYNTH1_PARAMETERS_8         , 0x00, "MULTISYNTH 1 PARAMETERS 8        "},

	{SI5351_REG_MULTISYNTH2_PARAMETERS_1         , 0x00, "MULTISYNTH 2 PARAMETERS 1        "},
	{SI5351_REG_MULTISYNTH2_PARAMETERS_2         , 0x00, "MULTISYNTH 2 PARAMETERS 2        "},
	{SI5351_REG_MULTISYNTH2_PARAMETERS_3         , 0x00, "MULTISYNTH 2 PARAMETERS 3        "},
	{SI5351_REG_MULTISYNTH2_PARAMETERS_4         , 0x00, "MULTISYNTH 2 PARAMETERS 4        "},
	{SI5351_REG_MULTISYNTH2_PARAMETERS_5         , 0x00, "MULTISYNTH 2 PARAMETERS 5        "},
	{SI5351_REG_MULTISYNTH2_PARAMETERS_6         , 0x00, "MULTISYNTH 2 PARAMETERS 6        "},
	{SI5351_REG_MULTISYNTH2_PARAMETERS_7         , 0x00, "MULTISYNTH 2 PARAMETERS 7        "},
	{SI5351_REG_MULTISYNTH2_PARAMETERS_8         , 0x00, "MULTISYNTH 2 PARAMETERS 8        "},

	{SI5351_REG_MULTISYNTH3_PARAMETERS_1         , 0x00, "MULTISYNTH 3 PARAMETERS 1        "},
	{SI5351_REG_MULTISYNTH3_PARAMETERS_2         , 0x00, "MULTISYNTH 3 PARAMETERS 2        "},
	{SI5351_REG_MULTISYNTH3_PARAMETERS_3         , 0x00, "MULTISYNTH 3 PARAMETERS 3        "},
	{SI5351_REG_MULTISYNTH3_PARAMETERS_4         , 0x00, "MULTISYNTH 3 PARAMETERS 4        "},
	{SI5351_REG_MULTISYNTH3_PARAMETERS_5         , 0x00, "MULTISYNTH 3 PARAMETERS 5        "},
	{SI5351_REG_MULTISYNTH3_PARAMETERS_6         , 0x00, "MULTISYNTH 3 PARAMETERS 6        "},
	{SI5351_REG_MULTISYNTH3_PARAMETERS_7         , 0x00, "MULTISYNTH 3 PARAMETERS 7        "},
	{SI5351_REG_MULTISYNTH3_PARAMETERS_8         , 0x00, "MULTISYNTH 3 PARAMETERS 8        "},

	{SI5351_REG_MULTISYNTH4_PARAMETERS_1         , 0x00, "MULTISYNTH 4 PARAMETERS 1        "},
	{SI5351_REG_MULTISYNTH4_PARAMETERS_2         , 0x00, "MULTISYNTH 4 PARAMETERS 2        "},
	{SI5351_REG_MULTISYNTH4_PARAMETERS_3         , 0x00, "MULTISYNTH 4 PARAMETERS 3        "},
	{SI5351_REG_MULTISYNTH4_PARAMETERS_4         , 0x00, "MULTISYNTH 4 PARAMETERS 4        "},
	{SI5351_REG_MULTISYNTH4_PARAMETERS_5         , 0x00, "MULTISYNTH 4 PARAMETERS 5        "},
	{SI5351_REG_MULTISYNTH4_PARAMETERS_6         , 0x00, "MULTISYNTH 4 PARAMETERS 6        "},
	{SI5351_REG_MULTISYNTH4_PARAMETERS_7         , 0x00, "MULTISYNTH 4 PARAMETERS 7        "},
	{SI5351_REG_MULTISYNTH4_PARAMETERS_8         , 0x00, "MULTISYNTH 4 PARAMETERS 8        "},

	{SI5351_REG_MULTISYNTH5_PARAMETERS_1         , 0x00, "MULTISYNTH 5 PARAMETERS 1        "},
	{SI5351_REG_MULTISYNTH5_PARAMETERS_2         , 0x00, "MULTISYNTH 5 PARAMETERS 2        "},
	{SI5351_REG_MULTISYNTH5_PARAMETERS_3         , 0x00, "MULTISYNTH 5 PARAMETERS 3        "},
	{SI5351_REG_MULTISYNTH5_PARAMETERS_4         , 0x00, "MULTISYNTH 5 PARAMETERS 4        "},
	{SI5351_REG_MULTISYNTH5_PARAMETERS_5         , 0x00, "MULTISYNTH 5 PARAMETERS 5        "},
	{SI5351_REG_MULTISYNTH5_PARAMETERS_6         , 0x00, "MULTISYNTH 5 PARAMETERS 6        "},
	{SI5351_REG_MULTISYNTH5_PARAMETERS_7         , 0x00, "MULTISYNTH 5 PARAMETERS 7        "},
	{SI5351_REG_MULTISYNTH5_PARAMETERS_8         , 0x00, "MULTISYNTH 5 PARAMETERS 8        "},

	{SI5351_REG_MULTISYNTH6_PARAMETERS           , 0x00, "MULTISYNTH 6 PARAMETERS          "},

	{SI5351_REG_MULTISYNTH7_PARAMETERS           , 0x00, "MULTISYNTH 7 PARAMETERS          "},

	{SI5351_REG_CLOCK_6_7_OUTPUT_DIVIDER         , 0x00, "CLOCK 6 & 7 OUTPUT DIVIDER       "},

	{SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_0     , 0x00, "SPREAD SPECTRUM PARAMETERS 0     "},
	{SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_1     , 0x00, "SPREAD SPECTRUM PARAMETERS 1     "},
	{SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_2     , 0x00, "SPREAD SPECTRUM PARAMETERS 2     "},
	{SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_3     , 0x00, "SPREAD SPECTRUM PARAMETERS 3     "},
	{SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_4     , 0x00, "SPREAD SPECTRUM PARAMETERS 4     "},
	{SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_5     , 0x00, "SPREAD SPECTRUM PARAMETERS 5     "},
	{SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_6     , 0x00, "SPREAD SPECTRUM PARAMETERS 6     "},
	{SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_7     , 0x00, "SPREAD SPECTRUM PARAMETERS 7     "},
	{SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_8     , 0x00, "SPREAD SPECTRUM PARAMETERS 8     "},
	{SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_9     , 0x00, "SPREAD SPECTRUM PARAMETERS 9     "},
	{SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_10    , 0x00, "SPREAD SPECTRUM PARAMETERS 10    "},
	{SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_11    , 0x00, "SPREAD SPECTRUM PARAMETERS 11    "},
	{SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_12    , 0x00, "SPREAD SPECTRUM PARAMETERS 12    "},

	{SI5351_REG_VCXO_PARAMTER_0                  , 0x00, "VCXO PARAMTER BITS  0 to  7"      },
	{SI5351_REG_VCXO_PARAMTER_1                  , 0x00, "VCXO PARAMTER BITS  8 to 15"      },
	{SI5351_REG_VCXO_PARAMTER_2                  , 0x00, "VCXO PARAMTER BITS 16 to 21"      },

	{SI5351_REG_CLK0_INITIAL_PHASE_OFFSET        , 0x00, "CLK 0 INITIAL PHASE OFFSET       "},
	{SI5351_REG_CLK1_INITIAL_PHASE_OFFSET        , 0x00, "CLK 1 INITIAL PHASE OFFSET       "},
	{SI5351_REG_CLK2_INITIAL_PHASE_OFFSET        , 0x00, "CLK 2 INITIAL PHASE OFFSET       "},
	{SI5351_REG_CLK3_INITIAL_PHASE_OFFSET        , 0x00, "CLK 3 INITIAL PHASE OFFSET       "},
	{SI5351_REG_CLK4_INITIAL_PHASE_OFFSET        , 0x00, "CLK 4 INITIAL PHASE OFFSET       "},
	{SI5351_REG_CLK5_INITIAL_PHASE_OFFSET        , 0x00, "CLK 5 INITIAL PHASE OFFSET       "},

	{SI5351_REG_PLL_RESET                        , 0x00, "PLL RESET                        "},
	{SI5351_REG_CRYSTAL_INTERNAL_LOAD_CAPACITANCE, 0xC0, "CRYSTAL INTERNAL LOAD CAPACITANCE"},
	{SI5351_REG_FANOUT                           , 0x00, "FAN OUT                          "}
};

typedef enum
{
	SI5351_CLK_NONE = -1,
	SI5351_CLK_0    =  0,
	SI5351_CLK_1    =  1,
	SI5351_CLK_2    =  2
} si5351Clk_t;

typedef enum
{
	SI5351_PLL_NONE = -1,
	SI5351_PLL_A    =  0,
	SI5351_PLL_B    =  1
} si5351PLL_t;

typedef enum
{
	SI5351_CRYSTAL_LOAD_NONE = -1,
//	SI5351_CRYSTAL_LOAD_0PF  =  0,
	SI5351_CRYSTAL_LOAD_6PF  =  1,
	SI5351_CRYSTAL_LOAD_8PF  =  2,
	SI5351_CRYSTAL_LOAD_10PF =  3
} si5351CrystalLoad_t;

typedef enum
{
	SI5351_MULTISYNTH_DIV_NONE = -1,
	SI5351_MULTISYNTH_DIV_4    =  4,
	SI5351_MULTISYNTH_DIV_6    =  6,
	SI5351_MULTISYNTH_DIV_8    =  8
} si5351MultisynthDiv_t;

typedef enum
{
	SI5351_OUT_DRIVE_NONE = -1,
	SI5351_OUT_DRIVE_2MA  =  0,
	SI5351_OUT_DRIVE_4MA  =  1,
	SI5351_OUT_DRIVE_6MA  =  2,
	SI5351_OUT_DRIVE_8MA  =  3
} si5351OutDrive_t;

typedef enum
{
	SI5351_R_DIV_NONE = -1,
	SI5351_R_DIV_1    =  0,
	SI5351_R_DIV_2    =  1,
	SI5351_R_DIV_4    =  2,
	SI5351_R_DIV_8    =  3,
	SI5351_R_DIV_16   =  4,
	SI5351_R_DIV_32   =  5,
	SI5351_R_DIV_64   =  6,
	SI5351_R_DIV_128  =  7
} si5351RDiv_t;

// ****************************************************************

TForm1 *Form1 = NULL;

__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}

void __fastcall TForm1::FormCreate(TObject *Sender)
{
	String s;

	m_closing = false;

	{
		TVersion version;
		TForm1::GetBuildInfo(Application->ExeName, &version);
		s.printf("v%d.%d.%d.%d", version.MajorVer, version.MinorVer, version.ReleaseVer, version.BuildVer);
		#ifdef _DEBUG
			s += " debug";
		#endif
		Caption = Application->Title + "   " + s + "   compiled " + __DATE__ + " " + __TIME__;
	}

	this->DoubleBuffered             = true;
	RegisterListView->DoubleBuffered = true;
	FileListView->DoubleBuffered     = true;

	// help stop flicker
	this->ControlStyle             = this->ControlStyle  << csOpaque;
	RegisterListView->ControlStyle = RegisterListView->ControlStyle << csOpaque;
	FileListView->ControlStyle     = FileListView->ControlStyle << csOpaque;
//	LineLabel->ControlStyle        = LineLabel->ControlStyle << csOpaque;

	m_ini_filename = ChangeFileExt(Application->ExeName, ".ini");

//	Memo1->Clear();

	FilenameEdit->Text = "";

	m_file_line_clicked = -1;

	m_xtal_Hz = 27000000;
	XtalFreqEdit->Text = "27.000000";

	PLLALabel->Caption = "--";
	PLLBLabel->Caption = "--";

	Clock0Label->Caption = "--";
	Clock1Label->Caption = "--";
	Clock2Label->Caption = "--";

	LineLabel->Caption = "-";

	OpenDialog1->InitialDir = IncludeTrailingPathDelimiter(ExtractFilePath(Application->ExeName));

	::PostMessage(this->Handle, WM_INIT_GUI, 0, 0);
}

void __fastcall TForm1::FormDestroy(TObject *Sender)
{
	m_closing = true;
}

void __fastcall TForm1::FormClose(TObject *Sender, TCloseAction &Action)
{
	m_closing = true;

	saveSettings();
}

bool __fastcall TForm1::GetBuildInfo(String filename, TVersion *version)
{
	DWORD  ver_info_size;
	char   *ver_info;
	UINT   buffer_size;
	LPVOID buffer;
	DWORD  dummy;

	if (version == NULL || filename.IsEmpty())
		return false;

	memset(version, 0, sizeof(TVersion));

	ver_info_size = ::GetFileVersionInfoSizeA(filename.c_str(), &dummy);
	if (ver_info_size == 0)
		return false;

	ver_info = new char [ver_info_size];
	if (ver_info == NULL)
		return false;

	if (::GetFileVersionInfoA(filename.c_str(), 0, ver_info_size, ver_info) == FALSE)
	{
		delete [] ver_info;
		return false;
	}

	if (::VerQueryValue(ver_info, "\\", &buffer, &buffer_size) == FALSE)
	{
		delete [] ver_info;
		return false;
	}

	PVSFixedFileInfo ver = (PVSFixedFileInfo)buffer;
	version->MajorVer    = (ver->dwFileVersionMS >> 16) & 0xFFFF;
	version->MinorVer    = (ver->dwFileVersionMS >>  0) & 0xFFFF;
	version->ReleaseVer  = (ver->dwFileVersionLS >> 16) & 0xFFFF;
	version->BuildVer    = (ver->dwFileVersionLS >>  0) & 0xFFFF;

	delete [] ver_info;

	return true;
}

void __fastcall TForm1::WMWindowPosChanging(TWMWindowPosChanging &msg)
{
	#define SWP_STATECHANGED            0x8000
	#define WINDOW_SNAP                 5

	const int thresh = WINDOW_SNAP;

	if (msg.WindowPos->flags & SWP_STATECHANGED)
	{
		if (msg.WindowPos->flags & SWP_FRAMECHANGED)
		{
			if (msg.WindowPos->x < 0 && msg.WindowPos->y < 0)
			{	// Window state is about to change to MAXIMIZED
				if ((msg.WindowPos->flags & (SWP_SHOWWINDOW | SWP_NOACTIVATE)) == (SWP_SHOWWINDOW | SWP_NOACTIVATE))
				{	// about to minimize
					return;
				}
				else
				{	// about to maximize
					return;
				}
			}
			else
			{	// about to normalize
			}
		}
	}

	if (msg.WindowPos->hwnd != this->Handle || Screen == NULL)
		return;

	const int dtLeft   = Screen->DesktopRect.left;
	//const int dtRight  = Screen->DesktopRect.right;
	const int dtTop    = Screen->DesktopRect.top;
	const int dtBottom = Screen->DesktopRect.bottom;
	const int dtWidth  = Screen->DesktopRect.Width();
	const int dtHeight = Screen->DesktopRect.Height();

	//const int waLeft   = Screen->WorkAreaRect.left;
	//const int waRight  = Screen->WorkAreaRect.right;
	//const int waTop    = Screen->WorkAreaRect.top;
	//const int waBottom = Screen->WorkAreaRect.bottom;
	//const int waWidth  = Screen->WorkAreaRect.Width();
	//const int waHeight = Screen->WorkAreaRect.Height();

	int x = msg.WindowPos->x;
	int y = msg.WindowPos->y;
	int w = msg.WindowPos->cx;
	int h = msg.WindowPos->cy;

	for (int i = 0; i < Screen->MonitorCount; i++)
	{	// sticky screen edges
		const int mLeft   = Screen->Monitors[i]->WorkareaRect.left;
		const int mRight  = Screen->Monitors[i]->WorkareaRect.right;
		const int mTop    = Screen->Monitors[i]->WorkareaRect.top;
		const int mBottom = Screen->Monitors[i]->WorkareaRect.bottom;
		const int mWidth  = Screen->Monitors[i]->WorkareaRect.Width();
		const int mHeight = Screen->Monitors[i]->WorkareaRect.Height();

		if (ABS(x - mLeft) < thresh)
				  x = mLeft;			// stick left to left side
		else
		if (ABS((x + w) - mRight) < thresh)
					x = mRight - w;	// stick right to right side

		if (ABS(y - mTop) < thresh)
				  y = mTop;				// stick top to top side
		else
		if (ABS((y + h) - mBottom) < thresh)
					y = mBottom - h;	// stick bottom to bottm side

		// stick the right side to the right side of the screen if the left side is stuck to the left side of the screen
		if (x == mLeft)
			if ((w >= (mWidth - thresh)) && (w <= (mWidth + thresh)))
				w = mWidth;

		// stick the bottom to the bottom of the screen if the top is stuck to the top of the screen
		if (y == mTop)
			if ((h >= (mHeight - thresh)) && (h <= (mHeight + thresh)))
				h = mHeight;
	}
/*
	{	// sticky screen edges
		if (ABS(x - waLeft) < thresh)
			x = waLeft;			// stick left to left side
		else
		if (ABS((x + w) - waRight) < thresh)
			x = waRight - w;	// stick right to right side

		if (ABS(y - waTop) < thresh)
			y = waTop;			// stick top to top side
		else
		if (ABS((y + h) - waBottom) < thresh)
			y = waBottom - h;	// stick bottom to bottm side

		// stick the right side to the right side of the screen if the left side is stuck to the left side of the screen
		if (x == waLeft)
			if ((w >= (waWidth - thresh)) && (w <= (waWidth + thresh)))
				w = waWidth;

		// stick the bottom to the bottom of the screen if the top is stuck to the top of the screen
		if (y == waTop)
			if ((h >= (waHeight - thresh)) && (h <= (waHeight + thresh)))
				h = waHeight;
	}
*/
	// limit minimum size
	if (w < Constraints->MinWidth)
		 w = Constraints->MinWidth;
	if (h < Constraints->MinHeight)
		 h = Constraints->MinHeight;

	// limit maximum size
	if (w > Constraints->MaxWidth && Constraints->MaxWidth > Constraints->MinWidth)
		 w = Constraints->MaxWidth;
	if (h > Constraints->MaxHeight && Constraints->MaxHeight > Constraints->MinHeight)
		 h = Constraints->MaxHeight;

	// limit maximum size
	if (w > dtWidth)
		 w = dtWidth;
	if (h > dtHeight)
		 h = dtHeight;
/*
	if (Application->MainForm && this != Application->MainForm)
	{	// stick to our main form sides
		const TRect rect = Application->MainForm->BoundsRect;

		if (ABS(x - rect.left) < thresh)
			x = rect.left;			// stick to left to left side
		else
		if (ABS((x + w) - rect.left) < thresh)
			x = rect.left - w;	// stick right to left side
		else
		if (ABS(x - rect.right) < thresh)
			x = rect.right;		// stick to left to right side
		else
		if (ABS((x + w) - rect.right) < thresh)
			x = rect.right - w;	// stick to right to right side

		if (ABS(y - rect.top) < thresh)
			y = rect.top;			// stick top to top side
		else
		if (ABS((y + h) - rect.top) < thresh)
			y = rect.top - h;		// stick bottom to top side
		else
		if (ABS(y - rect.bottom) < thresh)
			y = rect.bottom;		// stick top to bottom side
		else
		if (ABS((y + h) - rect.bottom) < thresh)
			y = rect.bottom - h;	// stick bottom to bottom side
	}
*/
	// stop it completely leaving the desktop area
	if (x < (dtLeft - Width + (dtWidth / 15)))
		 x =  dtLeft - Width + (dtWidth / 15);
	if (x > (dtWidth - (Screen->Width / 15)))
		 x =  dtWidth - (Screen->Width / 15);
	if (y < dtTop)
		 y = dtTop;
	if (y > (dtBottom - (dtHeight / 10)))
		 y =  dtBottom - (dtHeight / 10);

	msg.WindowPos->x  = x;
	msg.WindowPos->y  = y;
	msg.WindowPos->cx = w;
	msg.WindowPos->cy = h;
}

void __fastcall TForm1::WMInitGUI(TMessage &msg)
{
	String s;

	resetSi5351RegValues();

	loadSettings();

//	if (Application->MainForm)
//		Application->MainForm->Update();

	Update();

//	BringToFront();
//	::SetForegroundWindow(Handle);

	if (!m_filename.IsEmpty())
		loadFile(m_filename);

	updateRegisterListView(false);
}

void __fastcall TForm1::resetSi5351RegValues()
{	// set all the register values to their default reset states

	memset(&m_si5351_reg_values[0], 0, sizeof(m_si5351_reg_values));

	for (unsigned int i = 0; i < ARRAY_SIZE(si5351_reg_list); i++)
	{
		const int addr      = si5351_reg_list[i].addr;
		const uint8_t value = si5351_reg_list[i].reset_value;

		m_si5351_reg_values[addr] = value;
	}
}

void __fastcall TForm1::loadSettings()
{
	TIniFile *ini = new TIniFile(m_ini_filename);
	if (ini == NULL)
		return;

	Top    = ini->ReadInteger("MainForm", "Top", Top);
	Left   = ini->ReadInteger("MainForm", "Left", Left);
	Width  = ini->ReadInteger("MainForm", "Width", Width);
	Height = ini->ReadInteger("MainForm", "Height", Height);

	FileListView->Width = ini->ReadInteger("MainForm", "SplitterPos", FileListView->Width);

	m_filename = ini->ReadString("Misc", "Filename", m_filename).Trim();

	XtalFreqEdit->Text = ini->ReadString("Misc", "XtalFrequency", XtalFreqEdit->Text);

	delete ini;
}

void __fastcall TForm1::saveSettings()
{
	DeleteFile(m_ini_filename);

	TIniFile *ini = new TIniFile(m_ini_filename);
	if (ini == NULL)
		return;

	ini->WriteInteger("MainForm", "Top", Top);
	ini->WriteInteger("MainForm", "Left", Left);
	ini->WriteInteger("MainForm", "Width", Width);
	ini->WriteInteger("MainForm", "Height", Height);

	ini->WriteInteger("MainForm", "SplitterPos", FileListView->Width);

	ini->WriteString("Misc", "Filename", m_filename);

	ini->WriteString("Misc", "XtalFrequency", XtalFreqEdit->Text);

	delete ini;
}

int __fastcall TForm1::parseString(String s, String separator, std::vector <String> &params)
{
	params.resize(0);

	// replace any tabs with spaces
	while (true)
	{
		const int pos = s.Pos('\t');
		if (pos <= 0)
			break;
		s[pos] = ' ';
	}

	s = s.Trim();

	while (!s.IsEmpty())
	{
		const int m = s.Pos(separator);
		if (m < 1)
		{
			params.push_back(s);
			s = "";
		}
		else
		if (m == 1)
		{
			s = s.SubString(m + separator.Length(), s.Length()).Trim();
		}
		else
		{
			const String s2 = s.SubString(1, m - 1).Trim();
			s = s.SubString(m + separator.Length(), s.Length()).Trim();
			params.push_back(s2);
		}
	}

	return params.size();
}

bool __fastcall TForm1::loadFile(String filename)
{
	// ***************************
	// load the file in

	#if (__BORLANDC__ < 0x0600)
		FILE *file = fopen(filename.c_str(), "rb");
	#else
		FILE *file = NULL;
		int res = fopen_s(&file, filename.c_str(), "rb");
		if (res != 0 || !file)
		{
			if (file)
				fclose(file);
			file = NULL;
		}
	#endif

	if (file == NULL)
		return false;

	if (fseek(file, 0, SEEK_END) != 0)
	{
		fclose(file);
		file = NULL;
		return false;
	}
	const size_t file_size = (int)ftell(file);
	if (fseek(file, 0, SEEK_SET) != 0 || file_size < 10)
	{
		fclose(file);
		file = NULL;
		return false;
	}

	m_file_data.resize(file_size + 1);

	if (fread(&m_file_data[0], 1, file_size, file) != file_size)
	{
		fclose(file);
//		file = NULL;
		m_file_data.resize(0);
		return false;
	}

	m_file_data[file_size] = 0;

	fclose(file);

	// ***************************
	// extract each text line

	m_file_lines.resize(0);
	m_parsed_file_lines.resize(0);

	for (unsigned int i = 0; i < m_file_data.size(); )
	{
		String s;

		// find the end of the line
		while (i < m_file_data.size())
		{
			char c = m_file_data[i++];

			if (c == '\r' || c == '\n')
			{
				if (i < m_file_data.size())
				{	// drop any following cr/lf
					const char c2 = m_file_data[i];
					if ((c == '\r' && c2 == '\n') || (c == '\n' && c2 == '\r'))
						i++;
				}
				break;
			}

			if (c == '\t')
				c = ' ';

			if (c >= 32)
				s += c;
		}

		s = s.Trim();
//		if (s.IsEmpty())
//			continue;

		m_file_lines.push_back(s);
	}

	if (m_file_lines.empty())
		return false;

	m_filename = filename;

	FilenameEdit->Text = m_filename;

	// ***************************
	// parse up each text line of the file

	for (unsigned int i = 0; i < m_file_lines.size(); i++)
	{
		std::vector <String> params;
		parseString(m_file_lines[i], " ", params);
		m_parsed_file_lines.push_back(params);
	}

	// ***************************
	// convert the text values into data values

	resetSi5351RegValues();

	m_file_line_clicked = -1;

	LineLabel->Caption  = "";
	LineLabel->Update();

	m_file_line_reg_values.clear();
	m_file_line_reg_values.resize(m_file_lines.size());

	for (unsigned int i = 0; i < m_file_lines.size(); i++)
	{
		std::vector <String> &params = m_parsed_file_lines[i];

		if (params.size() < 2)
			continue;

		String s = params[0];

		if (s.Pos('.') > 0)
		{
			float seconds;
			if (TryStrToFloat(s, seconds))
			{
				if (seconds > 0.0)
				{
					s = params[1];

					if (s.Length() < 4)
						continue;

					if (s.SubString(1, 2).LowerCase() == "0x")
					{
						int value;
						if (TryStrToInt(s, value))
						{
							if (value >= 0 && value <= 255)
							{

							}
						}
					}
				}
			}
			continue;
		}

		if (s.Length() < 4)
			continue;

		if (s.SubString(1, 2).LowerCase() == "0x")
		{
			int addr;
			if (TryStrToInt(s, addr))
			{
				if (addr < 0 || addr >= (int)ARRAY_SIZE(m_si5351_reg_values))
					continue;

				m_file_line_reg_values[i].push_back(addr);

				for (unsigned int k = 1; k < params.size(); k++)
				{
					if (addr >= (int)ARRAY_SIZE(m_si5351_reg_values))
						break;

					s = params[k];
					if (s.Length() < 4)
						continue;

					if (s.SubString(1, 2).LowerCase() == "0x")
					{
						int value;
						if (TryStrToInt(s, value))
							if (value >= 0 && value <= 255)
								m_file_line_reg_values[i].push_back(value);
					}
				}
			}
		}
	}

	// ***************************
	// display the parsed up file lines

	{
		FileListView->Items->BeginUpdate();
		FileListView->Clear();
		for (unsigned int i = 0; i < m_file_lines.size(); i++)
		{
			std::vector <String> &params = m_parsed_file_lines[i];
			String s;
			for (unsigned int k = 0; k < params.size(); k++)
				s += " " + params[k];

			TListItem *item = FileListView->Items->Add();
			item->Caption = s;
		}
		FileListView->Items->EndUpdate();
	}

	// ***************************

	return !m_file_lines.empty();
}

String __fastcall TForm1::binToStr(const uint64_t value, const int digits)
{
	String s;

	if (digits > 0)
	{
		uint64_t mask = 1ull << (digits - 1);

		for (int i = 0; i < digits; i++, mask >>= 1)
			s += (value & mask) ? '1' : '0';
	}

	return s;
}

String __fastcall TForm1::regSettingDescription(const int addr, const uint8_t value)
{
	String s = "--";

	switch (addr)
	{
		case SI5351_REG_DEVICE_STATUS:
			s  = (value & 0x80) ? " SYS_INIT"   : " sys_init";
			s += (value & 0x40) ? "  LOL_B"     : "  lol_b";
			s += (value & 0x20) ? "  LOL_A"     : "  lol_a";
			s += (value & 0x10) ? "  LOS_CLKIN" : "  los_clkin";
			s += (value & 0x08) ? "  LOS_XTAL"  : "  los_xtal";
			s += (value & 0x04) ? "  RESERVED"  : "  reserved";
			s += "  RevID-" + IntToStr(value & 0x03);
			break;
		case SI5351_REG_INTERRUPT_STATUS_STICKY:
			s  = (value & 0x80) ? " SYS_INIT_STKY"   : " sys_init_stky";
			s += (value & 0x40) ? "  LOL_B_STKY"     : "  lol_b_stky";
			s += (value & 0x20) ? "  LOL_A_STKY"     : "  lol_a_stky";
			s += (value & 0x10) ? "  LOS_CLKIN_STKY" : "  los_clkin_stky";
			s += (value & 0x08) ? "  LOS_XTAL_STKY"  : "  los_xtal_stky";
			s += (value & 0x04) ? "  RESERVED"       : "  reserved";
			s += (value & 0x02) ? "  RESERVED"       : "  reserved";
			s += (value & 0x01) ? "  RESERVED"       : "  reserved";
			break;
		case SI5351_REG_INTERRUPT_STATUS_MASK:
			s  = (value & 0x80) ? " SYS_INIT_MASK"   : " sys_init_mask";
			s += (value & 0x40) ? "  LOL_B_MASK"     : "  lol_b_mask";
			s += (value & 0x20) ? "  LOL_A_MASK"     : "  lol_a_mask";
			s += (value & 0x10) ? "  LOS_CLKIN_MASK" : "  los_clkin_mask";
			s += (value & 0x08) ? "  LOS_XTAL_MASK"  : "  los_xtal_mask";
			s += (value & 0x04) ? "  RESERVED"       : "  reserved";
			s += (value & 0x02) ? "  RESERVED"       : "  reserved";
			s += (value & 0x01) ? "  RESERVED"       : "  reserved";
			break;

		case SI5351_REG_OUTPUT_ENABLE_CONTROL:
			s  = (value & 0x80) ? " CLK-7   "  : " clk-7-EN";
			s += (value & 0x40) ? "  CLK-6   " : "  clk-6-EN";
			s += (value & 0x20) ? "  CLK-5   " : "  clk-5-EN";
			s += (value & 0x10) ? "  CLK-4   " : "  clk-4-EN";
			s += (value & 0x08) ? "  CLK-3   " : "  clk-3-EN";
			s += (value & 0x04) ? "  CLK-2   " : "  clk-2-EN";
			s += (value & 0x02) ? "  CLK-1   " : "  clk-1-EN";
			s += (value & 0x01) ? "  CLK-0   " : "  clk-0-EN";
			break;
		case SI5351_REG_OEB_PIN_ENABLE_CONTROL:
			s  = (value & 0x80) ? " OEB-7"  : " oeb-7";
			s += (value & 0x40) ? "  OEB-6" : "  oeb-6";
			s += (value & 0x20) ? "  OEB-5" : "  oeb-5";
			s += (value & 0x10) ? "  OEB-4" : "  oeb-4";
			s += (value & 0x08) ? "  OEB-3" : "  oeb-3";
			s += (value & 0x04) ? "  OEB-2" : "  oeb-2";
			s += (value & 0x02) ? "  OEB-1" : "  oeb-1";
			s += (value & 0x01) ? "  OEB-0" : "  oeb-0";
			break;

		case SI5351_REG_PLL_INPUT_SOURCE:
			s  = " CLKIN_DIV-" + IntToStr((value >> 6) & 0x03);
			s += (value & 0x20) ? "  RES-5"          : "  res-5";
			s += (value & 0x10) ? "  RES-4"          : "  res-4";
			s += (value & 0x08) ? "  PLLB_SRC-CLKIN" : "  PLLB_SRC-XTAL";
			s += (value & 0x04) ? "  PLLA_SRC-CLKIN" : "  PLLA_SRC-XTAL";
			s += (value & 0x02) ? "  RES-1"          : "  res-1";
			s += (value & 0x01) ? "  RES-0"          : "  res-0";
			break;

		case SI5351_REG_CLK0_CONTROL:
			s  = (value & 0x80) ? " CLK0_PDN-DN"   : " clk0_pdn-UP";
			s += (value & 0x40) ? "  MS0_INT-INT " : "  ms0_int-FRAC";
			s += (value & 0x20) ? "  MS0_SRC-PLLB" : "  ms0_src-PLLA";
			s += (value & 0x10) ? "  CLK0_INV"     : "  clk0_inv";

			s += "  CLK0_SRC-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "XTAL "; break;
				case 1: s += "CLKIN"; break;
				case 2: s += "RES'D"; break;
				case 3: s += "MS0  "; break;
			}

			s += "  CLK0_IDRV-";
			switch ((value >> 0) & 0x03)
			{
				case 0: s += "2mA"; break;
				case 1: s += "4mA"; break;
				case 2: s += "6mA"; break;
				case 3: s += "8mA"; break;
			}
			break;
		case SI5351_REG_CLK1_CONTROL:
			s  = (value & 0x80) ? " CLK1_PDN-DN"   : " clk1_pdn-UP";
			s += (value & 0x40) ? "  MS1_INT-INT " : "  ms1_int-FRAC";
			s += (value & 0x20) ? "  MS1_SRC-PLLB" : "  ms1_src-PLLA";
			s += (value & 0x10) ? "  CLK1_INV"     : "  clk1_inv";

			s += "  CLK1_SRC-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "XTAL "; break;
				case 1: s += "CLKIN"; break;
				case 2: s += "MS0  "; break;
				case 3: s += "MS1  "; break;
			}

			s += "  CLK1_IDRV-";
			switch ((value >> 0) & 0x03)
			{
				case 0: s += "2mA"; break;
				case 1: s += "4mA"; break;
				case 2: s += "6mA"; break;
				case 3: s += "8mA"; break;
			}
			break;
		case SI5351_REG_CLK2_CONTROL:
			s  = (value & 0x80) ? " CLK2_PDN-DN"   : " clk2_pdn-UP";
			s += (value & 0x40) ? "  MS2_INT-INT " : "  ms2_int-FRAC";
			s += (value & 0x20) ? "  MS2_SRC-PLLB" : "  ms2_src-PLLA";
			s += (value & 0x10) ? "  CLK2_INV"     : "  clk2_inv";

			s += "  CLK2_SRC-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "XTAL "; break;
				case 1: s += "CLKIN"; break;
				case 2: s += "MS0  "; break;
				case 3: s += "MS2  "; break;
			}

			s += "  CLK2_IDRV-";
			switch ((value >> 0) & 0x03)
			{
				case 0: s += "2mA"; break;
				case 1: s += "4mA"; break;
				case 2: s += "6mA"; break;
				case 3: s += "8mA"; break;
			}
			break;
		case SI5351_REG_CLK3_CONTROL:
			s  = (value & 0x80) ? " CLK3_PDN-DN"   : " clk3_pdn-UP";
			s += (value & 0x40) ? "  MS3_INT-INT " : "  ms3_int-FRAC";
			s += (value & 0x20) ? "  MS3_SRC-PLLB" : "  ms3_src-PLLA";
			s += (value & 0x10) ? "  CLK3_INV"     : "  clk3_inv";

			s += "  CLK3_SRC-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "XTAL "; break;
				case 1: s += "CLKIN"; break;
				case 2: s += "MS0  "; break;
				case 3: s += "MS3  "; break;
			}

			s += "  CLK3_IDRV-";
			switch ((value >> 0) & 0x03)
			{
				case 0: s += "2mA"; break;
				case 1: s += "4mA"; break;
				case 2: s += "6mA"; break;
				case 3: s += "8mA"; break;
			}
			break;
		case SI5351_REG_CLK4_CONTROL:
			s  = (value & 0x80) ? " CLK4_PDN-DN"   : " clk4_pdn-UP";
			s += (value & 0x40) ? "  MS4_INT-INT " : "  ms4_int-FRAC";
			s += (value & 0x20) ? "  MS4_SRC-PLLB" : "  ms4_src-PLLA";
			s += (value & 0x10) ? "  CLK4_INV"     : "  clk4_inv";

			s += "  CLK4_SRC-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "XTAL "; break;
				case 1: s += "CLKIN"; break;
				case 2: s += "RES'D "; break;
				case 3: s += "MS4  "; break;
			}

			s += "  CLK4_IDRV-";
			switch ((value >> 0) & 0x03)
			{
				case 0: s += "2mA"; break;
				case 1: s += "4mA"; break;
				case 2: s += "6mA"; break;
				case 3: s += "8mA"; break;
			}
			break;
		case SI5351_REG_CLK5_CONTROL:
			s  = (value & 0x80) ? " CLK5_PDN-DN"   : " clk5_pdn-UP";
			s += (value & 0x40) ? "  MS5_INT-INT " : "  ms5_int-FRAC";
			s += (value & 0x20) ? "  MS5_SRC-PLLB" : "  ms5_src-PLLA";
			s += (value & 0x10) ? "  CLK5_INV"     : "  clk5_inv";

			s += "  CLK5_SRC-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "XTAL "; break;
				case 1: s += "CLKIN"; break;
				case 2: s += "MS4  "; break;
				case 3: s += "MS5  "; break;
			}

			s += "  CLK5_IDRV-";
			switch ((value >> 0) & 0x03)
			{
				case 0: s += "2mA"; break;
				case 1: s += "4mA"; break;
				case 2: s += "6mA"; break;
				case 3: s += "8mA"; break;
			}
			break;
		case SI5351_REG_CLK6_CONTROL:
			s  = (value & 0x80) ? " CLK6_PDN-DN"   : " clk6_pdn-UP";
			s += (value & 0x40) ? "  MS6_INT-INT " : "  ms6_int-FRAC";
			s += (value & 0x20) ? "  MS6_SRC-PLLB" : "  ms6_src-PLLA";
			s += (value & 0x10) ? "  CLK6_INV"     : "  clk6_inv";

			s += "  CLK6_SRC-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "XTAL "; break;
				case 1: s += "CLKIN"; break;
				case 2: s += "MS4  "; break;
				case 3: s += "MS6  "; break;
			}

			s += "  CLK6_IDRV-";
			switch ((value >> 0) & 0x03)
			{
				case 0: s += "2mA"; break;
				case 1: s += "4mA"; break;
				case 2: s += "6mA"; break;
				case 3: s += "8mA"; break;
			}
			break;
		case SI5351_REG_CLK7_CONTROL:
			s  = (value & 0x80) ? " CLK7_PDN-DN"   : " clk7_pdn-UP";
			s += (value & 0x40) ? "  MS7_INT-INT " : "  ms7_int-FRAC";
			s += (value & 0x20) ? "  MS7_SRC-PLLB" : "  ms7_src-PLLA";
			s += (value & 0x10) ? "  CLK7_INV"     : "  clk7_inv";

			s += "  CLK7_SRC-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "XTAL "; break;
				case 1: s += "CLKIN"; break;
				case 2: s += "MS4  "; break;
				case 3: s += "MS7  "; break;
			}

			s += "  CLK7_IDRV-";
			switch ((value >> 0) & 0x03)
			{
				case 0: s += "2mA"; break;
				case 1: s += "4mA"; break;
				case 2: s += "6mA"; break;
				case 3: s += "8mA"; break;
			}
			break;

		case SI5351_REG_CLK3_0_DISABLE_STATE:
			s  = " CLK3_DIS_STATE-";
			switch ((value >> 6) & 0x03)
			{
				case 0: s += "LOW    "; break;
				case 1: s += "HIGH   "; break;
				case 2: s += "HIGH-Z "; break;
				case 3: s += "ENABLED"; break;
			}
			s += "  CLK2_DIS_STATE-";
			switch ((value >> 4) & 0x03)
			{
				case 0: s += "LOW    "; break;
				case 1: s += "HIGH   "; break;
				case 2: s += "HIGH-Z "; break;
				case 3: s += "ENABLED"; break;
			}
			s += "  CLK1_DIS_STATE-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "LOW    "; break;
				case 1: s += "HIGH   "; break;
				case 2: s += "HIGH-Z "; break;
				case 3: s += "ENABLED"; break;
			}
			s += "  CLK0_DIS_STATE-";
			switch ((value >> 0) & 0x03)
			{
				case 0: s += "LOW    "; break;
				case 1: s += "HIGH   "; break;
				case 2: s += "HIGH-Z "; break;
				case 3: s += "ENABLED"; break;
			}
			break;
		case SI5351_REG_CLK7_4_DISABLE_STATE:
			s  = " CLK7_DIS_STATE-";
			switch ((value >> 6) & 0x03)
			{
				case 0: s += "LOW    "; break;
				case 1: s += "HIGH   "; break;
				case 2: s += "HIGH-Z "; break;
				case 3: s += "ENABLED"; break;
			}
			s += "  CLK6_DIS_STATE-";
			switch ((value >> 4) & 0x03)
			{
				case 0: s += "LOW    "; break;
				case 1: s += "HIGH   "; break;
				case 2: s += "HIGH-Z "; break;
				case 3: s += "ENABLED"; break;
			}
			s += "  CLK5_DIS_STATE-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "LOW    "; break;
				case 1: s += "HIGH   "; break;
				case 2: s += "HIGH-Z "; break;
				case 3: s += "ENABLED"; break;
			}
			s += "  CLK4_DIS_STATE-";
			switch ((value >> 0) & 0x03)
			{
				case 0: s += "LOW    "; break;
				case 1: s += "HIGH   "; break;
				case 2: s += "HIGH-Z "; break;
				case 3: s += "ENABLED"; break;
			}
			break;

		case SI5351_REG_PLLA_PARAMETERS_0:
			s = " MSNA_P3[15:8] " + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_PLLA_PARAMETERS_1:
			s = " MSNA_P3[ 7:0] " + IntToStr((uint32_t)value << 0);
			break;
		case SI5351_REG_PLLA_PARAMETERS_2:
			s  = " Reserved-" + IntToStr((value >> 2) & 0x03);
			s += "  MSNA_P1[17:16]-" + IntToStr((uint32_t)(value & 0x03) << 16);
			break;
		case SI5351_REG_PLLA_PARAMETERS_3:
			s = " MSNA_P1[15:8] " + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_PLLA_PARAMETERS_4:
			s = " MSNA_P1[ 7:0] " + IntToStr((uint32_t)value << 0);
			break;
		case SI5351_REG_PLLA_PARAMETERS_5:
			s  = " MSNA_P3[19:16]-" + IntToStr((uint32_t)((value >> 4) & 0x0f) << 16);
			s += "  MSNA_P2[19:16]-" + IntToStr((uint32_t)((value >> 0) & 0x0f) << 16);
			break;
		case SI5351_REG_PLLA_PARAMETERS_6:
			s = " MSNA_P2[15:8] " + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_PLLA_PARAMETERS_7:
			s = " MSNA_P2[ 7:0] " + IntToStr((uint32_t)value << 0);
			break;

		case SI5351_REG_PLLB_PARAMETERS_0:
			s = " MSNB_P3[15:8] " + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_PLLB_PARAMETERS_1:
			s = " MSNB_P3[ 7:0] " + IntToStr((uint32_t)value << 0);
			break;
		case SI5351_REG_PLLB_PARAMETERS_2:
			s  = " Reserved-" + IntToStr((value >> 2) & 0x03);
			s += "  MSNB_P1[17:16]-" + IntToStr((uint32_t)(value & 0x03) << 16);
			break;
		case SI5351_REG_PLLB_PARAMETERS_3:
			s = " MSNB_P1[15:8] " + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_PLLB_PARAMETERS_4:
			s = " MSNB_P1[ 7:0] " + IntToStr((uint32_t)value << 0);
			break;
		case SI5351_REG_PLLB_PARAMETERS_5:
			s  = " MSNB_P3[19:16]-" + IntToStr((uint32_t)((value >> 4) & 0x0f) << 16);
			s += "  MSNB_P2[19:16]-" + IntToStr((uint32_t)((value >> 0) & 0x0f) << 16);
			break;
		case SI5351_REG_PLLB_PARAMETERS_6:
			s = " MSNB_P2[15:8] " + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_PLLB_PARAMETERS_7:
			s = " MSNB_P2[ 7:0] " + IntToStr((uint32_t)value << 0);
			break;

		case SI5351_REG_MULTISYNTH0_PARAMETERS_1:
			s = " MS0_P3[15:8] " + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH0_PARAMETERS_2:
			s = " MS0_P3[ 7:0] " + IntToStr((uint32_t)value << 0);
			break;
		case SI5351_REG_MULTISYNTH0_PARAMETERS_3:
			s = " R0_DIV[2:0]-" + IntToStr(1u << ((value >> 4) & 0x07));

			s += "  MS0_DIVBY4-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "DIV-N"; break;
				case 1: s += "?????"; break;
				case 2: s += "?????"; break;
				case 3: s += "DIV-4"; break;
			}

			s += "  MS0_P1[17:16]-" + IntToStr((uint32_t)(value & 0x03) << 16);
			break;
		case SI5351_REG_MULTISYNTH0_PARAMETERS_4:
			s = " MS0_P1[15:8]-" + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH0_PARAMETERS_5:
			s = " MS0_P1[ 7:0]-" + IntToStr(value);
			break;
		case SI5351_REG_MULTISYNTH0_PARAMETERS_6:
			s  = " MS0_P3[19:16]-" + IntToStr((uint32_t)((value >> 4) & 0x0f) << 16);
			s += "  MS0_P2[19:16]-" + IntToStr((uint32_t)((value >> 0) & 0x0f) << 16);
			break;
		case SI5351_REG_MULTISYNTH0_PARAMETERS_7:
			s = " MS0_P2[15:8]-" + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH0_PARAMETERS_8:
			s = " MS0_P2[ 7:0]-" + IntToStr(value);
			break;

		case SI5351_REG_MULTISYNTH1_PARAMETERS_1:
			s = " MS1_P3[15:8]-" + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH1_PARAMETERS_2:
			s = " MS1_P3[ 7:0]-" + IntToStr((uint32_t)value << 0);
			break;
		case SI5351_REG_MULTISYNTH1_PARAMETERS_3:
			s = " R1_DIV[2:0]-" + IntToStr(1u << ((value >> 4) & 0x07));

			s += "  MS1_DIVBY4-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "DIV-N"; break;
				case 1: s += "?????"; break;
				case 2: s += "?????"; break;
				case 3: s += "DIV-4"; break;
			}

			s += "  MS1_P1[17:16]-" + IntToStr((uint32_t)(value & 0x03) << 16);
			break;
		case SI5351_REG_MULTISYNTH1_PARAMETERS_4:
			s = " MS1_P1[15:8]-" + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH1_PARAMETERS_5:
			s = " MS1_P1[ 7:0]-" + IntToStr(value);
			break;
		case SI5351_REG_MULTISYNTH1_PARAMETERS_6:
			s  = " MS1_P3[19:16]-" + IntToStr((uint32_t)((value >> 4) & 0x0f) << 16);
			s += "  MS1_P2[19:16]-" + IntToStr((uint32_t)((value >> 0) & 0x0f) << 16);
			break;
		case SI5351_REG_MULTISYNTH1_PARAMETERS_7:
			s = " MS1_P2[15:8]-" + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH1_PARAMETERS_8:
			s = " MS1_P2[ 7:0]-" + IntToStr(value);
			break;

		case SI5351_REG_MULTISYNTH2_PARAMETERS_1:
			s = " MS2_P3[15:8]-" + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH2_PARAMETERS_2:
			s = " MS2_P3[ 7:0]-" + IntToStr((uint32_t)value << 0);
			break;
		case SI5351_REG_MULTISYNTH2_PARAMETERS_3:
			s = " R2_DIV[2:0]-" + IntToStr(1u << ((value >> 4) & 0x07));

			s += "  MS2_DIVBY4-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "DIV-N"; break;
				case 1: s += "?????"; break;
				case 2: s += "?????"; break;
				case 3: s += "DIV-4"; break;
			}

			s += "  MS2_P1[17:16]-" + IntToStr((uint32_t)(value & 0x03) << 16);
			break;
		case SI5351_REG_MULTISYNTH2_PARAMETERS_4:
			s = " MS2_P1[15:8]-" + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH2_PARAMETERS_5:
			s = " MS2_P1[ 7:0]-" + IntToStr(value);
			break;
		case SI5351_REG_MULTISYNTH2_PARAMETERS_6:
			s  = " MS2_P3[19:16]-" + IntToStr((uint32_t)((value >> 4) & 0x0f) << 16);
			s += "  MS2_P2[19:16]-" + IntToStr((uint32_t)((value >> 0) & 0x0f) << 16);
			break;
		case SI5351_REG_MULTISYNTH2_PARAMETERS_7:
			s = " MS2_P2[15:8]-" + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH2_PARAMETERS_8:
			s = " MS2_P2[ 7:0]-" + IntToStr(value);
			break;

		case SI5351_REG_MULTISYNTH3_PARAMETERS_1:
			s = " MS3_P3[15:8]-" + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH3_PARAMETERS_2:
			s = " MS3_P3[ 7:0]-" + IntToStr((uint32_t)value << 0);
			break;
		case SI5351_REG_MULTISYNTH3_PARAMETERS_3:
			s = " R3_DIV[2:0]-" + IntToStr(1u << ((value >> 4) & 0x07));

			s += "  MS3_DIVBY4-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "DIV-N"; break;
				case 1: s += "?????"; break;
				case 2: s += "?????"; break;
				case 3: s += "DIV-4"; break;
			}

			s += "  MS3_P1[17:16]-" + IntToStr((uint32_t)(value & 0x03) << 16);
			break;
		case SI5351_REG_MULTISYNTH3_PARAMETERS_4:
			s = " MS3_P1[15:8]-" + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH3_PARAMETERS_5:
			s = " MS3_P1[ 7:0]-" + IntToStr(value);
			break;
		case SI5351_REG_MULTISYNTH3_PARAMETERS_6:
			s  = " MS3_P3[19:16]-" + IntToStr((uint32_t)((value >> 4) & 0x0f) << 16);
			s += "  MS3_P2[19:16]-" + IntToStr((uint32_t)((value >> 0) & 0x0f) << 16);
			break;
		case SI5351_REG_MULTISYNTH3_PARAMETERS_7:
			s = " MS3_P2[15:8]-" + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH3_PARAMETERS_8:
			s = " MS3_P2[ 7:0]-" + IntToStr(value);
			break;

		case SI5351_REG_MULTISYNTH4_PARAMETERS_1:
			s = " MS4_P3[15:8]-" + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH4_PARAMETERS_2:
			s = " MS4_P3[ 7:0]-" + IntToStr((uint32_t)value << 0);
			break;
		case SI5351_REG_MULTISYNTH4_PARAMETERS_3:
			s = " R4_DIV[2:0]-" + IntToStr(1u << ((value >> 4) & 0x07));

			s += "  MS4_DIVBY4-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "DIV-N"; break;
				case 1: s += "?????"; break;
				case 2: s += "?????"; break;
				case 3: s += "DIV-4"; break;
			}

			s += "  MS4_P1[17:16]-" + IntToStr((uint32_t)(value & 0x03) << 16);
			break;
		case SI5351_REG_MULTISYNTH4_PARAMETERS_4:
			s = " MS4_P1[15:8]-" + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH4_PARAMETERS_5:
			s = " MS4_P1[ 7:0]-" + IntToStr(value);
			break;
		case SI5351_REG_MULTISYNTH4_PARAMETERS_6:
			s  = " MS4_P3[19:16]-" + IntToStr((uint32_t)((value >> 4) & 0x0f) << 16);
			s += "  MS4_P2[19:16]-" + IntToStr((uint32_t)((value >> 0) & 0x0f) << 16);
			break;
		case SI5351_REG_MULTISYNTH4_PARAMETERS_7:
			s = " MS4_P2[15:8]-" + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH4_PARAMETERS_8:
			s = " MS4_P2[ 7:0]-" + IntToStr(value);
			break;

		case SI5351_REG_MULTISYNTH5_PARAMETERS_1:
			s = " MS5_P3[15:8]-" + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH5_PARAMETERS_2:
			s = " MS5_P3[ 7:0]-" + IntToStr((uint32_t)value << 0);
			break;
		case SI5351_REG_MULTISYNTH5_PARAMETERS_3:
			s = " R5_DIV[2:0]-" + IntToStr(1u << ((value >> 4) & 0x07));

			s += "  MS5_DIVBY4-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "DIV-N"; break;
				case 1: s += "?????"; break;
				case 2: s += "?????"; break;
				case 3: s += "DIV-4"; break;
			}

			s += "  MS5_P1[17:16]-" + IntToStr((uint32_t)(value & 0x03) << 16);
			break;
		case SI5351_REG_MULTISYNTH5_PARAMETERS_4:
			s = " MS5_P1[15:8]-" + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH5_PARAMETERS_5:
			s = " MS5_P1[ 7:0]-" + IntToStr(value);
			break;
		case SI5351_REG_MULTISYNTH5_PARAMETERS_6:
			s  = " MS5_P3[19:16]-" + IntToStr((uint32_t)((value >> 4) & 0x0f) << 16);
			s += " MS5_P2[19:16]-" + IntToStr((uint32_t)((value >> 0) & 0x0f) << 16);
			break;
		case SI5351_REG_MULTISYNTH5_PARAMETERS_7:
			s = " MS5_P2[15:8]-" + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH5_PARAMETERS_8:
			s = " MS5_P2[ 7:0]-" + IntToStr(value);
			break;

		case SI5351_REG_MULTISYNTH6_PARAMETERS:
			s = " MS6_P1[ 7:0]-" + IntToStr(value);
			break;

		case SI5351_REG_MULTISYNTH7_PARAMETERS:
			s = " MS7_P1[ 7:0]-" + IntToStr(value);
			break;

		case SI5351_REG_CLOCK_6_7_OUTPUT_DIVIDER:
			s  = (value & 0x80) ? " RESERVED"   : " reserved";
			s += "  R7_DIV[2:0]-" + IntToStr(1u << ((value >> 4) & 0x07));
			s += (value & 0x08) ? "  RESERVED"   : "  reserved";
			s += "  R6_DIV[2:0]-" + IntToStr(1u << ((value >> 0) & 0x07));
			break;

		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_0:
			s  = (value & 0x80) ? " SSC_EN"   : " ssc_en";
			s += "  SSDN_P2[14:8]-" + IntToStr((uint32_t)((value >> 4) & 0x7f) << 8);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_1:
			s = " SSDN_P2[ 7:0]-" + IntToStr(value);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_2:
			s  = (value & 0x80) ? " SSC_MODE-CENTER"   : " ssc_mode-DOWN";
			s += "  SSDN_P3[14:8]-" + IntToStr((uint32_t)((value >> 4) & 0x7f) << 8);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_3:
			s = " SSDN_P3[ 7:0]-" + IntToStr(value);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_4:
			s = " SSDN_P1[ 7:0]-" + IntToStr(value);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_5:
			s  =  " SSUDP[11:8]-"   + IntToStr((uint32_t)((value >> 4) & 0x0f) << 8);
			s += "  SSDN_P1[11:8]-" + IntToStr((uint32_t)((value >> 0) & 0x0f) << 8);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_6:
			s  = " SSUDP[ 7:0]-" + IntToStr(value);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_7:
			s  = " SSUP_P2[14:8]-" + IntToStr((uint32_t)((value >> 4) & 0x7f) << 8);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_8:
			s  = " SSUP_P2[ 7:0]-" + IntToStr(value);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_9:
			s  = " SSUP_P3[14:8]-" + IntToStr((uint32_t)((value >> 4) & 0x7f) << 8);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_10:
			s  = " SSUP_P3[ 7:0]-" + IntToStr(value);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_11:
			s  = " SSUP_P1[ 7:0]-" + IntToStr(value);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_12:
			s  =  " SS_NCLK[ 3:0]-" + IntToStr((uint32_t)((value >> 4) & 0x0f));
			s += "  SSUP_P1[11:8]-" + IntToStr((uint32_t)((value >> 0) & 0x0f) << 8);
			break;

		case SI5351_REG_VCXO_PARAMTER_0:
			s  = " VCXO_Param[ 7:0]-" + IntToStr(value);
			break;
		case SI5351_REG_VCXO_PARAMTER_1:
			s  = " VCXO_Param[15:8]-" + IntToStr((uint32_t)value << 8);
			break;
		case SI5351_REG_VCXO_PARAMTER_2:
			s  = (value & 0x80) ? " RESERVED"   : " reserved";
			s += (value & 0x40) ? " RESERVED"   : " reserved";
			s += "  VCXO_Param[21:16]-" + IntToStr((uint32_t)(value & 0x3f) << 16);
			break;

		case SI5351_REG_CLK0_INITIAL_PHASE_OFFSET:
			s  = (value & 0x80) ? " RESERVED"   : " reserved";
			s += "  CLK0_PHOFF[6:0]-" + IntToStr(value & 0x7f);
			break;
		case SI5351_REG_CLK1_INITIAL_PHASE_OFFSET:
			s  = (value & 0x80) ? " RESERVED"   : " reserved";
			s += "  CLK1_PHOFF[6:0]-" + IntToStr(value & 0x7f);
			break;
		case SI5351_REG_CLK2_INITIAL_PHASE_OFFSET:
			s  = (value & 0x80) ? " RESERVED"   : " reserved";
			s += "  CLK2_PHOFF[6:0]-" + IntToStr(value & 0x7f);
			break;
		case SI5351_REG_CLK3_INITIAL_PHASE_OFFSET:
			s  = (value & 0x80) ? " RESERVED"   : " reserved";
			s += "  CLK3_PHOFF[6:0]-" + IntToStr(value & 0x7f);
			break;
		case SI5351_REG_CLK4_INITIAL_PHASE_OFFSET:
			s  = (value & 0x80) ? " RESERVED"   : " reserved";
			s += "  CLK4_PHOFF[6:0]-" + IntToStr(value & 0x7f);
			break;
		case SI5351_REG_CLK5_INITIAL_PHASE_OFFSET:
			s  = (value & 0x80) ? " RESERVED"   : " reserved";
			s += "  CLK5_PHOFF[6:0]-" + IntToStr(value & 0x7f);
			break;

		case SI5351_REG_PLL_RESET:
			s  = (value & 0x80) ?  " PLLB_RST" : " pllb_rst";
			s += (value & 0x40) ? "  RESERVED" : "  reserved";
			s += (value & 0x20) ? "  PLLA_RST" : "  plla_rst";
			s += (value & 0x10) ? "  RESERVED" : "  reserved";
			s += (value & 0x08) ? "  RESERVED" : "  reserved";
			s += (value & 0x04) ? "  RESERVED" : "  reserved";
			s += (value & 0x02) ? "  RESERVED" : "  reserved";
			s += (value & 0x01) ? "  RESERVED" : "  reserved";
			break;
		case SI5351_REG_CRYSTAL_INTERNAL_LOAD_CAPACITANCE:
			s = " XTAL_CL[1:0]-";
			switch ((value >> 6) & 0x03)
			{
				case 0: s += "RES'D"; break;
				case 1: s += "6pF  "; break;
				case 2: s += "8pF  "; break;
				case 3: s += "10pF "; break;
			}
			s += "  RESERVED-" + binToStr(value & 0x3f, 6);
			break;
		case SI5351_REG_FANOUT:
			s  = (value & 0x80) ? " CLKIN_FANOUT_EN" : " clkin_fanout_en";
			s += (value & 0x40) ? "  XO_FANOUT_EN"   : "  xo_fanout_en";
			s += (value & 0x20) ? "  RESERVED"       : "  reserved";
			s += (value & 0x40) ? "  MS_FANOUT_EN"   : "  ms_fanout_en";
			s += "  RESERVED-" + binToStr(value & 0x0f, 4);
			break;

		default:
			s = "Unknown register";
			break;	}

	return s;
}

void __fastcall TForm1::updateFrequencies()
{
	String  s;

	uint8_t  *reg;
	uint8_t  r_div;
	uint8_t  div_by_4;
	uint32_t p1;
	uint32_t p2;
	uint32_t p3;

	double pll_a_Hz = 0.0;
	double pll_b_Hz = 0.0;

	double clk_0_Hz = 0.0;
	double clk_1_Hz = 0.0;
	double clk_2_Hz = 0.0;

	const uint8_t  clkin_div            = (m_si5351_reg_values[SI5351_REG_PLL_INPUT_SOURCE] >> 6) & 0x03;

	// extract pll data
	const bool     pll_a_src            = (m_si5351_reg_values[SI5351_REG_PLL_INPUT_SOURCE] & 0x40) ? true : false;
	const bool     pll_b_src            = (m_si5351_reg_values[SI5351_REG_PLL_INPUT_SOURCE] & 0x80) ? true : false;
	const bool     pll_a_reset          = (m_si5351_reg_values[SI5351_REG_PLL_RESET] & 0x20) ? true : false;
	const bool     pll_b_reset          = (m_si5351_reg_values[SI5351_REG_PLL_RESET] & 0x80) ? true : false;
	const int      pll_ref_div          = 1u << ((m_si5351_reg_values[SI5351_REG_PLL_INPUT_SOURCE] >> 6) & 0x03);
	const uint32_t pll_ref_Hz           = m_xtal_Hz / pll_ref_div;

	// extract clk-0 data
	const int      clk0_src             = (m_si5351_reg_values[SI5351_REG_CLK0_CONTROL] >> 2) & 0x03;
	const bool     clk0_int_mode        = (m_si5351_reg_values[SI5351_REG_CLK0_CONTROL] & 0x40) ? true : false;
	const bool     clk0_pll             = (m_si5351_reg_values[SI5351_REG_CLK0_CONTROL] & 0x20) ? true : false;
	const bool     clk0_powered_down    = (m_si5351_reg_values[SI5351_REG_CLK0_CONTROL] & 0x80) ? true : false;
	const int      clk0_dis_output_mode = (m_si5351_reg_values[SI5351_REG_CLK3_0_DISABLE_STATE] >> 0) & 0x03;
	const int      clk0_drive_current   = (m_si5351_reg_values[SI5351_REG_CLK0_CONTROL] >> 0) & 0x03;
	const bool     clk0_enabled         = (m_si5351_reg_values[SI5351_REG_OEB_PIN_ENABLE_CONTROL] & 0x01) ? true : (m_si5351_reg_values[SI5351_REG_OUTPUT_ENABLE_CONTROL] & 0x01) ? false : true;

	// extract clk-1 data
	const int      clk1_src             = (m_si5351_reg_values[SI5351_REG_CLK1_CONTROL] >> 2) & 0x03;
	const bool     clk1_int_mode        = (m_si5351_reg_values[SI5351_REG_CLK1_CONTROL] & 0x40) ? true : false;
	const bool     clk1_pll             = (m_si5351_reg_values[SI5351_REG_CLK1_CONTROL] & 0x20) ? true : false;
	const bool     clk1_powered_down    = (m_si5351_reg_values[SI5351_REG_CLK1_CONTROL] & 0x80) ? true : false;
	const int      clk1_dis_output_mode = (m_si5351_reg_values[SI5351_REG_CLK3_0_DISABLE_STATE] >> 2) & 0x03;
	const int      clk1_drive_current   = (m_si5351_reg_values[SI5351_REG_CLK1_CONTROL] >> 0) & 0x03;
	const bool     clk1_enabled         = (m_si5351_reg_values[SI5351_REG_OEB_PIN_ENABLE_CONTROL] & 0x02) ? true : (m_si5351_reg_values[SI5351_REG_OUTPUT_ENABLE_CONTROL] & 0x02) ? false : true;

	// extract clk-2 data
	const int      clk2_src             = (m_si5351_reg_values[SI5351_REG_CLK2_CONTROL] >> 2) & 0x03;
	const bool     clk2_int_mode        = (m_si5351_reg_values[SI5351_REG_CLK2_CONTROL] & 0x40) ? true : false;
	const bool     clk2_pll             = (m_si5351_reg_values[SI5351_REG_CLK2_CONTROL] & 0x20) ? true : false;
	const bool     clk2_powered_down    = (m_si5351_reg_values[SI5351_REG_CLK2_CONTROL] & 0x80) ? true : false;
	const int      clk2_dis_output_mode = (m_si5351_reg_values[SI5351_REG_CLK3_0_DISABLE_STATE] >> 4) & 0x03;
	const int      clk2_drive_current   = (m_si5351_reg_values[SI5351_REG_CLK2_CONTROL] >> 0) & 0x03;
	const bool     clk2_enabled         = (m_si5351_reg_values[SI5351_REG_OEB_PIN_ENABLE_CONTROL] & 0x04) ? true : (m_si5351_reg_values[SI5351_REG_OUTPUT_ENABLE_CONTROL] & 0x04) ? false : true;

	// extract spread spectrum data
	const bool     ss_enabled           = (m_si5351_reg_values[SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_0] & 0x80) ? true : false;
	const bool     ss_center            = (m_si5351_reg_values[SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_2] & 0x80) ? true : false;

	// ******************************
	// calculate PLL-A frequency

	s = "";

	reg = &m_si5351_reg_values[SI5351_REG_PLLA_PARAMETERS_0];
	p1 = ((uint32_t)(reg[2] & 0x03) << 16) | ((uint32_t)reg[3] << 8) | ((uint32_t)reg[4] << 0);
	p2 = ((uint32_t)(reg[5] & 0x0f) << 16) | ((uint32_t)reg[6] << 8) | ((uint32_t)reg[7] << 0);
	p3 = ((uint32_t)(reg[5] & 0xf0) << 12) | ((uint32_t)reg[0] << 8) | ((uint32_t)reg[1] << 0);

	if (p3 > 0)
	{
		pll_a_Hz = (double)pll_ref_Hz * (((double)p1 * p3) + (512.0 * p3) + p2) / (128.0 * p3);

		if (pll_a_src)		// CLKIN as PLL ref
			pll_a_Hz /= 1u << clkin_div;
	}

	if (pll_a_Hz > 0.0)
	{
		if (pll_a_Hz >= 1e6)
			s.printf(" %0.7f MHz", pll_a_Hz / 1e6);
		else
			s.printf(" %0.4f kHz", pll_a_Hz / 1e3);

		s += (m_si5351_reg_values[SI5351_REG_CLK6_CONTROL] & 0x40) ? " INT" : " FRAC";

		s += pll_a_src ? " SRC-CLKIN" : " SRC-XTAL";

		if (pll_a_reset)
			s += " RST";
	}
	else
	{
		s = "---";
	}

	PLLALabel->Caption = s;
	PLLALabel->Update();

	// ******************************
	// spread spectrum
	// this affects PLL-A (not PLL-B)

	if (ss_enabled)
	{
		reg = &m_si5351_reg_values[SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_0];
		const uint16_t ssdn_p1 = ((uint16_t)(reg[ 5] & 0x0f) << 8) | reg[ 4];
		const uint16_t ssdn_p2 = ((uint16_t)(reg[ 0] & 0x7f) << 8) | reg[ 1];
		const uint16_t ssdn_p3 = ((uint16_t)(reg[ 2] & 0x7f) << 8) | reg[ 3];
		const uint16_t ssudp   = ((uint16_t)(reg[ 5] & 0xf0) << 4) | reg[ 6];
		const uint16_t ssup_p1 = ((uint16_t)(reg[12] & 0x0f) << 8) | reg[11];
		const uint16_t ssup_p2 = ((uint16_t)(reg[ 7] & 0x7f) << 8) | reg[ 8];
		const uint16_t ssup_p3 = ((uint16_t)(reg[ 9] & 0x7f) << 8) | reg[10];
		const uint8_t  ss_nclk = (reg[12] >> 4) & 0x0f;

		if (ssudp > 0)
		{
			if (ss_center)
			{	// center spread
				// +-0.1% to +-1.5 in steps of 0.1%
				// spread spectrum rate 30kHz to 33kHz (typ 31.5kHz)

				// TODO:

			}
			else
			{	// down spread
				// -0.1% to -2.5% in steps of 0.1%
				// spread spectrum rate 30kHz to 33kHz (typ 31.5kHz)

				// TODO:

			}
		}
	}

	// ******************************
	// calculate PLL-B frequency

	s = "";

	reg = &m_si5351_reg_values[SI5351_REG_PLLB_PARAMETERS_0];
	p1 = ((uint32_t)(reg[2] & 0x03) << 16) | ((uint32_t)reg[3] << 8) | ((uint32_t)reg[4] << 0);
	p2 = ((uint32_t)(reg[5] & 0x0f) << 16) | ((uint32_t)reg[6] << 8) | ((uint32_t)reg[7] << 0);
	p3 = ((uint32_t)(reg[5] & 0xf0) << 12) | ((uint32_t)reg[0] << 8) | ((uint32_t)reg[1] << 0);

	if (p3 > 0)
	{
		pll_b_Hz = (double)pll_ref_Hz * (((double)p1 * p3) + (512.0 * p3) + p2) / (128.0 * p3);

		if (pll_b_src)		// CLKIN as PLL ref
			pll_b_Hz /= 1u << clkin_div;
	}

	if (pll_b_Hz > 0.0)
	{
		if (pll_b_Hz >= 1e6)
			s.printf(" %0.7f MHz", pll_b_Hz / 1e6);
		else
			s.printf(" %0.4f kHz", pll_b_Hz / 1e3);

		s += (m_si5351_reg_values[SI5351_REG_CLK7_CONTROL] & 0x40) ? " INT" : " FRAC";

		s += pll_b_src ? " SRC-CLKIN" : " SRC-XTAL";

		if (pll_b_reset)
			s += " RST";
	}
	else
	{
		s = "---";
	}

	PLLBLabel->Caption = s;
	PLLBLabel->Update();

	// ******************************
	// VCXO
/*
	reg = &m_si5351_reg_values[SI5351_REG_VCXO_PARAMTER_0];
	const uint32_t vcxo = ((uint32_t)(reg[2] & 0x3f) << 16) | ((uint32_t)reg[1] << 8) | reg[0];

	// TODO:

*/
	// ******************************
	// calculate CLK-0 output frequency

	s = "";

	reg = &m_si5351_reg_values[SI5351_REG_MULTISYNTH0_PARAMETERS_1];
	p1       = ((uint32_t)(reg[2] & 0x03) << 16) | ((uint32_t)reg[3] << 8) | ((uint32_t)reg[4] << 0);
	p2       = ((uint32_t)(reg[5] & 0x0f) << 16) | ((uint32_t)reg[6] << 8) | ((uint32_t)reg[7] << 0);
	p3       = ((uint32_t)(reg[5] & 0xf0) << 12) | ((uint32_t)reg[0] << 8) | ((uint32_t)reg[1] << 0);
	r_div    = (reg[2] >> 4) & 0x07;
	div_by_4 = (reg[2] >> 2) & 0x03;

	if (!clk0_powered_down && (p3 > 0 || div_by_4 == 0x03))
	{
		const double pll_Hz = clk0_pll ? pll_b_Hz : pll_a_Hz;
//		clk_0_Hz = (p1 == 0 && p2 == 0 && p3 == 1 && div_by_4 == 0x03) ? pll_Hz / 4 : (128.0 * p3 * pll_Hz) / (((double)p1 * p3) + p2 + (512.0 * p3));
		clk_0_Hz = (div_by_4 == 0x03) ? pll_Hz / 4 : (128.0 * p3 * pll_Hz) / (((double)p1 * p3) + p2 + (512.0 * p3));
		clk_0_Hz /= 1u << r_div;
	}

	// multisync6-7: fOUT = fIN / P1

	switch (clk0_src)
	{
		case 0:
			s += " SRC-XTAL";
			break;
		case 1:
			s += " SRC-CLKIN";
			break;
		case 2:
			s += " SRC-???";
			break;
		case 3:
			if (clk_0_Hz > 0.0)
			{
				String s2;
				if (clk_0_Hz >= 1e6)
					s2.printf(" %0.7f MHz", clk_0_Hz / 1e6);
				else
					s2.printf(" %0.4f kHz", clk_0_Hz / 1e3);
				s += s2;
				s += clk0_int_mode ? " INT" : " FRAC";
				s += clk0_pll ? " SRC-PLL-B" : " SRC-PLL-A";
			}
			break;
	}

	s += clk0_powered_down ? " Powered Down" : " Powered Up";

	if (clk0_enabled)
	{
		s += " ENABLED";
	}
	else
	{
		switch (clk0_dis_output_mode)
		{
			case 0: s += " LOW"; break;
			case 1: s += " HIGH"; break;
			case 2: s += " FLOAT"; break;
			case 3: s += " ENABLED"; break;
		}
	}

	switch (clk0_drive_current)
	{
		case 0: s += " 2mA"; break;
		case 1: s += " 4mA"; break;
		case 2: s += " 6mA"; break;
		case 3: s += " 8mA"; break;
	}

	Clock0Label->Caption = s;
	Clock0Label->Update();

	// ******************************
	// calculate CLK-1 output frequency

	s = "";

	reg      = &m_si5351_reg_values[SI5351_REG_MULTISYNTH1_PARAMETERS_1];
	p1       = ((uint32_t)(reg[2] & 0x03) << 16) | ((uint32_t)reg[3] << 8) | ((uint32_t)reg[4] << 0);
	p2       = ((uint32_t)(reg[5] & 0x0f) << 16) | ((uint32_t)reg[6] << 8) | ((uint32_t)reg[7] << 0);
	p3       = ((uint32_t)(reg[5] & 0xf0) << 12) | ((uint32_t)reg[0] << 8) | ((uint32_t)reg[1] << 0);
	r_div    = (reg[2] >> 4) & 0x07;
	div_by_4 = (reg[2] >> 2) & 0x03;

	if (!clk1_powered_down && (p3 > 0 || div_by_4 == 0x03))
	{
		const double pll_Hz = clk1_pll ? pll_b_Hz : pll_a_Hz;
//		clk_1_Hz = (p1 == 0 && p2 == 0 && p3 == 1 && div_by_4 == 0x03) ? pll_Hz / 4 : (128.0 * p3 * pll_Hz) / (((double)p1 * p3) + p2 + (512.0 * p3));
		clk_1_Hz = (div_by_4 == 0x03) ? pll_Hz / 4 : (128.0 * p3 * pll_Hz) / (((double)p1 * p3) + p2 + (512.0 * p3));
		clk_1_Hz /= 1u << r_div;
	}

	// multisync6-7: fOUT = fIN / P1

	switch (clk1_src)
	{
		case 0:
			s += " SRC-XTAL";
			break;
		case 1:
			s += " SRC-CLKIN";
			break;
		case 2:
			s += " SRC-???";
			break;
		case 3:
			if (clk_1_Hz > 0.0)
			{
				String s2;
				if (clk_1_Hz >= 1e6)
					s2.printf(" %0.7f MHz", clk_1_Hz / 1e6);
				else
					s2.printf(" %0.4f kHz", clk_1_Hz / 1e3);
				s += s2;
				s += clk1_int_mode ? " INT" : " FRAC";
				s += clk1_pll ? " SRC-PLL-B" : " SRC-PLL-A";
			}
			break;
	}

	s += clk1_powered_down ? " Powered Down" : " Powered Up";

	if (clk1_enabled)
	{
		s += " ENABLED";
	}
	else
	{
		switch (clk1_dis_output_mode)
		{
			case 0: s += " LOW"; break;
			case 1: s += " HIGH"; break;
			case 2: s += " FLOAT"; break;
			case 3: s += " ENABLED"; break;
		}
	}

	switch (clk1_drive_current)
	{
		case 0: s += " 2mA"; break;
		case 1: s += " 4mA"; break;
		case 2: s += " 6mA"; break;
		case 3: s += " 8mA"; break;
	}

	Clock1Label->Caption = s;
	Clock1Label->Update();

	// ******************************
	// calculate CLK-2 output frequency

	s = "";

	reg = &m_si5351_reg_values[SI5351_REG_MULTISYNTH2_PARAMETERS_1];
	p1       = ((uint32_t)(reg[2] & 0x03) << 16) | ((uint32_t)reg[3] << 8) | ((uint32_t)reg[4] << 0);
	p2       = ((uint32_t)(reg[5] & 0x0f) << 16) | ((uint32_t)reg[6] << 8) | ((uint32_t)reg[7] << 0);
	p3       = ((uint32_t)(reg[5] & 0xf0) << 12) | ((uint32_t)reg[0] << 8) | ((uint32_t)reg[1] << 0);
	r_div    = (reg[2] >> 4) & 0x07;
	div_by_4 = (reg[2] >> 2) & 0x03;

	if (!clk2_powered_down && (p3 > 0 || div_by_4 == 0x03))
	{
		const double pll_Hz = clk2_pll ? pll_b_Hz : pll_a_Hz;
//		clk_2_Hz = (p1 == 0 && p2 == 0 && p3 == 1 && div_by_4 == 0x03) ? pll_Hz / 4 : (128.0 * p3 * pll_Hz) / (((double)p1 * p3) + p2 + (512.0 * p3));
		clk_2_Hz = (div_by_4 == 0x03) ? pll_Hz / 4 : (128.0 * p3 * pll_Hz) / (((double)p1 * p3) + p2 + (512.0 * p3));
		clk_2_Hz /= 1u << r_div;
	}

	// multisync6-7: fOUT = fIN / P1

	switch (clk2_src)
	{
		case 0:
			s += " SRC-XTAL";
			break;
		case 1:
			s += " SRC-CLKIN";
			break;
		case 2:
			s += " SRC-???";
			break;
		case 3:
			if (clk_2_Hz > 0.0)
			{
				String s2;
				if (clk_2_Hz >= 1e6)
					s2.printf(" %0.7f MHz", clk_2_Hz / 1e6);
				else
					s2.printf(" %0.4f kHz", clk_2_Hz / 1e3);
				s += s2;
				s += clk2_int_mode ? " INT" : " FRAC";
				s += clk2_pll ? " SRC-PLL-B" : " SRC-PLL-A";
			}
			break;
	}

	s += clk2_powered_down ? " Powered Down" : " Powered Up";

	if (clk2_enabled)
	{
		s += " ENABLED";
	}
	else
	{
		switch (clk2_dis_output_mode)
		{
			case 0: s += " LOW"; break;
			case 1: s += " HIGH"; break;
			case 2: s += " FLOAT"; break;
			case 3: s += " ENABLED"; break;
		}
	}

	switch (clk2_drive_current)
	{
		case 0: s += " 2mA"; break;
		case 1: s += " 4mA"; break;
		case 2: s += " 6mA"; break;
		case 3: s += " 8mA"; break;
	}

	Clock2Label->Caption = s;
	Clock2Label->Update();

	// ******************************
}

void __fastcall TForm1::updateRegisterListView(const bool show_updated)
{
	if (m_closing)
		return;

	// ******************************

	// start with the reset values - maybe
	resetSi5351RegValues();

	bool updated_regs[ARRAY_SIZE(m_si5351_reg_values)];

	memset(&updated_regs[0], 0, sizeof(updated_regs));

	for (unsigned int i = 0; i < m_file_line_reg_values.size(); i++)
	{
		std::vector <uint8_t> &values = m_file_line_reg_values[i];

		memset(&updated_regs[0], 0, sizeof(updated_regs));

		if (!values.empty())
		{
			// clear the PLL self clearing bits
			m_si5351_reg_values[SI5351_REG_PLL_RESET] &= 0x5f;

			int addr = values[0];	// 1st byte is the register start address, following bytes is the register data values
			for (unsigned int k = 1; k < values.size() && addr < ARRAY_SIZE(m_si5351_reg_values); k++)
			{
				updated_regs[addr] = true;
				m_si5351_reg_values[addr++] = values[k];
			}
		}

		if (m_file_line_clicked >= 0 && (int)i >= m_file_line_clicked)
			break;	// stop on the clicked line
	}

	// ******************************
	// update the display

	TListView *lv = RegisterListView;

	if (lv->Items->Count != ARRAY_SIZE(si5351_reg_list))
	{	// new list

//		const int top_item = lv->TopItem  ? lv->TopItem->Index  : -1;

		lv->Items->BeginUpdate();
		lv->Clear();

		for (unsigned int i = 0; i < ARRAY_SIZE(si5351_reg_list); i++)
		{
			const int addr          = si5351_reg_list[i].addr;
//			const uint8_t reset_val = si5351_reg_list[i].reset_value;
			const String name       = String(si5351_reg_list[i].name);

			if (addr >= 0 && addr < ARRAY_SIZE(m_si5351_reg_values))
			{
				const int value = m_si5351_reg_values[addr];

				String s1;
				String s2;
				String s3;

				s1.printf("%4d", addr);

				s2.printf("%02X", value);
//				s2.printf("%02X", reset_val);
				s2 += "  " + binToStr(value, 8);

				s3 = regSettingDescription(addr, value);

				TListItem *item = lv->Items->Add();
				if (item)
				{
					item->Data    = (void *)addr;
					item->Caption = s1;				// register address
					item->SubItems->Add(name);		// register name
					item->SubItems->Add(s2);		// register value
					item->SubItems->Add(s3);		// register setting description
				}
			}
		}
/*
		if (lv->ViewStyle == vsReport)
		{	// move back to the previous scroll position
			if (top_item > 0 && top_item < lv->Items->Count)
			{
				const TRect rect = lv->Items->Item[0]->DisplayRect(drBounds);
				const int item_height = rect.bottom - rect.top;
				lv->Scroll(0, top_item * item_height);
			}
		}
*/
		lv->Items->EndUpdate();
	}
	else
	{	// only update the register values

		for (unsigned int i = 0; i < ARRAY_SIZE(si5351_reg_list); i++)
		{
			const int addr = si5351_reg_list[i].addr;
			if (addr >= 0 && addr < ARRAY_SIZE(m_si5351_reg_values))
			{
				String s1;
				String s2;

				const uint8_t value = m_si5351_reg_values[addr];

				s1.printf("%02X", value);
				s1 += "  " + binToStr(value, 8);

				s2 = regSettingDescription(addr, value);

				if (updated_regs[addr] && show_updated)
					s1 = "* " + s1;

				TListItem *item = lv->Items->Item[i];
				if (item && item->SubItems->Count >= 3 && (int)item->Data == addr)
				{
					item->SubItems->Strings[1] = s1;
					item->SubItems->Strings[2] = s2;
					item->Selected = updated_regs[addr] ? show_updated : false;
				}
			}
		}
	}

//	lv->Repaint();
	lv->Invalidate();

	// ******************************

	updateFrequencies();
}

void __fastcall TForm1::OpenFileButtonClick(TObject *Sender)
{
	selectFile();
}

void __fastcall TForm1::selectFile()
{
	Application->NormalizeTopMosts();
		const bool ok = OpenDialog1->Execute();
	Application->RestoreTopMosts();

	if (!ok)
		return;

	if (loadFile(OpenDialog1->FileName.Trim()))
		updateRegisterListView(false);
}

void __fastcall TForm1::XtalFreqEditChange(TObject *Sender)
{
	double freq = 0.0;

	if (!TryStrToFloat(XtalFreqEdit->Text, freq))
		return;

	if (freq < 0.0)
		return;

	m_xtal_Hz = (uint32_t)floor((freq * 1000000) + 0.5);
}

void __fastcall TForm1::RegisterListViewResize(TObject *Sender)
{
	const int border_3d_width  = GetSystemMetrics(SM_CXEDGE);
	const int border_width     = GetSystemMetrics(SM_CXFIXEDFRAME);
//	const int border_width     = GetSystemMetrics(SM_CXBORDER);
	const int vscroll_width    = GetSystemMetrics(SM_CXVSCROLL);
	const int width            = RegisterListView->Width;

	const int size_coloum = 3;	// the coloum we are going to resize to max

	if (RegisterListView->Columns->Count >= (1 + size_coloum))
	{
		int w = width - (border_width * 2) - (border_3d_width * 2) - vscroll_width;
		for (int k = 0; k < RegisterListView->Columns->Count; k++)
			if (k != size_coloum)
				w -= RegisterListView->Columns->Items[k]->Width;
		if (w < 50)
			w = 50;
		RegisterListView->Columns->Items[size_coloum]->MaxWidth = w;
		RegisterListView->Columns->Items[size_coloum]->MinWidth = w;
		RegisterListView->Columns->Items[size_coloum]->Width    = w;
		RegisterListView->Invalidate();
	}
}

void __fastcall TForm1::FilenameEditClick(TObject *Sender)
{
	selectFile();
}

void __fastcall TForm1::FileListViewSelectItem(TObject *Sender,
		TListItem *Item, bool Selected)
{
	TListView *lv = dynamic_cast<TListView *>(Sender);
	if (!lv || !Item)
		return;

	if (Selected)
	{
		m_file_line_clicked = Item->Index;

		LineLabel->Caption = IntToStr(1 + m_file_line_clicked);
		LineLabel->Update();

//		updateRegisterListView(false);
		updateRegisterListView(true);
	}
	else
	{
		m_file_line_clicked = -1;

		LineLabel->Caption = "";
		LineLabel->Update();

//		updateRegisterListView(false);
	}
}

void __fastcall TForm1::FileListViewClick(TObject *Sender)
{
/*	TListView *lv = dynamic_cast<TListView *>(Sender);
	if (!lv)
		return;

	if (lv->SelCount > 0)
	{
		for (int i = 0; i < lv->Items->Count; i++)
		{
			if (lv->Items->Item[i]->Selected)
			{
				m_file_line_clicked = lv->Items->Item[i]->Index;
				LineLabel->Caption = IntToStr(1 + m_file_line_clicked);
				break;
			}
		}
	}
	else
	{
		m_file_line_clicked = -1;
		LineLabel->Caption = "";
	}

	updateRegisterListView(false);
*/
}

