/*         ____     ______  __  ___  _____    ____             __________
 *        /   /    /  ___/  \ \/  / /     \  /    \    ____   /___   ___/
 *       /   /__  /  /__     \   / /  <>  / /  __  \  /    \     /  / 
 *      /   ___/ /   __/     /  / /     _/  \      / /  __  \   /  /
 *     /    \   /   /__     /  / /  /\  \    \____/  \      /   \  \
 *    /__/\__\  \_____/    /__/  \_/  \__\            \____/     \__\
 *
 *                Proudly Presents: SeeGeeEye.java v1.2
 *                Coded by: Wyzewun [w1@macroshaft.org]
 *
 * First Mass CGI vulnerability scanner with Wingate/Anon Proxy support.
 * I Would make it threaded, but I'm kinda proud of how small it is :P
 *
 * Example usage fer the clueless bastiches... :>
 *
 * host -l co.jp | grep 'has address' | grep www | awk '{print $1}' > luzers
 * java SeeGeeEye < luzers
 *
 * Or to scan through a proxy of some form...
 * java -Dhttp.proxyHost=proxy -Dhttp.proxyPort=port SeeGeeEye < luzers
 *
 */

import java.io.*;
import java.net.*;

public class SeeGeeEye {
      
  public static void main(String[] args) throws IOException {

    URL bastich;
    String victim;
    boolean bother;
    String response;
    BufferedReader stdIn = null;
    BufferedReader een = null;

    stdIn = new BufferedReader(new InputStreamReader(System.in));
    
    /* These are the vulnerabilities the proggy checks for. It's fine to put
       your own strings in here - nothing will break. And while you're at it,
       mail the vulnerability to me, so I can add it in next version. :) */
  
    String[] vulns = {  "cgi-bin/phf",
                        "cgi-bin/php.cgi",
                        "cgi-bin/campas",
                        "cgi-bin/htmlscript",
                        "cgi-bin/aglimpse",
                        "cgi-bin/websendmail",
                        "cgi-bin/info2www",
                        "cgi-bin/pfdispaly.cgi",
                        "scripts/convert.bas",
                        "cgi-bin/webdist.cgi",
                        "cgi-bin/Count.cgi",
                        "cgi-bin/webgais",
                        "_vti_pvt/service.pwd",
                        "cgi-bin/test-cgi",
                        "cgi-bin/handler",
                        "cgi-bin/cachemgr.cgi",
                        "_vti_pvt/administrators.pwd",
                        "_vti_pvt/users.pwd",
                        "_vti_pvt/authors.pwd",
                        "cgi-bin/pfdisplay",
                        "cgi-bin/pfdisplay.cgi",
                        "cgi-bin/perl.exe",
                        "cgi-bin/wwwboard.pl",
                        "cgi-bin/www-sql",
                        "cgi-bin/man.sh",
                        "cgi-bin/view-source",
                        "cgi-bin/nph-test-cgi",
                        "cgi-bin/nph-publish",
                        "cgi-bin/wrap",
                        "cgi-bin/textcounter.pl",
                        "cgi-bin/environ.cgi",
                        "cgi-bin/query",
                        "cgi-bin/rpm_query",
                        "cfdocs/expeval/openfile.cfm",
                        "cfdocs/expelval/openfile.cfm",
                        "cfdocs/expeval/displayopenedfile.cfm",
                        "cfdocs/expeval/exprcalc.cfm",
                        "cgi-bin/finger",
                        "cgi-bin/bnbform.cgi",
                        "cgi-bin/survey.cgi",
                        "cgi-bin/classifieds.cgi",
                        "cgi-bin/AnyForm2",
                        "cgi-bin/AT-admin.cgi",
                        "cgi-bin/unlg1.1",
                        "cgi-bin/filemail.pl",
                        "cgi-bin/maillist.pl",
                        "cgi-bin/jj",
                        "cgi-bin/files.pl",
                        "cgi-dos/args.bat",
                        "cgi-win/uploader.exe",
                        "search97.vts",
                        "config/import.txt",
                        "config/checks.txt",
                        "orders/import.txt",
                        "orders/checks.txt",
                        "cgi-bin/rwwwshell.pl",
                        "cgi-bin/faxsurvey",
                        "cgi-bin/view-source",
                        "cgi-bin/glimpse",
                        "cgi-bin/cgiwrap",
                        "cgi-bin/guestbook.cgi",
                        "cgi-bin/edit.pl",
                        "cgi-bin/perlshop.cgi",
                        "_vti_inf.html",
                        "_vti_bin/shtml.dll",
                        "_vti_bin/shtml.exe",
                        "cgi-bin/rguest.exe",
                        "cgi-bin/wguest.exe",
                        "scripts/issadmin/bdir.htr",
                        "scripts/CGImail.exe",
                        "scripts/tools/newdsn.exe",
                        "scripts/fpcount.exe",
                        "iissamples/exair/howitworks/codebrws.asp",
                        "iissamples/sdk/asp/docs/codebrws.asp",
                        "msads/Samples/SELECTOR/showcode.asp",
                        "carbo.dll",
                        "cgi-bin/dbmlparser.exe",
                        "cgi-bin/flexform.cgi",
                        "cgi-bin/bnbform.cgi",
                        "cgi-bin/responder.cgi",
                        "cgi-bin/whois_raw.cgi",
                        "iisadmpwd/achg.htr",
                        "iisadmpwd/aexp.htr",
                        "iisadmpwd/aexp2.htr",
                        "iisadmpwd/aexp2b.htr",
                        "iisadmpwd/aexp3.htr",
                        "iisadmpwd/aexp4.htr",
                        "iisadmpwd/aexp4b.htr",
                        "iisadmpwd/anot.htr",
                        "iisadmpwd/anot3.htr",
                        "scripts/cgimail.exe",
                        "cgi-bin/day5datacopier.cgi",
                        "cgi-bin/day5datanotifier.cgi"   };
                        
    System.out.print("SeeGeeEye.java v1.2 by Wyzewun loaded.\n\n");
    
    while ((victim = stdIn.readLine()) != null) {
      for (int i = 0; i < vulns.length; i++) {
        bother = true;
        try {
          bastich = new URL("http://" + victim + "/" + vulns[i]);
          een = new BufferedReader(new InputStreamReader(bastich.openStream()));
        } catch (Exception e) {
          bother = false;
          i = vulns.length;
        }
        if (bother) {
          try {
            response = een.readLine();
            try {
            // Make sure we get HTTP response code 2xx (RFC1945)
            if ((Integer.parseInt(String.valueOf(response.charAt(9)))) == 2) {
              System.out.println("Found " + bastich); 
            }
            } catch (Exception e) { }
            een.close();
          } catch (IOException e) { } } } } } }
