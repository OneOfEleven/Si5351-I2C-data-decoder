# Si5351-I2C-data-decoder

Display the Si5351 register values, PLL frequencies and CLK output states/frequencies from captured Si5351 I2C data.

## About

The example Si5351 I2C capture files were done using a cheap ebay logic analyser clone off ebay that is for use with the Saleae Logic Analyser v2.3.31 Windows software (free to download and use).

All you do is attach two of the logic analyser channels to the Si5351 SDA and SCL lines on the PCB you want to analyse (not forgetting the 0V connection of cause), then simply do a quick capture and save the HEX data stream text (from the I2C decoder terminal window) to a text file - remove all the text on each line before the 1st Si5351 byte (the register address byte).

Then just load that text file into this software. You can then single step through the captured data (select any line in the left list) to see what the exact PLL frequencies and CLK output states/frequencies are with each step - wonderful for debugging code or reverse engineering someone else's unit (such as laser range measuring units).

## Compiling

Now have a Borland C++ Builder v6 version and a Qt version.

## Stuff

Click the desired lines (and/or use your keyboard up/down keys) on the left hand listview to step through to see the register values/pll frequencies/clk-out states at each step ..

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
