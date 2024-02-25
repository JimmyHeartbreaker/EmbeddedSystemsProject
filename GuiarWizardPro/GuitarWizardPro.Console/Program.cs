using System.Net.Sockets;
using System.Net;
using System.Text;
using System.Diagnostics;

UdpClient udpClient = new UdpClient( 1234);
try
{

    //udpClient.Connect(new IPEndPoint(IPAddress.Any, 11000));
    IPEndPoint groupEP = new IPEndPoint(IPAddress.Any, 1234);


    var initRecieved = udpClient.Receive(ref groupEP);

    udpClient.Connect(groupEP);

    Stopwatch sw = new Stopwatch();
    var data = new short[500];
    sw.Start();
    int i = 0;
    while (i<64)
    {
        var recieved = await udpClient.ReceiveAsync();
        for(int j=0;j<recieved.Buffer.Length;j+=2)
        {
            data[j / 2] = BitConverter.ToInt16(recieved.Buffer, j);
        }
        
        i++;
    }
    sw.Stop();



    udpClient.Close();
}
catch (Exception e)
{
    Console.WriteLine(e.ToString());
}

// See https://aka.ms/new-console-template for more information
Console.WriteLine("Hello, World!");
