import java.net.*;
import java.io.*;
 
class Client {
 
  public static void usage() {
    System.err.println("usage : java Client ip_server port");
    System.exit(1);
  }
 
  public static void main(String[] args) {
 
    if (args.length != 2) {
      usage();
    }
 
    ClientTCP client = null;
    try {
      int port = Integer.parseInt(args[1]);
      client = new ClientTCP(args[0], port);
      client.requestLoop();
    }
    catch(NumberFormatException e) {
      System.err.println("invalid port number");
      System.exit(1); 
    }
    catch(IOException e) {
      System.err.println("cannot communicate with server");
      System.exit(1);
    }
  }
}