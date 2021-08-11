
// Si5351 I2C data decoder
//
// Written by Cathy G6AMU
// August 2021

#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QSettings>
#include <QStringList>
#include <QStringListModel>
#include <QTableWidget>
#include <QMessageBox>

#include <stdio.h>
#include <math.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"

// ****************************************************************

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
	char    name[48];
	//QString name;
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
	//SI5351_CRYSTAL_LOAD_0PF  =  0,
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
	setObjectName("MainWindow");	// useful for debugging

	ui->setupUi(this);

	m_shown = false;

	{
		QFileInfo info(QApplication::applicationFilePath());
		m_ini_filename = info.path() + info.baseName() + ".ini";

		// QString QFileInfo::completeBaseName () const
		// Returns file name with shortest extension removed (file.tar.gz -> file.tar)
		//
		// QString QFileInfo::baseName () const
		// Returns file name with longest extension removed (file.tar.gz -> file)
	}

	{
		QString s = QApplication::applicationVersion();
		#ifdef _DEBUG
			s += " debug";
		#endif
		this->setWindowTitle(QApplication::applicationName() + "   v" + s + "   compiled " + __DATE__ + " " + __TIME__);
	}

	ui->FilenameLabel->setText("");

	m_file_line_clicked = -1;

	m_xtal_Hz = 27000000.0;
	ui->RefHzLineEdit->setText("27.0");

	ui->PLLALabel->setText("--");
	ui->PLLBLabel->setText("--");

	ui->Clock0Label->setText("--");
	ui->Clock1Label->setText("--");
	ui->Clock2Label->setText("--");

	ui->LineLabel->setText("");

	ui->FileListView->setSelectionBehavior(QAbstractItemView::SelectRows);

	// *******************************
	// initialize the register display

	ui->RegisterTableWidget->setUpdatesEnabled(false);

	ui->RegisterTableWidget->setRowCount(1 + ARRAY_SIZE(si5351_reg_list));
	//ui->RegisterTableWidget->setColumnCount(4)

	ui->RegisterTableWidget->setColumnWidth(0, 40);
	ui->RegisterTableWidget->setColumnWidth(1, 220);
	ui->RegisterTableWidget->setColumnWidth(2, 100);
	ui->RegisterTableWidget->setColumnWidth(3, 400);

	//ui->RegisterTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QFont font = ui->RegisterTableWidget->font();
    font.setFamily("Consolas");
    font.setPointSize(8);

	for (unsigned int i = 0; i < ARRAY_SIZE(si5351_reg_list); i++)
	{
		const int addr          = si5351_reg_list[i].addr;
		//const uint8_t reset_val = si5351_reg_list[i].reset_value;
		const QString name      = si5351_reg_list[i].name;

		if (addr >= 0 && addr < (int)ARRAY_SIZE(m_si5351_reg_values))
		{
			const int k = 1 + i;

			const int value = m_si5351_reg_values[addr];

			QString s1;
			QString s2;
			QString s3;

			s1.sprintf("%4d  ", addr);

			s2.sprintf("%02X  ", value);
			//s2.printf("%02X", reset_val);
			s2 += QString("%1").arg(value, 8, 2, QChar('0')) + "  ";

			s3 = regSettingDescription(addr, value);

			ui->RegisterTableWidget->setItem(k, 0, new QTableWidgetItem(s1));
			ui->RegisterTableWidget->setItem(k, 1, new QTableWidgetItem(name));
			ui->RegisterTableWidget->setItem(k, 2, new QTableWidgetItem(s2));
			//ui->RegisterTableWidget->setItem(k, 3, new QTableWidgetItem(s3));
			ui->RegisterTableWidget->setItem(k, 3, new QTableWidgetItem("---"));

			//ui->RegisterTableWidget->item(k, 0)->setFont(font);
			//ui->RegisterTableWidget->item(k, 1)->setFont(font);
			//ui->RegisterTableWidget->item(k, 2)->setFont(font);
			//ui->RegisterTableWidget->item(k, 3)->setFont(font);

			ui->RegisterTableWidget->item(k, 0)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui->RegisterTableWidget->item(k, 1)->setTextAlignment(Qt::AlignLeft  | Qt::AlignVCenter);
			ui->RegisterTableWidget->item(k, 2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
			ui->RegisterTableWidget->item(k, 3)->setTextAlignment(Qt::AlignLeft  | Qt::AlignVCenter);

			//ui->RegisterTableWidget->item(k, 0)->setFlags(Qt::ItemIsEnabled);
			//ui->RegisterTableWidget->item(k, 1)->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled);
			//ui->RegisterTableWidget->item(k, 2)->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled);
			//ui->RegisterTableWidget->item(k, 3)->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled);
		}
	}

	ui->RegisterTableWidget->setUpdatesEnabled(true);

	connect(ui->RegisterTableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(onTableWidgetCellSelected(int, int)));

	// ************************

	loadSettings();

	if (!m_filename.isEmpty())
		loadFile(m_filename);

	updateRegisterListView(false);
}

MainWindow::~MainWindow()
{
	saveSettings();

	delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);

	if (!m_shown)
	{
		m_shown = true;

		// ************

		const int h = ui->RefHzLineEdit->height();

		ui->Clock0Label->setMaximumHeight(h);
		ui->Clock1Label->setMaximumHeight(h);
		ui->Clock2Label->setMaximumHeight(h);

		ui->Clock0Label->setMinimumHeight(h);
		ui->Clock1Label->setMinimumHeight(h);
		ui->Clock2Label->setMinimumHeight(h);

		// ************

		sizeRegisterColoumns();
	}
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	QMainWindow::resizeEvent(event);

	sizeRegisterColoumns();
}

void __fastcall MainWindow::sizeRegisterColoumns()
{
	const int width = ui->RegisterTableWidget->width();
	const int size_coloum = 3;	// the coloum we are going to resize to max

	if (ui->RegisterTableWidget->columnCount() >= (1 + size_coloum))
	{
		int w = width;
		for (int k = 0; k < ui->RegisterTableWidget->columnCount(); k++)
			if (k != size_coloum)
				w -= ui->RegisterTableWidget->columnWidth(k);
		if (w < 50)
			w = 50;
		ui->RegisterTableWidget->setColumnWidth(size_coloum, w);
	}
}

void __fastcall MainWindow::selectFile()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open I2C capture file"), QDir::currentPath(), tr("I2C capture (*.txt);;All Files (*)"));
    if (filename.isEmpty())
        return;

    if (loadFile(filename))
        updateRegisterListView(false);
}

void __fastcall MainWindow::loadSettings()
{
	QSettings settings(m_ini_filename, QSettings::IniFormat);

	settings.beginGroup("MainForm");
	{
		const int left   = settings.value("Left",   this->pos().x()).toInt();
		const int top    = settings.value("Top",    this->pos().y()).toInt();
		const int width  = settings.value("Width",  this->size().width()).toInt();
		const int height = settings.value("Height", this->size().height()).toInt();
		this->setGeometry(left, top, width, height);
	}
	settings.endGroup();

	settings.beginGroup("Misc");
	{
		m_filename = settings.value("Filename", m_filename).toString();
		ui->RefHzLineEdit->setText(settings.value("XtalFrequency", ui->RefHzLineEdit->text()).toString());
		ui->splitter->restoreState(settings.value("SplitterPos").toByteArray());
	}
	settings.endGroup();
}

void __fastcall MainWindow::saveSettings()
{
	{	// make sure any left over settings from previous versions are deleted
		QFile file(m_ini_filename);
		//const bool res = file.setPermissions(file.fileName(), QFile::WriteOwner | QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther);
		//if (!res)
		//	return;
		if (file.exists())
			file.remove();
	}

	QSettings settings(m_ini_filename, QSettings::IniFormat);

	settings.beginGroup("MainForm");
	{
		settings.setValue("Left",   this->pos().x());
		settings.setValue("Top",    this->pos().y());
		settings.setValue("Width",  this->size().width());
		settings.setValue("Height", this->size().height());
	}
	settings.endGroup();

	settings.beginGroup("Misc");
	{
		settings.setValue("Filename", m_filename);
		settings.setValue("XtalFrequency", ui->RefHzLineEdit->text());
		settings.setValue("SplitterPos", ui->splitter->saveState());
	}
	settings.endGroup();
}

bool __fastcall MainWindow::loadFile(QString filename)
{
	std::vector <uint8_t> m_file_data;

	//QMutexLocker locker(&file_mutex);

	// ***************************
	// load the file in

	QFile file(filename);

	qDebug(" Loading file (%s) .. ", file.fileName().toLatin1().constData());

	if (!file.exists())
	{
		qDebug("  file not found\n");
		return false;
	}

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QFile::FileError error = file.error();
		QString error_str = file.errorString();
		file.unsetError();
		qDebug("  failed [%d] .. %s\n", error, error_str.toLatin1().constData());
		return false;
	}

	//if (file.handle() < 0)
	//{	// the file is not open
	//}

	qDebug("   reading lines ..");

	QTextStream stream(&file);

	m_parsed_file_lines.resize(0);

	while (!stream.atEnd())
	{
		QString line = stream.readLine().trimmed();
		if (line.isEmpty())
			continue;

		if (line.startsWith('#') || line.startsWith(';'))	// drop comment lines
			continue;

		line = line.replace(QChar('\t'), QChar(' '), Qt::CaseInsensitive).trimmed();	// replace tabs with spaces
		line = line.remove(QChar('\0'), Qt::CaseInsensitive).trimmed();                 // remove any NULL characters from the line

		// convert any multi-spaces to single spaces
		while (!line.isEmpty())
		{
			const int line_len = line.length();
			line = line.replace("  ", QChar(' '), Qt::CaseInsensitive).trimmed();
			if (line.length() == line_len)
				break;
		}

		if (line.isEmpty())
			continue;

		// parse the line up
		QStringList params = line.split(" ");
		if (params.count() < 1)
			continue;

		std::vector <QString> parms;
		for (int i = 0; i < params.size(); i++)
			parms.push_back(params[i]);
		m_parsed_file_lines.push_back(parms);
	}

	file.close();
	qDebug("    done\n");

	// ***************************
	// convert the text values into data values

	resetSi5351RegValues();

	m_file_line_clicked = -1;

	ui->LineLabel->setText("");
	ui->LineLabel->update();

	m_file_line_reg_values.clear();
	m_file_line_reg_values.resize(m_parsed_file_lines.size());

	for (unsigned int i = 0; i < m_parsed_file_lines.size(); i++)
	{
		std::vector <QString> &params = m_parsed_file_lines[i];

		if (params.size() < 2)
			continue;

		QString s = params[0];

		if (s.length() > 1 && s.indexOf('.', 0, Qt::CaseInsensitive) >= 0)
		{
			bool ok = false;
			const float seconds = s.toFloat(&ok);
			if (ok && seconds > 0.0)
			{
				s = params[1];

				if (s.length() < 4)
					continue;

				if (s.left(2).toLower() == "0x")
				{
					bool ok = false;
					const int value = s.toInt(&ok, 16);
					if (ok && value >= 0 && value <= 255)
					{

					}
				}
			}
			continue;
		}

		if (s.length() < 4)
			continue;

		if (s.left(2).toLower() == "0x")
		{
			bool ok = false;
			const int addr = s.toInt(&ok, 16);
			if (!ok || addr < 0 || addr >= (int)ARRAY_SIZE(m_si5351_reg_values))
				continue;

			m_file_line_reg_values[i].push_back(addr);

			for (unsigned int k = 1; k < params.size(); k++)
			{
				if (addr >= (int)ARRAY_SIZE(m_si5351_reg_values))
					break;

				s = params[k];
				if (s.length() < 4)
					continue;

				if (s.left(2).toLower() == "0x")
				{
					bool ok = false;
					const int value = s.toInt(&ok, 16);
					if (ok && value >= 0 && value <= 255)
						m_file_line_reg_values[i].push_back(value);
				}
			}
		}
	}

	// ***************************
	// display the parsed up file lines

	{
		ui->FileListView->setUpdatesEnabled(false);

		QStringList List;
		for (unsigned int i = 0; i < m_parsed_file_lines.size(); i++)
		{
			std::vector <QString> &params = m_parsed_file_lines[i];
			QString s;
			for (unsigned int k = 0; k < params.size(); k++)
				s += " " + params[k];
			List.append(s);
		}

		QStringListModel *model = new QStringListModel(this);
		if (model)
		{
			model->setStringList(List);

			if (ui->FileListView->model())
				if (ui->FileListView->model()->rowCount() > 0)
					ui->FileListView->model()->removeRows(0, ui->FileListView->model()->rowCount());

			ui->FileListView->setModel(model);

			connect(ui->FileListView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)), this, SLOT(onSelectionChanged()));

			//QListView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
		}

		ui->FileListView->setUpdatesEnabled(true);
	}

	// ***************************

	m_filename = !m_parsed_file_lines.empty() ? filename : "";

	ui->FilenameLabel->setText(m_filename);

	return !m_parsed_file_lines.empty();
}

void MainWindow::on_FileOpenPushButton_clicked()
{
	selectFile();
}

void MainWindow::on_RefHzLineEdit_textChanged(const QString &arg1)
{
	bool ok = false;
	double freq = arg1.toDouble(&ok);

	if (!ok)
		return;

	if (freq < 0.0)
		freq = 0.0;

	m_xtal_Hz = freq * 1e6;

	if (!m_filename.isEmpty())
	{	// update the display
		if (ui->FileListView->selectionModel())
		{
			if (ui->FileListView->selectionModel()->hasSelection())
			{
				m_file_line_clicked = ui->FileListView->selectionModel()->currentIndex().row();

				ui->LineLabel->setText(QString::number(1 + m_file_line_clicked));
				ui->LineLabel->update();

				updateRegisterListView(true);
			}
		}
	}
}

void MainWindow::onTableWidgetCellSelected(int nRow, int nCol)
{
	Q_UNUSED(nRow);
	Q_UNUSED(nCol);

	//QMessageBox::information(this, "", "Cell at row " + QString::number(nRow) + " column " + QString::number(nCol) + " was double clicked.");
}

void MainWindow::onSelectionChanged()
{
	if (ui->FileListView->selectionModel())
		if (ui->FileListView->selectionModel()->hasSelection())
			on_FileListView_clicked(ui->FileListView->selectionModel()->currentIndex());
}

void MainWindow::on_FileListView_clicked(const QModelIndex &index)
{
	const int row = index.row();
	if (row < 0)
		return;

	if (!ui->FileListView->selectionModel())
	{
		m_file_line_clicked = -1;

		ui->LineLabel->setText("");
		ui->LineLabel->update();
	}
	else
	if (!ui->FileListView->selectionModel()->hasSelection())
	{
		m_file_line_clicked = -1;

		ui->LineLabel->setText("");
		ui->LineLabel->update();
	}
	else
	{
		m_file_line_clicked = row;

		ui->LineLabel->setText(QString::number(1 + m_file_line_clicked));
		ui->LineLabel->update();

		updateRegisterListView(true);
	}
}

void __fastcall MainWindow::resetSi5351RegValues()
{	// set all the register values to their default reset states
	memset(&m_si5351_reg_values[0], 0, sizeof(m_si5351_reg_values));

	for (unsigned int i = 0; i < ARRAY_SIZE(si5351_reg_list); i++)
	{
		const int addr      = si5351_reg_list[i].addr;
		const uint8_t value = si5351_reg_list[i].reset_value;
		m_si5351_reg_values[addr] = value;
	}
}

QString __fastcall MainWindow::regSettingDescription(const int addr, const uint8_t value)
{
	QString s = "--";

	switch (addr)
	{
		case SI5351_REG_DEVICE_STATUS:
			s  = (value & 0x80) ? " SYS_INIT"   : " sys_init";
			s += (value & 0x40) ? "  LOL_B"     : "  lol_b";
			s += (value & 0x20) ? "  LOL_A"     : "  lol_a";
			s += (value & 0x10) ? "  LOS_CLKIN" : "  los_clkin";
			s += (value & 0x08) ? "  LOS_XTAL"  : "  los_xtal";
			s += (value & 0x04) ? "  RESERVED"  : "  reserved";
			s += "  RevID-" + QString::number(value & 0x03);
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
			s  = " CLKIN_DIV-" + QString::number((value >> 6) & 0x03);
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
			s = " MSNA_P3[15:8] " + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_PLLA_PARAMETERS_1:
			s = " MSNA_P3[ 7:0] " + QString::number((uint32_t)value << 0);
			break;
		case SI5351_REG_PLLA_PARAMETERS_2:
			s  = " Reserved-" + QString::number((value >> 2) & 0x03);
			s += "  MSNA_P1[17:16]-" + QString::number((uint32_t)(value & 0x03) << 16);
			break;
		case SI5351_REG_PLLA_PARAMETERS_3:
			s = " MSNA_P1[15:8] " + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_PLLA_PARAMETERS_4:
			s = " MSNA_P1[ 7:0] " + QString::number((uint32_t)value << 0);
			break;
		case SI5351_REG_PLLA_PARAMETERS_5:
			s  = " MSNA_P3[19:16]-" + QString::number((uint32_t)((value >> 4) & 0x0f) << 16);
			s += "  MSNA_P2[19:16]-" + QString::number((uint32_t)((value >> 0) & 0x0f) << 16);
			break;
		case SI5351_REG_PLLA_PARAMETERS_6:
			s = " MSNA_P2[15:8] " + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_PLLA_PARAMETERS_7:
			s = " MSNA_P2[ 7:0] " + QString::number((uint32_t)value << 0);
			break;

		case SI5351_REG_PLLB_PARAMETERS_0:
			s = " MSNB_P3[15:8] " + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_PLLB_PARAMETERS_1:
			s = " MSNB_P3[ 7:0] " + QString::number((uint32_t)value << 0);
			break;
		case SI5351_REG_PLLB_PARAMETERS_2:
			s  = " Reserved-" + QString::number((value >> 2) & 0x03);
			s += "  MSNB_P1[17:16]-" + QString::number((uint32_t)(value & 0x03) << 16);
			break;
		case SI5351_REG_PLLB_PARAMETERS_3:
			s = " MSNB_P1[15:8] " + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_PLLB_PARAMETERS_4:
			s = " MSNB_P1[ 7:0] " + QString::number((uint32_t)value << 0);
			break;
		case SI5351_REG_PLLB_PARAMETERS_5:
			s  = " MSNB_P3[19:16]-" + QString::number((uint32_t)((value >> 4) & 0x0f) << 16);
			s += "  MSNB_P2[19:16]-" + QString::number((uint32_t)((value >> 0) & 0x0f) << 16);
			break;
		case SI5351_REG_PLLB_PARAMETERS_6:
			s = " MSNB_P2[15:8] " + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_PLLB_PARAMETERS_7:
			s = " MSNB_P2[ 7:0] " + QString::number((uint32_t)value << 0);
			break;

		case SI5351_REG_MULTISYNTH0_PARAMETERS_1:
			s = " MS0_P3[15:8] " + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH0_PARAMETERS_2:
			s = " MS0_P3[ 7:0] " + QString::number((uint32_t)value << 0);
			break;
		case SI5351_REG_MULTISYNTH0_PARAMETERS_3:
			s = " R0_DIV[2:0]-" + QString::number(1u << ((value >> 4) & 0x07));

			s += "  MS0_DIVBY4-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "DIV-N"; break;
				case 1: s += "?????"; break;
				case 2: s += "?????"; break;
				case 3: s += "DIV-4"; break;
			}

			s += "  MS0_P1[17:16]-" + QString::number((uint32_t)(value & 0x03) << 16);
			break;
		case SI5351_REG_MULTISYNTH0_PARAMETERS_4:
			s = " MS0_P1[15:8]-" + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH0_PARAMETERS_5:
			s = " MS0_P1[ 7:0]-" + QString::number(value);
			break;
		case SI5351_REG_MULTISYNTH0_PARAMETERS_6:
			s  = " MS0_P3[19:16]-" + QString::number((uint32_t)((value >> 4) & 0x0f) << 16);
			s += "  MS0_P2[19:16]-" + QString::number((uint32_t)((value >> 0) & 0x0f) << 16);
			break;
		case SI5351_REG_MULTISYNTH0_PARAMETERS_7:
			s = " MS0_P2[15:8]-" + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH0_PARAMETERS_8:
			s = " MS0_P2[ 7:0]-" + QString::number(value);
			break;

		case SI5351_REG_MULTISYNTH1_PARAMETERS_1:
			s = " MS1_P3[15:8]-" + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH1_PARAMETERS_2:
			s = " MS1_P3[ 7:0]-" + QString::number((uint32_t)value << 0);
			break;
		case SI5351_REG_MULTISYNTH1_PARAMETERS_3:
			s = " R1_DIV[2:0]-" + QString::number(1u << ((value >> 4) & 0x07));

			s += "  MS1_DIVBY4-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "DIV-N"; break;
				case 1: s += "?????"; break;
				case 2: s += "?????"; break;
				case 3: s += "DIV-4"; break;
			}

			s += "  MS1_P1[17:16]-" + QString::number((uint32_t)(value & 0x03) << 16);
			break;
		case SI5351_REG_MULTISYNTH1_PARAMETERS_4:
			s = " MS1_P1[15:8]-" + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH1_PARAMETERS_5:
			s = " MS1_P1[ 7:0]-" + QString::number(value);
			break;
		case SI5351_REG_MULTISYNTH1_PARAMETERS_6:
			s  = " MS1_P3[19:16]-" + QString::number((uint32_t)((value >> 4) & 0x0f) << 16);
			s += "  MS1_P2[19:16]-" + QString::number((uint32_t)((value >> 0) & 0x0f) << 16);
			break;
		case SI5351_REG_MULTISYNTH1_PARAMETERS_7:
			s = " MS1_P2[15:8]-" + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH1_PARAMETERS_8:
			s = " MS1_P2[ 7:0]-" + QString::number(value);
			break;

		case SI5351_REG_MULTISYNTH2_PARAMETERS_1:
			s = " MS2_P3[15:8]-" + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH2_PARAMETERS_2:
			s = " MS2_P3[ 7:0]-" + QString::number((uint32_t)value << 0);
			break;
		case SI5351_REG_MULTISYNTH2_PARAMETERS_3:
			s = " R2_DIV[2:0]-" + QString::number(1u << ((value >> 4) & 0x07));

			s += "  MS2_DIVBY4-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "DIV-N"; break;
				case 1: s += "?????"; break;
				case 2: s += "?????"; break;
				case 3: s += "DIV-4"; break;
			}

			s += "  MS2_P1[17:16]-" + QString::number((uint32_t)(value & 0x03) << 16);
			break;
		case SI5351_REG_MULTISYNTH2_PARAMETERS_4:
			s = " MS2_P1[15:8]-" + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH2_PARAMETERS_5:
			s = " MS2_P1[ 7:0]-" + QString::number(value);
			break;
		case SI5351_REG_MULTISYNTH2_PARAMETERS_6:
			s  = " MS2_P3[19:16]-" + QString::number((uint32_t)((value >> 4) & 0x0f) << 16);
			s += "  MS2_P2[19:16]-" + QString::number((uint32_t)((value >> 0) & 0x0f) << 16);
			break;
		case SI5351_REG_MULTISYNTH2_PARAMETERS_7:
			s = " MS2_P2[15:8]-" + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH2_PARAMETERS_8:
			s = " MS2_P2[ 7:0]-" + QString::number(value);
			break;

		case SI5351_REG_MULTISYNTH3_PARAMETERS_1:
			s = " MS3_P3[15:8]-" + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH3_PARAMETERS_2:
			s = " MS3_P3[ 7:0]-" + QString::number((uint32_t)value << 0);
			break;
		case SI5351_REG_MULTISYNTH3_PARAMETERS_3:
			s = " R3_DIV[2:0]-" + QString::number(1u << ((value >> 4) & 0x07));

			s += "  MS3_DIVBY4-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "DIV-N"; break;
				case 1: s += "?????"; break;
				case 2: s += "?????"; break;
				case 3: s += "DIV-4"; break;
			}

			s += "  MS3_P1[17:16]-" + QString::number((uint32_t)(value & 0x03) << 16);
			break;
		case SI5351_REG_MULTISYNTH3_PARAMETERS_4:
			s = " MS3_P1[15:8]-" + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH3_PARAMETERS_5:
			s = " MS3_P1[ 7:0]-" + QString::number(value);
			break;
		case SI5351_REG_MULTISYNTH3_PARAMETERS_6:
			s  = " MS3_P3[19:16]-" + QString::number((uint32_t)((value >> 4) & 0x0f) << 16);
			s += "  MS3_P2[19:16]-" + QString::number((uint32_t)((value >> 0) & 0x0f) << 16);
			break;
		case SI5351_REG_MULTISYNTH3_PARAMETERS_7:
			s = " MS3_P2[15:8]-" + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH3_PARAMETERS_8:
			s = " MS3_P2[ 7:0]-" + QString::number(value);
			break;

		case SI5351_REG_MULTISYNTH4_PARAMETERS_1:
			s = " MS4_P3[15:8]-" + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH4_PARAMETERS_2:
			s = " MS4_P3[ 7:0]-" + QString::number((uint32_t)value << 0);
			break;
		case SI5351_REG_MULTISYNTH4_PARAMETERS_3:
			s = " R4_DIV[2:0]-" + QString::number(1u << ((value >> 4) & 0x07));

			s += "  MS4_DIVBY4-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "DIV-N"; break;
				case 1: s += "?????"; break;
				case 2: s += "?????"; break;
				case 3: s += "DIV-4"; break;
			}

			s += "  MS4_P1[17:16]-" + QString::number((uint32_t)(value & 0x03) << 16);
			break;
		case SI5351_REG_MULTISYNTH4_PARAMETERS_4:
			s = " MS4_P1[15:8]-" + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH4_PARAMETERS_5:
			s = " MS4_P1[ 7:0]-" + QString::number(value);
			break;
		case SI5351_REG_MULTISYNTH4_PARAMETERS_6:
			s  = " MS4_P3[19:16]-" + QString::number((uint32_t)((value >> 4) & 0x0f) << 16);
			s += "  MS4_P2[19:16]-" + QString::number((uint32_t)((value >> 0) & 0x0f) << 16);
			break;
		case SI5351_REG_MULTISYNTH4_PARAMETERS_7:
			s = " MS4_P2[15:8]-" + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH4_PARAMETERS_8:
			s = " MS4_P2[ 7:0]-" + QString::number(value);
			break;

		case SI5351_REG_MULTISYNTH5_PARAMETERS_1:
			s = " MS5_P3[15:8]-" + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH5_PARAMETERS_2:
			s = " MS5_P3[ 7:0]-" + QString::number((uint32_t)value << 0);
			break;
		case SI5351_REG_MULTISYNTH5_PARAMETERS_3:
			s = " R5_DIV[2:0]-" + QString::number(1u << ((value >> 4) & 0x07));

			s += "  MS5_DIVBY4-";
			switch ((value >> 2) & 0x03)
			{
				case 0: s += "DIV-N"; break;
				case 1: s += "?????"; break;
				case 2: s += "?????"; break;
				case 3: s += "DIV-4"; break;
			}

			s += "  MS5_P1[17:16]-" + QString::number((uint32_t)(value & 0x03) << 16);
			break;
		case SI5351_REG_MULTISYNTH5_PARAMETERS_4:
			s = " MS5_P1[15:8]-" + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH5_PARAMETERS_5:
			s = " MS5_P1[ 7:0]-" + QString::number(value);
			break;
		case SI5351_REG_MULTISYNTH5_PARAMETERS_6:
			s  = " MS5_P3[19:16]-" + QString::number((uint32_t)((value >> 4) & 0x0f) << 16);
			s += " MS5_P2[19:16]-" + QString::number((uint32_t)((value >> 0) & 0x0f) << 16);
			break;
		case SI5351_REG_MULTISYNTH5_PARAMETERS_7:
			s = " MS5_P2[15:8]-" + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_MULTISYNTH5_PARAMETERS_8:
			s = " MS5_P2[ 7:0]-" + QString::number(value);
			break;

		case SI5351_REG_MULTISYNTH6_PARAMETERS:
			s = " MS6_P1[ 7:0]-" + QString::number(value);
			break;

		case SI5351_REG_MULTISYNTH7_PARAMETERS:
			s = " MS7_P1[ 7:0]-" + QString::number(value);
			break;

		case SI5351_REG_CLOCK_6_7_OUTPUT_DIVIDER:
			s  = (value & 0x80) ? " RESERVED"   : " reserved";
			s += "  R7_DIV[2:0]-" + QString::number(1u << ((value >> 4) & 0x07));
			s += (value & 0x08) ? "  RESERVED"   : "  reserved";
			s += "  R6_DIV[2:0]-" + QString::number(1u << ((value >> 0) & 0x07));
			break;

		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_0:
			s  = (value & 0x80) ? " SSC_EN"   : " ssc_en";
			s += "  SSDN_P2[14:8]-" + QString::number((uint32_t)(value & 0x7f) << 8);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_1:
			s = " SSDN_P2[ 7:0]-" + QString::number(value);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_2:
			s  = (value & 0x80) ? " SSC_MODE-CENTER"   : " ssc_mode-DOWN";
			s += "  SSDN_P3[14:8]-" + QString::number((uint32_t)(value & 0x7f) << 8);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_3:
			s = " SSDN_P3[ 7:0]-" + QString::number(value);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_4:
			s = " SSDN_P1[ 7:0]-" + QString::number(value);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_5:
			s  =  " SSUDP[11:8]-"   + QString::number((uint32_t)((value >> 4) & 0x0f) << 8);
			s += "  SSDN_P1[11:8]-" + QString::number((uint32_t)((value >> 0) & 0x0f) << 8);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_6:
			s  = " SSUDP[ 7:0]-" + QString::number(value);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_7:
			s  = " SSUP_P2[14:8]-" + QString::number((uint32_t)(value & 0x7f) << 8);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_8:
			s  = " SSUP_P2[ 7:0]-" + QString::number(value);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_9:
			s  = " SSUP_P3[14:8]-" + QString::number((uint32_t)(value & 0x7f) << 8);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_10:
			s  = " SSUP_P3[ 7:0]-" + QString::number(value);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_11:
			s  = " SSUP_P1[ 7:0]-" + QString::number(value);
			break;
		case SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_12:
			s  =  " SS_NCLK[ 3:0]-" + QString::number((uint32_t)((value >> 4) & 0x0f));
			s += "  SSUP_P1[11:8]-" + QString::number((uint32_t)((value >> 0) & 0x0f) << 8);
			break;

		case SI5351_REG_VCXO_PARAMTER_0:
			s  = " VCXO_Param[ 7:0]-" + QString::number(value);
			break;
		case SI5351_REG_VCXO_PARAMTER_1:
			s  = " VCXO_Param[15:8]-" + QString::number((uint32_t)value << 8);
			break;
		case SI5351_REG_VCXO_PARAMTER_2:
			s  = (value & 0x80) ? " RESERVED"   : " reserved";
			s += (value & 0x40) ? " RESERVED"   : " reserved";
			s += "  VCXO_Param[21:16]-" + QString::number((uint32_t)(value & 0x3f) << 16);
			break;

		case SI5351_REG_CLK0_INITIAL_PHASE_OFFSET:
			s  = (value & 0x80) ? " RESERVED"   : " reserved";
			s += "  CLK0_PHOFF[6:0]-" + QString::number(value & 0x7f);
			break;
		case SI5351_REG_CLK1_INITIAL_PHASE_OFFSET:
			s  = (value & 0x80) ? " RESERVED"   : " reserved";
			s += "  CLK1_PHOFF[6:0]-" + QString::number(value & 0x7f);
			break;
		case SI5351_REG_CLK2_INITIAL_PHASE_OFFSET:
			s  = (value & 0x80) ? " RESERVED"   : " reserved";
			s += "  CLK2_PHOFF[6:0]-" + QString::number(value & 0x7f);
			break;
		case SI5351_REG_CLK3_INITIAL_PHASE_OFFSET:
			s  = (value & 0x80) ? " RESERVED"   : " reserved";
			s += "  CLK3_PHOFF[6:0]-" + QString::number(value & 0x7f);
			break;
		case SI5351_REG_CLK4_INITIAL_PHASE_OFFSET:
			s  = (value & 0x80) ? " RESERVED"   : " reserved";
			s += "  CLK4_PHOFF[6:0]-" + QString::number(value & 0x7f);
			break;
		case SI5351_REG_CLK5_INITIAL_PHASE_OFFSET:
			s  = (value & 0x80) ? " RESERVED"   : " reserved";
			s += "  CLK5_PHOFF[6:0]-" + QString::number(value & 0x7f);
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
			s += "  RESERVED-" + QString("%1").arg(value & 0x3f, 6, 2, QChar('0'));
			break;
		case SI5351_REG_FANOUT:
			s  = (value & 0x80) ? " CLKIN_FANOUT_EN" : " clkin_fanout_en";
			s += (value & 0x40) ? "  XO_FANOUT_EN"   : "  xo_fanout_en";
			s += (value & 0x20) ? "  RESERVED"       : "  reserved";
			s += (value & 0x40) ? "  MS_FANOUT_EN"   : "  ms_fanout_en";
			s += "  RESERVED-" + QString("%1").arg(value & 0x0f, 4, 2, QChar('0'));
			break;

		default:
			s = "Unknown register";
			break;
	}

	return s;
}

void __fastcall MainWindow::updateFrequencies()
{
	QString  s;
	QString  s2;

	uint8_t  *reg;

	uint8_t  ms0_r_div;
	uint8_t  ms0_div_by_4;

	uint8_t  ms1_r_div;
	uint8_t  ms1_div_by_4;

	uint8_t  ms2_r_div;
	uint8_t  ms2_div_by_4;

	uint32_t plla_p1;
	uint32_t plla_p2;
	uint32_t plla_p3;
					
	uint32_t pllb_p1;
	uint32_t pllb_p2;
	uint32_t pllb_p3;

	uint32_t ms0_p1 = 0;
	uint32_t ms0_p2 = 0;
	uint32_t ms0_p3 = 0;

	uint32_t ms1_p1 = 0;
	uint32_t ms1_p2 = 0;
	uint32_t ms1_p3 = 0;

	uint32_t ms2_p1 = 0;
	uint32_t ms2_p2 = 0;
	uint32_t ms2_p3 = 0;

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
	const double   pll_ref_Hz           = m_xtal_Hz;

	reg     = &m_si5351_reg_values[SI5351_REG_PLLA_PARAMETERS_0];
	plla_p1 = ((uint32_t)(reg[2] & 0x03) << 16) | ((uint32_t)reg[3] << 8) | ((uint32_t)reg[4] << 0);
	plla_p2 = ((uint32_t)(reg[5] & 0x0f) << 16) | ((uint32_t)reg[6] << 8) | ((uint32_t)reg[7] << 0);
	plla_p3 = ((uint32_t)(reg[5] & 0xf0) << 12) | ((uint32_t)reg[0] << 8) | ((uint32_t)reg[1] << 0);

	reg     = &m_si5351_reg_values[SI5351_REG_PLLB_PARAMETERS_0];
	pllb_p1 = ((uint32_t)(reg[2] & 0x03) << 16) | ((uint32_t)reg[3] << 8) | ((uint32_t)reg[4] << 0);
	pllb_p2 = ((uint32_t)(reg[5] & 0x0f) << 16) | ((uint32_t)reg[6] << 8) | ((uint32_t)reg[7] << 0);
	pllb_p3 = ((uint32_t)(reg[5] & 0xf0) << 12) | ((uint32_t)reg[0] << 8) | ((uint32_t)reg[1] << 0);

	// extract clk-0 data
	const int      clk0_src             = (m_si5351_reg_values[SI5351_REG_CLK0_CONTROL] >> 2) & 0x03;
	const bool     clk0_int_mode        = (m_si5351_reg_values[SI5351_REG_CLK0_CONTROL] & 0x40) ? true : false;
	const bool     clk0_pll             = (m_si5351_reg_values[SI5351_REG_CLK0_CONTROL] & 0x20) ? true : false;
	const bool     clk0_powered_down    = (m_si5351_reg_values[SI5351_REG_CLK0_CONTROL] & 0x80) ? true : false;
	const int      clk0_dis_output_mode = (m_si5351_reg_values[SI5351_REG_CLK3_0_DISABLE_STATE] >> 0) & 0x03;
	const int      clk0_drive_current   = (m_si5351_reg_values[SI5351_REG_CLK0_CONTROL] >> 0) & 0x03;
	const bool     clk0_inv             = (m_si5351_reg_values[SI5351_REG_CLK0_CONTROL] & 0x10) ? true : false;
	const bool     clk0_enabled         = (m_si5351_reg_values[SI5351_REG_OEB_PIN_ENABLE_CONTROL] & 0x01) ? true : (m_si5351_reg_values[SI5351_REG_OUTPUT_ENABLE_CONTROL] & 0x01) ? false : true;

	reg = &m_si5351_reg_values[SI5351_REG_MULTISYNTH0_PARAMETERS_1];
	ms0_p1       = ((uint32_t)(reg[2] & 0x03) << 16) | ((uint32_t)reg[3] << 8) | ((uint32_t)reg[4] << 0);
	ms0_p2       = ((uint32_t)(reg[5] & 0x0f) << 16) | ((uint32_t)reg[6] << 8) | ((uint32_t)reg[7] << 0);
	ms0_p3       = ((uint32_t)(reg[5] & 0xf0) << 12) | ((uint32_t)reg[0] << 8) | ((uint32_t)reg[1] << 0);
	ms0_r_div    = (reg[2] >> 4) & 0x07;
	ms0_div_by_4 = (reg[2] >> 2) & 0x03;

	// extract clk-1 data
	const int      clk1_src             = (m_si5351_reg_values[SI5351_REG_CLK1_CONTROL] >> 2) & 0x03;
	const bool     clk1_int_mode        = (m_si5351_reg_values[SI5351_REG_CLK1_CONTROL] & 0x40) ? true : false;
	const bool     clk1_pll             = (m_si5351_reg_values[SI5351_REG_CLK1_CONTROL] & 0x20) ? true : false;
	const bool     clk1_powered_down    = (m_si5351_reg_values[SI5351_REG_CLK1_CONTROL] & 0x80) ? true : false;
	const int      clk1_dis_output_mode = (m_si5351_reg_values[SI5351_REG_CLK3_0_DISABLE_STATE] >> 2) & 0x03;
	const int      clk1_drive_current   = (m_si5351_reg_values[SI5351_REG_CLK1_CONTROL] >> 0) & 0x03;
	const bool     clk1_inv             = (m_si5351_reg_values[SI5351_REG_CLK1_CONTROL] & 0x10) ? true : false;
	const bool     clk1_enabled         = (m_si5351_reg_values[SI5351_REG_OEB_PIN_ENABLE_CONTROL] & 0x02) ? true : (m_si5351_reg_values[SI5351_REG_OUTPUT_ENABLE_CONTROL] & 0x02) ? false : true;

	reg          = &m_si5351_reg_values[SI5351_REG_MULTISYNTH1_PARAMETERS_1];
	ms1_p1       = ((uint32_t)(reg[2] & 0x03) << 16) | ((uint32_t)reg[3] << 8) | ((uint32_t)reg[4] << 0);
	ms1_p2       = ((uint32_t)(reg[5] & 0x0f) << 16) | ((uint32_t)reg[6] << 8) | ((uint32_t)reg[7] << 0);
	ms1_p3       = ((uint32_t)(reg[5] & 0xf0) << 12) | ((uint32_t)reg[0] << 8) | ((uint32_t)reg[1] << 0);
	ms1_r_div    = (reg[2] >> 4) & 0x07;
	ms1_div_by_4 = (reg[2] >> 2) & 0x03;

	// extract clk-2 data
	const int      clk2_src             = (m_si5351_reg_values[SI5351_REG_CLK2_CONTROL] >> 2) & 0x03;
	const bool     clk2_int_mode        = (m_si5351_reg_values[SI5351_REG_CLK2_CONTROL] & 0x40) ? true : false;
	const bool     clk2_pll             = (m_si5351_reg_values[SI5351_REG_CLK2_CONTROL] & 0x20) ? true : false;
	const bool     clk2_powered_down    = (m_si5351_reg_values[SI5351_REG_CLK2_CONTROL] & 0x80) ? true : false;
	const int      clk2_dis_output_mode = (m_si5351_reg_values[SI5351_REG_CLK3_0_DISABLE_STATE] >> 4) & 0x03;
	const int      clk2_drive_current   = (m_si5351_reg_values[SI5351_REG_CLK2_CONTROL] >> 0) & 0x03;
	const bool     clk2_inv             = (m_si5351_reg_values[SI5351_REG_CLK2_CONTROL] & 0x10) ? true : false;
	const bool     clk2_enabled         = (m_si5351_reg_values[SI5351_REG_OEB_PIN_ENABLE_CONTROL] & 0x04) ? true : (m_si5351_reg_values[SI5351_REG_OUTPUT_ENABLE_CONTROL] & 0x04) ? false : true;

	reg          = &m_si5351_reg_values[SI5351_REG_MULTISYNTH2_PARAMETERS_1];
	ms2_p1       = ((uint32_t)(reg[2] & 0x03) << 16) | ((uint32_t)reg[3] << 8) | ((uint32_t)reg[4] << 0);
	ms2_p2       = ((uint32_t)(reg[5] & 0x0f) << 16) | ((uint32_t)reg[6] << 8) | ((uint32_t)reg[7] << 0);
	ms2_p3       = ((uint32_t)(reg[5] & 0xf0) << 12) | ((uint32_t)reg[0] << 8) | ((uint32_t)reg[1] << 0);
	ms2_r_div    = (reg[2] >> 4) & 0x07;
	ms2_div_by_4 = (reg[2] >> 2) & 0x03;

	// extract spread spectrum data
	//const bool   ss_enabled           = (m_si5351_reg_values[SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_0] & 0x80) ? true : false;
	//const bool   ss_center            = (m_si5351_reg_values[SI5351_REG_SPREAD_SPECTRUM_PARAMETERS_2] & 0x80) ? true : false;

	// ******************************
	// calculate PLL-A frequency

	s = "";

	if (plla_p3 > 0)
	{
		const double ref_Hz = (pll_a_src) ? pll_ref_Hz / (1u << clkin_div) : pll_ref_Hz;	// CLKIN/XTAL
		pll_a_Hz = ref_Hz * (((double)plla_p1 * plla_p3) + (512.0 * plla_p3) + plla_p2) / (128.0 * plla_p3);
	}

	s += pll_a_src ? " SRC-CLKIN" : " SRC-XTAL";

	s += (m_si5351_reg_values[SI5351_REG_CLK6_CONTROL] & 0x40) ? " INT " : " FRAC";

	if (pll_a_Hz > 0.0)
	{
		if (pll_a_Hz >= 1e6)
			s2.sprintf(" %0.7f MHz", pll_a_Hz / 1e6);
		else
			s2.sprintf(" %0.4f kHz", pll_a_Hz / 1e3);
		s += s2;

		if (pll_a_reset)
			s += " RST";
	}

	ui->PLLALabel->setText(s);
	ui->PLLALabel->update();

	// ******************************
	// calculate PLL-B frequency

	s = "";

	if (pllb_p3 > 0)
	{
		const double ref_Hz = (pll_b_src) ? pll_ref_Hz / (1u << clkin_div) : pll_ref_Hz;	// CLKIN/XTAL
		pll_b_Hz = ref_Hz * (((double)pllb_p1 * pllb_p3) + (512.0 * pllb_p3) + pllb_p2) / (128.0 * pllb_p3);
	}

	s += pll_b_src ? " SRC-CLKIN" : " SRC-XTAL";

	s += (m_si5351_reg_values[SI5351_REG_CLK7_CONTROL] & 0x40) ? " INT " : " FRAC";

	if (pll_b_Hz > 0.0)
	{
		if (pll_b_Hz >= 1e6)
			s2.sprintf(" %0.7f MHz", pll_b_Hz / 1e6);
		else
			s2.sprintf(" %0.4f kHz", pll_b_Hz / 1e3);
		s += s2;

		if (pll_b_reset)
			s += " RST";
	}

	ui->PLLBLabel->setText(s);
	ui->PLLBLabel->update();

	// ******************************
	// spread spectrum
	// this affects PLL-A (not PLL-B)
/*
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
			const double pfd_Hz = (pll_a_src) ? pll_ref_Hz / (1u << clkin_div) : pll_ref_Hz;	// CLKIN/XTAL
			const double pll_div = (((double)plla_p1 * plla_p3) + (512.0 * plla_p3) + plla_p2) / (128.0 * plla_p3);	// a + (b / c)

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
*/
	// ******************************
	// VCXO

	//	reg = &m_si5351_reg_values[SI5351_REG_VCXO_PARAMTER_0];
	//	const uint32_t vcxo = ((uint32_t)(reg[2] & 0x3f) << 16) | ((uint32_t)reg[1] << 8) | reg[0];
	//
	//	// TODO:

	// ******************************
	// calculate CLK-0 output frequency

	s = "";

	if (!clk0_powered_down && clk0_enabled)
	{
		const double pll_Hz = clk0_pll ? pll_b_Hz : pll_a_Hz;
		switch (clk0_src)
		{
			case 0:	// XTAL
				clk_0_Hz = m_xtal_Hz;
				break;
			case 1:	// CLK-IN
				clk_0_Hz = m_xtal_Hz;
				break;
			case 2:	// reserved
				break;
			case 3:	// MS0
				if (ms0_p3 > 0 || ms0_div_by_4 == 0x03)
					clk_0_Hz = (ms0_div_by_4 == 0x03) ? pll_Hz / 4 : (128.0 * ms0_p3 * pll_Hz) / (((double)ms0_p1 * ms0_p3) + ms0_p2 + (512.0 * ms0_p3));
				break;
		}
		clk_0_Hz /= 1u << ms0_r_div;
	}

	s += clk0_powered_down ? " PWR-DN" : " PWR-UP";

	switch (clk0_src)
	{
		case 0: s += " SRC-XTAL "; break;
		case 1: s += " SRC-CLKIN"; break;
		case 2: s += " SRC-???  "; break;
		case 3: s += " SRC-MS0  "; break;
	}

	s += clk0_pll ? " PLL-B" : " PLL-A";

	switch (clk0_drive_current)
	{
		case 0: s += " 2mA"; break;
		case 1: s += " 4mA"; break;
		case 2: s += " 6mA"; break;
		case 3: s += " 8mA"; break;
	}

	s += clk0_int_mode ? " INT " : " FRAC";

	if (clk0_enabled)
	{
//		s += " ENABLED";
	}
	else
	{
		switch (clk0_dis_output_mode)
		{
			case 0: s += " LOW    "; break;
			case 1: s += " HIGH   "; break;
			case 2: s += " HIGH-Z "; break;
//			case 3: s += " ENABLED"; break;
		}
	}

	if (clk_0_Hz > 0.0 && (clk0_enabled || clk0_dis_output_mode == 3))
	{
		if (clk_0_Hz >= 1e6)
			s2.sprintf(" %0.7f MHz", clk_0_Hz / 1e6);
		else
			s2.sprintf(" %0.4f kHz", clk_0_Hz / 1e3);
		s += s2;
	}

	if (clk0_inv)
		s += " INV";

	ui->Clock0Label->setText(s);
	ui->Clock0Label->update();

	// ******************************
	// calculate CLK-1 output frequency

	s = "";

	reg      = &m_si5351_reg_values[SI5351_REG_MULTISYNTH1_PARAMETERS_1];

	ms1_p1       = ((uint32_t)(reg[2] & 0x03) << 16) | ((uint32_t)reg[3] << 8) | ((uint32_t)reg[4] << 0);
	ms1_p2       = ((uint32_t)(reg[5] & 0x0f) << 16) | ((uint32_t)reg[6] << 8) | ((uint32_t)reg[7] << 0);
	ms1_p3       = ((uint32_t)(reg[5] & 0xf0) << 12) | ((uint32_t)reg[0] << 8) | ((uint32_t)reg[1] << 0);
	ms1_r_div    = (reg[2] >> 4) & 0x07;
	ms1_div_by_4 = (reg[2] >> 2) & 0x03;

	if (!clk1_powered_down && clk1_enabled)
	{
		const double pll_Hz = clk1_pll ? pll_b_Hz : pll_a_Hz;
		switch (clk1_src)
		{
			case 0:	// XTAL
				clk_1_Hz = m_xtal_Hz;
				break;
			case 1:	// CLK-IN
				clk_1_Hz = m_xtal_Hz;
				break;
			case 2:	// MS0
				if (ms0_p3 > 0 || ms0_div_by_4 == 0x03)
					clk_1_Hz = (ms0_div_by_4 == 0x03) ? pll_Hz / 4 : (128.0 * ms0_p3 * pll_Hz) / (((double)ms0_p1 * ms0_p3) + ms0_p2 + (512.0 * ms0_p3));
				break;
			case 3:	// MS1
				if (ms1_p3 > 0 || ms1_div_by_4 == 0x03)
					clk_1_Hz = (ms1_div_by_4 == 0x03) ? pll_Hz / 4 : (128.0 * ms1_p3 * pll_Hz) / (((double)ms1_p1 * ms1_p3) + ms1_p2 + (512.0 * ms1_p3));
				break;
		}
		clk_1_Hz /= 1u << ms1_r_div;
	}

	s += clk1_powered_down ? " PWR-DN" : " PWR-UP";

	switch (clk1_src)
	{
		case 0: s += " SRC-XTAL "; break;
		case 1: s += " SRC-CLKIN"; break;
		case 2: s += " SRC-MS0  "; break;
		case 3: s += " SRC-MS1  "; break;
	}

	s += clk1_pll ? " PLL-B" : " PLL-A";

	switch (clk1_drive_current)
	{
		case 0: s += " 2mA"; break;
		case 1: s += " 4mA"; break;
		case 2: s += " 6mA"; break;
		case 3: s += " 8mA"; break;
	}

	s += clk1_int_mode ? " INT " : " FRAC";

	if (clk1_enabled)
	{
//		s += " ENABLED";
	}
	else
	{
		switch (clk1_dis_output_mode)
		{
			case 0: s += " LOW    "; break;
			case 1: s += " HIGH   "; break;
			case 2: s += " HIGH-Z "; break;
//			case 3: s += " ENABLED"; break;
		}
	}

	if (clk_1_Hz > 0.0 && (clk1_enabled || clk1_dis_output_mode == 3))
	{
		if (clk_1_Hz >= 1e6)
			s2.sprintf(" %0.7f MHz", clk_1_Hz / 1e6);
		else
			s2.sprintf(" %0.4f kHz", clk_1_Hz / 1e3);
		s += s2;
	}

	if (clk1_inv)
		s += " INV";

	ui->Clock1Label->setText(s);
	ui->Clock1Label->update();

	// ******************************
	// calculate CLK-2 output frequency

	s = "";

	reg = &m_si5351_reg_values[SI5351_REG_MULTISYNTH2_PARAMETERS_1];

	ms2_p1       = ((uint32_t)(reg[2] & 0x03) << 16) | ((uint32_t)reg[3] << 8) | ((uint32_t)reg[4] << 0);
	ms2_p2       = ((uint32_t)(reg[5] & 0x0f) << 16) | ((uint32_t)reg[6] << 8) | ((uint32_t)reg[7] << 0);
	ms2_p3       = ((uint32_t)(reg[5] & 0xf0) << 12) | ((uint32_t)reg[0] << 8) | ((uint32_t)reg[1] << 0);
	ms2_r_div    = (reg[2] >> 4) & 0x07;
	ms2_div_by_4 = (reg[2] >> 2) & 0x03;

	if (!clk2_powered_down && clk2_enabled)
	{
		const double pll_Hz = clk2_pll ? pll_b_Hz : pll_a_Hz;
		switch (clk2_src)
		{
			case 0:	// XTAL
				clk_2_Hz = m_xtal_Hz;
				break;
			case 1:	// CLK-IN
				clk_2_Hz = m_xtal_Hz;
				break;
			case 2:	// MS0
				if (ms0_p3 > 0 || ms0_div_by_4 == 0x03)
					clk_2_Hz = (ms0_div_by_4 == 0x03) ? pll_Hz / 4 : (128.0 * ms0_p3 * pll_Hz) / (((double)ms0_p1 * ms0_p3) + ms0_p2 + (512.0 * ms0_p3));
				break;
			case 3:	// MS2
				if (ms2_p3 > 0 || ms2_div_by_4 == 0x03)
					clk_2_Hz = (ms2_div_by_4 == 0x03) ? pll_Hz / 4 : (128.0 * ms2_p3 * pll_Hz) / (((double)ms2_p1 * ms2_p3) + ms2_p2 + (512.0 * ms2_p3));
				break;
		}
		clk_2_Hz /= 1u << ms2_r_div;
	}

	// multisync6-7: fOUT = fIN / P1

	s += clk2_powered_down ? " PWR-DN" : " PWR-UP";

	switch (clk2_src)
	{
		case 0: s += " SRC-XTAL "; break;
		case 1: s += " SRC-CLKIN"; break;
		case 2: s += " SRC-MS0  "; break;
		case 3: s += " SRC-MS2  "; break;
	}

	s += clk2_pll ? " PLL-B" : " PLL-A";

	switch (clk2_drive_current)
	{
		case 0: s += " 2mA"; break;
		case 1: s += " 4mA"; break;
		case 2: s += " 6mA"; break;
		case 3: s += " 8mA"; break;
	}

	s += clk2_int_mode ? " INT " : " FRAC";

	if (clk2_enabled)
	{
//		s += " ENABLED";
	}
	else
	{
		switch (clk2_dis_output_mode)
		{
			case 0: s += " LOW    "; break;
			case 1: s += " HIGH   "; break;
			case 2: s += " HIGH-Z "; break;
//			case 3: s += " ENABLED"; break;
		}
	}

	if (clk_2_Hz > 0.0 && (clk2_enabled || clk2_dis_output_mode == 3))
	{
		if (clk_2_Hz >= 1e6)
			s2.sprintf(" %0.7f MHz", clk_2_Hz / 1e6);
		else
			s2.sprintf(" %0.4f kHz", clk_2_Hz / 1e3);
		s += s2;
	}

	if (clk2_inv)
		s += " INV";

	ui->Clock2Label->setText(s);
	ui->Clock2Label->update();

	// ******************************
}

void __fastcall MainWindow::updateRegisterListView(const bool show_updated)
{
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
			for (unsigned int k = 1; k < values.size() && addr < (int)ARRAY_SIZE(m_si5351_reg_values); k++)
			{
				updated_regs[addr] = true;
				m_si5351_reg_values[addr++] = values[k];
			}
		}

		if (m_file_line_clicked >= 0 && (int)i >= m_file_line_clicked)
			break;	// stop on the clicked line
	}

	// ******************************
	// update the register list display

	ui->RegisterTableWidget->setUpdatesEnabled(false);

	ui->RegisterTableWidget->clearSelection();

	for (unsigned int i = 0; i < ARRAY_SIZE(si5351_reg_list); i++)
	{
		const int addr = si5351_reg_list[i].addr;
		if (addr >= 0 && addr < (int)ARRAY_SIZE(m_si5351_reg_values))
		{
			QString s1;
			QString s2;

			const int k = 1 + i;

			const uint8_t value = m_si5351_reg_values[addr];

			const bool updated = (updated_regs[addr] && show_updated) ? true : false;

			s1.sprintf("%02X", value);
			s1 += "  " + QString("%1").arg(value, 8, 2, QChar('0'));
			if (updated)
				s1 = "* " + s1;  // highlight the updated registers

			s2 = regSettingDescription(addr, value);

			ui->RegisterTableWidget->item(k, 2)->setText(s1);
			ui->RegisterTableWidget->item(k, 3)->setText(s2);

			if (updated)
				ui->RegisterTableWidget->selectRow(k);  // highlight the updated registers
		}
	}

	ui->RegisterTableWidget->setUpdatesEnabled(true);

	// ******************************

	updateFrequencies();
}

void MainWindow::on_splitter_splitterMoved(int pos, int index)
{
	Q_UNUSED(pos);
	Q_UNUSED(index);

	sizeRegisterColoumns();
}
