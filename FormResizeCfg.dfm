object frmResizeCfg: TfrmResizeCfg
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Resize'
  ClientHeight = 129
  ClientWidth = 398
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poMainFormCenter
  OnKeyPress = FormKeyPress
  PixelsPerInch = 96
  TextHeight = 13
  object lblResizeTo: TLabel
    Left = 8
    Top = 40
    Width = 44
    Height = 13
    Caption = 'Resize to'
  end
  object lblResizeToPct: TLabel
    Left = 133
    Top = 40
    Width = 11
    Height = 13
    Caption = '%'
  end
  object lblResizeMode: TLabel
    Left = 8
    Top = 11
    Width = 60
    Height = 13
    Caption = 'Resize mode'
  end
  object lblResizeIf: TLabel
    Left = 8
    Top = 66
    Width = 40
    Height = 13
    Caption = 'Resize if'
  end
  object lblResizeIfPxTo: TLabel
    Left = 247
    Top = 66
    Width = 72
    Height = 13
    Caption = 'px   to (target)'
  end
  object lblResizeIfPx: TLabel
    Left = 379
    Top = 66
    Width = 12
    Height = 13
    Caption = 'px'
  end
  object lblThan: TLabel
    Left = 167
    Top = 66
    Width = 22
    Height = 13
    Caption = 'than'
  end
  object pnlBottom: TPanel
    Left = 0
    Top = 92
    Width = 398
    Height = 37
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    ExplicitWidth = 355
    object btnCancel: TButton
      Left = 317
      Top = 6
      Width = 75
      Height = 25
      Caption = 'Cancel'
      ModalResult = 2
      TabOrder = 1
    end
    object btnApply: TButton
      Left = 236
      Top = 6
      Width = 75
      Height = 25
      Caption = 'Apply'
      ModalResult = 1
      TabOrder = 0
      OnClick = btnApplyClick
    end
  end
  object cbResizePct: TComboBox
    Left = 74
    Top = 37
    Width = 47
    Height = 21
    ItemHeight = 13
    TabOrder = 1
    Text = '50'
    Items.Strings = (
      '25'
      '33'
      '50'
      '66'
      '100')
  end
  object edResizeThreshold: TEdit
    Left = 195
    Top = 63
    Width = 44
    Height = 21
    TabOrder = 2
    Text = '1366'
  end
  object cbResizeMode: TComboBox
    Left = 74
    Top = 8
    Width = 318
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 3
    Text = 'regular - same, proportional resizing for all images'
    OnChange = cbResizeModeChange
    Items.Strings = (
      'regular - same, proportional resizing for all images'
      'resize if oversized')
  end
  object edResizeTarget: TEdit
    Left = 326
    Top = 63
    Width = 44
    Height = 21
    TabOrder = 4
    Text = '1366'
  end
  object cbConditionalResizeMode: TComboBox
    Left = 54
    Top = 63
    Width = 107
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 5
    Text = 'wider or higher'
    Items.Strings = (
      'wider or higher'
      'wider'
      'higher')
  end
end
