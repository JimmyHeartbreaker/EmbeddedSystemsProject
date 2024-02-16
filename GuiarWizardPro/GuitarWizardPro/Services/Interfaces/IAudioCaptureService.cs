using GuitarWizardPro.ViewModel;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MauiApp3.Services.Interfaces
{
    public interface IAudioCapureService
    {
        void Start();
        Task Initialize();

        unsafe event EventHandler<(nint, int)>? AudioFrameProcessed;

    }

    public class StubAudioCaptureService : IAudioCapureService
    {

        public event EventHandler<(nint, int)>? AudioFrameProcessed;

        public Task Initialize()
        {
            throw new NotImplementedException();
        }

        public void Start() { }


    }

}
