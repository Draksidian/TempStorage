﻿using System;
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
using System.Windows.Shapes;

namespace BLFClient
{
    /// <summary>
    /// Interaction logic for PhoneNumberNew.xaml
    /// </summary>
    public partial class PhoneNumberNew : Window
    {
        int ParentX,ParentY;
        PhoneNumberSetupSettings GeneralSettings;
        string Prefix;

        public PhoneNumberNew(PhoneNumber EditedNumber)
        {
            InitializeComponent();
            GeneralSettings = new PhoneNumberSetupSettings();
            ParentX = EditedNumber.GetX();
            ParentY = EditedNumber.GetY();
            GeneralSettings.FontSize = 0;
            if (EditedNumber.GetExtension() != 0)
                this.SubscriberExtension.Text += EditedNumber.GetExtension().ToString();
            else if (App.Current != null && App.Current.MainWindow != null)
            {
                Prefix = (App.Current.MainWindow as MainWindow).GetPrefix();
                this.SubscriberExtension.Text = Prefix;
            }
            this.SubscriberEmail.Text = EditedNumber.GetEmail();
            this.SubscriberNote.Text = EditedNumber.GetNote();
            this.TB_UserName.Text = EditedNumber.GetUserName();
        }

        private void Button_Click_Ok(object sender, RoutedEventArgs e)
        {
            if (App.Current != null && App.Current.MainWindow != null)
            {
                PhoneNumber EditedPhoneNumber = (App.Current.MainWindow as MainWindow).PhoneNumberGet(ParentX, ParentY);
                EditedPhoneNumber.SetName(this.TB_UserName.Text);
                try
                {
                    string sExtension = this.SubscriberExtension.Text;
                    //does the number still have the prefix we set earlier ?
                    try { 
                        if (sExtension.StartsWith(Prefix))
                        {
                            sExtension = sExtension.Substring(Prefix.Length);
                            EditedPhoneNumber.SetPrefix(Prefix);
                        }
                    } catch (Exception) { };
                    int Extension = Int32.Parse(sExtension);
                    EditedPhoneNumber.SetExtension(Extension);
                }catch (Exception) { };
                EditedPhoneNumber.SetEmail(this.SubscriberEmail.Text);
                EditedPhoneNumber.SetNote(this.SubscriberNote.Text);
                EditedPhoneNumber.OnFontSettingChanged(GeneralSettings);
            }

            //all done. close it
            this.Close();
        }

        private void Button_Click_Cancel(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void Button_Click_Font(object sender, RoutedEventArgs e)
        {
            System.Windows.Forms.FontDialog fd = new System.Windows.Forms.FontDialog();
            System.Windows.Forms.DialogResult dr = fd.ShowDialog();
            if (dr != System.Windows.Forms.DialogResult.Cancel)
            {
                GeneralSettings.FontFamily = new System.Windows.Media.FontFamily(fd.Font.Name);
                GeneralSettings.FontSize = fd.Font.Size * 96.0 / 72.0;
                GeneralSettings.FontWeight_ = fd.Font.Bold ? FontWeights.Bold : FontWeights.Regular;
                GeneralSettings.FontStyle_ = fd.Font.Italic ? FontStyles.Italic : FontStyles.Normal;
            }
        }
    }
}
