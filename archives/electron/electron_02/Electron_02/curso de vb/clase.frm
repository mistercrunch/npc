VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Intro a Visual Basic"
   ClientHeight    =   3195
   ClientLeft      =   165
   ClientTop       =   450
   ClientWidth     =   5160
   LinkTopic       =   "Form1"
   ScaleHeight     =   3195
   ScaleWidth      =   5160
   StartUpPosition =   2  'CenterScreen
   Begin VB.Frame Frame1 
      Caption         =   "OS que Usas"
      Height          =   1335
      Left            =   120
      TabIndex        =   5
      Top             =   1680
      Width           =   4815
      Begin VB.OptionButton Option2 
         Caption         =   "Linux"
         Height          =   495
         Left            =   1320
         TabIndex        =   9
         Top             =   480
         Width           =   1095
      End
      Begin VB.OptionButton Option1 
         Caption         =   "Windows"
         Height          =   495
         Left            =   120
         TabIndex        =   8
         Top             =   480
         Width           =   1095
      End
      Begin VB.CommandButton Command4 
         Caption         =   "Status"
         Height          =   375
         Left            =   2520
         TabIndex        =   7
         Top             =   480
         Width           =   2055
      End
      Begin VB.Label Label3 
         Alignment       =   2  'Center
         BeginProperty Font 
            Name            =   "Courier New"
            Size            =   9.75
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   255
         Left            =   120
         TabIndex        =   6
         Top             =   960
         Width           =   4575
      End
   End
   Begin VB.CommandButton Command3 
      Caption         =   "Input Box"
      Height          =   375
      Left            =   120
      TabIndex        =   3
      Top             =   1080
      Width           =   1335
   End
   Begin VB.CommandButton Command2 
      Caption         =   "Msg Box"
      Height          =   375
      Left            =   120
      TabIndex        =   2
      Top             =   600
      Width           =   1335
   End
   Begin VB.CommandButton Command1 
      Caption         =   "Calculadora"
      Height          =   375
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   1335
   End
   Begin VB.Label Label2 
      Alignment       =   2  'Center
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   1560
      TabIndex        =   4
      Top             =   1080
      Width           =   3015
   End
   Begin VB.Label Label1 
      Alignment       =   2  'Center
      BeginProperty Font 
         Name            =   "Courier New"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   1560
      TabIndex        =   1
      Top             =   120
      Width           =   3015
   End
   Begin VB.Menu archivo 
      Caption         =   "&Archivo"
      Begin VB.Menu sonido 
         Caption         =   "S&onido"
      End
      Begin VB.Menu salir 
         Caption         =   "&Salir"
      End
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Command1_Click()
    Label1.Caption = "La Calculadora se Ejecuto"
    Dim a
    a = Shell("C:\windows\calc.exe")
End Sub

Private Sub Command2_Click()
Dim b
b = MsgBox("31337 Msg Box")
End Sub

Private Sub Command3_Click()
Dim c As String
c = InputBox("Ingresa tu Nick...")
Label2.Caption = c
End Sub

Private Sub Command4_Click()
    If Option1 Then
    Label3.Caption = "Ocupas Windows"
    End If
    
    If Option2 Then
    Label3.Caption = "Ocupas Linux"
    End If
End Sub

Private Sub salir_Click()
Unload Me
End Sub

Private Sub sonido_Click()
Beep
End Sub
