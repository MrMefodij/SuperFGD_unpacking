# Unpacking

This project is the home for all the unpacking-related codes. All of the unpacking-related codes should be in this central repository in their respective directories.

At the time of writing this, the full framework for the ND280 integration is not yet complete and some classes are still to be defined in [oaEvent](https://git.t2k.org/nd280/base/oaEvent) and [oaRawEvent](https://git.t2k.org/nd280/base/oaRawEvent). With this in mind, we should be thinking about when the unpacking software will be integrated into eventUnpack. We should be thinking about the ND280 classes that will be used by the unpacking in both the reading and outputting of the raw data.

See [eventUnpack](https://git.t2k.org/nd280/calib/eventUnpack)

SuperFGD Unpacking is commissioning software to convert binary data from OCB and FEB to root file with oaEventDummy.
Unpacking include libs:
- SFGDUnpack (src) is FEBs 32 bits words convector;
- SFGDoaEventDummy (oaEventDummy) with TSFGDDigit class is define of RootDict;
- Commissioning (commissioning) - libs for commissioning features;
- macros (examples)
- SFGDUnpack -  to unpack daq files from OCB and FEBs
- SFGDEventDisplay - to plot events
- SFGDCalibration - to calibrate MPPCs


## Requirements:
- ROOT 6 (tested);
- Boost 1.81 (tested) (Only for commissioning library)
- Or you can use container. Please see [README.md](container_kekcc/README.md) for detail.

### Tips for boost compilation in Linux
 - Boost can be installed by homebrew (macOS) or from source code
 - Here I wrote some tips to install it from the source code in Linux
 1. Download the source code from [here](https://www.boost.org/).
 2. Unzip it (the directory is denoted as `$BOOST_ROOT` in the following).
 3. `cd $BOOST_ROOT` && `./bootstrap.sh` && `./b2`
 4. Specify the following environmental variables. In the case of bash, 
 ```
export Boost_INCLUDE_DIR=$BOOST_ROOT
export Boost_DIR=$BOOST_ROOT/stage/lib/cmake/Boost-1.82.0
 ```
 - Notes: 
   - Rewrite `$BOOST_ROOT` for your environment.
   - This setting is not necessary in the case you use homebrew.

## Build:
    cd build
    source ${ROOT}/bin/thisroot.sh
    cmake ../
    make

## Run:
    cd build

    ./unpack -f ../{DataFolder}/DaqFileName.bin {output file will be created at ../{datafolder}/DaqFileName_plots.root}
    ./printEvents -f ../{DataFolder}/DaqFileName_plots.root {output file will be created at ../{datafolder}/DaqFileName_events.root}
    ./calibration -f ../{DataFolder}/DaqFileName.bin {output file will be created at ../{datafolder}/DaqFileName_channels_signal.root}

## Recommended environmental settings
If you specify `$UNPACKING_ROOT` and `$PATH`, you can execute calibration from wherever you are.
```
export UNPACKING_ROOT=[YOUR UNKACKER DIRECTORY]
export PATH=$UNPACKING_ROOT/bin:$PATH
```
