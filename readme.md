### Python section

Receiver flowgraph: TetraDMO-Receiver/DmoReceiver/tetraDMO_Receiver.grc

Transmitter flowgraph: TetraDMO-Transmitter/pluto_DmoTransmitter.grc

libraries for decoder: tetraDMO-Receiver/DmoDecoder/libs

libraries for encoder: tetraDMO-Transmitter/libs

when using python flowgraph in GNU radio, you may experience include errors of self-defined Python blocks, mostly about TETRA. It is because the module doesn't automatically update library paths that fits per machine. 
To fix the errors, in GNU radio, you need to open the source code and manually correct these paths through these steps:
- Double click to the error TETRA block to see its detailed parameters
- Choose "open editor" option. You may pick your favorite IDE
- In the source code, navigate to variables that define the include path, change it to the actual path in your computer
- Save it
  
By double-checking the flowgraph, you should see the selected TETRA block is fine and ready to use. 

### C++ section

for MAC decoder embedded in C++: 
- using terminal, navigate to TetraDMO-Receiver/DMO_CC_BLOCK/gr-TETRA_DMO
```
cd TetraDMO-Receiver/DMO_CC_BLOCK/gr-TETRA_DMO
```
- delete the previous build cache first, replace with a fresh build
```
rm -rf build/
mkdir build/
cd build/
cmake ../
make/
sudo make install/
sudo ldconfig
```
- Now, when you open GNU Radio, TETRA_DMO module will apapear and ready to use. 

Within the module folder, source code can be adjusted in include and libraries can be found in lib. It is up to your preference to make changes for the module, just remember to tell Cmake about those updates


