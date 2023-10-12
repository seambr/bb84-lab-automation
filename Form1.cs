using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace bb84
{

    public partial class Form1 : Form
    {

        string dataOUT;
        string dataIN;
        public Form1()
        {
            InitializeComponent();

        }

        private void button1_Click(object sender, EventArgs e)
        {
            // Send One Pulse

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void label5_Click(object sender, EventArgs e)
        {

        }

        private void comboBox5_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void Form1_Load(object sender, EventArgs e)
        {
            string[] ports = SerialPort.GetPortNames();
            comboBox1.Items.AddRange(ports);
        }

        private void button1_Click_1(object sender, EventArgs e)
        {

            try
            {
                serialPort1.PortName = comboBox1.Text;
                serialPort1.BaudRate = Convert.ToInt32(comboBox2.Text);
                serialPort1.DataBits = Convert.ToInt32(comboBox3.Text);
                serialPort1.StopBits = (StopBits)Enum.Parse(typeof(StopBits), comboBox4.Text);
                serialPort1.Parity = (Parity)Enum.Parse(typeof(Parity), comboBox4.Text);

                serialPort1.Open();
                

                serialPort2.PortName = "COM3";
                serialPort2.BaudRate = Convert.ToInt32(comboBox2.Text);
                serialPort2.DataBits = Convert.ToInt32(comboBox3.Text);
                serialPort2.StopBits = (StopBits)Enum.Parse(typeof(StopBits), comboBox4.Text);
                serialPort2.Parity = (Parity)Enum.Parse(typeof(Parity), comboBox4.Text);

                serialPort2.Open();

                progressBar1.Value = 100;

            }
            catch (Exception err)
            {
                MessageBox.Show(err.Message, "Error", MessageBoxButtons.OK,MessageBoxIcon.Error);
            }


        }

        private void button2_Click(object sender, EventArgs e)
        {

            if (serialPort1.IsOpen)
            {
                serialPort1.Close();
                serialPort2.Close();
                progressBar1.Value = 0;
            }

        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                dataOUT = richTextBox1.Text;
                if (checkBoxWriteLine.Checked)
                {
                    serialPort1.WriteLine(dataOUT);
                }
                else
                {
                    serialPort1.Write(dataOUT);
                }
            }
        }

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            dataIN = serialPort1.ReadExisting();
            this.Invoke(new EventHandler(ShowData));
        }


        private void ShowData(object sender, EventArgs e)
        {
            rtbOutput.Text = dataIN;
        }

        private void buttonClearIn_Click(object sender, EventArgs e)
        {
            dataOUT = "";
            richTextBox1.Text = dataOUT;
        }

        private void contextMenuStrip1_Opening(object sender, CancelEventArgs e)
        {

        }

        private void button4_Click(object sender, EventArgs e)
        {
            dataIN = "";
            rtbOutput.Text = dataIN;
        }

        private void serialPort2_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            dataIN = serialPort2.ReadExisting();
            this.Invoke(new EventHandler(ShowData));
        }
    }
}
