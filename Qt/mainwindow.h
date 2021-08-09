
// Si5351 I2C data decoder
//
// Written by Cathy G6AMU
// August 2021

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMutex>

#include <vector>
#include <stdint.h>

QT_BEGIN_NAMESPACE
    namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct TVersion
{
	uint16_t MajorVer;
	uint16_t MinorVer;
	uint16_t ReleaseVer;
	uint16_t BuildVer;
	TVersion() : MajorVer(0), MinorVer(0), ReleaseVer(0), BuildVer(0) {}
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void on_FileOpenPushButton_clicked();

	void on_RefHzLineEdit_textChanged(const QString &arg1);

	void on_FileListView_clicked(const QModelIndex &index);

	void onTableWidgetCellSelected(int nRow, int nCol);

	void onSelectionChanged();

	void on_splitter_splitterMoved(int pos, int index);

protected:
	void showEvent(QShowEvent *event);
	void resizeEvent(QResizeEvent *event);

private:
	Ui::MainWindow *ui;

	bool m_shown;

	QString m_ini_filename;

	QString                               m_filename;
	std::vector < std::vector <QString> > m_parsed_file_lines;
	std::vector < std::vector <uint8_t> > m_file_line_reg_values;

	int m_file_line_clicked;

	double m_xtal_Hz;

	uint8_t m_si5351_reg_values[256];

	QMutex m_file_mutex;

	void __fastcall sizeRegisterColoumns();

	void __fastcall selectFile();

	void __fastcall loadSettings();
	void __fastcall saveSettings();

	bool __fastcall loadFile(QString filename);

	void __fastcall resetSi5351RegValues();

	QString __fastcall regSettingDescription(const int addr, const uint8_t value);

	void __fastcall updateFrequencies();

	void __fastcall updateRegisterListView(const bool show_updated);
};

#endif
