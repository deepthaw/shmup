<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.11.0" name="PLAYER" tilewidth="30" tileheight="17" tilecount="5" columns="0" fillmode="preserve-aspect-fit">
 <grid orientation="orthogonal" width="1" height="1"/>
 <tile id="11" type="VICVIPER">
  <properties>
   <property name="state" propertytype="frames" value="SPFULLDN"/>
  </properties>
  <image source="images/sprite000.png" width="30" height="17"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="0" y="0" width="30" height="16"/>
  </objectgroup>
 </tile>
 <tile id="12" type="VICVIPER">
  <properties>
   <property name="state" propertytype="frames" value="SPTILTDN"/>
  </properties>
  <image source="images/sprite001.png" width="30" height="13"/>
 </tile>
 <tile id="13" type="VICVIPER">
  <properties>
   <property name="state" propertytype="frames" value="SPNEUTRAL"/>
  </properties>
  <image source="images/sprite002.png" width="30" height="11"/>
 </tile>
 <tile id="14" type="VICVIPER">
  <properties>
   <property name="state" propertytype="frames" value="SPTILTUP"/>
  </properties>
  <image source="images/sprite003.png" width="30" height="9"/>
 </tile>
 <tile id="15" type="VICVIPER">
  <properties>
   <property name="state" propertytype="frames" value="SPFULLUP"/>
  </properties>
  <image source="images/sprite004.png" width="30" height="16"/>
 </tile>
</tileset>
