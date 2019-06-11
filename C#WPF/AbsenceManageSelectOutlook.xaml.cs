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
    /// Interaction logic for AbsenceManageSelectOutlook.xaml
    /// </summary>
    public partial class AbsenceManageSelectOutlook : Window
    {
        public AbsenceManageSelectOutlook()
        {
            InitializeComponent();

            this.Profiles.Items.Add("(default)");
            List<string> accts = Globals.OutlookService.GetAccountList();
            foreach (string act in accts)
                this.Profiles.Items.Add(act);
            this.Profiles.SelectedIndex = 0;

            //if we push enter we presume we pushed button "ok"
            RoutedCommand firstSettings = new RoutedCommand();
            firstSettings.InputGestures.Add(new KeyGesture(Key.Enter, ModifierKeys.None));
            CommandBindings.Add(new CommandBinding(firstSettings, Button_Click_OK));
            RoutedCommand SecondSettings = new RoutedCommand();
            SecondSettings.InputGestures.Add(new KeyGesture(Key.Escape, ModifierKeys.None));
            CommandBindings.Add(new CommandBinding(SecondSettings, Button_Click_Cancel));

            TranslateLocalize();
            this.Owner = App.Current.MainWindow;
            this.Left = this.Owner.Left + this.Owner.Width / 2 - this.Width / 2;
            this.Top = this.Owner.Top + this.Owner.Height / 2 - this.Height / 2;
        }

        private void Button_Click_OK(object sender, RoutedEventArgs e)
        {
//            Globals.Config.SetConfig("AbsenceInfo", "OutlookProfile", this.Profiles.SelectedItem.ToString());
            this.Close();
        }

        private void Button_Click_Cancel(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void TranslateLocalize()
        {
            Globals.MultilangManager.TranslateUIComponent(this);
        }
    }
}
