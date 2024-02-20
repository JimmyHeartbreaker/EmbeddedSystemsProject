namespace GuitarWizardPro
{
    public partial class App : Application
    {
        public const string DeviceName = "GuitarWizardPro";
        public App()
        {
            InitializeComponent();

            MainPage = new AppShell();
        }
    }
}
