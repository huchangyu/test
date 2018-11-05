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
    
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void label3_Click(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            Form2 obj = new Form2();
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
            string sql = @"select * from login ";
            SqlCommand cmd = new SqlCommand(sql,conn);
            try
            {
                SqlDataReader rdr = cmd.ExecuteReader();
                while (rdr.Read())
                {
                    if (rdr[0].ToString() == textBox1.Text && rdr[1].ToString() == textBox2.Text)
                    {
                        temp.tem = textBox1.Text;
                        obj.Show();
                        return;
                    } 
                }

                MessageBox.Show("用户名或密码错误", "Sorry", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }
            catch(SqlException ex) {
               MessageBox.Show(ex.Message + ex.StackTrace,"exception details");
                return;
            }

            

        }
    }
}
