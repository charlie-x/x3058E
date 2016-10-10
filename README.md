# x3058E
Windows x64 remote VISA interface to Rigol RM3058E Digital Multimeter.

Visual Studio 2015 x64 

Works with devices
  :0x1AB1::0x09C4:
  
Tested with NI VISA

Currently needs this font to be installed...
http://www.dafont.com/alarm-clock.font

Only partially implemented, but functional. Early POC

<b>Todo List

Find modes DMM in is at boot

Figure out "Sensor"

Fix auto switch modes, it'll set wrong scale modes for range buttons, doesn't harm anything just means some presses are ghosts.

Add remaining bitmaps for buttons, subclass CButton for indicators

Pull in more info from DMM, ranges, min, max etc.


