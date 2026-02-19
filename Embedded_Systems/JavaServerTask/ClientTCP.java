import java.io.*;
import java.net.*;
import java.util.*;
 
class ClientTCP  {
 
  Socket sockComm;
  BufferedReader br; PrintStream ps;
  String id; // the client id sent by the server with request 0
 
  public ClientTCP(String serverIp, int serverPort) throws IOException {
    sockComm = new Socket(serverIp, serverPort);
    ps = new PrintStream(sockComm.getOutputStream());
    br = new BufferedReader(new InputStreamReader(sockComm.getInputStream()));
    // get my id
    id = br.readLine();
  }
 
  public void requestLoop() throws IOException { 
 
    boolean stop = false;
    String reqLine = null;
    BufferedReader consoleIn = null;
    String[] reqParts = null;
 
    consoleIn = new BufferedReader(new InputStreamReader(System.in));
 
    while (!stop) {
      System.out.print("Client> ");
      reqLine = consoleIn.readLine();
      reqParts = reqLine.split(" ");
 
      if ("req1".equals(reqParts[0])) {
        String req = "REQ1 "+id+" ";
        // adding to req parameters obtined from reqParts[1] reqParts[2], ...)
        ps.println(req);
        String answer = br.readLine();
        // processing answer
      }
      else if ("req2".equals(reqParts[0])) {
        String req = "REQ2 "+id+ " ";
        // adding to req parameters obtained from reqParts[1] reqParts[2], ...)
        ps.println(req);
        String answer = br.readLine();
        // processing answer
      }
      /* etc. with other requests */
       else if ("quit".equals(reqParts[0])) {
        ps.println("quit");
        stop = true;
      }
    }
  }
}