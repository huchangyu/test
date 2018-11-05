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

namespace WindowsFormsApp2
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            this.richTextBoxSend.Text = "";
            this.richTextBoxReceive.Text = "";
            this.listBoxState.Items.Clear();
        }
        private Socket socket;
        private Thread thread;

        private void buttonRequest_Click(object sender, EventArgs e)
        {
            IPAddress ip = IPAddress.Parse(this.textBoxIP.Text);
            IPEndPoint server = new IPEndPoint(ip, Int32.Parse(this.textBoxPort.Text));
            socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                socket.Connect(server);
            }
            catch
            {
                MessageBox.Show("连接失败");
                return;
            }
            this.buttonRequest.Enabled = false;
            this.listBoxState.Items.Add("连接服务器成功");
            Thread thread = new Thread(new ThreadStart(AcceptMessage));
            thread.Start();
        }
        private void AcceptMessage()
        {
            while (true)
                try
                {
                    NetworkStream netStream = new NetworkStream(socket);
                    byte[] datasize = new byte[4];
                    netStream.Read(datasize, 0, 4);
                    int size = System.BitConverter.ToInt32(datasize, 0);
                    Byte[] message = new byte[size];
                    int dataleft = size;
                    int start = 0;
                    while (dataleft > 0)
                    {
                        int recv = netStream.Read(message, start, dataleft);
                        start += recv;
                        dataleft -= recv;
                    }
                    this.richTextBoxReceive.Rtf = System.Text.Encoding.Unicode.GetString(message);
                }
                catch
                {
                    this.listBoxState.Items.Add("连接中断");
                    break;
                }
        }

        private void buttonClose_Click(object sender, EventArgs e)
        {
            try
            {
                
                socket.Close();
                this.listBoxState.Items.Add("与主机连接断开");
                thread.Abort();
            }
            catch
            {
               
            }
            this.buttonRequest.Enabled = true;
        }

        private void Form1_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
              try
                {
                  socket.Shutdown(SocketShutdown.Both);
                  socket.Close();
                }
              catch{}
        }

        private void buttonSend_Click(object sender, EventArgs e)
        {
            string str = this.richTextBoxSend.Rtf;
            int i = str.Length;
            if (i == 0)
            {
                return;
            }
            else
            {
                //字节数为字符数乘以2
                i *= 2;
            }
            byte[] datasize = new byte[4];
            //将三十二位整数值转化为字节数组
            datasize = System.BitConverter.GetBytes(i);
            byte[] sendbytes = System.Text.Encoding.Unicode.GetBytes(str);
            try
            {
                NetworkStream netStream = new NetworkStream(socket);
                netStream.Write(datasize, 0, 4);
                netStream.Write(sendbytes,0,sendbytes.Length);
                netStream.Flush();
                this.richTextBoxSend.Text = "";
            }
            catch
            {
                MessageBox.Show("无法发送");
            }
        }
    }
   

}
