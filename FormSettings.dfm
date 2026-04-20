object frmSettings: TfrmSettings
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Settings'
  ClientHeight = 465
  ClientWidth = 514
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  OnKeyPress = FormKeyPress
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object pnlBottom: TPanel
    Left = 0
    Top = 428
    Width = 514
    Height = 37
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 1
    object btnCancel: TButton
      Left = 435
      Top = 6
      Width = 75
      Height = 25
      Caption = 'Cancel'
      TabOrder = 1
      OnClick = btnCancelClick
    end
    object btnApply: TButton
      Left = 354
      Top = 6
      Width = 75
      Height = 25
      Caption = 'Apply'
      TabOrder = 0
      OnClick = btnApplyClick
    end
  end
  object pcGeneral: TPageControl
    Left = 0
    Top = 0
    Width = 514
    Height = 428
    ActivePage = tsGeneral
    Align = alClient
    TabOrder = 0
    object tsGeneral: TTabSheet
      Caption = 'General'
      object lblProcessPriority: TLabel
        Left = 190
        Top = 32
        Width = 100
        Height = 13
        Caption = 'Worker proc. priority'
      end
      object Label1: TLabel
        Left = 3
        Top = 312
        Width = 74
        Height = 13
        Caption = 'Default resizing'
      end
      object Label2: TLabel
        Left = 3
        Top = 219
        Width = 149
        Height = 13
        Caption = 'ImageMagick extra parameters'
      end
      object Label3: TLabel
        Left = 3
        Top = 138
        Width = 287
        Height = 13
        Caption = 'Output extension (format like: webp, jpg, png, gif, avif, jxl)'
      end
      object lblImExtraAfterResize: TLabel
        Left = 29
        Top = 263
        Width = 55
        Height = 13
        Caption = 'after resize'
      end
      object lblImExtraBeforeResize: TLabel
        Left = 29
        Top = 238
        Width = 63
        Height = 13
        Caption = 'before resize'
      end
      object lblWorkerThreadCount: TLabel
        Left = 3
        Top = 32
        Width = 100
        Height = 13
        Caption = 'Worker thread count'
      end
      object lblFilesToSkip: TLabel
        Left = 3
        Top = 167
        Width = 405
        Height = 13
        Caption = 
          'Files to skip (file name masks separated by semicolon, e.g. *.we' +
          'bp or *.webp;*.nfo)'
      end
      object lblUnpackPassword: TLabel
        Left = 3
        Top = 82
        Width = 231
        Height = 13
        Caption = 'Password for unpacking source files (if required)'
      end
      object chbAlwaysOnTop: TCheckBox
        Left = 3
        Top = 3
        Width = 325
        Height = 17
        Caption = 'Window always on top'
        TabOrder = 0
        OnClick = chbAlwaysOnTopClick
      end
      object cbWorkerProcPriority: TComboBox
        Left = 296
        Top = 29
        Width = 188
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        ItemIndex = 0
        TabOrder = 1
        Text = 'idle'
        Items.Strings = (
          'idle'
          'below normal (recommended)'
          'normal'
          'above normal (not recommended)')
      end
      object chbPlaySoundWhenDone: TCheckBox
        Left = 3
        Top = 53
        Width = 325
        Height = 17
        Caption = 'Play sound when done'
        TabOrder = 2
        OnClick = chbAlwaysOnTopClick
      end
      object vledResizing: TValueListEditor
        Left = 109
        Top = 308
        Width = 297
        Height = 87
        DefaultColWidth = 170
        KeyOptions = [keyEdit, keyAdd, keyDelete, keyUnique]
        TabOrder = 3
        TitleCaptions.Strings = (
          'Most common width in archive'
          'Default resizing [%]')
        ColWidths = (
          170
          121)
      end
      object edImExtraParameters: TEdit
        Left = 109
        Top = 259
        Width = 361
        Height = 23
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        TabOrder = 4
      end
      object edOutputExtension: TEdit
        Left = 296
        Top = 134
        Width = 105
        Height = 23
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        TabOrder = 5
      end
      object edImExtraParametersBeforeResize: TEdit
        Left = 109
        Top = 234
        Width = 361
        Height = 23
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        TabOrder = 6
      end
      object cbWorkerThreadCount: TComboBox
        Left = 109
        Top = 29
        Width = 43
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 7
      end
      object edConversionFilesToSkip: TEdit
        Left = 109
        Top = 185
        Width = 361
        Height = 23
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        TabOrder = 8
      end
      object edUnpackPassword: TEdit
        Left = 296
        Top = 78
        Width = 105
        Height = 23
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        TabOrder = 9
      end
      object chbCopyFileToOutputOnError: TCheckBox
        Left = 3
        Top = 110
        Width = 467
        Height = 17
        Caption = 
          'Copy source file directly to output directory on conversion erro' +
          'r (e.g. invalid password)'
        TabOrder = 10
        OnClick = chbAlwaysOnTopClick
      end
      object btnAddResizingEntry: TButton
        Left = 412
        Top = 370
        Width = 94
        Height = 25
        Caption = 'Add new rule'
        TabOrder = 11
        OnClick = btnAddResizingEntryClick
      end
    end
    object pcPdfImport: TTabSheet
      Caption = 'PDF import'
      ImageIndex = 3
      object lblPdfImportInfo: TLabel
        Left = 3
        Top = 3
        Width = 257
        Height = 13
        Caption = 'PDF import requires GhostScript for Windows installed'
      end
      object lblGsLocation: TLabel
        Left = 25
        Top = 28
        Width = 106
        Height = 13
        Caption = 'gswin32c.exe location'
      end
      object lblGsParameters: TLabel
        Left = 25
        Top = 53
        Width = 55
        Height = 13
        Caption = 'parameters'
      end
      object lblFilePattern: TLabel
        Left = 25
        Top = 78
        Width = 89
        Height = 13
        Caption = 'image files pattern'
      end
      object edGsLocation: TEdit
        Left = 137
        Top = 24
        Width = 288
        Height = 23
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
      end
      object btnGsLocation: TButton
        Left = 431
        Top = 25
        Width = 25
        Height = 22
        Caption = '...'
        TabOrder = 1
        OnClick = btnGsLocationClick
      end
      object edGsParams: TEdit
        Left = 137
        Top = 49
        Width = 240
        Height = 23
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        TabOrder = 2
      end
      object edGsFilePattern: TEdit
        Left = 137
        Top = 74
        Width = 240
        Height = 23
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        TabOrder = 3
      end
    end
    object pcDirectories: TTabSheet
      Caption = 'Directories'
      ImageIndex = 2
      object lblTmpDirectoryType: TLabel
        Left = 3
        Top = 3
        Width = 112
        Height = 13
        Caption = 'Temp directory location'
      end
      object lblOutDirectoryLocation: TLabel
        Left = 3
        Top = 131
        Width = 120
        Height = 13
        Caption = 'Output directory location'
      end
      object cbTmpDirectoryType: TComboBox
        Left = 137
        Top = 0
        Width = 288
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 0
        OnChange = cbTmpDirectoryTypeChange
      end
      object edTmpDirectory: TEdit
        Left = 137
        Top = 24
        Width = 288
        Height = 23
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        TabOrder = 1
      end
      object cbOutDirectoryType: TComboBox
        Left = 137
        Top = 128
        Width = 288
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 2
        OnChange = cbOutDirectoryTypeChange
      end
      object edOutDirectory: TEdit
        Left = 137
        Top = 152
        Width = 288
        Height = 23
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Courier New'
        Font.Style = []
        ParentFont = False
        TabOrder = 3
      end
      object btnSelectTmpDirectory: TButton
        Left = 431
        Top = 25
        Width = 25
        Height = 22
        Caption = '...'
        TabOrder = 4
        OnClick = btnSelectTmpDirectoryClick
      end
      object btnSelectOutDirectory: TButton
        Left = 431
        Top = 152
        Width = 25
        Height = 23
        Caption = '...'
        TabOrder = 5
        OnClick = btnSelectTmpDirectoryClick
      end
      object chbUseSourceDirectoryForOutput: TCheckBox
        Left = 3
        Top = 109
        Width = 500
        Height = 17
        Caption = 
          'Use source directory as output directory, adding extra extension' +
          ' to file name (e.g. AAA.webp.cbz)'
        TabOrder = 6
        OnClick = chbUseSourceDirectoryForOutputClick
      end
      object chbUseNumericTmpFileDirectory: TCheckBox
        Left = 3
        Top = 53
        Width = 500
        Height = 17
        Caption = 'Use numeric tmp file directory (reducing total path length)'
        TabOrder = 7
        OnClick = chbUseSourceDirectoryForOutputClick
      end
      object chbRecreateSourceDirectoryForOutput: TCheckBox
        Left = 137
        Top = 177
        Width = 500
        Height = 17
        Caption = 
          'Recreate source directory structure if directory is added as sou' +
          'rce'
        TabOrder = 8
        OnClick = chbUseSourceDirectoryForOutputClick
      end
    end
    object pcLogging: TTabSheet
      Caption = 'Logging'
      ImageIndex = 1
      object lblUiCapacity: TLabel
        Left = 5
        Top = 26
        Width = 194
        Height = 13
        Caption = 'Visible (buffered) log lines in log window:'
      end
      object cmbMaxUiLogLines: TComboBox
        Left = 216
        Top = 23
        Width = 111
        Height = 21
        Style = csDropDownList
        ItemHeight = 13
        TabOrder = 0
        OnChange = cmbMaxUiLogLinesChange
        Items.Strings = (
          '100'
          '200'
          '500'
          '1000'
          '2000'
          '5000'
          '10000')
      end
      object chbLogToFile: TCheckBox
        Left = 5
        Top = 3
        Width = 325
        Height = 17
        Caption = 'Log to file'
        TabOrder = 1
        OnClick = chbAlwaysOnTopClick
      end
    end
  end
  object openDialog: TOpenDialog
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Left = 8
    Top = 280
  end
end
