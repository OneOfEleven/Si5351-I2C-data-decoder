
#ifndef Unit1H
#define Unit1H

#define VC_EXTRALEAN
#define WIN32_EXTRA_LEAN
#define WIN32_LEAN_AND_MEAN

#include <stdint.h>
#include <Classes.hpp>
#include <Controls.hpp>
#include <Dialogs.hpp>
#include <StdCtrls.hpp>
#include <Grids.hpp>
#include <ValEdit.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <vector>

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>

#define WM_INIT_GUI        (WM_USER + 100)
#define WM_UPDATE_STATUS   (WM_USER + 101)

struct TVersion
{
	uint16_t MajorVer;
	uint16_t MinorVer;
	uint16_t ReleaseVer;
	uint16_t BuildVer;
	TVersion() : MajorVer(0), MinorVer(0), ReleaseVer(0), BuildVer(0) {}
};

class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TOpenDialog *OpenDialog1;
	TButton *OpenFileButton;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TLabel *Clock2Label;
	TLabel *Clock0Label;
	TLabel *Clock1Label;
	TEdit *XtalFreqEdit;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *PLLALabel;
	TLabel *PLLBLabel;
	TEdit *FilenameEdit;
	TLabel *Label8;
	TLabel *LineLabel;
	TPanel *Panel1;
	TListView *FileListView;
	TListView *RegisterListView;
	TSplitter *Splitter1;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall OpenFileButtonClick(TObject *Sender);
	void __fastcall XtalFreqEditChange(TObject *Sender);
	void __fastcall RegisterListViewResize(TObject *Sender);
	void __fastcall FilenameEditClick(TObject *Sender);
	void __fastcall FileListViewSelectItem(TObject *Sender, TListItem *Item,
          bool Selected);
	void __fastcall FileListViewClick(TObject *Sender);

private:	// User declarations

	String m_ini_filename;

	bool m_closing;
	
	String                                m_filename;
	std::vector <uint8_t>                 m_file_data;
	std::vector <String>                  m_file_lines;
	std::vector < std::vector <String> >  m_parsed_file_lines;
	std::vector < std::vector <uint8_t> > m_file_line_reg_values;

	int m_file_line_clicked;

	uint32_t m_xtal_Hz;

	uint8_t m_si5351_reg_values[256];

	bool __fastcall GetBuildInfo(String filename, TVersion *version);

	void __fastcall WMWindowPosChanging(TWMWindowPosChanging &msg);
	void __fastcall WMInitGUI(TMessage &msg);

	void __fastcall resetSi5351RegValues();

	void __fastcall loadSettings();
	void __fastcall saveSettings();

	int __fastcall parseString(String s, String separator, std::vector <String> &params);

	bool __fastcall loadFile(String filename);

	String __fastcall binToStr(const uint64_t value, const int digits);

	String __fastcall regSettingDescription(const int addr, const uint8_t value);

	void __fastcall updateFrequencies();

	void __fastcall updateRegisterListView(const bool show_updated);

	void __fastcall selectFile();
	
protected:

	#pragma option push -vi-
	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, TWMWindowPosMsg, WMWindowPosChanging);
		VCL_MESSAGE_HANDLER(WM_INIT_GUI, TMessage, WMInitGUI);
//		VCL_MESSAGE_HANDLER(WM_UPDATE_STATUS, TMessage, WMUpdateStatus);
	END_MESSAGE_MAP(TForm)
	#pragma option pop

public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};

extern PACKAGE TForm1 *Form1;

#endif
