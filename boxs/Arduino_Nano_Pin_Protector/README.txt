                   .:                     :,                                          
,:::::::: ::`      :::                   :::                                          
,:::::::: ::`      :::                   :::                                          
.,,:::,,, ::`.:,   ... .. .:,     .:. ..`... ..`   ..   .:,    .. ::  .::,     .:,`   
   ,::    :::::::  ::, :::::::  `:::::::.,:: :::  ::: .::::::  ::::: ::::::  .::::::  
   ,::    :::::::: ::, :::::::: ::::::::.,:: :::  ::: :::,:::, ::::: ::::::, :::::::: 
   ,::    :::  ::: ::, :::  :::`::.  :::.,::  ::,`::`:::   ::: :::  `::,`   :::   ::: 
   ,::    ::.  ::: ::, ::`  :::.::    ::.,::  :::::: ::::::::: ::`   :::::: ::::::::: 
   ,::    ::.  ::: ::, ::`  :::.::    ::.,::  .::::: ::::::::: ::`    ::::::::::::::: 
   ,::    ::.  ::: ::, ::`  ::: ::: `:::.,::   ::::  :::`  ,,, ::`  .::  :::.::.  ,,, 
   ,::    ::.  ::: ::, ::`  ::: ::::::::.,::   ::::   :::::::` ::`   ::::::: :::::::. 
   ,::    ::.  ::: ::, ::`  :::  :::::::`,::    ::.    :::::`  ::`   ::::::   :::::.  
                                ::,  ,::                               ``             
                                ::::::::                                              
                                 ::::::                                               
                                  `,,`


https://www.thingiverse.com/thing:880427
Arduino Nano Pin Protector by enif is licensed under the Creative Commons - Attribution - Share Alike license.
http://creativecommons.org/licenses/by-sa/3.0/

# Summary

I really like to tinker with an Arduino Nano (or one its many compatibles), since it provides the same functionality as the Uno, but on a much smaller footprint.  However, what I like less are the vertical pins underneath the PCB, since they are exposed to the working desk and are therefore prone for wrong contacts and even shorting due to tools or other metallic items on the desk.  Also, since the leads are attached to the lower side of the PCB, it is difficult to keep the Nano upright, so that the pin labels and LED indicators on the top side stay visible.  
   
So this project presents a way to overcome these difficulties by making all pin connections horizontal and providing a shell around the Nano which protects all pins from accidental exposure and keeps  the Nano in a stable horizontal position.  

Right angle pin headers mounted on the upper side of the Nano PCB are used instead of the straight headers normally used. Two 15-pin single row and one 3x2-pin double row pin header arrays are needed.  This way, all connections to he Nano are horizontal and are safely "hidden" between the upper and lower part of the shell.   

The so covered horizontal pin connectors increase the surface of the Nano shell, so that there is enough space for attaching a sticker on the top shell which identifies each pin in a nicely readable size.  

The top of the shell has a cutout for the 4 LEDs and has an integrated RESET button which is printed in-situ and held in position by its waisted shape.  

Given that there are several variants of the Nano design which essentially differ slightly in the placement of the LEDs and the RESET button, so their longitudinal position is parametrized (_ledy_ and _resety_, measured in mm from the USB end of the PCB) and can thus be easily adapted when needed.  

In addition to this SCAD file, there is also a corresponding PostScript file which generates the labels to stick on the top part of the shell. It, too, is configurable for the exact position of the RESET button and the LEDs (using again the same variables _ledy_ and _resety_)  

Ready-to-print STL files (3D-printing of the Nano shell) and PDF files (2D printing of the label stickers) are provided for the 3 main variants of the Nano:  

* Original Arduino Nano:  _ledy_=29mm, _resety_=24mm, LED order is "_TX RX PWR D13_"  
* "Baite" Compatible: _ledy_=30mm, _resety_=25mm, LED order is "_D13 PWR RX TX_"  
* "New" Nano 3.0 Compatible with small LED spacing:  _ledy_=32mm, _resety_=26mm, LED order is "_TX RX PWR D13_"  

Measure the distances of LEDs and Reset button from the USB end of the PCB and print out the corresponding STL and PDF files, or modify the SCAD and PS sources accordingly.  

_Update 2015-07-21: I have now uploaded an improved version of the Nano Pin Protector which contains air flow windows underneath the lateral pins in order to improve the air flow below the Nano PCB. I have also added a new variant with ledy=33 and resety=27, which gives a better fit on some Nano versions._  

_Update 2015-08-14: If you are interested in using a much more powerful 32-bit micro-controller board with about the same size, price and Arduino-IDE as the Nano, then check out my new [Maple Mini Pin Protector](http://www.thingiverse.com/thing:966920)._

# Instructions

The Nano protector shells were printed from 3mm ABS filament on my DIY Prusa i3 using a 0.4mm nozzle with 0.56mm extrusion width and 0.252 layer height. I print on a clear mirror without Kapton tape but thoroughly cleaned with acetone and double concentrated lemon juice.   

Since the two parts of the protective shell hold to each other by a "click"-type mechanism, ABS seems to be the material of choice for this project.