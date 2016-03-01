using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;

using System.Management;
using System.IO;

namespace GUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        public double xValue;
        public double yValue;
        public double zValue;


        SerialProvider serial;


        ObservableCollection<KeyValuePair<double, double>> accelerometru_X = new ObservableCollection<KeyValuePair<double, double>>();
        ObservableCollection<KeyValuePair<double, double>> accelerometru_Y = new ObservableCollection<KeyValuePair<double, double>>();
        ObservableCollection<KeyValuePair<double, double>> accelerometru_Z = new ObservableCollection<KeyValuePair<double, double>>();

        List<ObservableCollection<KeyValuePair<double, double>>> dataSeries = new List<ObservableCollection<KeyValuePair<double, double>>>();
         
        public MainWindow()
        {
            InitializeComponent();
            GetPorts();
            SeTBaud();

        }
       

        private void GetPorts()
        {
            string[] ports = SerialPort.GetPortNames();
            Ports.Items.Clear();
            foreach (string port in ports)
            {
                Ports.Items.Add(port);
            }
        }

        private void SeTBaud()
        {
            Bauds.Items.Add("2400");
            Bauds.Items.Add("4800");
            Bauds.Items.Add("9600");
            Bauds.Items.Add("19200");
            Bauds.Items.Add("38400");
            Bauds.Items.Add("57600");
            Bauds.Items.Add("115200");
          
        }

        //private void GetSerialName()
        //{
        //    string result = "";
        //    using (var searcher = new ManagementObjectSearcher("SELECT * FROM WIN32_SerialPort"))
        //    {
        //        string[] portnames = SerialPort.GetPortNames();
        //        var ports = searcher.Get().Cast<ManagementBaseObject>().ToList();
        //        var tList = (from n in portnames join p in ports on n equals p["DeviceID"].ToString() select n + " - " + p["Caption"]).ToList();

        //        foreach (string s in tList)
        //        {
        //            result = result + s;
        //        }
        //    }
        //}
        

        private void buttonConnect_Click(object sender, RoutedEventArgs e)
        {
            if ((string)buttonConnect.Content == "Connect")
            {
                string selectedPort = Ports.Text;
                string Baud = Bauds.Text;
                int selectedBaud = int.Parse(Baud);

                
                serial = new SerialProvider(selectedPort,selectedBaud);
                dataSeries.Add(accelerometru_X);
                dataSeries.Add(accelerometru_Y);
                dataSeries.Add(accelerometru_Z);
                showColumnChart();

                DispatcherTimer timer = new DispatcherTimer();
                timer.Interval = new TimeSpan(0, 0, 0, 0, 100);
                timer.Tick += new EventHandler(timer_Tick);
                timer.IsEnabled = true;
                buttonConnect.Content = "Disconnect";
            }
            else
            {
                try
                {
                    serial.Dispose();
                    buttonConnect.Content = "Connect";
                }
                catch
                {

                }
            }

        }


        double i = 0;
        private void timer_Tick(object sender, EventArgs e)
        {
            var databuff = serial.GetFirstDataBuff();
            if (databuff != null)
            {
                string[] values = databuff.Split(';');
                xValue = Double.Parse(values[0]);
                yValue = Double.Parse(values[1]);
                zValue = Double.Parse(values[2]);

                accelerometru_X.Add(new KeyValuePair<double, double>(i, xValue));
                accelerometru_Y.Add(new KeyValuePair<double, double>(i, yValue));
                accelerometru_Z.Add(new KeyValuePair<double, double>(i, zValue));
                i += 1;
                //Application.Current.Dispatcher.Invoke(new Action(() => { textBlock.Text = "X:" + xValue + "Y:" + yValue + "Z:" + zValue + "\n"; }));
            }
        }



        private void showColumnChart()
        {
            lineChart.DataContext = dataSeries;
        }

    }
}
