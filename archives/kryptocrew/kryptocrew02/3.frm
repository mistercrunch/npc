VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Vinegere Verschlüsselung"
   ClientHeight    =   6090
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   10920
   LinkTopic       =   "Form1"
   ScaleHeight     =   6090
   ScaleWidth      =   10920
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Command2 
      Caption         =   "Entschlüsseln"
      Height          =   375
      Left            =   5520
      TabIndex        =   7
      Top             =   5640
      Width           =   5295
   End
   Begin VB.CommandButton Command1 
      Caption         =   "Verschlüsseln"
      Height          =   375
      Left            =   120
      TabIndex        =   6
      Top             =   5640
      Width           =   5295
   End
   Begin VB.TextBox Text3 
      BeginProperty Font 
         Name            =   "Courier"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   4575
      Left            =   5520
      MultiLine       =   -1  'True
      ScrollBars      =   3  'Both
      TabIndex        =   5
      Top             =   960
      Width           =   5295
   End
   Begin VB.TextBox Text2 
      BeginProperty Font 
         Name            =   "Courier"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   4575
      Left            =   120
      MultiLine       =   -1  'True
      ScrollBars      =   3  'Both
      TabIndex        =   3
      Top             =   960
      Width           =   5295
   End
   Begin VB.TextBox Text1 
      Height          =   285
      Left            =   1080
      TabIndex        =   0
      Text            =   "Passwort"
      Top             =   240
      Width           =   8895
   End
   Begin VB.Label Label3 
      Caption         =   "Verschlüsselter Text : "
      Height          =   255
      Left            =   5520
      TabIndex        =   4
      Top             =   720
      Width           =   1815
   End
   Begin VB.Label Label2 
      Caption         =   "Originaltext :"
      Height          =   255
      Left            =   120
      TabIndex        =   2
      Top             =   720
      Width           =   2055
   End
   Begin VB.Label Label1 
      Caption         =   "Passwort :"
      Height          =   255
      Left            =   120
      TabIndex        =   1
      Top             =   240
      Width           =   1215
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'text1 enthält das Passwort
'text2 den Originaltext, der versteckt werden soll
'text3 den versteckten text


Private Sub Command1_Click()

MainString = ""         ' String mit allen verfügbaren Zeichen laden
For x = 32 To 126
 MainString = MainString & Chr$(x)
Next x

Text3.Text = ""
Counter = 1
For i = 1 To Len(Text2.Text)
                                     ' Buchstaben ermitteln, der verschlüsselt werden soll ( ASCII Wert )
 x = Asc(Mid(Text2.Text, i, 1)) - 32
 If x > 94 Then x = 32
 
                                     ' Passwortbuchstaben ermitteln ( ASCII )
 y = Asc(Mid(Text1.Text, Counter, 1)) - 32
 If y > 94 Then y = 32
                                   ' Neues Alphabet erzeugen
 SubString = Mid(MainString, y, Len(MainString) - y) & Mid(MainString, 1, y)
                                   ' Buchstaben auslesen
 Text3.Text = Text3.Text & Mid(SubString, x + 1, 1)
 
                                   ' Passwort Counter erhöhen
 Counter = Counter + 1
 If Counter > Len(Text1.Text) Then Counter = 1
Next i                             ' Text-Counter erhöhen1

End Sub

Private Sub Command2_Click()

MainString = ""         ' String mit allen verfügbaren Zeichen laden
For x = 32 To 126
 MainString = MainString & Chr$(x)
Next x

Text2.Text = ""
Counter = 1
For i = 1 To Len(Text3.Text)
                                    ' Passwortbuchstaben ermitteln ( ASCII )
 y = Asc(Mid(Text1.Text, Counter, 1)) - 32
 If y > 94 Then y = 32
                                   ' Neues Alphabet erzeugen
 SubString = Mid(MainString, y, Len(MainString) - y) & Mid(MainString, 1, y)
                                   
                                    ' Stelle and der der Buchstabe im neuen Alphabet steht
 z = InStr(1, SubString, (Mid(Text3.Text, i, 1)), vbBinaryCompare)
                                   
                                   ' Buchstaben aus dem echten ABC auslesen & speichern
 Text2.Text = Text2.Text & Mid(MainString, z, 1)
 
                                   ' Passwort Counter erhöhen
 Counter = Counter + 1
 If Counter > Len(Text1.Text) Then Counter = 1
Next i                             ' Text-Counter erhöhen1

End Sub
