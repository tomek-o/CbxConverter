object frmRename: TfrmRename
  Left = 0
  Top = 0
  Caption = 'Rename images in temporary folder'
  ClientHeight = 335
  ClientWidth = 775
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
    Width = 775
    Height = 79
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    ExplicitWidth = 627
    DesignSize = (
      775
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
      Left = 695
      Top = 49
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Rename'
      TabOrder = 0
      OnClick = btnRenameClick
      ExplicitLeft = 547
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
    Width = 775
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
      end
      item
        Caption = 'Size [B]'
        Width = 60
      end
      item
        Caption = 'Width'
        Width = 45
      end
      item
        Caption = 'Height'
        Width = 45
      end>
    HideSelection = False
    MultiSelect = True
    OwnerData = True
    RowSelect = True
    PopupMenu = popupList
    TabOrder = 1
    ViewStyle = vsReport
    OnData = lvFilesData
    ExplicitTop = 1
    ExplicitWidth = 627
  end
  object popupList: TPopupMenu
    Left = 8
    Top = 24
    object miDeleteSelectedFiles: TMenuItem
      Caption = 'Delete selected file(s)'
      OnClick = miDeleteSelectedFilesClick
    end
  end
end
