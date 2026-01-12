VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Easy Stegano"
   ClientHeight    =   6090
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   10920
   LinkTopic       =   "Form1"
   ScaleHeight     =   6090
   ScaleWidth      =   10920
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Command2 
      Caption         =   "Finden"
      Height          =   375
      Left            =   5520
      TabIndex        =   7
      Top             =   5640
      Width           =   5295
   End
   Begin VB.CommandButton Command1 
      Caption         =   "Verstecken"
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
      Text            =   "STEGANO.frx":0000
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
      Text            =   "STEGANO.frx":002F
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
      Caption         =   "Geheimtext :"
      Height          =   255
      Left            =   5520
      TabIndex        =   4
      Top             =   720
      Width           =   975
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

' Zuerst Generieren wir zwei Ziffern aus dem
' Passwort, die angeben wo in dem neuen Text unser
' Originaltext verteckt ist
' hier eine simple Methode der CRC-Bildung,
' da der Endwert auf 1-32 beschränkt ist


' Anfangsstelle
z = 0
For x = 1 To Len(Text1.Text)
 z = Asc(Mid(Text1.Text, x, 1)) + z
Next x

stelle = (z Mod 32) + 1


' Zum verschieben, da ansonsten der Text in einer
' Spalte stehen würd und dadurch leicht zu entdecken ist

z = stelle + z + x
For x = 1 To Len(Text1.Text)
 z = Asc(Mid(Text1.Text, x, 1)) + x + z
Next x

Stelle2 = (z Mod 32) + 1


' Für jeden Buchstaben des Originaltextes erstellen
' wir nun 8 4er Päärchen Zufallszahlen, in dem
' wir später unseren Buchstaben verstecken, die Botschaft
' ist also 32 mal so groß wie der Originaltext

Text3.Text = ""
Randomize


For x = 1 To Len(Text2.Text)
 tempstr = ""
 
 For y = 1 To 32
  
  If y = stelle Then
   ' Echter Buchstabe
   tempstr = tempstr & Mid(Text2.Text, x, 1)
  Else
   ' Zufallsbuchstabe
   tempstr = tempstr & Chr$((Rnd * 94) + 32)
  End If
  
 Next y

  ' In Textbox ausgeben und Zeilenumbruch hinzufügen
 Text3.Text = Text3.Text & tempstr & vbCrLf
 
  ' Stelle verschieben
 stelle = ((stelle + Stelle2) Mod 32) + 1
Next x

End Sub

Private Sub Command2_Click()

' Wir erstellen wieder unsere beiden Zahlen
' Passwort, die angeben wo in dem neuen Text unser

' Anfangsstelle
z = 0
For x = 1 To Len(Text1.Text)
 z = Asc(Mid(Text1.Text, x, 1)) + z
Next x

stelle = (z Mod 32) + 1


' Zum verschieben

z = stelle + z + x
For x = 1 To Len(Text1.Text)
 z = Asc(Mid(Text1.Text, x, 1)) + x + z
Next x

Stelle2 = (z Mod 32) + 1


Text2.Text = ""

' raussuchen des Buchstabens aus dem Text

x = Len(Text3.Text) / 34
For z = 0 To x
 tempstr = Mid(Text3.Text, (z * 34) + 1, 32)
 Text2.Text = Text2.Text & Mid(tempstr, stelle, 1)
 stelle = ((stelle + Stelle2) Mod 32) + 1
Next z
 
End Sub
