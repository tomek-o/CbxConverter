object frmRename: TfrmRename
  Left = 0
  Top = 0
  Caption = 'Rename images in temporary folder'
  ClientHeight = 335
  ClientWidth = 627
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object pnlBottom: TPanel
    Left = 0
    Top = 256
    Width = 627
    Height = 79
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    DesignSize = (
      627
      79)
    object lblPattern: TLabel
      Left = 0
      Top = 56
      Width = 212
      Height = 13
      Caption = 'Pattern for new names (using C/C++ printf)'
    end
    object lblOffset: TLabel
      Left = 0
      Top = 35
      Width = 151
      Height = 13
      Caption = 'Offset to add / starting number'
    end
    object btnRename: TButton
      Left = 547
      Top = 49
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Rename'
      TabOrder = 0
      OnClick = btnRenameClick
    end
    object cbRenameType: TComboBox
      Left = 0
      Top = 7
      Width = 217
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      ItemIndex = 0
      TabOrder = 1
      Text = 'extract numbers from file names'
      OnChange = cbRenameTypeChange
      Items.Strings = (
        'extract numbers from file names'
        'start from specified number')
    end
    object edPattern: TEdit
      Left = 223
      Top = 53
      Width = 178
      Height = 21
      TabOrder = 2
      Text = '%04d'
      OnChange = edPatternChange
    end
    object edOffset: TEdit
      Left = 223
      Top = 30
      Width = 58
      Height = 21
      TabOrder = 3
      Text = '0'
      OnChange = edOffsetChange
    end
  end
  object lvFiles: TListView
    Left = 0
    Top = 0
    Width = 627
    Height = 256
    Align = alClient
    Columns = <
      item
        Caption = 'Old name'
        Width = 300
      end
      item
        Caption = 'New name'
        Width = 300
      end>
    HideSelection = False
    OwnerData = True
    RowSelect = True
    TabOrder = 1
    ViewStyle = vsReport
    OnData = lvFilesData
  end
end
