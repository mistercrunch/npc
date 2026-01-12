/* SeeGeeEye.java v2.0 by Wyzewun
 * Performs Mass Scans for 172 CGI-Related vulnerabilities with proxy support
 *
 * Example: java SeeGeeEye hosts.txt
 * Example java -Dhttp.proxyHost=host -Dhttp.proxyPort=port SeeGeeEye hosts.txt
 *
 * Shout Outz to: Pneuma, Jus, Moe1, Cyberphreak, Egodeath, Meiso, and most of
 * all, to Slash for donating a whole bunch of CGI vulnerabilities and thus
 * inspiring a less buggy re-code of this.
 */

import java.io.*;
import java.net.*;

public class SeeGeeEye {
      
  public static void main(String[] args) throws IOException, UnknownHostException {

    URL bastich = null;
    Socket zocket = null;
    String ghostname = null;
    BufferedReader in = null;
    BufferedReader erl = null;

    char owld = 32;
    char gnu = 0;

    if (args.length < 1) {
      System.out.println("Syntax: java SeeGeeEye [list-of-hosts-to-scan]");
      System.exit(1); }

    try { in = new BufferedReader(new FileReader(args[0])); } catch (Exception e) {
      System.out.println("File not found: " + args[0]); System.exit(1); }
    
    /* These are the vulnerabilities the proggy checks for. It's fine to put
       your own strings in here - nothing will break. :) */
  
    String[] vulns = {  
			"/_vti_bin/shtml.dll",                    
			"/_vti_bin/shtml.exe",                                          
			"/_vti_pvt/administrators.pwd",                        
			"/_vti_pvt/authors.pwd",                           
			"/_vti_pvt/service.pwd",                          
			"/_vti_pvt/service.grp",                           
			"/_vti_pvt/users.pwd",                                                   
			"/cfappman/index.cfm",                             
	 		"/cfdocs/cfmlsyntaxcheck.cfm",                    
			"/cfdocs/exampleapp/docs/sourcewindow.cfm",        
	 		"/cfdocs/examples/cvbeans/beaninfo.cfm",          
	 		"/cfdocs/examples/httpclient/mainframeset.cfm",    
	 		"/cfdocs/examples/parks/detail.cfm",              
			"/cfdocs/expeval/displayopenedfile.cfm",         
			"/cfdocs/expeval/exprcalc.cfm",                  
			"/cfdocs/expeval/openfile.cfm",                 
	 		"/cfdocs/expeval/sendmail.cfm",                  
			"/cfdocs/snippets/evaluate.cfm",                  
			"/cfdocs/snippets/fileexists.cfm",                
			"/cfdocs/snippets/gettempdirectory.cfm",          
			"/cfdocs/snippets/viewexample.cfm",                                     
			"/....../autoexec.bat",                        
			"/_AuthChangeUrl?",                                
			"/adsamples/config/site.csc",                      
			"/cgi-dos/args.bat",                               
			"/cgi-dos/args.cmd",                               
			"/cgi-shl/win-c-sample.exe",                       
			"/cgi-win/uploader.exe",                           
			"/domcfg.nsf/?open",                                                    
			"/iisadmpwd/achg.htr",                            
			"/iisadmpwd/aexp.htr",                             
			"/iisadmpwd/aexp2.htr",                            
			"/iisadmpwd/aexp2b.htr",                           
			"/iisadmpwd/aexp3.htr",                            
			"/iisadmpwd/aexp4.htr",                            
			"/iisadmpwd/aexp4b.htr",                           
			"/iisadmpwd/anot.htr",                             
			"/iisadmpwd/anot3.htr",                             
			"/msadc/Samples/SELECTOR/showcode.asp",            
			"/msadc/msadcs.dll",                               
			"/msads/samples/selector/showcode.asp",            
			"/scripts/CGImail.exe",                            
			"/scripts/convert.bas",                           
			"/scripts/cpshost.dll",                            
			"/scripts/fpcount.exe",                            
			"/scripts/iisadmin/bdir.htr",                      
			"/scripts/proxy/w3proxy.dll",                      
			"/scripts/samples/ctguestb.idc",                   
			"/scripts/samples/details.idc",                    
			"/scripts/tools/getdrvs.exe",                      
			"/scripts/tools/newdsn.exe",                       
			"/search97.vts",                                  
			"/session/adminlogin?RCpage=/sysadmin/index.stm",  
			"/~root",                                          
			"/iissamples/exair/howitworks/codebrws.asp",     
			"/iissamples/sdk/asp/docs/codebrws.asp",           
			"/cgi-bin/visadmin.exe",                           
			"/scripts/../../cmd.exe",                          
			"/secure/.htaccess",                               
			"/secure/.wwwacl",                                       
			"/cgi-bin/AT-admin.cgi",                           
			"/cgi-bin/AnyForm",                                
			"/cgi-bin/AnyForm2",                               
			"/cgi-bin/Count.cgi",                              
			"/cgi-bin/LWGate",                                 
			"/cgi-bin/LWGate.cgi",                             
			"/cgi-bin/MachineInfo",                            
			"/cgi-bin/aglimpse",                               
			"/cgi-bin/archie",                                 
			"/cgi-bin/architext_query.pl",                     
			"/cgi-bin/ash",                                    
			"/cgi-bin/bash",                                   
			"/cgi-bin/bb-hist.sh",                             
			"/cgi-bin/bnbform",                                
			"/cgi-bin/bnbform.cgi",                            
			"/cgi-bin/calendar",                               
			"/cgi-bin/campas",                                 
			"/cgi-bin/carbo.dll",                              
			"/cgi-bin/cgimail.exe",                            
			"/cgi-bin/cgiwrap",                                
			"/cgi-bin/classified.cgi",                         
			"/cgi-bin/classifieds",                            
			"/cgi-bin/classifieds.cgi",                        
			"/cgi-bin/count.cgi",                              
			"/cgi-bin/csh",                                    
			"/cgi-bin/date",                                   
			"/cgi-bin/day5datacopier.cgi",                     
			"/cgi-bin/day5datanotifier.cgi",                   
			"/cgi-bin/download.cgi",                           
			"/cgi-bin/dumpenv.pl",                             
			"/cgi-bin/edit.pl",                                
			"/cgi-bin/environ.cgi",                            
			"/cgi-bin/faxsurvey",                              
 			"/cgi-bin/filemail",                               
			"/cgi-bin/filemail.pl",                            
			"/cgi-bin/files.pl",                               
			"/cgi-bin/finger",                                 
			"/cgi-bin/finger.cgi",                             
			"/cgi-bin/finger.pl",                             
			"/cgi-bin/flexform",                               
			"/cgi-bin/flexform.cgi",                           
			"/cgi-bin/formmail.pl",                            
			"/cgi-bin/fortune",                                
			"/cgi-bin/glimpse",                                
			"/cgi-bin/guestbook.cgi",                          
			"/cgi-bin/handler",                                
			"/cgi-bin/handler.cgi",                            
	 		"/cgi-bin/htmlscript",                             
	 		"/cgi-bin/info2www",                               
			"/cgi-bin/jj",                                    
			"/cgi-bin/ksh",                                    
			"/cgi-bin/lwgate",                                 
			"/cgi-bin/lwgate.cgi",                             
			"/cgi-bin/mail",                                   
			"/cgi-bin/maillist.pl",                            
			"/cgi-bin/man.sh",                                 
			"/cgi-bin/mlog.phtml",                             
			"/cgi-bin/mylog.phtml",                            
			"/cgi-bin/nlog-smb.pl",                            
			"/cgi-bin/nph-error.pl",                          
	 		"/cgi-bin/nph-publish",                           
			"/cgi-bin/nph-test-cgi",                           
			"/cgi-bin/perl",                                   
			"/cgi-bin/perl.exe",                               
			"/cgi-bin/perlshop.cgi",                           
			"/cgi-bin/pfdisplay.cgi",                          
			"/cgi-bin/phf",                                    
			"/cgi-bin/phf.cgi",                                
			"/cgi-bin/phf.pp",                                 
			"/cgi-bin/php",                                    
			"/cgi-bin/php.cgi",                                
			"/cgi-bin/post-query",                             
	 		"/cgi-bin/ppdscgi.exe",                            
			"/cgi-bin/query",                                  
			"/cgi-bin/redirect",                               
			"/cgi-bin/rguest.exe",                             
			"/cgi-bin/rksh",                                   
			"/cgi-bin/rsh",                                    
			"/cgi-bin/rwwwshell.pl",                           
			"/cgi-bin/sam._",                                  
			"/cgi-bin/search97.vts",                           
			"/cgi-bin/sendform.cgi",                           
			"/cgi-bin/sh",                                     
			"/cgi-bin/snorkerz.bat",                           
			"/cgi-bin/snorkerz.cmd",                          
			"/cgi-bin/survey",                                
			"/cgi-bin/survey.cgi",                             
			"/cgi-bin/tcsh",                                   
			"/cgi-bin/test-cgi",                               
			"/cgi-bin/test-cgi.tcl",                           
			"/cgi-bin/test-env",                             
			"/cgi-bin/textcounter.pl",                        
			"/cgi-bin/unlg1.1",                               
			"/cgi-bin/upload.pl",                             
			"/cgi-bin/uptime",                                 
			"/cgi-bin/view-source",                            
			"/cgi-bin/w3-msql",                                
			"/cgi-bin/w3tvars.pm",                             
			"/cgi-bin/wais.pl",                                
			"/cgi-bin/webdist.cgi",                            
			"/cgi-bin/webgais",                                
			"/cgi-bin/webmap.cgi",                             
			"/cgi-bin/websendmail",                            
			"/cgi-bin/wguest.exe",                             
			"/cgi-bin/whois_raw.cgi",                          		
			"/cgi-bin/wrap",                                   
			"/cgi-bin/www-sql",                               
			"/cgi-bin/wwwadmin.pl",                            
			"/cgi-bin/wwwboard.pl",                            
			"/cgi-bin/zsh",                                    
			"/cgi-bin/webwho.pl",
                        "/cgi-bin/getadmin.exe" };
                        
    System.out.print("SeeGeeEye.java v2.0 by Wyzewun loaded.\n\n");
    
    while ((ghostname = in.readLine()) != null) {
    ghostname.replace(owld, gnu);
    for (int i = 0; i < vulns.length; i++) {
    for (;;) { try {
      bastich = new URL("http://" + ghostname + vulns[i]);
      erl = new BufferedReader(new InputStreamReader(bastich.openStream()));
      erl.close(); } catch (Exception e) { break; }
      System.out.println("Found: " + bastich); break; } } } } }