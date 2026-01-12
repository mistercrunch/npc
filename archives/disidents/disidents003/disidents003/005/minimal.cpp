#ifdef __GNUG__
    #pragma implementation "minimal.cpp"
    #pragma interface "minimal.cpp"
#endif

#include "wx/wxprec.h"

#include "wx/panel.h"
#include "wx/colour.h"
#include "wx/font.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "bebefoca.ico"
#endif

class MyApp : public wxApp
{

public:
	virtual bool OnInit();
	
};

class calc : public wxFrame
{
public:
        calc(const wxString& title, const wxPoint& pos, const wxSize& size);
        wxButton *btn[21];
	wxStaticText *pantalla;
        wxTextCtrl *texto;
        void accion(wxCommandEvent& event);
        void salir(wxCommandEvent& event);
        void numero0(wxCommandEvent& event);
        void numero1(wxCommandEvent& event);
        void numero2(wxCommandEvent& event);
        void numero3(wxCommandEvent& event);
        void numero4(wxCommandEvent& event);
        void numero5(wxCommandEvent& event);
        void numero6(wxCommandEvent& event);
        void numero7(wxCommandEvent& event);
        void numero8(wxCommandEvent& event);
        void numero9(wxCommandEvent& event);
        void punto(wxCommandEvent& event);
        void negpos(wxCommandEvent& event);
        void borrar(wxCommandEvent& event);
        void suma(wxCommandEvent& event);
        void resta(wxCommandEvent& event);
        void multiplicacion(wxCommandEvent& event);
        void division(wxCommandEvent& event);
        void igual(wxCommandEvent& event);
        void del(wxCommandEvent& event);
        void flotante(char c);
private:
        DECLARE_EVENT_TABLE()
};


enum
{
        n0=1, n1,n2,n3,n4,n5,n6,n7,n8,n9, idsalir, idborrar, npunto, nmenos,iddel, nnegpos,idsuma, idresta, idmultiplicacion, iddivision, idigual
        
};
BEGIN_EVENT_TABLE(calc,wxFrame)
        EVT_BUTTON(n0, calc::numero0)
        EVT_BUTTON(n1, calc::numero1)
        EVT_BUTTON(n2, calc::numero2)
        EVT_BUTTON(n3, calc::numero3)
        EVT_BUTTON(n4, calc::numero4)
        EVT_BUTTON(n5, calc::numero5)
        EVT_BUTTON(n6, calc::numero6)
        EVT_BUTTON(n7, calc::numero7)
        EVT_BUTTON(n8, calc::numero8)
        EVT_BUTTON(n9, calc::numero9)
        EVT_BUTTON(idborrar, calc::borrar)
        EVT_BUTTON(idsalir, calc::salir)
        EVT_BUTTON(npunto, calc::punto)
        EVT_BUTTON(nnegpos, calc::negpos)
        EVT_BUTTON(idsuma, calc::suma)
        EVT_BUTTON(idresta, calc::resta)
        EVT_BUTTON(idmultiplicacion, calc::multiplicacion)
        EVT_BUTTON(iddivision, calc::division)
        EVT_BUTTON(idigual, calc::igual)
        EVT_BUTTON(iddel,calc::del)
END_EVENT_TABLE()
IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    calc *frame = new calc("Calculadora",wxPoint(50, 50), wxSize(330, 320));
    frame->Show(TRUE);
    return TRUE;
    
}

calc::calc(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
        SetIcon(wxICON(bebefoca));
        wxPanel *Panel=new wxPanel(this, -1);
        Panel->SetBackgroundColour(wxColour(23,143,183));
        //se colocan los botones en el panel
        btn[1] = new wxButton(Panel, n0, "0", wxPoint(15,230), wxSize(50,50));
        btn[1]->SetBackgroundColour(wxColour(48,184,228));
        btn[1]->SetForegroundColour(*wxWHITE);
        
        btn[2] = new wxButton(Panel, npunto, ".", wxPoint(75,230), wxSize(50,50));
        btn[2]->SetBackgroundColour(wxColour(48,184,228));
        btn[2]->SetForegroundColour(*wxWHITE);
        btn[3] = new wxButton(Panel, nnegpos, "-/+", wxPoint(135,230), wxSize(50,50));
        btn[3]->SetBackgroundColour(wxColour(48,184,228));
        btn[3]->SetForegroundColour(*wxWHITE);
        btn[4] = new wxButton(Panel, idsuma, "+", wxPoint(195,230), wxSize(50,50));
        btn[4]->SetBackgroundColour(wxColour(48,184,228));
        btn[4]->SetForegroundColour(*wxWHITE);
        btn[5] = new wxButton(Panel, idigual, "=", wxPoint(255,230), wxSize(50,50));
        btn[5]->SetBackgroundColour(wxColour(48,184,228));
        btn[5]->SetForegroundColour(*wxWHITE);
        btn[6] = new wxButton(Panel, n1, "1", wxPoint(15,170), wxSize(50,50));
        btn[6]->SetBackgroundColour(wxColour(48,184,228));
        btn[6]->SetForegroundColour(*wxWHITE);
        btn[7] = new wxButton(Panel, n2, "2", wxPoint(75,170), wxSize(50,50));
        btn[7]->SetBackgroundColour(wxColour(48,184,228));
        btn[7]->SetForegroundColour(*wxWHITE);
        btn[8] = new wxButton(Panel, n3, "3", wxPoint(135,170), wxSize(50,50));
        btn[8]->SetBackgroundColour(wxColour(48,184,228));
        btn[8]->SetForegroundColour(*wxWHITE);
        btn[9] = new wxButton(Panel, idresta, "-", wxPoint(195,170), wxSize(50,50));
        btn[9]->SetBackgroundColour(wxColour(48,184,228));
        btn[9]->SetForegroundColour(*wxWHITE);
        btn[10] = new wxButton(Panel, idmultiplicacion, "*", wxPoint(255,170), wxSize(50,50));
        btn[10]->SetBackgroundColour(wxColour(48,184,228));
        btn[10]->SetForegroundColour(*wxWHITE);
        btn[11] = new wxButton(Panel, n4, "4", wxPoint(15, 110), wxSize(50,50));
        btn[11]->SetBackgroundColour(wxColour(48,184,228));
        btn[11]->SetForegroundColour(*wxWHITE);
        btn[12] = new wxButton(Panel, n5, "5", wxPoint(75,110), wxSize(50,50));
        btn[12]->SetBackgroundColour(wxColour(48,184,228));
        btn[12]->SetForegroundColour(*wxWHITE);
        btn[13] = new wxButton(Panel, n6, "6", wxPoint(135,110), wxSize(50,50));
        btn[13]->SetBackgroundColour(wxColour(48,184,228));
        btn[13]->SetForegroundColour(*wxWHITE);
        btn[14] = new wxButton(Panel, idborrar, "CLS", wxPoint(195,110), wxSize(50,50));
        btn[14]->SetBackgroundColour(wxColour(48,184,228));
        btn[14]->SetForegroundColour(*wxWHITE);
        btn[15] = new wxButton(Panel, iddivision, "/", wxPoint(255,110), wxSize(50,50));
        btn[15]->SetBackgroundColour(wxColour(48,184,228));
        btn[15]->SetForegroundColour(*wxWHITE);
        btn[16] = new wxButton(Panel, n7, "7", wxPoint(15, 50), wxSize(50,50));
        btn[16]->SetBackgroundColour(wxColour(48,184,228));
        btn[16]->SetForegroundColour(*wxWHITE);
        btn[17] = new wxButton(Panel, n8, "8", wxPoint(75, 50), wxSize(50,50));
        btn[17]->SetBackgroundColour(wxColour(48,184,228));
        btn[17]->SetForegroundColour(*wxWHITE);
        btn[18] = new wxButton(Panel, n9, "9", wxPoint(135, 50), wxSize(50,50));
        btn[18]->SetBackgroundColour(wxColour(48,184,228));
        btn[18]->SetForegroundColour(*wxWHITE);
        btn[19] = new wxButton(Panel, iddel, "DEL", wxPoint(195, 50), wxSize(50,50));
        btn[19]->SetBackgroundColour(wxColour(48,184,228));
        btn[19]->SetForegroundColour(*wxWHITE);
        btn[20] = new wxButton(Panel, idsalir, "OFF", wxPoint(255, 50), wxSize(50,50));
        btn[20]->SetBackgroundColour(wxColour(48,184,228));
        btn[20]->SetForegroundColour(*wxWHITE);

        //Se coloca la etiqueta que contiene los n£mero presionado
        pantalla = new wxStaticText(Panel,-1,"0.",wxPoint (15,15), wxSize(297,15),wxALIGN_RIGHT | wxST_NO_AUTORESIZE);
        pantalla->SetForegroundColour(*wxBLUE);
        pantalla->SetBackgroundColour(*wxWHITE);
} 
//declaracion de variables
char s[60];
int l=0,v=0, op; // v=para saber si hay algun punto escrito 
float valor1, valor2;
void calc::flotante(char c)
{
        if ((c>='0' && c<='9')&&(l<=20))
        {
                s[l]=c;
                l++;
         }
         s[l]='\0';
         pantalla->SetLabel(s);
}
void calc::numero0(wxCommandEvent& event)
{
        flotante('0');
}
void calc::numero1(wxCommandEvent& event)
{
        flotante('1');
}
void calc::numero2(wxCommandEvent& event)
{
        flotante('2');
}
void calc::numero3(wxCommandEvent& event)
{
        flotante('3');
}
void calc::numero4(wxCommandEvent& event)
{
        flotante('4');
}
void calc::numero5(wxCommandEvent& event)
{
        flotante('5');
}
void calc::numero6(wxCommandEvent& event)
{
        flotante('6');
}
void calc::numero7(wxCommandEvent& event)
{
        flotante('7');
}
void calc::numero8(wxCommandEvent& event)
{
        flotante('8');
}
void calc::numero9(wxCommandEvent& event)
{
        flotante('9');
}
void calc::punto(wxCommandEvent& event)
{
      
        if(v==0)
        {
          s[l]='.';
          l++;
          v=1;
        }
        
}
void calc::borrar(wxCommandEvent& event)
{
        s[0]='\0';
        l=0;
        pantalla->SetLabel("0.");
        valor1=0;
        valor2=0;
        v=0;

}
void calc::negpos(wxCommandEvent& event)
{
        sprintf(s,"%f",(-1* atof(s)));
        pantalla->SetLabel (s);
}
void calc::salir(wxCommandEvent& event)
{
        Close(TRUE);
}
void calc::suma(wxCommandEvent& event)
{
      valor1 = atof(s);
      op = 1;
      s[0]='\0';
      l=0;
      v=0;
/*      if (valor1!=0)
         idigual(); */
}
void calc::resta(wxCommandEvent& event)
{
        valor1 = atof(s);
        op = 2;
        s[0]='\0';
        l=0;
        v=0;
}
void calc::multiplicacion(wxCommandEvent& event)
{
        valor1 = atof(s);
        op = 3;
        s[0]='\0';
        l=0;
        v=0;
}
void calc::division(wxCommandEvent& event)
{
         
        valor1 = atof(s);
        op=4;
        s[0]='\0';
        l=0;
        v=0;
}
void calc::del(wxCommandEvent& event)
{
        s[strlen(s)-1]='\0';
        l=l-1;
        pantalla->SetLabel(s);
} 
void calc::igual(wxCommandEvent& event)
{
        if (l>0)
        {
                valor2 = atof(s);
                switch (op)
                {
                        case (1):
                                sprintf(s,"%2f", valor1 + valor2);
                                pantalla->SetLabel(s);
                                break;
                        case (2):
                                sprintf(s,"%2f", valor1 - valor2);
                                pantalla->SetLabel(s);
                                break;
                        case (3):
                                sprintf(s,"%2f", valor1 * valor2);
                                pantalla->SetLabel(s);
                                break;
                        case (4):
                                if (valor2!=0)
                                {
                                        sprintf(s,"%2f", valor1/valor2);
                                        pantalla->SetLabel(s);
                                        break;
                                }
                                else
                                {
                                  pantalla->SetLabel("Error");
                                }
                        
                 }
                 v=0;
           }
        
          
}
