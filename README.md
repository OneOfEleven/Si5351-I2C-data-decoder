# Si5351-I2C-data-decoder

Display the Si5351 register values, PLL frequencies and CLK output states/frequencies from captured Si5351 I2C data.

## Compiling

Use either the free Qt dev software or Borland C++ Builder v6 version (quite old now but still very nice and simple).

## Capture hardware

If you don't already have a logic analyser then get yourself one of these 24MHz 8-channel units off ebay or such like ..

<div align="center">
<img src="/LogicAnalyser_24MHz_8ch.png">
</div>

## Using

All you do is attach two of the logic analyser channels to the Si5351 SDA and SCL lines on the PCB you want to analyse (not forgetting the 0V connection of cause), then simply do a quick capture and save the HEX data stream text (from the I2C decoder terminal window) to a text file - remove all the text on each line before the 1st Si5351 byte (the register address byte).

There are some example Si5351 I2C capture text files to play with.

Load that text file into this software, then click the desired line (and/or use your keyboard up/down keys) on the left hand listview to step through to see the register values/pll frequencies/clk-out states at each step ..

<div align="center">
<img src="/Image1.png">
</div>

Free Saleae Logic Analyser v2.3.31 Windows software captured I2C data ..

<div align="center">
<img src="/Image2.png">
</div>

<div align="center">
<img src="/Image3.png">
</div>
