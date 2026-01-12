/* tcpConnection.java v1.2 by Andrew Lewis <andrew@wizdumb.net>
 *
 * The seriously lazy man's API for TCP socket programming in Java.
 * Licensed under the LGPL (see www.gnu.org for details)
 */

package net.wizdumb.tcp;

import java.io.IOException;
import java.io.PrintWriter;
import java.io.BufferedReader;

import java.net.Socket;
import java.util.StringTokenizer;

public class tcpConnection {

  /////////////////////////////////////////////////
  // Variables                                   //
  /////////////////////////////////////////////////

  static int port;
  static String host;

  static Socket soq;
  static PrintWriter os;
  static BufferedReader is;

  /////////////////////////////////////////////////
  // Constructors                                //
  /////////////////////////////////////////////////

  public tcpConnection(String connection) throws Exception {
    StringTokenizer st = new StringTokenizer(connection, ":");
    if ((st.countTokens()) != 2)
      throw new Exception("Token Miscount");
    host = st.nextToken();
    try {
      port = Integer.parseInt(st.nextToken());
    } catch (NumberFormatException e) {
      throw new Exception("Unable to parse port value"); }
    try {
      soq = new Socket(host, port);
      is = new BufferedReader(new InputStreamReader(soq.getInputStream()));
      os = new PrintWriter(soq.getOutputStream(), true);
    } catch (IOException e) {
      throw new Exception("Couldn't connect to specified service"); } }

  public tcpConnection(InetAddress inet, int port) throws Exception {
    this.port = port;
    host = inet.toString();
    try {
      soq = new Socket(inet, port);
      is = new BufferedReader(new InputStreamReader(soq.getInputStream()));
      os = new PrintWriter(soq.getOutputStream(), true);
    } catch (IOException e) {
        throw new Exception("Couldn't connect to specified service"); } }

  public tcpConnection(String host, int port) throws Exception {
    this.host = host;
    this.port = port;
    try {
      soq = new Socket(host, port);
      is = new BufferedReader(new InputStreamReader(soq.getInputStream()));
      os = new PrintWriter(soq.getOutputStream(), true);
    } catch (IOException e) {
        throw new Exception("Couldn't connect to specified service"); } }

  public tcpConnection(String connection, boolean read, boolean write) throws Exception {
    StringTokenizer st = new StringTokenizer(connection, ":");
    if ((st.countTokens()) != 2)
      throw new Exception("Token Miscount");
    host = st.nextToken();
    try {
      port = Integer.parseInt(st.nextToken());
    } catch (NumberFormatException e) {
      throw new Exception("Unable to parse port value"); }
    try {
      soq = new Socket(host, port);
      if (read) is = new BufferedReader(new InputStreamReader(soq.getInputStream()));
      if (write) os = new PrintWriter(soq.getOutputStream(), true);
    } catch (IOException e) {
      throw new Exception("Couldn't connect to specified service"); } }

  public tcpConnection(InetAddress inet, int port, boolean read, boolean write) throws Exception {
    this.port = port;
    host = inet.toString();
    try {
      soq = new Socket(inet, port);
      if (read) is = new BufferedReader(new InputStreamReader(soq.getInputStream()));
      if (write) os = new PrintWriter(soq.getOutputStream(), true);
    } catch (IOException e) {
        throw new Exception("Couldn't connect to specified service"); } }

  public tcpConnection(String host, int port, boolean read, boolean write) throws Exception {
    this.host = host;
    this.port = port;
    try {
      soq = new Socket(host, port);
      if (read) is = new BufferedReader(new InputStreamReader(soq.getInputStream()));
      if (write) os = new PrintWriter(soq.getOutputStream(), true);
    } catch (IOException e) {
        throw new Exception("Couldn't connect to specified service"); } }

  /////////////////////////////////////////////////
  // Routines                                    //
  /////////////////////////////////////////////////

  public void disconnect() throws Exception {
    try {
      os.close();
      is.close();
      soq.close();
    } catch (IOException e) { } }

  public void reconnect() throws Exception {
    disconnect();
    new tcpConnection(host, port); }

  public void reconnect(String connection) throws Exception {
    disconnect();
    new tcpConnection(connection); }

  public void reconnect(String host, int port) throws Exception {
    disconnect();
    this.host = host;
    this.port = port;
    new tcpConnection(host, port); }

  public void reconnect(boolean read, boolean write) throws Exception {
    disconnect();
    new tcpConnection(host, port, read, write); }

  public void reconnect(String connection, boolean read, boolean write) throws Exception {
    disconnect();
    new tcpConnection(connection, read, write); }

  public void reconnect(String host, int port, boolean read, boolean write) throws Exception {
    disconnect();
    this.host = host;
    this.port = port;
    new tcpConnection(host, port, read, write); }

  /////////////////////////////////////////////////
  // Wrapper stuff for BufferedReader            //
  /////////////////////////////////////////////////

  public int read(char[] cbuf, int off, int len) throws Exception, IOException {
    try {
      return is.read(cbuf, off, len);
    } catch (IOException e) {
      throw new Exception("BufferedReader: " + e); } }
    
  public String readLine() throws Exception, IOException {
    try {
      return is.readLine();
    } catch (IOException e) {
      throw new Exception("BufferedReader: " + e); } }

  public void mark(int readAheadLimit) throws Exception, IOException {
    try {
      is.mark(readAheadLimit);
    } catch (IOException e) {
      throw new Exception("BufferedReader: " + e); } }

  public void reset() throws Exception, IOException {
    try {
      is.reset();
    } catch (IOException e) {
      throw new Exception("BufferedReader: " + e); } }

  public void skip(long n) throws Exception, IOException {
    try {
      is.skip(n);
    } catch (IOException e) {
      throw new Exception("BufferedReader: " + e); } }

  public int eread(char[] cbuf, int off, int len) {
    int eweed = 0;
    try {
      eweed = is.read(cbuf, off, len);
    } catch (IOException e) { }
    return eweed; }
    
  public String ereadLine() {
    String ereedln = null;
    try {
      ereedln = is.readLine();
    } catch (IOException e) { }
    return ereedln; }

  public void emark(int readAheadLimit) {
    try {
      is.mark(readAheadLimit);
    } catch (IOException e) { } }

  public void ereset() {
    try {
      is.reset();
    } catch (IOException e) { } }

  public void eskip(long n) {
    try {
      is.skip(n);
    } catch (IOException e) { } }

  /////////////////////////////////////////////////
  // Wrapper stuff for PrintWriter               //
  /////////////////////////////////////////////////

  public boolean checkError() {
    return os.checkError(); }

  public void print(char c) {
    os.print(c); }

  public void print(char[] s) {
    os.print(s); }

  public void print(int i) {
    os.print(i); }

  public void print(long l) {
    os.print(l); }

  public void print(boolean b) {
    os.print(b); }

  public void print(double d) {
    os.print(d); }

  public void print(Object obj) {
    os.print(obj); }

  public void print(String s) {
    os.print(s); }

  public void println() {
    os.println(); }

  public void println(char c) {
    os.println(c); }

  public void println(char[] s) {
    os.println(s); }

  public void println(int i) {
    os.println(i); }

  public void println(long l) {
    os.println(l); }

  public void println(boolean b) {
    os.println(b); }

  public void println(double d) {
    os.println(d); }

  public void println(Object obj) {
    os.println(obj); }

  public void println(String s) {
    os.println(s); }

  public void write(char[] buf) {
    os.write(buf); }

  public void write(char[] buf, int off, int len) {
    os.write(buf, off, len); }

  public void write(int c) {
    os.write(c); }

  public void write(String s) {
    os.write(s); }

  public void write(String s, int off, int len) {
    os.write(s, off, len); } }
