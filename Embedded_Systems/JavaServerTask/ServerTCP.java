import java.io.*;
import java.net.*;

class ServerTCP {
    BufferedReader br; 
    PrintStream ps;
    ServerSocket sockConn;
    Socket sockComm;
    int idClient;

    public ServerTCP(int port) throws IOException {
        idClient = 0;
        sockConn = new ServerSocket(port);
    }

    public void mainLoop() throws IOException {
        while (true) {
            sockComm = sockConn.accept();
            try {
                br = new BufferedReader(new InputStreamReader(sockComm.getInputStream()));
                ps = new PrintStream(sockComm.getOutputStream());
                idClient += 1;
                ps.println(idClient); // send client ID
                requestLoop();
                br.close();
                ps.close();
            } catch (IOException e) {
                System.out.println("client disconnected");
            }
        }
    }

    public void requestLoop() throws IOException {
        String req;
        boolean stop = false;

        while (!stop) {
            req = br.readLine();
            if (req == null) return;
            if (req.isEmpty()) continue;

            String[] reqParts = req.split(" ");
            if (reqParts.length < 2) {
                ps.println("ERR missing parameters");
                continue;
            }

            int id;
            try {
                id = Integer.parseInt(reqParts[1]);
            } catch (NumberFormatException e) {
                ps.println("ERR invalid type of parameter");
                continue;
            }

            if (id != idClient) {
                ps.println("ERR invalid id");
                continue;
            }

            switch (reqParts[0]) {
                case "REQ1":
                    if (reqParts.length != 4) {
                        ps.println("ERR invalid number of parameters");
                    } else {
                        processRequest1(reqParts[2], reqParts[3]);
                    }
                    break;

                case "REQ2":
                    if (reqParts.length != 3) {
                        ps.println("ERR invalid number of parameters");
                    } else {
                        processRequest2(reqParts[2]);
                    }
                    break;

                case "quit":
                    stop = true;
                    break;

                default:
                    ps.println("ERR unknown request");
            }
        }
    }

    public void processRequest1(String param1, String param2) throws IOException {
        try {
            int num1 = Integer.parseInt(param1);
            int num2 = Integer.parseInt(param2);
            ps.println("OK " + (num1 + num2));
        } catch (NumberFormatException e) {
            ps.println("ERR invalid type of parameter");
        }
    }

    public void processRequest2(String param) throws IOException {
        ps.println("OK " + param.toUpperCase());
    }

    public static void main(String[] args) throws IOException {
        ServerTCP server = new ServerTCP(12345);
        server.mainLoop();
    }
}
