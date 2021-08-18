object Form1: TForm1
  Left = 325
  Top = 130
  Width = 1000
  Height = 565
  Caption = 'Form1'
  Color = clBtnFace
  Constraints.MinHeight = 400
  Constraints.MinWidth = 1000
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  DesignSize = (
    984
    526)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 639
    Top = 44
    Width = 32
    Height = 13
    Alignment = taRightJustify
    Caption = 'CLK-0 '
  end
  object Label2: TLabel
    Left = 639
    Top = 68
    Width = 32
    Height = 13
    Alignment = taRightJustify
    Caption = 'CLK-1 '
  end
  object Label3: TLabel
    Left = 639
    Top = 92
    Width = 32
    Height = 13
    Alignment = taRightJustify
    Caption = 'CLK-2 '
  end
  object Clock2Label: TLabel
    Left = 679
    Top = 92
    Width = 77
    Height = 14
    Caption = 'Clock2Label'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -12
    Font.Name = 'Consolas'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Clock0Label: TLabel
    Left = 679
    Top = 44
    Width = 77
    Height = 14
    Caption = 'Clock0Label'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -12
    Font.Name = 'Consolas'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Clock1Label: TLabel
    Left = 679
    Top = 68
    Width = 77
    Height = 14
    Caption = 'Clock1Label'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -12
    Font.Name = 'Consolas'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label4: TLabel
    Left = 235
    Top = 42
    Width = 25
    Height = 13
    Caption = 'MHz '
  end
  object Label5: TLabel
    Left = 15
    Top = 42
    Width = 128
    Height = 13
    Alignment = taRightJustify
    Caption = 'XTAL / CLK-IN Frequency '
  end
  object Label6: TLabel
    Left = 283
    Top = 44
    Width = 32
    Height = 13
    Alignment = taRightJustify
    Caption = 'PLL-A '
  end
  object Label7: TLabel
    Left = 283
    Top = 68
    Width = 32
    Height = 13
    Alignment = taRightJustify
    Caption = 'PLL-B '
  end
  object PLLALabel: TLabel
    Left = 323
    Top = 44
    Width = 63
    Height = 14
    Caption = 'PLLALabel'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -12
    Font.Name = 'Consolas'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object PLLBLabel: TLabel
    Left = 323
    Top = 68
    Width = 63
    Height = 14
    Caption = 'PLLBLabel'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -12
    Font.Name = 'Consolas'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label8: TLabel
    Left = 8
    Top = 92
    Width = 23
    Height = 13
    Alignment = taRightJustify
    Caption = 'Line '
  end
  object LineLabel: TLabel
    Left = 39
    Top = 92
    Width = 56
    Height = 13
    Caption = 'LineLabel'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object OpenFileButton: TButton
    Left = 8
    Top = 8
    Width = 75
    Height = 25
    Cursor = crHandPoint
    Caption = 'Open File'
    TabOrder = 0
    OnClick = OpenFileButtonClick
  end
  object XtalFreqEdit: TEdit
    Left = 151
    Top = 40
    Width = 77
    Height = 21
    TabOrder = 2
    Text = 'XtalFreqEdit'
    OnChange = XtalFreqEditChange
  end
  object FilenameEdit: TEdit
    Left = 88
    Top = 13
    Width = 889
    Height = 17
    Cursor = crHandPoint
    Anchors = [akLeft, akTop, akRight]
    BorderStyle = bsNone
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentColor = True
    ParentFont = False
    ReadOnly = True
    TabOrder = 1
    Text = 'FilenameEdit'
    OnClick = FilenameEditClick
  end
  object Panel1: TPanel
    Left = 0
    Top = 112
    Width = 981
    Height = 413
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 3
    object Splitter1: TSplitter
      Left = 401
      Top = 1
      Width = 10
      Height = 411
      Cursor = crHSplit
      AutoSnap = False
      MinSize = 100
      ResizeStyle = rsUpdate
    end
    object FileListView: TListView
      Left = 1
      Top = 1
      Width = 400
      Height = 411
      Cursor = crHandPoint
      Align = alLeft
      Columns = <
        item
          AutoSize = True
        end>
      ColumnClick = False
      Constraints.MinWidth = 100
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Consolas'
      Font.Style = []
      GridLines = True
      HideSelection = False
      ReadOnly = True
      RowSelect = True
      ParentFont = False
      ShowColumnHeaders = False
      TabOrder = 0
      ViewStyle = vsReport
      OnClick = FileListViewClick
      OnSelectItem = FileListViewSelectItem
    end
    object RegisterListView: TListView
      Left = 411
      Top = 1
      Width = 569
      Height = 411
      Cursor = crHandPoint
      Align = alClient
      Columns = <
        item
          Caption = 'Reg'
          MaxWidth = 60
          MinWidth = 60
          Width = 60
        end
        item
          Caption = 'Reg Name'
          MaxWidth = 250
          MinWidth = 250
          Width = 250
        end
        item
          Alignment = taRightJustify
          Caption = 'Value'
          MaxWidth = 110
          MinWidth = 110
          Width = 110
        end
        item
          Caption = 'Settings'
          MinWidth = 100
          Width = 100
        end>
      ColumnClick = False
      Constraints.MinWidth = 100
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Consolas'
      Font.Style = []
      GridLines = True
      HideSelection = False
      MultiSelect = True
      ReadOnly = True
      RowSelect = True
      ParentFont = False
      TabOrder = 1
      ViewStyle = vsReport
      OnResize = RegisterListViewResize
    end
  end
  object TestButton: TButton
    Left = 8
    Top = 64
    Width = 75
    Height = 25
    Caption = 'Test'
    TabOrder = 4
    OnClick = TestButtonClick
  end
  object OpenDialog1: TOpenDialog
    Filter = 'All files (*.*)|*.*|Text files (*.txt)|*.txt'
    FilterIndex = 2
    Options = [ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Left = 180
    Top = 8
  end
end
