object frmChoseItem: TfrmChoseItem
  Left = 579
  Top = 333
  BorderStyle = bsDialog
  Caption = 'Select Item'
  ClientHeight = 331
  ClientWidth = 346
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 208
    Top = 0
    Width = 138
    Height = 331
    Align = alRight
    BevelOuter = bvNone
    TabOrder = 0
    object sbSelect: TExtBtn
      Left = 2
      Top = 312
      Width = 67
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Ok'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = sbSelectClick
    end
    object sbCancel: TExtBtn
      Left = 70
      Top = 312
      Width = 67
      Height = 18
      Align = alNone
      BevelShow = False
      CloseButton = False
      Caption = 'Cancel'
      Transparent = False
      FlatAlwaysEdge = True
      OnClick = sbCancelClick
    end
    object lbItemName: TLabel
      Left = 33
      Top = 141
      Width = 106
      Height = 13
      AutoSize = False
      Caption = '-'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object lbFileName: TLabel
      Left = 33
      Top = 155
      Width = 106
      Height = 13
      AutoSize = False
      Caption = '-'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
    end
    object mxLabel1: TLabel
      Left = 1
      Top = 141
      Width = 32
      Height = 13
      AutoSize = False
      Caption = 'Item:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object mxLabel2: TLabel
      Left = 1
      Top = 155
      Width = 28
      Height = 13
      AutoSize = False
      Caption = 'File:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object mxLabel3: TLabel
      Left = 1
      Top = 168
      Width = 28
      Height = 13
      AutoSize = False
      Caption = 'Info:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lbInfo: TLabel
      Left = 33
      Top = 168
      Width = 106
      Height = 13
      AutoSize = False
      Caption = '-'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clNavy
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      WordWrap = True
    end
    object paImage: TPanel
      Left = 3
      Top = 6
      Width = 132
      Height = 132
      BevelOuter = bvLowered
      Caption = '<no image>'
      TabOrder = 0
      object pbImage: TPaintBox
        Left = 1
        Top = 1
        Width = 130
        Height = 130
        Align = alClient
        Color = clGray
        ParentColor = False
        OnPaint = pbImagePaint
      end
    end
    object paMulti: TPanel
      Left = 1
      Top = 226
      Width = 136
      Height = 83
      TabOrder = 1
      Visible = False
      object ebMultiUp: TExtBtn
        Left = 120
        Top = 3
        Width = 14
        Height = 18
        Align = alNone
        CloseButton = False
        Glyph.Data = {
          F6000000424DF600000000000000360000002800000008000000080000000100
          180000000000C0000000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000000000
          00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFDDDDDC000000000000DDDDDCFFFFFFFF
          FFFFFFFFFF000000000000000000000000191919000000FFFFFFFFFFFFDDDDDC
          000000000000000000000000DDDDDCFFFFFFFFFFFFFFFFFF0000000000000000
          00000000FFFFFFFFFFFFFFFFFFFFFFFFDDDDDC000000000000DDDDDCFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFF}
        Spacing = 0
        Transparent = False
        OnClick = ebMultiUpClick
      end
      object ebMultiDown: TExtBtn
        Left = 120
        Top = 63
        Width = 14
        Height = 18
        Align = alNone
        CloseButton = False
        Glyph.Data = {
          F6000000424DF600000000000000360000002800000008000000080000000100
          180000000000C0000000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFDDDDDC0000000000
          00DDDDDCFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000000000FFFFFFFF
          FFFFFFFFFFDDDDDC000000000000000000000000DDDDDCFFFFFFFFFFFF000000
          191919000000000000000000000000FFFFFFFFFFFFFFFFFFDDDDDC0000000000
          00DDDDDCFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFF}
        Spacing = 0
        Transparent = False
        OnClick = ebMultiDownClick
      end
      object ebMultiRemove: TExtBtn
        Left = 120
        Top = 23
        Width = 14
        Height = 18
        Align = alNone
        CloseButton = False
        Glyph.Data = {
          56010000424D560100000000000036000000280000000A000000090000000100
          18000000000020010000120B0000120B00000000000000000000C8D0D4C8D0D4
          C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D40000C8D0D4000000
          000000C8D0D4C8D0D4C8D0D4C8D0D4000000000000C8D0D40000C8D0D4C8D0D4
          000000000000C8D0D4C8D0D4000000000000C8D0D4C8D0D40000C8D0D4C8D0D4
          C8D0D4000000000000000000000000C8D0D4C8D0D4C8D0D40000C8D0D4C8D0D4
          C8D0D4C8D0D4000000000000C8D0D4C8D0D4C8D0D4C8D0D40000C8D0D4C8D0D4
          C8D0D4000000000000000000000000C8D0D4C8D0D4C8D0D40000C8D0D4C8D0D4
          000000000000C8D0D4C8D0D4000000000000C8D0D4C8D0D40000C8D0D4000000
          000000C8D0D4C8D0D4C8D0D4C8D0D4000000000000C8D0D40000C8D0D4C8D0D4
          C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D4C8D0D40000}
        Spacing = 0
        Transparent = False
        OnClick = ebMultiRemoveClick
      end
      object ebMultiClear: TExtBtn
        Left = 120
        Top = 43
        Width = 14
        Height = 18
        Align = alNone
        CloseButton = False
        Glyph.Data = {
          C2010000424DC20100000000000036000000280000000C0000000B0000000100
          1800000000008C010000120B0000120B00000000000000000000FFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFF000000000000FF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF000000000000
          FFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF0000000000000000000000
          00FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFF00000000
          0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF000000000000000000
          000000000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000
          00000000FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00
          0000000000000000000000FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFF
          000000000000FFFFFFFFFFFF000000000000FFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FF000000000000FFFFFFFFFFFFFFFFFFFFFFFF000000000000FFFFFFFFFFFFFF
          FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFF}
        Spacing = 0
        Transparent = False
        OnClick = ebMultiClearClick
      end
      object tvMulti: TElTree
        Left = 1
        Top = 1
        Width = 119
        Height = 81
        Cursor = crDefault
        LeftPosition = 0
        DoubleBuffered = False
        DragCursor = crDrag
        AutoCollapse = False
        AutoExpand = True
        DockOrientation = doNoOrient
        DefaultSectionWidth = 120
        BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
        DragAllowed = True
        ExplorerEditMode = False
        Flat = True
        FocusedSelectColor = 10526880
        FocusedSelectTextColor = clBlack
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        FullRowSelect = False
        GradientSteps = 64
        HeaderHeight = 19
        HeaderHotTrack = False
        HeaderSections.Data = {
          F4FFFFFF070000000000000000000000FFFFFFFF000001010100000078000000
          0000000010270000000100006489770600000000000000000000000000000100
          0000000000000000000100000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000010000000001000000000000
          0000000000000000000000000000FFFFFFFF0000010101000000780000000000
          00001027000000010000BC083404010000000000000000000000000001000000
          0000000000000001000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000100000000010000000000000000
          000000000000000000000000FFFFFFFF00000101010000007800000000000000
          1027000000010000DC3D26040200000000000000000000000000010000000000
          0000000000010000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000001000000000100000000000000000000
          00000000000000000000FFFFFFFF000001010100000078000000000000001027
          0000000100004022300D03000000000000000000000000000100000000000000
          0000000100000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000010000000001000000000000000000000000
          0000000000000000FFFFFFFF0000010101000000780000000000000010270000
          00010000FCCF2A04040000000000000000000000000001000000000000000000
          0001000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000100000000010000000000000000000000000000
          000000000000FFFFFFFF00000101010000007800000000000000102700000001
          00001C0E440D0500000000000000000000000000010000000000000000000001
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000001000000000100000000000000000000000000000000
          00000000FFFFFFFF000001010100000078000000000000001027000000010000
          28886A0006000000000000000000000000000100000000000000000000010000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000000000000000000000000000000000000000000000000000
          0000000000000000010000000001000000000000000000000000}
        HeaderFont.Charset = DEFAULT_CHARSET
        HeaderFont.Color = clWindowText
        HeaderFont.Height = -11
        HeaderFont.Name = 'MS Sans Serif'
        HeaderFont.Style = []
        HorzDivLinesColor = 7368816
        HorzScrollBarStyles.ShowTrackHint = False
        HorzScrollBarStyles.Width = 16
        HorzScrollBarStyles.ButtonSize = 14
        HorzScrollBarStyles.UseSystemMetrics = False
        IgnoreEnabled = False
        IncrementalSearch = False
        KeepSelectionWithinLevel = False
        LineBorderActiveColor = clBlack
        LineBorderInactiveColor = clBlack
        LineHeight = 17
        LinesColor = clBtnShadow
        MouseFrameSelect = True
        MultiSelect = False
        OwnerDrawMask = '~~@~~'
        PlusMinusTransparent = True
        ScrollbarOpposite = False
        ScrollTracking = True
        ShowButtons = False
        ShowLeafButton = False
        ShowLines = False
        StoragePath = '\Tree'
        TabOrder = 0
        TabStop = True
        TrackColor = 10526880
        VertDivLinesColor = 7368816
        VertScrollBarStyles.ShowTrackHint = True
        VertScrollBarStyles.Width = 16
        VertScrollBarStyles.ButtonSize = 14
        VertScrollBarStyles.UseSystemMetrics = False
        VirtualityLevel = vlNone
        TextColor = clBtnText
        BkColor = clGray
        OnDragDrop = tvMultiDragDrop
        OnDragOver = tvMultiDragOver
        OnStartDrag = tvMultiStartDrag
      end
    end
  end
  object tvItems: TElTree
    Left = 0
    Top = 0
    Width = 208
    Height = 331
    Cursor = crDefault
    LeftPosition = 0
    DragCursor = crDrag
    Align = alClient
    AutoCollapse = False
    AutoExpand = True
    DockOrientation = doNoOrient
    DefaultSectionWidth = 120
    BorderSides = [ebsLeft, ebsRight, ebsTop, ebsBottom]
    Color = clWhite
    CustomPlusMinus = True
    DrawFocusRect = False
    ExplorerEditMode = False
    Flat = True
    FocusedSelectColor = 10526880
    FocusedSelectTextColor = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    FullRowSelect = False
    GradientSteps = 64
    HeaderColor = 7368816
    HeaderHeight = 19
    HeaderHotTrack = False
    HeaderSections.Data = {
      F4FFFFFF07000000D8F08B0300F08B03FFFFFFFF0000010101008B0378000000
      000000001027000000010003206F2C040000000000008B030000000000000103
      000000000000000000010000000000008AF38B03AFF38B03DCF38B03FFF38B03
      24F48B034EF48B0371F48B0395F48B03B8F48B03DDF48B0301F58B0329F58B03
      51F58B0375F58B03360000000000000024000000010000000001000000000000
      000000000000D8F08B0300F08B03FFFFFFFF0000010101008B03780000000000
      00001027000000010003184529040100000000008B0300000000000001030000
      00000000000000010000000000008AF38B03AFF38B03DCF38B03FFF38B0324F4
      8B034EF48B0371F48B0395F48B03B8F48B03DDF48B0301F58B0329F58B0351F5
      8B0375F58B033600000000000000240000000100000000010000000000000000
      00000000D8F08B0300F08B03FFFFFFFF0000010101008B037800000000000000
      1027000000010003AC2976060200000000008B03000000000000010300000000
      0000000000010000000000008AF38B03AFF38B03DCF38B03FFF38B0324F48B03
      4EF48B0371F48B0395F48B03B8F48B03DDF48B0301F58B0329F58B0351F58B03
      75F58B0336000000000000002400000001000000000100000000000000000000
      0000D8F08B0300F08B03FFFFFFFF0000010101008B0378000000000000001027
      000000010003D8CF160C0300000000008B030000000000000103000000000000
      000000010000000000008AF38B03AFF38B03DCF38B03FFF38B0324F48B034EF4
      8B0371F48B0395F48B03B8F48B03DDF48B0301F58B0329F58B0351F58B0375F5
      8B03360000000000000024000000010000000001000000000000000000000000
      D8F08B0300F08B03FFFFFFFF0000010101008B03780000000000000010270000
      00010003F0A923040400000000008B0300000000000001030000000000000000
      00010000000000008AF38B03AFF38B03DCF38B03FFF38B0324F48B034EF48B03
      71F48B0395F48B03B8F48B03DDF48B0301F58B0329F58B0351F58B0375F58B03
      360000000000000024000000010000000001000000000000000000000000D8F0
      8B0300F08B03FFFFFFFF0000010101008B037800000000000000102700000001
      0003106126040500000000008B03000000000000010300000000000000000001
      0000000000008AF38B03AFF38B03DCF38B03FFF38B0324F48B034EF48B0371F4
      8B0395F48B03B8F48B03DDF48B0301F58B0329F58B0351F58B0375F58B033600
      00000000000024000000010000000001000000000000000000000000D8F08B03
      00F08B03FFFFFFFF0000010101008B0378000000000000001027000000010003
      106572000600000000008B030000000000000103000000000000000000010000
      000000008AF38B03AFF38B03DCF38B03FFF38B0324F48B034EF48B0371F48B03
      95F48B03B8F48B03DDF48B0301F58B0329F58B0351F58B0375F58B0336000000
      0000000024000000010000000001000000000000000000000000}
    HeaderFont.Charset = DEFAULT_CHARSET
    HeaderFont.Color = clWindowText
    HeaderFont.Height = -11
    HeaderFont.Name = 'MS Sans Serif'
    HeaderFont.Style = []
    HorizontalLines = True
    HorzDivLinesColor = 7368816
    HorzScrollBarStyles.ShowTrackHint = False
    HorzScrollBarStyles.Width = 16
    HorzScrollBarStyles.ButtonSize = 14
    HorzScrollBarStyles.UseSystemMetrics = False
    HorzScrollBarStyles.UseXPThemes = False
    IgnoreEnabled = False
    IncrementalSearch = False
    KeepSelectionWithinLevel = False
    LineBorderActiveColor = clBlack
    LineBorderInactiveColor = clBlack
    LineHeight = 18
    LinesColor = clBtnShadow
    MinusPicture.Data = {
      F6000000424DF600000000000000360000002800000008000000080000000100
      180000000000C0000000120B0000120B00000000000000000000808080808080
      808080808080808080808080808080808080808080808080808080E0E0E08080
      80808080808080808080808080808080808080E0E0E080808080808080808080
      8080808080808080E0E0E0E0E0E0E0E0E0808080808080808080808080808080
      E0E0E0E0E0E0E0E0E0808080808080808080808080E0E0E0E0E0E0E0E0E0E0E0
      E0E0E0E0808080808080808080E0E0E0E0E0E0E0E0E0E0E0E0E0E0E080808080
      8080808080808080808080808080808080808080808080808080}
    MouseFrameSelect = True
    MultiSelect = False
    OwnerDrawMask = '~~@~~'
    PlusMinusTransparent = True
    PlusPicture.Data = {
      F6000000424DF600000000000000360000002800000008000000080000000100
      180000000000C0000000120B0000120B00000000000000000000808080808080
      8080808080808080808080808080808080808080808080808080808080808080
      80808080808080808080808080E0E0E0E0E0E080808080808080808080808080
      8080808080E0E0E0E0E0E0E0E0E0E0E0E0808080808080808080808080E0E0E0
      E0E0E0E0E0E0E0E0E0E0E0E0E0E0E0808080808080E0E0E0E0E0E0E0E0E0E0E0
      E0808080808080808080808080E0E0E0E0E0E080808080808080808080808080
      8080808080808080808080808080808080808080808080808080}
    ScrollbarOpposite = False
    ScrollTracking = True
    ShowLeafButton = False
    ShowLines = False
    SortMode = smAdd
    StoragePath = '\Tree'
    TabOrder = 1
    TabStop = True
    Tracking = False
    TrackColor = 10526880
    VertDivLinesColor = 7368816
    VerticalLines = True
    VertScrollBarStyles.ShowTrackHint = True
    VertScrollBarStyles.Width = 16
    VertScrollBarStyles.ButtonSize = 14
    VertScrollBarStyles.UseSystemMetrics = False
    VertScrollBarStyles.UseXPThemes = False
    VirtualityLevel = vlNone
    UseXPThemes = False
    TextColor = clBtnText
    BkColor = clGray
    OnItemChange = tvItemsItemChange
    OnItemFocused = tvItemsItemFocused
    OnDblClick = tvItemsDblClick
    OnKeyPress = tvItemsKeyPress
  end
  object fsStorage: TFormStorage
    StoredValues = <
      item
        Name = 'EmitterDirX'
        Value = 0s
      end
      item
        Name = 'EmitterDirY'
        Value = 0s
      end
      item
        Name = 'EmitterDirZ'
        Value = 0s
      end>
    Left = 8
    Top = 8
  end
end
