using Microsoft.UI.Xaml;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Security.Policy;
using System.Text;
using System.Threading.Tasks;

namespace GuitarWizardPro.WPF
{
    public static class GuitarNotes
    {
        public static class OpenNotes
        {
            public const int E1 = 82;
            public const int A1 = 110;
            public const int D1 = 147;
            public const int G2 = 196;
            public const int B2 = 248;
            public const int E3 = 328;
        }
        public const int A1sFret1 = 117;
        public const int C1Fret3 = 131;
        public const int C1sFret4 = 139;
        public const int D1sFret1 = 156;
        public const int G2sFret1 = 208;
        public const int F1Fret1 = 87;
        public const int F1sFret2 = 93;

        public static List<Note> Notes = new List<Note>();

        private static void GenerateNotes(string rootName, int givenOctave, int frequency)
        {
            for (int octave = 1; octave <= 4; octave++)
            {
                if (octave == givenOctave)
                {
                    Notes.Add(new Note()
                    {
                        Name = rootName + octave,
                        Frequency = frequency
                    });
                }
                else
                {
                    var diff = octave - givenOctave;
                    var octaveFrequency = (int)Math.Round(frequency * Math.Pow(2, diff));
                    Notes.Add(new Note()
                    {
                        Name = rootName + octave,
                        Frequency = octaveFrequency
                    });
                }
            }

        }
        static GuitarNotes()
        {
            GenerateNotes("A", 1, OpenNotes.A1);
            GenerateNotes("A#", 1, A1sFret1);
            GenerateNotes("B", 2, OpenNotes.B2);
            GenerateNotes("C", 1, C1Fret3);
            GenerateNotes("C#", 1, C1sFret4);
            GenerateNotes("D", 1, OpenNotes.D1);
            GenerateNotes("D#", 1, D1sFret1);
            GenerateNotes("E", 1, OpenNotes.E1);
            GenerateNotes("F", 1, F1Fret1);
            GenerateNotes("F#", 1, F1sFret2);
            GenerateNotes("G", 2, OpenNotes.G2);
            GenerateNotes("G#", 2, G2sFret1);
            Notes = Notes.OrderBy(n => n.Frequency).ToList();
        }
    }
    public class Note
    {
        public int Frequency { get; set; }
        public string Name { get; set; }
    }

}
