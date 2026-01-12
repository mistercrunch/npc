VERSION 5.00
Object = "{48E59290-9880-11CF-9754-00AA00C00908}#1.0#0"; "MSINET.OCX"
Begin VB.Form FTP 
   BackColor       =   &H00FFFFFF&
   Caption         =   "FTP Client v1.0 para Win32, por bankzide - www.est.cl"
   ClientHeight    =   3825
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   7170
   LinkTopic       =   "Form1"
   ScaleHeight     =   3825
   ScaleWidth      =   7170
   StartUpPosition =   2  'CenterScreen
   Begin VB.CommandButton cmdNuevoHost 
      BackColor       =   &H00FFFFFF&
      Caption         =   "&Nuevo Host"
      Enabled         =   0   'False
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   5280
      Style           =   1  'Graphical
      TabIndex        =   15
      Top             =   2760
      Width           =   1695
   End
   Begin VB.CommandButton cmdSalir 
      BackColor       =   &H00FFFFFF&
      Caption         =   "&Salir"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   5280
      Style           =   1  'Graphical
      TabIndex        =   14
      Top             =   3240
      Width           =   1695
   End
   Begin VB.TextBox txtCopy 
      Enabled         =   0   'False
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   285
      Left            =   5160
      TabIndex        =   13
      Top             =   600
      Width           =   1935
   End
   Begin VB.CommandButton cmdConectar 
      BackColor       =   &H00FFFFFF&
      Caption         =   "&Conectar..."
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   375
      Left            =   480
      Style           =   1  'Graphical
      TabIndex        =   10
      Top             =   2160
      Width           =   1695
   End
   Begin VB.ListBox lstArchivos 
      Enabled         =   0   'False
      Height          =   2985
      Left            =   3000
      TabIndex        =   0
      Top             =   600
      Width           =   2055
   End
   Begin VB.TextBox txtPassword 
      Alignment       =   2  'Center
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   285
      IMEMode         =   3  'DISABLE
      Left            =   1440
      PasswordChar    =   "*"
      TabIndex        =   9
      Top             =   1560
      Width           =   1215
   End
   Begin VB.TextBox txtUsuario 
      Alignment       =   2  'Center
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   285
      Left            =   1080
      TabIndex        =   8
      Top             =   1200
      Width           =   1575
   End
   Begin VB.TextBox txtPuerto 
      Alignment       =   2  'Center
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   285
      Left            =   960
      TabIndex        =   7
      Text            =   "21"
      Top             =   840
      Width           =   615
   End
   Begin VB.TextBox txtHost 
      Alignment       =   2  'Center
      Height          =   285
      Left            =   960
      TabIndex        =   6
      Top             =   480
      Width           =   1695
   End
   Begin VB.PictureBox Picture1 
      Appearance      =   0  'Flat
      BackColor       =   &H80000005&
      BorderStyle     =   0  'None
      ForeColor       =   &H80000008&
      Height          =   855
      Left            =   240
      Picture         =   "Form1.frx":0000
      ScaleHeight     =   855
      ScaleWidth      =   2295
      TabIndex        =   1
      Top             =   2760
      Width           =   2295
   End
   Begin InetCtlsObjects.Inet Inet1 
      Left            =   6600
      Top             =   0
      _ExtentX        =   1005
      _ExtentY        =   1005
      _Version        =   393216
      AccessType      =   1
      Protocol        =   2
      RemotePort      =   21
      URL             =   "ftp://"
   End
   Begin VB.Label Label7 
      BackColor       =   &H00FFFFFF&
      Caption         =   $"Form1.frx":0AF2
      Height          =   1695
      Left            =   5280
      TabIndex        =   16
      Top             =   960
      Width           =   1815
   End
   Begin VB.Label Label6 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Copiar a:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   5160
      TabIndex        =   12
      Top             =   240
      Width           =   1215
   End
   Begin VB.Label Label5 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Archivos:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   3000
      TabIndex        =   11
      Top             =   240
      Width           =   1215
   End
   Begin VB.Shape Shape1 
      BorderWidth     =   2
      Height          =   1815
      Left            =   120
      Top             =   240
      Width           =   2655
   End
   Begin VB.Label Label4 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Contraseña:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   240
      TabIndex        =   5
      Top             =   1560
      Width           =   1095
   End
   Begin VB.Label Label3 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Usuario:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   240
      TabIndex        =   4
      Top             =   1200
      Width           =   735
   End
   Begin VB.Label Label2 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Puerto:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   240
      TabIndex        =   3
      Top             =   840
      Width           =   615
   End
   Begin VB.Label Label1 
      BackColor       =   &H00FFFFFF&
      Caption         =   "Host:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   240
      TabIndex        =   2
      Top             =   480
      Width           =   495
   End
End
Attribute VB_Name = "FTP"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub cmdConectar_Click()
lstArchivos.Enabled = True
txtCopy.Enabled = True
cmdNuevoHost.Enabled = True
cmdConectar.Enabled = False
txtHost.Enabled = False
txtPuerto.Enabled = False
txtUsuario.Enabled = False
txtPassword.Enabled = False

On Error GoTo errorhandler
Dim varList As Variant

  With Inet1
   .URL = txtHost.Text
   .RemotePort = txtPuerto.Text
   .UserName = txtUsuario.Text
   .Password = txtPassword.Text
   .Execute , "DIR"
      
   varList = .GetChunk(1024)
   subMostrarFiles (varList)
  End With
  
errorhandler:
 Select Case Err.Number
  Case 35764
   DoEvents
   Resume
 End Select
End Sub

Private Sub cmdNuevoHost_Click()
txtHost.Enabled = True
txtPuerto.Enabled = True
txtUsuario.Enabled = True
txtPassword.Enabled = True
cmdConectar.Enabled = True
cmdNuevoHost.Enabled = False
lstArchivos.Enabled = False
txtCopy.Enabled = False
txtPassword.Text = ""
txtCopy.Text = ""
lstArchivos.Clear
End Sub

Private Sub cmdSalir_Click()
Unload Me
End
End Sub

Public Sub subMostrarFiles(var As Variant)

  Dim strArreglo() As String
  Dim Num As Integer
   lstArchivos.Clear
   
   strArreglo = Split(CStr(var), Chr(13) & Chr(10))
   lstArchivos.AddItem ("../")
   For Num = 0 To UBound(strArreglo)
      lstArchivos.AddItem (strArreglo(Num))
   Next
End Sub



Private Sub lstArchivos_DblClick()
On Error GoTo errorhandler

Dim arch As String

MousePointer = vbHourglass

With Inet1
 If (Left(lstArchivos.Text, 2) = "./" Or Left(lstArchivos.Text, 3) = "../" Or Right(lstArchivos.Text, 1) = "/") Then
   
   .Execute , "cd " & lstArchivos.Text
   .Execute , "DIR"
   subMostrarFiles (.GetChunk(1024))
   
 Else
    arch = subNomExit
   .Execute , "get " & lstArchivos.Text & " " & arch
   .Execute , "pwd"
   MsgBox "Descarga Completa!"
   End If
End With

MousePointer = vbDefault

errorhandler:
   Select Case Err.Number
   Case 35764
   DoEvents
   Resume
   End Select
End Sub

Private Function subNomExit() As String

 If Right(txtCopy.Text, 1) = "\" Then
   subNomExit = txtCopy.Text & lstArchivos.Text
 ElseIf Right(txtCopy.Text, 1) = ":" Then
   subNomExit = txtCopy.Text & "\" & lstArchivos.Text
 ElseIf txtCopy.Text = "" Then
   subNomExit = lstArchivos.Text
 Else
   subNomExit = txtCopy.Text
 End If
 
End Function
