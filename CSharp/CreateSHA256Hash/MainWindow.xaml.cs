using System;
using System.Collections.Generic;
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
using Microsoft.Win32;
using System.Security.Cryptography;
using System.IO;

namespace CreateSHA256Hash
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "Executable files (*.exe)|*.exe|All Files (*.*)|*.*";
            if (openFileDialog.ShowDialog() ?? false)
                FileNameTextBox.Text = openFileDialog.FileName;
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            SHA256 hashAlgorithm = SHA256.Create();
            FileStream fs = new FileStream(FileNameTextBox.Text, FileMode.Open);
            byte[] hash = hashAlgorithm.ComputeHash(fs);
            fs.Close();
            hashAlgorithm.Dispose();
            StringBuilder sb = new StringBuilder();
            foreach (byte b in hash)
                sb.Append(String.Format("{0:x2}", b));
            ResultTextBox.Text = sb.ToString();
        }
    }
}
