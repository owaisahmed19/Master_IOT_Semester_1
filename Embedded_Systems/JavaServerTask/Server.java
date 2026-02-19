import java.net.*;
import java.io.*;
 
class Server {
 
  public static void usage() {
    System.err.println("usage : java Server port");
    System.exit(1);
  }
 
  public static void main(String[] args) {
 
    if (args.length != 1) {
      usage();
    }
 
    ServerTCP server = null;
    try {
      int port = Integer.parseInt(args[0]);
      server = new ServerTCP(port);
      server.mainLoop();
    }
    catch(NumberFormatException e) {
      System.err.println("invalid port number");
      System.exit(1);
    }
    catch(IOException e) {
      System.err.println("cannot communicate with client");
      System.exit(1);
    }
  }
}