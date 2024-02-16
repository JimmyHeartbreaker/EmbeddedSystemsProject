﻿using Microsoft.Extensions.Logging;


namespace GuitarWizardPro
{
    public static class MauiProgram
    {
        public static MauiApp CreateMauiApp(Action<IServiceCollection> serviceRegistrar)
        {
            var builder = MauiApp.CreateBuilder();
            builder
                .UseMauiApp<App>()
                .ConfigureFonts(fonts =>
                {
                    fonts.AddFont("OpenSans-Regular.ttf", "OpenSansRegular");
                    fonts.AddFont("OpenSans-Semibold.ttf", "OpenSansSemibold");
                });
            serviceRegistrar(builder.Services);
#if DEBUG
            builder.Logging.AddDebug();
#endif

            return builder.Build();
        }
    }
}
