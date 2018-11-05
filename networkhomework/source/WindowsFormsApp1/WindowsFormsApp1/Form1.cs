using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace WindowsFormsApp1
{   
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            this.ListBoxState.Items.Clear();

        }
        private Socket socket;
        private Socket clientsocket1;
        private Socket clientsocket2;
        private Thread thread1;
        private Thread thread2;
        private void button1_Click(object sender, EventArgs e)
        {
            this.buttonstart.Enabled = false;
            IPAddress ip = IPAddress.Parse(this.textBox1.Text);
            IPEndPoint server = new IPEndPoint(ip, Int32.Parse(this.textBox2.Text));
            socket = new Socket(AddressFamily.InterNetwork,SocketType.Stream,ProtocolType.Tcp);
            socket.Bind(server);
            socket.Listen(10);
            
                clientsocket1 = socket.Accept();
                clientsocket2 = socket.Accept();
            
            this.ListBoxState.Items.Add(clientsocket1.RemoteEndPoint.ToString() + "与" + clientsocket2.RemoteEndPoint.ToString() + "建立连接");
            thread1 = new Thread(new ThreadStart(Message1));
            thread2 = new Thread(new ThreadStart(Message2));
            thread1.Start();
            thread2.Start();
        }
        private void Message1()
        {
            while (true)
            {
                try
                {
                    NetworkStream netStream = new NetworkStream(clientsocket1);
                    //得到一条消息的长度
                    byte[] datasize = new byte[4];
                    netStream.Read(datasize, 0, 4);
                    int size = System.BitConverter.ToInt32(datasize, 0);
                    //循环接收直到一条完整的消息全部读入到字节数组中
                    Byte[] message = new byte[size];
                    int dataleft = size;
                    int start = 0;
                    while (dataleft > 0)
                    {
                        int recv = netStream.Read(message, start, dataleft);
                        start += recv;
                        dataleft -= recv;
                    }
                    NetworkStream netStream2 = new NetworkStream(clientsocket2);
                    netStream2.Write(datasize, 0, 4);
                    netStream2.Write(message,0,message.Length);
                    netStream2.Flush();
                }
                catch
                {
                    this.ListBoxState.Items.Add("与客户端1连接断开，请停止监听");
                    break;
                }

            }
        }
        private void Message2()
        {
            while (true)
            {
                try
                {
                    NetworkStream netStream = new NetworkStream(clientsocket2);
                    //得到一条消息的长度
                    byte[] datasize = new byte[4];
                    netStream.Read(datasize, 0, 4);
                    int size = System.BitConverter.ToInt32(datasize, 0);
                    //循环接收直到一条完整的消息全部读入到字节数组中
                    Byte[] message = new byte[size];
                    int dataleft = size;
                    int start = 0;
                    while (dataleft > 0)
                    {
                        int recv = netStream.Read(message, start, dataleft);
                        start += recv;
                        dataleft -= recv;
                    }
                    NetworkStream netStream2 = new NetworkStream(clientsocket1);
                    netStream2.Write(datasize, 0, 4);
                    netStream2.Write(message, 0, message.Length);
                    netStream2.Flush();
                }
                catch
                {
                    this.ListBoxState.Items.Add("与客户端2连接断开，请停止监听");
                    break;
                }

            }
        }

        private void buttonstop_Click(object sender, EventArgs e)
        {
            this.buttonstart.Enabled = true;
            try
            {
                
                socket.Close();
                if(clientsocket1.Connected)
                {
                    clientsocket1.Close();
                    thread1.Abort();
                }
                if (clientsocket2.Connected)
                {
                    clientsocket2.Close();
                    thread2.Abort();
                }
            }
            catch
            {
                MessageBox.Show("尚未开始工作，关闭无效");
            }
        }
        private void Form1_Closing(object sender,System.ComponentModel.CancelEventArgs e)
        {
            try
            {
                socket.Shutdown(SocketShutdown.Both);
                socket.Close();
                if (clientsocket1.Connected)
                {
                    clientsocket1.Close();
                    thread1.Abort();
                }
                if (clientsocket2.Connected)
                {
                    clientsocket2.Close();
                    thread2.Abort();
                }
            }
            catch { }
        }
    }
}
