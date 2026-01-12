Option Strict Off
Option Explicit On
Friend Class Form1
	Inherits System.Windows.Forms.Form
#Region "Windows Form Designer generated code "
	Public Sub New()
		MyBase.New()
		If m_vb6FormDefInstance Is Nothing Then
			If m_InitializingDefInstance Then
				m_vb6FormDefInstance = Me
			Else
				Try 
					'For the start-up form, the first instance created is the default instance.
					If System.Reflection.Assembly.GetExecutingAssembly.EntryPoint.DeclaringType Is Me.GetType Then
						m_vb6FormDefInstance = Me
					End If
				Catch
				End Try
			End If
		End If
		'This call is required by the Windows Form Designer.
		InitializeComponent()
	End Sub
	'Form overrides dispose to clean up the component list.
	Protected Overloads Overrides Sub Dispose(ByVal Disposing As Boolean)
		If Disposing Then
			If Not components Is Nothing Then
				components.Dispose()
			End If
		End If
		MyBase.Dispose(Disposing)
	End Sub
    Private components As System.ComponentModel.IContainer
	'Required by the Windows Form Designer
		Public ToolTip1 As System.Windows.Forms.ToolTip
	Public WithEvents Combo1 As System.Windows.Forms.ComboBox
	Public WithEvents Text3 As System.Windows.Forms.TextBox
	Public WithEvents Text2 As System.Windows.Forms.TextBox
	Public WithEvents Text1 As System.Windows.Forms.TextBox
	Public WithEvents Label2 As System.Windows.Forms.Label
	Public WithEvents _Label1_3 As System.Windows.Forms.Label
	Public WithEvents Command1 As System.Windows.Forms.Label
	Public WithEvents _Label1_2 As System.Windows.Forms.Label
	Public WithEvents _Label1_1 As System.Windows.Forms.Label
	Public WithEvents _Label1_0 As System.Windows.Forms.Label
	Public WithEvents Label1 As Microsoft.VisualBasic.Compatibility.VB6.LabelArray
	'NOTE: The following procedure is required by the Windows Form Designer
	'It can be modified using the Windows Form Designer.
	'Do not modify it using the code editor.
	<System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container()
        Dim resources As System.Resources.ResourceManager = New System.Resources.ResourceManager(GetType(Form1))
        Me.ToolTip1 = New System.Windows.Forms.ToolTip(Me.components)
        Me.Label1 = New Microsoft.VisualBasic.Compatibility.VB6.LabelArray(Me.components)
        Me._Label1_3 = New System.Windows.Forms.Label()
        Me._Label1_2 = New System.Windows.Forms.Label()
        Me._Label1_1 = New System.Windows.Forms.Label()
        Me._Label1_0 = New System.Windows.Forms.Label()
        Me.Text2 = New System.Windows.Forms.TextBox()
        Me.Text1 = New System.Windows.Forms.TextBox()
        Me.Command1 = New System.Windows.Forms.Label()
        Me.Label2 = New System.Windows.Forms.Label()
        Me.Text3 = New System.Windows.Forms.TextBox()
        Me.Combo1 = New System.Windows.Forms.ComboBox()
        Me.SuspendLayout()
        '
        '_Label1_3
        '
        Me._Label1_3.AutoSize = True
        Me._Label1_3.BackColor = System.Drawing.Color.Transparent
        Me._Label1_3.Cursor = System.Windows.Forms.Cursors.Default
        Me._Label1_3.ForeColor = System.Drawing.Color.White
        Me.Label1.SetIndex(Me._Label1_3, CType(3, Short))
        Me._Label1_3.Location = New System.Drawing.Point(8, 120)
        Me._Label1_3.Name = "_Label1_3"
        Me._Label1_3.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me._Label1_3.Size = New System.Drawing.Size(135, 13)
        Me._Label1_3.TabIndex = 7
        Me._Label1_3.Text = "Instrucciones Soportadas:"
        '
        '_Label1_2
        '
        Me._Label1_2.AutoSize = True
        Me._Label1_2.BackColor = System.Drawing.Color.Transparent
        Me._Label1_2.Cursor = System.Windows.Forms.Cursors.Default
        Me._Label1_2.ForeColor = System.Drawing.Color.White
        Me.Label1.SetIndex(Me._Label1_2, CType(2, Short))
        Me._Label1_2.Location = New System.Drawing.Point(8, 64)
        Me._Label1_2.Name = "_Label1_2"
        Me._Label1_2.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me._Label1_2.Size = New System.Drawing.Size(115, 13)
        Me._Label1_2.TabIndex = 5
        Me._Label1_2.Text = "Argumento Numero 2:"
        '
        '_Label1_1
        '
        Me._Label1_1.AutoSize = True
        Me._Label1_1.BackColor = System.Drawing.Color.Transparent
        Me._Label1_1.Cursor = System.Windows.Forms.Cursors.Default
        Me._Label1_1.ForeColor = System.Drawing.Color.White
        Me.Label1.SetIndex(Me._Label1_1, CType(1, Short))
        Me._Label1_1.Location = New System.Drawing.Point(8, 40)
        Me._Label1_1.Name = "_Label1_1"
        Me._Label1_1.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me._Label1_1.Size = New System.Drawing.Size(115, 13)
        Me._Label1_1.TabIndex = 4
        Me._Label1_1.Text = "Argumento Numero 1:"
        '
        '_Label1_0
        '
        Me._Label1_0.AutoSize = True
        Me._Label1_0.BackColor = System.Drawing.Color.Transparent
        Me._Label1_0.Cursor = System.Windows.Forms.Cursors.Default
        Me._Label1_0.ForeColor = System.Drawing.Color.White
        Me.Label1.SetIndex(Me._Label1_0, CType(0, Short))
        Me._Label1_0.Location = New System.Drawing.Point(8, 16)
        Me._Label1_0.Name = "_Label1_0"
        Me._Label1_0.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me._Label1_0.Size = New System.Drawing.Size(109, 13)
        Me._Label1_0.TabIndex = 3
        Me._Label1_0.Text = "Instruccion a emular:"
        '
        'Text2
        '
        Me.Text2.AcceptsReturn = True
        Me.Text2.AutoSize = False
        Me.Text2.BackColor = System.Drawing.SystemColors.Window
        Me.Text2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Text2.Cursor = System.Windows.Forms.Cursors.IBeam
        Me.Text2.ForeColor = System.Drawing.SystemColors.WindowText
        Me.Text2.Location = New System.Drawing.Point(136, 40)
        Me.Text2.MaxLength = 0
        Me.Text2.Name = "Text2"
        Me.Text2.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Text2.Size = New System.Drawing.Size(265, 19)
        Me.Text2.TabIndex = 1
        Me.Text2.Text = "DisidentS"
        '
        'Text1
        '
        Me.Text1.AcceptsReturn = True
        Me.Text1.AutoSize = False
        Me.Text1.BackColor = System.Drawing.SystemColors.Window
        Me.Text1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Text1.Cursor = System.Windows.Forms.Cursors.IBeam
        Me.Text1.ForeColor = System.Drawing.SystemColors.WindowText
        Me.Text1.Location = New System.Drawing.Point(136, 16)
        Me.Text1.MaxLength = 0
        Me.Text1.Name = "Text1"
        Me.Text1.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Text1.Size = New System.Drawing.Size(265, 19)
        Me.Text1.TabIndex = 0
        Me.Text1.Text = "RandomLoG"
        '
        'Command1
        '
        Me.Command1.BackColor = System.Drawing.Color.Transparent
        Me.Command1.Cursor = System.Windows.Forms.Cursors.Default
        Me.Command1.Font = New System.Drawing.Font("Microsoft Sans Serif", 18!, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Command1.ForeColor = System.Drawing.Color.Yellow
        Me.Command1.Location = New System.Drawing.Point(0, 88)
        Me.Command1.Name = "Command1"
        Me.Command1.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Command1.Size = New System.Drawing.Size(407, 29)
        Me.Command1.TabIndex = 6
        Me.Command1.Text = "InterPretaR!!!"
        Me.Command1.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'Label2
        '
        Me.Label2.BackColor = System.Drawing.Color.Transparent
        Me.Label2.Cursor = System.Windows.Forms.Cursors.Default
        Me.Label2.ForeColor = System.Drawing.Color.LawnGreen
        Me.Label2.Location = New System.Drawing.Point(16, 160)
        Me.Label2.Name = "Label2"
        Me.Label2.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Label2.Size = New System.Drawing.Size(377, 17)
        Me.Label2.TabIndex = 9
        Me.Label2.Text = "RandomLoG * DisidentS Argentina ( xxmorochoxx@aol.com )"
        Me.Label2.TextAlign = System.Drawing.ContentAlignment.TopCenter
        '
        'Text3
        '
        Me.Text3.AcceptsReturn = True
        Me.Text3.AutoSize = False
        Me.Text3.BackColor = System.Drawing.SystemColors.Window
        Me.Text3.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.Text3.Cursor = System.Windows.Forms.Cursors.IBeam
        Me.Text3.ForeColor = System.Drawing.SystemColors.WindowText
        Me.Text3.Location = New System.Drawing.Point(136, 64)
        Me.Text3.MaxLength = 0
        Me.Text3.Name = "Text3"
        Me.Text3.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Text3.Size = New System.Drawing.Size(265, 19)
        Me.Text3.TabIndex = 2
        Me.Text3.Text = "Argentina"
        '
        'Combo1
        '
        Me.Combo1.BackColor = System.Drawing.Color.Blue
        Me.Combo1.Cursor = System.Windows.Forms.Cursors.Default
        Me.Combo1.DropDownWidth = 217
        Me.Combo1.ForeColor = System.Drawing.Color.White
        Me.Combo1.Location = New System.Drawing.Point(160, 120)
        Me.Combo1.Name = "Combo1"
        Me.Combo1.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Combo1.Size = New System.Drawing.Size(217, 21)
        Me.Combo1.Sorted = True
        Me.Combo1.TabIndex = 8
        Me.Combo1.Text = "Instrucciones"
        '
        'Form1
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.BackColor = System.Drawing.Color.FromArgb(CType(0, Byte), CType(0, Byte), CType(128, Byte))
        Me.ClientSize = New System.Drawing.Size(409, 185)
        Me.Controls.AddRange(New System.Windows.Forms.Control() {Me.Combo1, Me.Text3, Me.Text2, Me.Text1, Me.Label2, Me._Label1_3, Me.Command1, Me._Label1_2, Me._Label1_1, Me._Label1_0})
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.Location = New System.Drawing.Point(3, 22)
        Me.MaximizeBox = False
        Me.Name = "Form1"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "Interprete by RandomLoG ( DisidentS Ezine N5 )"
        Me.ResumeLayout(False)

    End Sub
#End Region 
#Region "Upgrade Support "
	Private Shared m_vb6FormDefInstance As Form1
	Private Shared m_InitializingDefInstance As Boolean
	Public Shared Property DefInstance() As Form1
		Get
			If m_vb6FormDefInstance Is Nothing OrElse m_vb6FormDefInstance.IsDisposed Then
				m_InitializingDefInstance = True
				m_vb6FormDefInstance = New Form1()
				m_InitializingDefInstance = False
			End If
			DefInstance = m_vb6FormDefInstance
		End Get
		Set
			m_vb6FormDefInstance = Value
		End Set
	End Property
#End Region 
	'=============== Disidents Argentina =================================
	'===============      RandomLoG      =================================
	'==============       Ezine N 5      =================================
	
	
	Private Sub Command1_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles Command1.Click
		On Error GoTo Rnd_Renamed ' si hay error vamos a la etiqueta
		Select Case LCase(Trim(Text1.Text)) ' Segun, el texto en minusculas
			Case "msgbox" : CallByName(Me, "cuadro_d_msg", CallType.Method, Text2.Text, Text3.Text) ' si es = a msgbox ejecuta cuadro_d_msg
			Case "instr" : CallByName(Me, "buscar_subcadena", CallType.Method, Text2.Text, Text3.Text)
			Case "split" : CallByName(Me, "splitea", CallType.Method, Text2.Text, Text3.Text)
		End Select
		Exit Sub
Rnd_Renamed: ' etiqueta de error
		MsgBox("La Instruccion a emular no ha sido reconocida!", MsgBoxStyle.Critical) ' damos una alerta de que no existe la funcion
		Form1_Load(Me, New System.EventArgs()) ' Limpiamos todo
	End Sub
	
	Public Sub cuadro_d_msg(ByRef Arg1 As String, ByRef arg2 As Object)
        arg2 = Replace(arg2, """", "") ' Quitamos comillas, si las hay
        MsgBox(Arg1, Val(arg2), arg2)
	End Sub
	
	Public Sub splitea(ByRef Arg1 As String, ByRef arg2 As String)
		Dim datos As Object
		Dim a As Object
		Dim arg3 As Object
        arg3 = Split(Arg1, arg2)
		For a = 0 To UBound(arg3)
            datos = datos & ControlChars.CrLf & "arg3[" & a & "] " & arg3(a)
		Next a
		MsgBox(datos)
	End Sub
	Public Sub suma(ByRef Arg1 As Object, ByRef arg2 As Object)
        cuadro_d_msg(CStr(Val(Arg1) + Val(arg2)), MsgBoxStyle.Information)
	End Sub
	
	Public Sub resta(ByRef Arg1 As Object, ByRef arg2 As Object)
        cuadro_d_msg(CStr(Val(Arg1) - Val(arg2)), MsgBoxStyle.Information)
	End Sub
	
	Public Sub divide(ByRef Arg1 As Object, ByRef arg2 As Object)
        If Val(Arg1) <> 0 And Val(arg2) <> 0 Then
            cuadro_d_msg(CStr(Val(Arg1) / Val(arg2)), MsgBoxStyle.Information)
        Else : cuadro_d_msg("No se puede dividir por Cero!", MsgBoxStyle.Information)
        End If
	End Sub
	
	Public Sub multiplica(ByRef Arg1 As Object, ByRef arg2 As Object)
        cuadro_d_msg(CStr(Val(Arg1) * Val(arg2)), MsgBoxStyle.Information)
	End Sub
	
	Public Sub buscar_subcadena(ByRef Arg1 As Object, ByRef arg2 As Object)
        cuadro_d_msg(CStr(InStr(1, Arg1, arg2)), MsgBoxStyle.OKOnly)
	End Sub
	
	Private Sub Form1_Load(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles MyBase.Load
		Dim RandomLoG As System.Windows.Forms.Control ' Dimensionamos la variable RandomLoG como Control
		For	Each RandomLoG In Controls ' recorremos todos los controles dentro del form
            If TypeOf RandomLoG Is System.Windows.Forms.TextBox Then ' si es un textbox entonces
                RandomLoG.Text = ""  ' quitamos cualquier texto
                RandomLoG.ForeColor = System.Drawing.ColorTranslator.FromOle(&HFFFFFF) ' Ponemos letras blancas
                RandomLoG.BackColor = System.Drawing.ColorTranslator.FromOle(&HFF0000) ' Ponemos fondo azul
            End If ' cerramos condicion
        Next RandomLoG ' proximo objeto, si lo hay
		Combo1.Items.Clear() ' Limpiamos el/los items que haya dentro del ComboBox
		' Agregramso Items
		Combo1.Items.Add("Instr")
		Combo1.Items.Add("MsgBox")
		Combo1.Items.Add("Suma")
		Combo1.Items.Add("Resta")
		Combo1.Items.Add("Divide")
		Combo1.Items.Add("Multiplica")
		Combo1.Items.Add("Split")
		Combo1.Text = "Instrucciones Soportadas" ' le damo un toke ma de pinta :)
	End Sub
End Class