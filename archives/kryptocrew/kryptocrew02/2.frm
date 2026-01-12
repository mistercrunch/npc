VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form Form1 
   Caption         =   "Simple Zip"
   ClientHeight    =   5295
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   5640
   LinkTopic       =   "Form1"
   ScaleHeight     =   5295
   ScaleWidth      =   5640
   StartUpPosition =   3  'Windows Default
   Begin VB.TextBox Text2 
      Height          =   3615
      Left            =   120
      MultiLine       =   -1  'True
      TabIndex        =   3
      Top             =   1560
      Width           =   5415
   End
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   3840
      Top             =   0
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin VB.TextBox Text1 
      Height          =   375
      Left            =   120
      Locked          =   -1  'True
      TabIndex        =   1
      Top             =   480
      Width           =   5295
   End
   Begin VB.CommandButton Command1 
      Caption         =   "Ent / Verpacken"
      Height          =   375
      Left            =   120
      TabIndex        =   0
      Top             =   1080
      Width           =   5295
   End
   Begin VB.Label Label1 
      Caption         =   "Datei wählen :"
      Height          =   255
      Left            =   120
      TabIndex        =   2
      Top             =   120
      Width           =   2175
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Text1_Click()
  With CommonDialog1
  .FileName = ""
  .DialogTitle = "Datei zum Ent / Verpacken wählen:"
  .CancelError = False
  .Filter = "All files (*.*)|*.*"
  .ShowOpen
  Text1.Text = .FileName
 End With
End Sub

Private Sub Command1_Click()
Dim xbyte As String

 Open Text1.Text For Binary As #1
 lenght = LOF(1)
 xbyte = Space$(lenght)
 Get #1, , xbyte
 Close #1

 If Mid(xbyte, 1, 2) = "SZ" Then
  On Error Resume Next
  Call decompress(xbyte)
  Test = Mid(Text1.Text, 1, Len(Text1.Text) - 3)
  Kill (Test)
  Open Test For Binary As #1
  Put #1, , xbyte
  Close #1
 Else
  On Error Resume Next
  Call compress(xbyte)
  Kill (Text1.Text & ".sz")
  Open Text1.Text & ".sz" For Binary As #1
  Put #1, , xbyte
  Close #1
 End If
 
' MsgBox xbyte
 MsgBox "Aktion beendet"
End Sub

Public Sub compress(xbyte As String)
Dim ybyte As String
On Error Resume Next

ybyte = "SZ"

For x = 1 To Len(xbyte)
tmpByte = ""

'Wiederholungen testen
If Mid(xbyte, x, 1) = Mid(xbyte, x + 1, 1) And Mid(xbyte, x + 1, 1) = Mid(xbyte, x + 2, 1) Then

' Testen nach Regel  -- 06, 07, 08
For y = 1 To 3
Select Case y
' Testen nach Regel -- 06 -- Leerzeichen
 Case Is = 1: temp = " "
' Testen nach Regel -- 07 -- Nullen
 Case Is = 2: temp = Chr$(0)
' Testen nach Regel -- 08 -- 0ffh Folge
 Case Is = 3: temp = Chr$(255)
End Select
DoEvents
temp2 = 5 + y
 If Mid(xbyte, x, 1) = Mid(xbyte, x + 1, 1) And Mid(xbyte, x + 1, 1) = Mid(xbyte, x + 2, 1) And Mid(xbyte, x + 2, 1) = temp Then
  For Z = 1 To Len(xbyte) - x
   If Mid(xbyte, x, 1) <> Mid(xbyte, x + Z, 1) Then GoTo End0678
   If Z = 255 Then GoTo End0678
  Next Z
End0678:
  tmpByte = Chr$(temp2) & Chr$(Z)
  x = x + Z - 1
  GoTo BlockIsDone
 End If
Next y

' Testen nach Regel -- 00 -- sonstige Wiederholungen
If Mid(xbyte, x, 1) = Mid(xbyte, x + 1, 1) And Mid(xbyte, x + 1, 1) = Mid(xbyte, x + 2, 1) Then
 For Z = 1 To Len(xbyte) - x
  If Mid(xbyte, x, 1) <> Mid(xbyte, x + Z, 1) Then GoTo End00
  If Z = 255 Then GoTo End00
 Next Z
End00:
 tmpByte = Chr$(0) & Chr$(Z) & Mid(xbyte, x, 1)
 x = x + Z - 1
 GoTo BlockIsDone
End If

End If

' Testen nach Regel -- 05 -- Zeilenumbruch
If Mid(xbyte, x, 2) = Chr(13) & Chr(10) And (Len(xbyte) - x) > 1 Then
 tmpByte = Chr$(5)
 x = x + 1
 GoTo BlockIsDone
End If

' Testen nach Regel -- 04 -- Wide Zeichenfolge
If Mid(xbyte, x + 1, 1) = Chr(0) And Mid(xbyte, x + 3, 1) = Chr(0) Then
 For Z = 0 To Len(xbyte) - x - 1
  If Chr(Asc(Mid(xbyte, x + 1 + (2 * Z), 1))) <> Chr(0) Then GoTo End04
  If Z = 255 Then GoTo End04
 Next Z
End04:
Dim tempx As String
tempx = ""
  For y = 0 To Z - 1
   tempx = tempx & Mid(xbyte, x + (y * 2), 1)
 Next y
 
 tmpByte = Chr$(4) & Chr$(Z - 1) & tempx
 x = x + 1 + ((Z - 1) * 2)
 GoTo BlockIsDone
End If



tmpByte2 = tmpByte2 & Mid(xbyte, x, 1)
' Wir wollen nichts 2 mal Checken
BlockIsDone:

If tmpByte <> "" Or Len(tmpByte2) = 255 Then   ' wenn 01 nicht erfüllt ist
 If tmpByte2 <> "" Then tmpByte2 = Chr$(1) & Chr$(Len(tmpByte2)) & tmpByte2
 ybyte = ybyte & tmpByte2 & tmpByte
 tmpByte2 = ""
End If
DoEvents
Next x

If tmpByte2 <> "" Then
 tmpByte2 = Chr$(1) & Chr$(Len(tmpByte2)) & tmpByte2
 ybyte = ybyte & tmpByte2
End If

' Ausgabe zum Testen

'Text2.Text = ""
'For x = 1 To Len(ybyte)
' Text2.Text = Text2.Text & Asc(Mid(ybyte, x, 1)) & " "
' If x Mod 10 = 0 Then Text2.Text = Text2.Text & vbCrLf
' DoEvents
'Next x

xbyte = ybyte

End Sub

Public Sub decompress(xbyte As String)


' strip SZ-Mark

ybyte = Mid(xbyte, 3, Len(xbyte) - 2)
xbyte = ""

Do While Len(ybyte) > 0
 tt = Asc(Mid(ybyte, 1, 1))
 Select Case Mid(ybyte, 1, 1)

 Case Is = Chr(0):      ' Gleich
  For x = 1 To Asc(Mid(ybyte, 2, 1))
   xbyte = xbyte & Mid(ybyte, 3, 1)
  Next x
  ybyte = Mid(ybyte, 4, Len(ybyte) - 3)
 
 Case Is = Chr(1):      ' ungeordnet
  x = Asc(Mid(ybyte, 2, 1))
  xbyte = xbyte & Mid(ybyte, 3, x)
  ybyte = Right(ybyte, Len(ybyte) - (2 + x))
      
 Case Is = Chr(4):      ' Wide Zeichenfolge
  c = (Asc(Mid(ybyte, 2, 1)))
  For x = 0 To c
   xbyte = xbyte & (Mid(ybyte, 3 + x, 1)) & Chr(0)
  Next x
  ybyte = Mid(ybyte, 3 + x, Len(ybyte) - (2 + x))
 
 Case Is = Chr(5):      ' Zeilenumbruch
  xbyte = xbyte & Chr(13) & Chr(10)
  ybyte = Mid(ybyte, 2, Len(ybyte) - 1)
 
 Case Is = Chr(6):      ' Leerzeichen
  For x = 1 To Asc(Mid(ybyte, 2, 1))
   xbyte = xbyte & " "
  Next x
  ybyte = Mid(ybyte, 3, Len(ybyte) - 2)
 
 Case Is = Chr(7):      ' Nullenfolge
  For x = 1 To Asc(Mid(ybyte, 2, 1))
   xbyte = xbyte & Chr(0)
  Next x
  ybyte = Mid(ybyte, 3, Len(ybyte) - 2)
 
 Case Is = Chr(8):      ' 255er folge
  For x = 1 To Asc(Mid(ybyte, 2, 1))
   xbyte = xbyte & Chr(255)
  Next x
  ybyte = Mid(ybyte, 3, Len(ybyte) - 2)
  
 Case Else:
  MsgBox "An Error occured"
  
 End Select
 DoEvents
Loop

'Text2.Text = xbyte

End Sub

