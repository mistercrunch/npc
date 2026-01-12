VERSION 5.00
Begin VB.Form Form1 
   BackColor       =   &H00800000&
   BorderStyle     =   1  'Fixed Single
   Caption         =   "Interprete by RandomLoG ( DisidentS Ezine N5 )"
   ClientHeight    =   2775
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   6135
   Icon            =   "Form1.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   ScaleHeight     =   2775
   ScaleWidth      =   6135
   StartUpPosition =   2  'CenterScreen
   Begin VB.ComboBox Combo1 
      Appearance      =   0  'Flat
      BackColor       =   &H00FF0000&
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   315
      Left            =   2400
      Sorted          =   -1  'True
      TabIndex        =   8
      Text            =   "Instrucciones"
      Top             =   1800
      Width           =   3255
   End
   Begin VB.TextBox Text3 
      Appearance      =   0  'Flat
      Height          =   285
      Left            =   2040
      TabIndex        =   2
      Text            =   "Argentina"
      Top             =   960
      Width           =   3975
   End
   Begin VB.TextBox Text2 
      Appearance      =   0  'Flat
      Height          =   285
      Left            =   2040
      TabIndex        =   1
      Text            =   "DisidentS"
      Top             =   600
      Width           =   3975
   End
   Begin VB.TextBox Text1 
      Appearance      =   0  'Flat
      Height          =   285
      Left            =   2040
      TabIndex        =   0
      Text            =   "RandomLoG"
      Top             =   240
      Width           =   3975
   End
   Begin VB.Label Label2 
      Alignment       =   2  'Center
      BackStyle       =   0  'Transparent
      Caption         =   "RandomLoG * DisidentS Argentina ( xxmorochoxx@aol.com )"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H0000FF00&
      Height          =   255
      Left            =   240
      TabIndex        =   9
      Top             =   2400
      Width           =   5655
   End
   Begin VB.Label Label1 
      AutoSize        =   -1  'True
      BackStyle       =   0  'Transparent
      Caption         =   "Instrucciones Soportadas:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   195
      Index           =   3
      Left            =   120
      TabIndex        =   7
      Top             =   1800
      Width           =   2235
   End
   Begin VB.Label Command1 
      Alignment       =   2  'Center
      BackStyle       =   0  'Transparent
      Caption         =   "InterPretaR!!!"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   18
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H0000FFFF&
      Height          =   435
      Left            =   0
      TabIndex        =   6
      Top             =   1320
      Width           =   6105
   End
   Begin VB.Label Label1 
      AutoSize        =   -1  'True
      BackStyle       =   0  'Transparent
      Caption         =   "Argumento Numero 2:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   195
      Index           =   2
      Left            =   120
      TabIndex        =   5
      Top             =   960
      Width           =   1845
   End
   Begin VB.Label Label1 
      AutoSize        =   -1  'True
      BackStyle       =   0  'Transparent
      Caption         =   "Argumento Numero 1:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   195
      Index           =   1
      Left            =   120
      TabIndex        =   4
      Top             =   600
      Width           =   1845
   End
   Begin VB.Label Label1 
      AutoSize        =   -1  'True
      BackStyle       =   0  'Transparent
      Caption         =   "Instruccion a emular:"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      ForeColor       =   &H00FFFFFF&
      Height          =   195
      Index           =   0
      Left            =   120
      TabIndex        =   3
      Top             =   240
      Width           =   1800
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'=============== Disidents Argentina =================================
'===============      RandomLoG      =================================
'==============       Ezine N 5      =================================


Private Sub Command1_Click()
 On Error GoTo Rnd ' si hay error vamos a la etiqueta
 Select Case LCase(Trim$(Text1.Text)) ' Segun, el texto en minusculas
  Case "msgbox": CallByName Me, "cuadro_d_msg", VbMethod, Text2.Text, Text3.Text ' si es = a msgbox ejecuta cuadro_d_msg
  Case "instr": CallByName Me, "buscar_subcadena", VbMethod, Text2.Text, Text3.Text
  Case "split": CallByName Me, "splitea", VbMethod, Text2.Text, Text3.Text
 End Select
 Exit Sub
Rnd: ' etiqueta de error
 MsgBox "La Instruccion a emular no ha sido reconocida!", vbCritical ' damos una alerta de que no existe la funcion
 Form_Load ' Limpiamos todo
End Sub

Public Sub cuadro_d_msg(Arg1 As String, arg2)
 arg2 = Replace(arg2, """", "") ' Quitamos comillas, si las hay
 MsgBox Arg1, Val(arg2), arg2
End Sub

Public Sub splitea(Arg1 As String, arg2 As String)
 arg3 = Split(Arg1, arg2)
 For a = 0 To UBound(arg3)
  datos = datos & vbCrLf & "arg3[" & a & "] " & arg3(a)
 Next a
 MsgBox datos
End Sub
Public Sub suma(Arg1, arg2)
 cuadro_d_msg Val(Arg1) + Val(arg2), vbInformation
End Sub

Public Sub resta(Arg1, arg2)
 cuadro_d_msg Val(Arg1) - Val(arg2), vbInformation
End Sub

Public Sub divide(Arg1, arg2)
 If Val(Arg1) <> 0 And Val(arg2) <> 0 Then cuadro_d_msg Val(Arg1) / Val(arg2), vbInformation Else: cuadro_d_msg "No se puede dividir por Cero!", vbInformation
End Sub

Public Sub multiplica(Arg1, arg2)
 cuadro_d_msg Val(Arg1) * Val(arg2), vbInformation
End Sub

Public Sub buscar_subcadena(Arg1, arg2)
  cuadro_d_msg InStr(1, Arg1, arg2), vbOKOnly
End Sub

Private Sub Form_Load()
 Dim RandomLoG As Control ' Dimensionamos la variable RandomLoG como Control
 For Each RandomLoG In Controls ' recorremos todos los controles dentro del form
  If TypeOf RandomLoG Is TextBox Then ' si es un textbox entonces
   RandomLoG = "" ' quitamos cualquier texto
   RandomLoG.ForeColor = &HFFFFFF ' Ponemos letras blancas
   RandomLoG.BackColor = &HFF0000 ' Ponemos fondo azul
  End If ' cerramos condicion
 Next ' proximo objeto, si lo hay
 Combo1.Clear ' Limpiamos el/los items que haya dentro del ComboBox
  ' Agregramso Items
 Combo1.AddItem "Instr"
 Combo1.AddItem "MsgBox"
 Combo1.AddItem "Suma"
 Combo1.AddItem "Resta"
 Combo1.AddItem "Divide"
 Combo1.AddItem "Multiplica"
 Combo1.AddItem "Split"
 Combo1.Text = "Instrucciones Soportadas" ' le damo un toke ma de pinta :)
End Sub
