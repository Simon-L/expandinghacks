# Expand This!

Little experiment to expand "foreign" modules from another plugin.

This works by integrating a header file that declares a module struct matching exactly the one used to compile another module/plugin, then at runtime when the Expand This module is put right next to one of either VCV Compare or Impromptu PhraseSeq16, press B1 to detect the module and then B2. For PhraseSeq16, when pressing B2 it prints the current running state and the current step the module is at.  
Well, now, what could that be used for?

Contains code from ImpromptuModular, provided here under its original license: https://github.com/MarcBoule/ImpromptuModular See for details. Same for VCV Compare, code is here https://github.com/VCVRack/Fundamental
The PhraseSeq16 header was made by editing this module's original cpp file 