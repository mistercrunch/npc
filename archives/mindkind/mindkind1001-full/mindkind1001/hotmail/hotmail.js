function SetFocus(oF,szFN){try{if(g_bFS==false&&typeof oF=="object"){var bF=false;var 
bL=true;if(szFN==""){bF=true;}for(i=0;i<oF.length;i++){try{var 
oI=oF.elements[i];}catch(e){continue;}if(oI.name==szFN){bF=true;}if(oI.type!="hidden"&&oI.disabled!=true&&oI.style.visibility!="hidden"&&bF){oI.focus();if(oI.type=="text"||oI.type=="password"){oI.select();}break;}if(i==oF.length-1&&bL){i=-1;bF=true;bL=false;}}}}catch(e){}}
function SetF(){g_bFS=true;}
function SetCursorAfterValue(){event.srcElement.value = event.srcElement.value;}
function CleanPhoneNum(oFN){var 
szFV=oFN.value;szFV=szFV.replace(/\)/,"");szFV=szFV.replace(/\(/,"");szFV=szFV.replace(/\ 
/,"");szFV=szFV.replace(/\./,"");szFV=szFV.replace(/\-/,"");szFV=szFV.replace(/\,/,"");oFN.value=szFV;}
function 
IsParent(){if(top.location!=location){top.location.href=document.location.href;}}
function DoCSH(oF){if (typeof(CSH)=="function"){CSH(oF);}}
var g_fAS=false;function OnSubmit(oF){DoCSH(oF);}
function SubmitForm(oF){if(g_fAS){return 
false;}OnSubmit(oF);g_fAS=true;oF.submit();return false;}
function 
DoSubmit(oF,szF){if(oF[szF]!="undefined"){oF[szF].value="TRUE";}SubmitForm(oF);}
function OnSubmitEventHandler(oF){oF['Dirty'].value="TRUE";DoCSH(oF);}
function OnBackEventHandler(oF,szU){oF.action=szU;DoSubmit(oF,"Back");}
function OnSubmitWithAction(oF,szU){oF.action=szU;oF.submit();return false;}
var WinObj;
function CloseWindow(){if(WinObj&&!WinObj.closed){WinObj.close();}return true;}
function 
OpenWindow(szU,Name,Param){if(typeof(CustomOpenWindow)=="function"){CustomOpenWindow(szU,Name,Param);}else{var 
bO=false;if(!WinObj||WinObj.closed){bO=true;}else{if(szU==WinObj.href){WinObj.focus();}else{CloseWindow();bO=true;}}if(bO){WinObj=window.open(szU,Name,Param);if(!WinObj.opener){WinObj.opener=window;}}}}
function 
OpenWindowEx(szU,Name,Param){if(typeof(CustomOpenWindow)=="function"){CustomOpenWindow(szU,Name,Param);}else{window.open(szU,'_blank',Param);}}
function GoA(sA){var strTemphref = document.location.href;var strTemp = 
strTemphref;if(strTemphref.indexOf("#")>-1){strTemp = 
strTemphref.substring(0,strTemphref.indexOf("#"));}document.location.href=strTemp+sA;}function 
ClearAllFields(oF){var 
szT;for(i=0;i<oF.length;i++){szT=""+oF.elements[i].type;if(szT=="text"||szT=="password"||szT=="textarea"){oF.elements[i].value="";}if(szT=="select-one"){oF.elements[i].selectedIndex=0;}if(szT=="radio"){if(oF.elements[i].value==""){oF.elements[i].checked=true;}else{oF.elements[i].checked=false;}}if(szT=="checkbox"){oF.elements[i].checked=false;}}}
function 
PostRegion(oF,szF,szH){if(oF[szF]!="undefined"){if(oF[szF].selectedIndex==0){oF[szH].value="TRUE";SubmitForm(oF);}}}
function 
DoCookieTest(sK,sV,sUrl){if(GetCookieData(sK)!=sV){document.location=sUrl;}return;}
function GetCookieData(sL){var arg=sL+"=";var iL=document.cookie.length;var 
i=0;while(i<iL){var j=i+arg.length;if(document.cookie.substring(i,j)==arg){var 
sE=document.cookie.indexOf(";",j);if(sE==-1){sE=iL;}return 
unescape(document.cookie.substring(j, sE));}i=document.cookie.indexOf(" 
",i)+1;if(i==0){break;}}}
function DrDw(F){var aCL=F.split("~");var iC=aCL.length;if(aCL[iC-1]==""){iC--;}var 
i=0;var 
sS="";while(i<iC){if(aCL[i]=="SELECTED"){sS=aCL[i+1];i++;}document.write("<option 
id=\""+aCL[i]+"\" value=\""+aCL[i]+"\""+((sS==aCL[i])?" 
selected":"")+">"+aCL[i+1]);i=i+2;}}
function DrDw2(F,sS){var aCL=F.split("~");var iC=aCL.length;if(aCL[iC-1]==""){iC--;}var 
i=0;while(i<iC){document.write("<option value=\""+aCL[i]+"\""+((sS==aCL[i])?" 
selected":"")+">"+aCL[i+1]);i=i+2;}}
function AHidden(fm,fn,fv){if(document.getElementById){var 
I=document.createElement('INPUT');if(document.all){I.type='HIDDEN';I.name=fn;I.value=fv;}else 
if(document.getElementById){I.setAttribute('type','HIDDEN');I.setAttribute('name',fn);I.setAttribute('value',fv);}fm.appendChild(I);}}
function GEId(sID){return document.getElementById(sID);}
function GetEl(szN){try{return document.getElementsByName(szN)[0]}catch(e){};}
function GetElCol(szN){try{return document.getElementsByName(szN)}catch(e){};}
var g_hipS;var g_hipR;var g_hipI;var g_hipF;var g_hipC;function 
HIPInit(r,i,f){g_hipS=i.src;g_hipR=r;g_hipI=i;g_hipF=f;g_hipC=0;}
function 
HIPSetAudSoln(s){if(s!=""){g_hipF.value=s;g_hipI.src=g_hipR;}else{GEId("iHIPHelp").style.display="block";}g_hipF.focus();}
function HIPNew(){var h=g_hipI.height;var w=g_hipI.width;g_hipF.value=""; 
g_hipI.src=g_hipS+"&c="+g_hipC;g_hipI.height=h;g_hipI.width=w;g_hipC++;g_hipF.focus();}
function 
HIPHelp(u){g_hipI.src=g_hipR;g_hipF.value="";OpenWindow(u,"PPHIPHelp","width=520,height=505,resizable");window.setTimeout("HIPHelpCheck()",1000);}
function 
HIPHelpCheck(){if((!WinObj||WinObj.closed)&&(g_hipF.value=="")){HIPNew();}else{window.setTimeout("HIPHelpCheck()",50);}}
var h_win;function DoHelp(SM,T,LC,SKU,U,VV){var 
sQP="",W,H,sWD="toolbar=0,status=0,menubar=0,resizable=1,top=0,width=",sc=screen.width;var 
ag=navigator.userAgent.toLowerCase();var ap=navigator.appName.toLowerCase();var 
av=navigator.appVersion;var fMAC=(ag.indexOf('mac')>-1);var 
fIE=(ag.indexOf("msie")>-1&&parseInt(av.substring(0,1))>=4);var 
sUB=(U=='')?('/hp.srf?lc='+LC+'&vv='+VV):(U+'?lc='+LC+'&vv='+VV);var 
oDL=document.location;var fMSN=false,vi=ag.indexOf('msn 
'),fMACMSN=false;if(vi>-1){fMSN=ag.substring(vi+4);fMSN=parseFloat(fMSN.substring(0,fMSN.indexOf(";")));fMSN=(fMSN!=NaN&&fMSN>=6)}fMACMSN=ag.indexOf('ppc 
mac os x')>-1&&ag.indexOf('msn 
explorer')>-1;sQP+=(T=="")?'&SEARCHTERM='+escape(SM):'&TOPIC='+T;sQP+='&v2='+escape(oDL.search)+'&tmt='+escape(window.name);sQP+='&v4='+escape(SKU);if(sc<=800){sQP+="&sp=1";W=180;}else{W=230;}if(fMAC&&fIE){W=224;}H=(ag.indexOf("windows")>0&&ag.indexOf("aol")>0)?screen.availHeight-window.screenTop-22:screen.availHeight;sWD+=W;if(fMSN){window.external.showHelpPane(oDL.protocol+"//"+oDL.hostname+sUB+sQP,W);}else 
if(ag.indexOf('webtv')>0||ag.indexOf('msn 
companion')>0||ag.indexOf('stb')>0){oDL=sUB+sQP;}else 
if(ap.indexOf('netscape')>-1&&av.indexOf('4.')>-1){var 
fw=12;if(fMAC){sWD+=",height="+(H-38)+",left="+(sc-W-16);fw=30;}else{sWD+=",height="+(H-30)+",left="+(sc-W-12);}top.window.resizeTo(screen.availWidth-W-(window.outerWidth-window.innerWidth)-fw,screen.availHeight-(window.outerHeight-window.innerHeight));top.window.moveTo(0,0);h_win=window.open(sUB+sQP,'_help',sWD);}else 
if(ag.indexOf("opera")>-1){sWD+=",height="+H+",left="+(sc-W-(fMAC?5:0));window.resizeTo(screen.availWidth-W-(fMAC?20:0),screen.availHeight);window.moveTo(0,0);h_win=window.open(sUB+sQP,'_help',sWD);}else 
if(ag.indexOf("aol")>-1){sWD+=",height="+(H-115);window.open(sUB+sQP,'_help',sWD);}else 
if(fIE||ag.indexOf('netscape6')>-1){sWD+=",height="+H+",left="+(sc-W);if(ag.indexOf("msie 
4")>0){window.open(sUB+sQP,'_help',sWD);}else{h_win=window.open(sUB+sQP,'_help',sWD);}if(h_win&&!fMAC&&ap.indexOf("netscape")<0){h_win.opener=self;}}else{window.open(sUB+sQP,'_help');}}
function 
EvalPwdStrength(oF,sP){PadPasswd(oF,sP.length*2);if(ClientSideStrongPassword(sP,gSimilarityMap,gDictionary)){DispPwdStrength(3,'css0165');}else 
if(ClientSideMediumPassword(sP,gSimilarityMap,gDictionary)){DispPwdStrength(2,'css0164');}else 
if(ClientSideWeakPassword(sP,gSimilarityMap,gDictionary)){DispPwdStrength(1,'css0163');}else{DispPwdStrength(0,'css0162');}}function 
SetPwdStrengthEx(oF,sP){EvalPwdStrength(oF,sP);if(ClientSideStrongPassword(sP,gSimilarityMap,gDictionary)){document.cookie="pwdstrength=3";}else 
if(ClientSideMediumPassword(sP,gSimilarityMap,gDictionary)){document.cookie="pwdstrength=2";}else 
if(ClientSideWeakPassword(sP,gSimilarityMap,gDictionary)){document.cookie="pwdstrength=1";}else{document.cookie="pwdstrength=0";}}function 
SetPwdStrength(sP,oF){if(ClientSideStrongPassword(sP,gSimilarityMap,gDictionary)){oF.value 
= 3;}else if(ClientSideMediumPassword(sP,gSimilarityMap,gDictionary)){oF.value = 
2;}else if(ClientSideWeakPassword(sP,gSimilarityMap,gDictionary)){oF.value = 
1;}else{oF.value = 0;}}function XPWCont(){if 
(typeof(parent.opener.RegistrationFinishedCallback)!="undefined"){parent.opener.RegistrationFinishedCallback();}parent.close();}function 
OnSigninSubmit(oF){if(g_fAS){return false;}if(typeof oF!="object"){return false;}var 
bL=true,bP=true,bI=true,bH=true;bL=Val(oF.login);var 
sEM=oF.login.value;bL=ValEM(sEM);if(typeof 
oF.passwd=="object"){bP=Val(oF.passwd);}if(typeof 
oF.pin=="object"){bI=Val(oF.pin);}if(typeof 
oF.HIPSolution=="object"){bH=Val(oF.HIPSolution);}if(!bL||!bP||!bI||!bH){var 
fSF=true;var 
aE=["i0518","i0519","i0512","i0527","i0545","i0562","i0517"];HDivs(aE);if(!bL){fSF=SwErr("i0519",oF.login,fSF)};if(!bI){fSF=SwErr("i0527",oF.pin,fSF)};if(!bH){fSF=SwErr("i0517",oF.HIPSolution,fSF)};if(!bP){fSF=SwErr("i0512",oF.passwd,fSF)};return 
false;}if(typeof oF.passwd=="object"){PadPasswd(oF,oF.passwd.value.length);}if(typeof 
g_DO!="undefined"){var dom=sEM.substr(sEM.indexOf('@')+1);var 
sU=g_DO[dom.toLowerCase()];if(sU){oF.action=sU;}}if(typeof 
g_QS!="undefined"){if(g_QS){var 
sS="&";if(oF.action.indexOf('?')==-1){sS="?";}if(oF.action.indexOf(g_QS)==-1){oF.action+=sS+g_QS;}}}g_fAS=true;oF.login.value=oF.login.value.toLowerCase();oF.submit();return 
false;}function OnPadSubmit(oF){if(typeof 
oF.CurrPW=="object"){PadPasswd(oF,oF.CurrPW.value.length);}oF.submit();return 
false;}function OnPadSubmitWithAction(oF,szU){if(typeof 
oF.CurrPW=="object"){PadPasswd(oF,oF.CurrPW.value.length);}return 
OnSubmitWithAction(oF,szU);}function PadPasswd(oF,lPwd){if(typeof 
oF.PwdPad=="object"){var sPad="IfYouAreReadingThisYouHaveTooMuchFreeTime";var 
lPad=sPad.length-lPwd;oF.PwdPad.value=sPad.substr(0,(lPad<0)?0:lPad);}}function 
HDivs(aE){for(var i=0;i<aE.length;++i){var 
o=GEId(aE[i]);if(o){o.style.display="none";}}}function 
SwErr(sID,oFN,fSF){GEId(sID).style.display="block";if(fSF){oFN.focus();oFN.select();}return 
false;
}function Val(oFN){if(oFN.type!="hidden"&&!oFN.value){return false;}else{return 
true;}}function ValEM(sEM){var tsEM = Trim(sEM);var iA=tsEM.indexOf('@');var 
aE=tsEM.split('@');if((iA==-1)||(tsEM.charAt(0)>'~')||(tsEM.indexOf('.',iA)==-1)||(tsEM.indexOf(' 
',0)!=-1)||aE.length>2){return false;}else{if(aE[0].length > 0 && aE[1].length > 
0){return true;}else{return false;}}}
function ValHex(oFN){var hex="0123456789ABCDEF";var sFV=oFN.value;if(!Val(oFN)){return 
false;}for(i=0;i<sFV.length;i++){if(hex.indexOf(sFV.charAt(i).toUpperCase())==-1){return 
false;}}return true;}
function ShowWDiv(sLD,sWD,TP,LT){var 
cN=GEId(sLD),oD=GEId(sWD).style;while(cN.tagName!='BODY'){TP+=cN.offsetTop;LT+=cN.offsetLeft;cN=cN.offsetParent;}oD.top=TP+'px';oD.left=LT+'px';oD.display='block';}
function AUp(oFN,sV,sC){if(typeof oFN=="object"){var 
sFNV=oFN.value;if(sFNV.length>0&&sFNV.indexOf(sC)==-1){oFN.value=sFNV+sV;}}}var 
g_bClearCache=false;function 
ClearSSLCache(){g_bClearCache=false;try{g_bClearCache=document.execCommand('ClearAuthenticationCache');}catch(e){}}function 
EndPPCRL(rs,u){if(window.external){try{if(rs){if 
(rs&-2147483648){window.external.RequestStatus=rs;window.external.WebFlowUrl=u;window.external.NotifyIdentityChanged();window.external.ReturnToApp();}else{window.external.RequestStatus=rs;window.external.WebFlowUrl=u;window.external.NotifyIdentityChanged(true);window.external.BrowseToAuthUI();}}else{window.external.Submit();window.external.BrowseToAuthUI();}}catch(e){}}}function 
WebHancer(oF,pid,err)
{if(typeof(oF.sWH)!="undefined"&&pid.length!=0){oF.sWH.value+="["+pid+","+err+"]";}}function 
ParseQS(sU){var sQ=sU.substr(sU.indexOf('?')+1);var oA=new Object();if(sQ){var 
aF=sQ.split('&');for(var f=0;f<aF.length;f++){var 
sFD=aF[f].split('=');oA[sFD[0]]=sFD[1];}}return oA;}
function GetQSVal(sU,sN){var oAs=ParseQS(sU);var sR=null;for(var oA in 
oAs){if(sN==oA){sR=oAs[oA];}}return sR;}
function SetFF(sF,sV){var oF=GetEl(sF);if(oF==null||oF.value!=""){return;}oF.value=sV;}
function LTrim(str){for (var k=0; k<str.length && str.charAt(k)<=" " ; k++) ;return 
str.substring(k,str.length);}
function RTrim(str){for (var j=str.length-1; j>=0 && str.charAt(j)<=" " ; j--) ;return 
str.substring(0,j+1);}
function Trim(str){return LTrim(RTrim(str));}
function 
IsPPCRLEnv(){if(window.external){try{if((window.external.GUID=="bd3c0d89-e162-4de2-a1bf-942b86200f99")|| 
(window.external.GUID=="29dfb9ad-d01f-494d-8418-786b695add57")){return 
true;}}catch(e){return false;}}else{return false;}}var g_d=":";
function PushError(sErr)
{
    var sErrs=g_d+s_prop38+g_d,sE=g_d+sErr+g_d;
    
if(!sErr||typeof(s_prop38)=='undefined'||typeof(s_prop48)=='undefined'||(sErrs.indexOf(sE)!=-1))
        return;
    s_prop38+=(s_prop38.length>0)?g_d+sErr:sErr;
    s_prop48=s_prop38;
}
function SendReport()
{
    if(typeof(s_account)!="undefined")
        s_gs(s_account.toLowerCase());
}
function ClearError()
{
    if(typeof(s_prop38)!="undefined")
	    s_prop38='';
    if(typeof(s_prop48)!="undefined")
	    s_prop48='';
}
function AppPN(s)
{
   	return (s&&s.length>0)?s+g_d+s_pageName:'';
}
