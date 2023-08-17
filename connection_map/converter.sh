#!bin/bash

FILECSV=SFG_Geometry_Map_v11.csv
FILETXT=connectionMap_v11.txt
if [ -e $FILETXT ] ; then 
  rm $FILETXT
fi

while read line
do
  PCBPosition=$(echo $line} | cut -d , -f 1)
  PCBID=$(echo $line} | cut -d , -f 2)
  PCBTYPE=$(echo $line} | cut -d , -f 3)
  RotateAngle=$(echo $line} | cut -d , -f 4)
  MPPCCh=$(echo $line} | cut -d , -f 5)
  x=$(echo $line} | cut -d , -f 6)
  y=$(echo $line} | cut -d , -f 7)
  z=$(echo $line} | cut -d , -f 8)
  channelIDMC=$(echo $line} | cut -d , -f 9)
  CableType=$(echo $line} | cut -d , -f 10)
  CableLength=$(echo $line} | cut -d , -f 11)
  Crate=$(echo $line} | cut -d , -f 12)
  Slot=$(echo $line} | cut -d , -f 13)
  Position=$(echo $line} | cut -d , -f 14)
  CITIROCCh32=$(echo $line} | cut -d , -f 15)
  CITIROCCh256=$(echo $line} | cut -d , -f 16)
  channelIDELEC=$(echo $line} | cut -d , -f 17)

  #PCBPosition=$(echo $line} | cut -d , -f 1)
  #PCBID=$(echo $line} | cut -d , -f 2)
  #PCBTYPE=$(echo $line} | cut -d , -f 3)
  #RotateAngle=$(echo $line} | cut -d , -f 4)
  #MPPCCh=$(echo $line} | cut -d , -f 5)
  #x=$(echo $line} | cut -d , -f 6)
  #y=$(echo $line} | cut -d , -f 7)
  #z=$(echo $line} | cut -d , -f 8)
  #channelIDMC=$(echo $line} | cut -d , -f 9)
  #CableType=$(echo $line} | cut -d , -f 10)
  #CableLength=$(echo $line} | cut -d , -f 11)
  #Crate=$(echo $line} | cut -d , -f 12)
  #Slot=$(echo $line} | cut -d , -f 13)
  #Position=$(echo $line} | cut -d , -f 14)
  #CITIROCCh32=$(echo $line} | cut -d , -f 15)
  #CITIROCCh256=$(echo $line} | cut -d , -f 16)
  #channelIDELEC=$(echo $line} | cut -d , -f 17)
  echo "$PCBPosition $x $y $z $Crate $Slot $Position $CITIROCCh32 $CITIROCCh256" >> $FILETXT
done < $FILECSV
