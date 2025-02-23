Some projects (e.g. Shield_Base_v14) print text on the LCD. The font bitmap data is large, so including it in a project may push it over MDK-ARM Lite's object image size limit of 32 kB and make the build fail.  
To solve this problem, we will use a separate persistent area of memory (an overlay in flash ROM) to hold font bitmaps (and possibly profiling data). This area is retained even if we reprogram the MCU with code from a different project, as long as it goes into different memory sectors than the overlay. 

**Overlay steps 1-4:**
1. Open the Overlay project and build it. This will create the Overlay.axf image file with font data in flash ROM starting at 0x0000 8000 (or however P_LUCIDA_CONSOLE8x13 is defined). Bitmaps for three sizes of the Lucida Console font are provided: 8x13 pixels, 12x19 and 20x31. 
2. Confirm only the necessary flash sectors will be erased: Options for Target -> Utilities -> Settings -> Flash Download -> Erase Sectors must be selected (not Erase Full Chip or Do not Erase).
3. Program the MCU with this axf file. 
4. Close the Overlay project.  
Repeat Overlay steps 1-4 if you change the overlay data or erase the sectors holding the overlay data. 
You can now work on the application project. 

**Application Steps 1-3:**
1. Open the application project (e.g. Shield_Base_v14) and build it. This will create an axf image file using memory before 0x0000 8000 (32 kB). 
2. Confirm only the necessary flash sectors will be erased: Options for Target -> Utilities -> Settings -> Flash Download -> Erase Sectors must be selected (not Erase Full Chip or Do not Erase).
3. Program the MCU with this axf file.

FYI, the function LCD_Text_Init() will return 0 if valid fonts are not found in memory at the expected locations. The Shield_Base_v14 project will light the red LED and halt if it doesn't find the fonts. 
