using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Data.SqlClient;

namespace steamappallinone
{
    public partial class Form2 : Form
    {
        public Form2()
        {
            InitializeComponent();
        }

        private void textBox3_TextChanged(object sender, EventArgs e)
        {

        }

        private void textBox3_TextChanged_1(object sender, EventArgs e)
        {

        }

        private void Form2_Load(object sender, EventArgs e)
        {
            textBox3.AppendText(temp.tem);

            SqlConnection conn = new SqlConnection(@"server=.\SQLEXPRESS;integrated security=true;database=work");
            try
            {
                conn.Open();
            }
            catch (SqlException ex)
            {
                MessageBox.Show("Can't connect to the server, please check your computer", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }
            string sql = @"select * from steamgame ";
            SqlCommand cmd = new SqlCommand(sql, conn);
            int count=0;

            try
            {
                SqlDataReader rdr = cmd.ExecuteReader();
                while (rdr.Read())
                {
                    if (rdr[0].ToString() == temp.tem)
                    {
                        count++;
                        
                    }
                }

                
            }
            catch (SqlException ex)
            {
                MessageBox.Show(ex.Message + ex.StackTrace, "exception details");
                return;
            }

            textBox1.AppendText(count.ToString ());

        }

        private void button1_Click(object sender, EventArgs e)
        {
            SqlConnection conn = new SqlConnection(@"server=.\SQLEXPRESS;integrated security=true;database=work");
            try
            {
                conn.Open();
            }
            catch (SqlException ex)
            {
                MessageBox.Show("Can't connect to the server, please check your computer", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }
            if (comboBox1.Text == "dota2")
            {
                string sql = @"select * from dota2 ";
                SqlCommand cmd = new SqlCommand(sql, conn);
                try
                {
                    SqlDataReader rdr = cmd.ExecuteReader();
                    while (rdr.Read())
                    {
                        if (rdr[0].ToString() == temp.tem )
                        {
                            textBox2.Text=("游戏场次：       "+rdr[1].ToString()+" 场\r\n胜率：           "+rdr[2].ToString()+"\r\n账号价值:        "+rdr[3].ToString()+"\r\n最常使用英雄:    "+rdr[4].ToString());
                            
                            return;
                        }
                    }

                }
                catch (SqlException ex)
                {
                    
                    return;
                }
            }
            else if(comboBox1.Text=="csgo" )
            {
                string sql = @"select * from csgo ";
                SqlCommand cmd = new SqlCommand(sql, conn);
                try
                {
                    SqlDataReader rdr = cmd.ExecuteReader();
                    while (rdr.Read())
                    {
                        if (rdr[0].ToString() == temp.tem)
                        {
                            textBox2.Text=("游戏场次：       " + rdr[1].ToString() + " 场\r\n胜率：           " + rdr[2].ToString() + "\r\n账号价值:        " + rdr[3].ToString() + "\r\n最常使用武器:    " + rdr[4].ToString());

                            return;
                        }
                    }

                }
                catch (SqlException ex)
                {

                    return;
                }
            }
            else if(comboBox1.Text=="battleground")
            {
                string sql = @"select * from battleground ";
                SqlCommand cmd = new SqlCommand(sql, conn);
                try
                {

                    SqlDataReader rdr = cmd.ExecuteReader();
                    while (rdr.Read())
                    {
                        if (rdr[0].ToString() == temp.tem)
                        {
                            textBox2.Text =("游戏场次：       " + rdr[1].ToString() + " 场\r\n胜率：           " + rdr[2].ToString() + "\r\n账号价值:        " + rdr[3].ToString() + "\r\n平均击杀:        " + rdr[4].ToString()+"\r\n平均存活时间:    "+rdr[5].ToString());

                            return;
                        }
                    }

                }
                catch (SqlException ex)
                {

                    return;
                }
            }        
            else
            {
                MessageBox.Show("请选择查询项目", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
                      

        }
    }
}
