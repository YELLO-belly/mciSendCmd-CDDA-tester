The mciSendString CDDA tester accepts plain sentences in the command line.
(as long as they are valid mci strings...)
For example:

open cdaudio
set cdaudio time format tmsf
play cdaudio from 1 to 2 notify
pause cdaudio
play cdaudio to 2 notify

Or using alias:

open d: type cdaudio alias cd1
set cd1 time format tmsf
play cd1 from 1 to 2 notify
stop cd1
close cd1

Status information:

status cdaudio mode
status cdaudio position
status cdaudio number of tracks
status cdaudio length
status cdaudio length track 5
status cdaudio position track 5

Note that you can also open and play wave files or midi (or even .mp3 on newer Windows versions).
e.g.:

open c:\sound\wavefile.wav alias wave1
play wave1

For a complete list of mciSendStrings see:
https://docs.microsoft.com/fi-fi/windows/win32/multimedia/multimedia-command-strings
