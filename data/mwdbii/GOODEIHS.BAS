' Goode Homolosine - Interrupted for Continents
' Central longitude of map (Lambda0) = 0 Degrees
'
' ===================================
'
'  Program Written by:
'    Paul B. Anderson (804) 853-7595
'    3214 Chalfin Ave.
'    Norfolk, Va. 23513
'
' ===================================
'
' Main Map Formula From "AN ALBUM OF MAP PROJECTIONS"
'                        by John P. Snyder & Philip M. Voxland, 1987.
'                        (U.S. Geological Survey Professional Paper 1453)
'
' Function MollweideFormula# (Used in Mollweide Iteration) is from:
'                       "MAP PROJECTIONS -- A WORKING MANUAL"
'                        by John P. Snyder, 1987.
'                        (U.S. Geological Survey Professional Paper 1395)
'
' ==============
'  Declarations
' ==============
'
DECLARE SUB Alert ()
DECLARE SUB CRTColorMenu ()
DECLARE SUB DataPointsMenu ()
DECLARE SUB DrawGrid ()
DECLARE SUB DrawOutline ()
DECLARE SUB DrawProjection (PolyLine%)
DECLARE SUB FeaturesMenu ()
DECLARE SUB GetMapData (FileName$)
DECLARE SUB Goode (DeltaLambda#, Radius#, XCoord#, YCoord#)
DECLARE SUB LatLongEntry (Title$, Note$)
DECLARE SUB LatLongMenu ()
DECLARE SUB LongitudeFOR (Begin#, Finish#, Incr#)
DECLARE SUB MakeLatitudeLines (LatRange%, LatDetail%)
DECLARE SUB MakeLongitudeLines (LatRange%, LongDetail!)
DECLARE SUB PlotFileColorMenu ()
DECLARE SUB ReadInfile (Byte%)
DECLARE FUNCTION ArcCos# (n#)
DECLARE FUNCTION ArcSin# (n#)
DECLARE FUNCTION ArcTanH# (n#)
DECLARE FUNCTION Atan2# (b#, a#)
DECLARE FUNCTION Center% (Length%)
DECLARE FUNCTION ConvertCoordToDecDeg# (coord#)
DECLARE FUNCTION CoTan# (n#)
DECLARE FUNCTION LN# (n#)
DECLARE FUNCTION MollweideFormula# (ThetaPrime#, MPhi#)
DECLARE FUNCTION Normalize# (LambdaVal#, Lambda0Val#)
DECLARE FUNCTION PointCount& (PtCnt&)
DECLARE FUNCTION PolyLineColor% (PolyLineHeader%)
DECLARE FUNCTION Raise# (n#, Power#)
DECLARE FUNCTION Round# (n#, PowerOfTen%)
DECLARE FUNCTION Sec# (n#)
DECLARE FUNCTION Sign# (n#)
'
' ===========
'  Constants
' ===========
'
CONST FALSE = 0
CONST TRUE = NOT FALSE
CONST SQRT2 = 1.414213562373095#             ' SQR(2)
CONST DEG180 = 3.1415926535898#              ' PI
CONST DEG90 = 1.5707963267949#               ' PI / 2
CONST DEG360 = 6.2831853071796#              ' PI * 2
CONST DEG45 = .7853981633974501#             ' PI / 4
CONST RAD2DEG = 57.29577951308219#           ' 180 / PI
CONST DEG2RAD = 1.745329251994333D-02        ' PI / 180
CONST MIN2RAD = 2.908882086657222D-04        ' DEG2RAD / 60
CONST MERGEPOINT = .710988432654746#         ' 40.73666 * DEG2RAD
CONST TOLERANCE = .0000001# ' Tolerance value for Newton-Raphson routine
CONST DTPINCR = .00001# ' Increment value for Delta Theta Prime
'                          (inside Newton-Raphson routine)
'
' additional Degree Values used in this projection
' Put Here to avoid repetitive calculation
'
CONST DEG10 = 10 * DEG2RAD
CONST DEG20 = 20 * DEG2RAD
CONST DEG30 = 30 * DEG2RAD
CONST DEG40 = 40 * DEG2RAD
CONST DEG50 = 50 * DEG2RAD
CONST DEG60 = 60 * DEG2RAD
CONST DEG80 = 80 * DEG2RAD
CONST DEG100 = 100 * DEG2RAD
CONST DEG140 = 140 * DEG2RAD
CONST DEG160 = 160 * DEG2RAD

CONST XCONST = .9003163161571041#            ' (8# ^ .5#) / Deg180#
CONST MOLLRFACTOR = .0528#                   ' Reduction factor applied to
'                                              Mollweide to match parallels
'                                              of sinusoidal portion of
'                                              the projection
'
CONST XCENTER = 320           ' VGA Center X
CONST YCENTER = 240           ' VGA Center Y
CONST ASPECT = 1              ' VGA mode 12 -- 640x480 aspect ratio = 1
CONST CRTRADIUS = 100         ' Radius Value for CRT display
CONST PLOTRADIUS = 1          ' Radius Value for Plot File
CONST PLTXCENTER = 5588       ' (25.4 * 5.5#) / .025
CONST PLTYCENTER = 4318       ' (25.4 * 4.25#) / .025
CONST LAMBDA0 = 0             ' Central Longitude of map for Goode
'                                 Homolosine Interrupted for Continental
'                                 Lobes.
'
' ============================
' TYPE Definition For PNT file
' ============================
'
TYPE PNTRecord
  Header AS INTEGER
  Lat AS INTEGER
  Lon AS INTEGER
END TYPE
'
DIM SHARED PNTData AS PNTRecord
'
' =========================================
' Type Definition for Global Variables (G.)
' =========================================
'
TYPE GlobalVariables
  CrtX AS DOUBLE             ' Intermediate value for X Coordinate
  CrtY AS DOUBLE             ' Intermediate value for Y Coordinate
  ModCrtX AS DOUBLE          ' Modified to be centered on CRT
  ModCrtY AS DOUBLE          ' Modified to be centered on CRT
  LastModCrtX AS DOUBLE
  LastModCrtY AS DOUBLE
  LastCrtY AS DOUBLE
  Lambda AS DOUBLE           ' Longitude in Radians
  Lambda2 AS DOUBLE          ' Central Longitude of Lobe
  Phi AS DOUBLE              ' Latitude in Radians
  Visible AS INTEGER         ' When True --> Draw PolyLine%
  Grid AS INTEGER            ' When True --> Draw Grid
  Outline AS INTEGER         ' When True --> Skip Lobe IF-Then statements
  MapDataLevel AS INTEGER    ' Levels 1 to 5
  ExtensionLobe1 AS INTEGER  ' When True --> Draw extension 1
  ExtensionsLobe2 AS INTEGER ' When True --> Draw extensions 2 & 3
  ColorVal AS INTEGER        ' CRT Color Out
  GridColor AS INTEGER       ' CRT Color for Grid
  CoastColor AS INTEGER      ' CRT Color for Coasts
  BorderColor AS INTEGER     ' CRT Color for Political Borders
  IslandColor AS INTEGER     ' CRT Color for Islands
  StateColor AS INTEGER      ' CRT Color U.S. State borders
  LakeColor AS INTEGER       ' CRT Color for Lakes
  RiverColor AS INTEGER      ' CRT Color for Rivers
  ProvinceColor AS INTEGER   ' CRT Color for Canadian Provinces
  AustColor AS INTEGER       ' CRT Color for Australian States
  MexicoColor AS INTEGER     ' CRT Color for Mexican States
  GridPen AS INTEGER         ' Grid color for Plot File
  CoastPen AS INTEGER        ' Coast color for Plot File
  BorderPen AS INTEGER       ' Border color for Plot File
  IslandPen AS INTEGER       ' Island color for Plot File
  StatePen AS INTEGER        ' State color for Plot File
  LakePen AS INTEGER         ' Lake color for Plot File
  RiverPen AS INTEGER        ' River color for Plot File
  ProvincePen AS INTEGER     ' Canadian Province color for Plot File
  AustPen AS INTEGER         ' Australian State color for Plot File
  MexicoPen AS INTEGER       ' Mexican State color for Plot File
  LastPlotPen AS STRING * 1  '
  PlotFile AS INTEGER        ' When True Plot to a File
  CoastSW AS INTEGER         ' When True Plot to Crt or file
  BorderSW AS INTEGER        ' When True Plot to Crt or file
  IslandSW AS INTEGER        ' When True Plot to Crt or file
  StateSW AS INTEGER         ' When True Plot to Crt or file
  LakeSW AS INTEGER          ' When True Plot to Crt or file
  RiverSW AS INTEGER         ' When True Plot to Crt or file
  ProvinceSW AS INTEGER      ' When True Plot to Crt or file
  AustSW AS INTEGER          ' When True Plot to Crt or file
  MexicoSW AS INTEGER        ' When True Plot to Crt or file
  LongStep AS INTEGER        ' Grid Longitude Increment
  LatStep AS DOUBLE          ' Grid Latitude Increment
  LongOption AS INTEGER      ' 1 - Longitude to pole, 2 - ends at +/-85 deg.
END TYPE

DIM SHARED G AS GlobalVariables
'
' > variables used by GetPKDData example routines
'
' for the *.PKD file format
' DIM SHARED Index AS DOUBLE
' DIM SHARED PointsInLine AS LONG
' DIM SHARED LonI AS DOUBLE
' DIM SHARED LatI AS DOUBLE
' DIM SHARED LonR AS DOUBLE
' DIM SHARED LatR AS DOUBLE
' DIM SHARED FeatureType AS INTEGER
' DIM SHARED LoopCount AS LONG
' DIM SHARED Infile$(23)
 
'
' =========================
'  Program starting values
' =========================
'
' > Flags to tell the program when to draw the map inside the extensions
'
G.ExtensionLobe1 = FALSE
G.ExtensionsLobe2 = FALSE
'
' > Starting CRT colors for Geographical features
'
G.GridColor = 0       ' Black
G.CoastColor = 0      ' Black
G.BorderColor = 4     ' Red
G.IslandColor = 0     ' Black
G.StateColor = 4      ' Red
G.LakeColor = 1       ' Blue
G.RiverColor = 1      ' Blue
'
' > Note: Color values for the following actually use U.S. State color
'         they are not hooked into the CRT color menu
'
G.ProvinceColor = 4   ' Red
G.AustColor = 4       ' Red
G.MexicoColor = 4     ' Red
'
' > Starting Plot File colors for Geographical features (Used by Corel Draw)
'
G.GridPen = 1         ' Black
G.CoastPen = 1        ' Black
G.BorderPen = 3       ' Red
G.IslandPen = 1       ' Black
G.StatePen = 3        ' Red
G.LakePen = 2         ' Blue
G.RiverPen = 2        ' Blue
'
' > Note: Color values for the following actually use U.S. State color
'         they are not hooked into the Pen color menu
'
G.ProvincePen = 3     ' Red
G.AustPen = 3         ' Red
G.MexicoPen = 3       ' Red
'
' > Starting Geographical Features (All ON)
'
G.CoastSW = TRUE
G.BorderSW = TRUE
G.IslandSW = TRUE
G.StateSW = TRUE
G.LakeSW = TRUE
G.RiverSW = TRUE
G.ProvinceSW = TRUE
G.AustSW = TRUE
G.MexicoSW = TRUE
'
' > Starting Latitude and longitude increment -- Traditional View
'
G.LongStep = 10#
G.LatStep = 10#
G.LongOption = 2
'
' > Starting Database level
'
G.MapDataLevel = 3
'
' > Plotting Variables, Plot Size = A
'
G.PlotFile = TRUE
Plotpass% = 0 'WARNING:  Will not write over existing file while in program.
'
'                       --------------- HOWEVER -------------------
'                       IF you EXIT the program and then restart it
'                       It WILL write over existing Plot files!!!!!
'                       -------------------------------------------
'
UserQuits% = FALSE  ' Don't Quit yet

VIEW PRINT 1 TO 25
'
' { Main Program }
'
DO
  MenuItem% = 0 ' No Menu Items Selected
  COLOR 7, 9
  CLS
'
  LOCATE 2, 29: PRINT "======================="
  LOCATE 3, 30: PRINT "W O R L D   V I E W S"
  LOCATE 4, 29: PRINT "======================="
  LOCATE 6, 24: PRINT "Map Projection Library, Volume 1"
  LOCATE 8, 24: PRINT " 1. Draw"
  LOCATE 16, 30: PRINT "World Views' Options"
  LOCATE 18, 12: PRINT " 2. Change CRT Color of Geographical Features"
  LOCATE 19, 12: PRINT " 3. Turn Geographical Features On/Off "
  LOCATE 20, 12: PRINT " 4. Modify Latitude/Longitude Display"
  LOCATE 21, 12: PRINT " 5. Increase/Decrease Amount of Points to Plot"
  LOCATE 22, 12: PRINT " 6. Change Plot File Pen Colors of Geographical Features"
  LOCATE 24, 21
  INPUT "Select Menu Item (1-6) or 0 to Quit: ", MenuItem%
'
  SELECT CASE MenuItem%
'
    CASE 0
      UserQuits% = TRUE

    CASE 1
    ' Draw the Map
      Title$ = " Goode Homolosine Projection "
      Note$ = "Central Longitude of Map is 0 Degrees"
      CALL LatLongEntry(Title$, Note$)
'
      IF G.PlotFile THEN
        Plotpass% = Plotpass% + 1
        PlotFile$ = "C:\WV\WVPLOT" + LTRIM$(STR$(Plotpass%)) + ".PLT"
        OPEN PlotFile$ FOR OUTPUT AS #1
        PRINT #1, "IN; IP0,0,11176,8636; SP0;"
        PRINT #1, "SC-5498,5498,-4249,4249;"
        PRINT #1, "VS4; PT 0.1; SP1;"
        G.LastPlotPen = "1"
      END IF

      SCREEN 12 ' Switch to VGA 480/640, 16 COLOR mode
      VIEW (0, 0)-(639, 479), 7
'
      COLOR 10
      Col% = Center%(LEN(Title$))
      LOCATE 1, Col%: PRINT Title$;

      CALL DrawOutline
 
'     CALL GetPKDData
 
      IF G.Grid THEN CALL DrawGrid
      IF G.CoastSW THEN CALL GetMapData("PCoast.pnt")
      IF G.IslandSW THEN CALL GetMapData("PIsland.pnt")
      IF G.LakeSW THEN CALL GetMapData("Plake.pnt")
      IF G.RiverSW THEN CALL GetMapData("River.pnt")
      IF G.BorderSW THEN CALL GetMapData("PBorder.pnt")
      IF G.StateSW THEN CALL GetMapData("PUSA48.pnt")
      IF G.ProvinceSW THEN CALL GetMapData("PCanProv.pnt")
      IF G.AustSW THEN CALL GetMapData("PAust.pnt")
      IF G.MexicoSW THEN CALL GetMapData("PMexico.pnt")

      G.ExtensionLobe1 = TRUE
      IF G.CoastSW THEN CALL GetMapData("PCoast.pnt")
      IF G.IslandSW THEN CALL GetMapData("PIsland.pnt")

      G.ExtensionLobe1 = FALSE
      G.ExtensionsLobe2 = TRUE
 
      IF G.CoastSW THEN CALL GetMapData("PCoast.pnt")
      IF G.IslandSW THEN CALL GetMapData("PIsland.pnt")
 
      G.ExtensionsLobe2 = FALSE
 
      IF G.PlotFile THEN
        PRINT #1, "PU 0,0; SP00;"
        CLOSE #1
      END IF

      CALL Alert ' BEEP
 
      COLOR 12
      LOCATE 1, Col%: PRINT Title$;
    
      DO
        Brk$ = INKEY$
      LOOP UNTIL Brk$ > ""
   
      SCREEN 0  'Switch back to Text mode
      VIEW PRINT 1 TO 25
   
    CASE 2
    ' Change Line Color of Geographical features
      CALL CRTColorMenu
        
    CASE 3
    ' Turn Geographical features OFF or ON
      CALL FeaturesMenu
'
    CASE 4
    ' Change Latitude and Longitude Values
      CALL LatLongMenu
'
    CASE 5
    ' Change the Amount of Database Latitude and Longitude Values to plot
      CALL DataPointsMenu
'
    CASE 6
    ' Change the Line color of the Geographical features sent to the plot file
      CALL PlotFileColorMenu
'
    CASE ELSE
      PRINT CHR$(7);

  END SELECT '{ MenuItem. }
'
LOOP UNTIL UserQuits%
'
END

SUB Alert
' { Sounds a tone when map is complete. }

  SOUND 880, 36.4

END SUB ' { Alert. }

FUNCTION ArcCos# (n#) STATIC
'
IF n# <> 0 THEN
  ArcCos# = ATN(Raise#(1 - (n# * n#), .5) / n#) + DEG180 * (n# - ABS(n#)) / (2 * n#)
ELSE
  n# = 0
END IF
'
END FUNCTION '{ ArcCos#. }

FUNCTION ArcSin# (n#) STATIC
'
  IF ABS(n#) < 1 THEN
    ArcSin# = ATN(n# / Raise#(1 - (n# * n#), .5))
    EXIT FUNCTION
  END IF

  IF n# = 1 THEN
    ArcSin# = DEG90
    EXIT FUNCTION
  END IF

  IF n# = -1 THEN
    ArcSin# = -DEG90
  END IF
'
END FUNCTION ' { ArcSin#. }

FUNCTION ArcTanH# (n#) STATIC
'
    AT1# = ABS(n#)
'
    IF AT1# < 1 THEN
      AT2# = .5 * LN#((1 + AT1#) / (1 - AT1#))
      ArcTanH# = AT2# * Sign#(n#)
    END IF
'
END FUNCTION ' { ArcTanH#.}

FUNCTION Atan2# (b#, a#)
'
IF a# = 0 THEN
  IF b# > 0 THEN
    Atan2# = DEG90
  ELSEIF b# < 0 THEN
    Atan2# = -DEG90
  ELSE
    Atan2# = 0
  END IF
'
ELSEIF b# = 0 THEN
  IF a# < 0 THEN
    Atan2# = DEG180
  ELSE
    Atan2# = 0
  END IF
'
ELSE
  IF a# < 0 THEN
    IF b# > 0 THEN
      Atan2# = ATN(b# / a#) + DEG180
    ELSE
      Atan2# = ATN(b# / a#) - DEG180
    END IF
'
  ELSE
    Atan2# = ATN(b# / a#)
  END IF
END IF
'
END FUNCTION '{ ATan2#. }

FUNCTION Center% (Length%)
'
    IF Length% MOD 2 = 0 THEN
      Column% = 40 - (Length% \ 2)
    ELSE
      Column% = (40 - (Length% \ 2)) + 1
    END IF
'
    Center% = Column%
'
END FUNCTION '{ Center$. }

FUNCTION ConvertCoordToDecDeg# (coord#)
' Used by GetPKDData subroutine
'
  ConvertCoordToDecDeg# = coord# / 3600
'
END FUNCTION '{ ConvertCoordToDecDeg#. }

FUNCTION CoTan# (n#)
'
  Sine# = SIN(n#)
  IF ABS(Sine#) <= .0001 THEN
    PRINT "Error: CoTan#(n#) Where n# <= 0"
    SYSTEM
  ELSE
    CoTan# = COS(n#) / Sine#
  END IF
'
END FUNCTION '{ CoTan#. }

SUB CRTColorMenu
'
ExitMenu% = FALSE
'
DO
  CLS
  LOCATE 2, 18: PRINT "** Change CRT Color of Geographical Features **"
  LOCATE 4, 36: PRINT "Color Codes"
  LOCATE 5, 22: PRINT "0 - Black"
  LOCATE 6, 22: PRINT "1 - Blue    6 - Brown      11 - Lt. Cyan "
  LOCATE 7, 22: PRINT "2 - Green   7 - White      12 - Lt. Red  "
  LOCATE 8, 22: PRINT "3 - Cyan    8 - Dk. Grey   13 - Lt. Magenta"
  LOCATE 9, 22: PRINT "4 - Red     9 - Lt. Blue   14 - Yellow"
  LOCATE 10, 22: PRINT "5 - Magenta 10 - Lt. Green 15 - Br. White"
  LOCATE 12, 28: PRINT "1. Grid Color is:"; G.GridColor
  LOCATE 13, 28: PRINT "2. Coast Color is:"; G.CoastColor
  LOCATE 14, 28: PRINT "3. Border Color is:"; G.BorderColor
  LOCATE 15, 28: PRINT "4. Island Color is:"; G.IslandColor
  LOCATE 16, 28: PRINT "5. State Border Color is:"; G.StateColor
  LOCATE 17, 28: PRINT "6. Lake Color is:"; G.LakeColor
  LOCATE 18, 28: PRINT "7. River Color is:"; G.RiverColor
  LOCATE 20, 14
  INPUT "Select Option (1-7) or 0 to Return to Main Menu: ", Menu%
     
  SELECT CASE Menu%
'
    CASE 0
      ExitMenu% = TRUE
'
    CASE 1
      LOCATE 20, 1: PRINT SPACE$(80)
      LOCATE 20, 14
      INPUT "Change Grid Color to (0-15):", NewColor%
'
      IF NewColor% >= 0 AND NewColor% < 16 THEN
        G.GridColor = NewColor%
      ELSE
        PRINT CHR$(7)
      END IF
'
    CASE 2
      LOCATE 20, 1: PRINT SPACE$(80)
      LOCATE 20, 14
      INPUT "Change Coastline Color to (0-15):", NewColor%
'
      IF NewColor% >= 0 AND NewColor% < 16 THEN
        G.CoastColor = NewColor%
      ELSE
        PRINT CHR$(7)
      END IF
'
    CASE 3
      LOCATE 20, 1: PRINT SPACE$(80)
      LOCATE 20, 14
      INPUT "Change Border Color to (0-15):", NewColor%
'
      IF NewColor% >= 0 AND NewColor% < 16 THEN
        G.BorderColor = NewColor%
      ELSE
        PRINT CHR$(7)
      END IF
'
    CASE 4
      LOCATE 20, 1: PRINT SPACE$(80)
      LOCATE 20, 14
      INPUT "Change Island Color to (0-15):", NewColor%
'
      IF NewColor% >= 0 AND NewColor% < 16 THEN
        G.IslandColor = NewColor%
      ELSE
        PRINT CHR$(7)
      END IF
'
    CASE 5
      LOCATE 20, 1: PRINT SPACE$(80)
      LOCATE 20, 14
      INPUT "Change State Border Color to (0-15):", NewColor%
'
      IF NewColor% >= 0 AND NewColor% < 16 THEN
        G.StateColor = NewColor%
      ELSE
        PRINT CHR$(7)
      END IF
'
    CASE 6
      LOCATE 20, 1: PRINT SPACE$(80)
      LOCATE 20, 14
      INPUT "Change Lake Color to (0-15):", NewColor%
'
      IF NewColor% >= 0 AND NewColor% < 16 THEN
        G.LakeColor = NewColor%
      ELSE
        PRINT CHR$(7)
      END IF
'
    CASE 7
      LOCATE 20, 1: PRINT SPACE$(80)
      LOCATE 20, 14
      INPUT "Change River Color to (0-15):", NewColor%
'
      IF NewColor% >= 0 AND NewColor% < 16 THEN
        G.RiverColor = NewColor%
      ELSE
        PRINT CHR$(7)
      END IF
'
    CASE ELSE
      PRINT CHR$(7)
'
    END SELECT '{ Menu. }
'
  LOOP UNTIL ExitMenu%
'
END SUB ' { CRTColor. }

SUB DataPointsMenu
'
ExitMenu% = FALSE
'
DO
  CLS
'
  LOCATE 2, 16: PRINT "** Increase/Decrease Amount of Points to Plot **"
  LOCATE 4, 27: PRINT "Current Database Level is"; G.MapDataLevel
  LOCATE 6, 25: PRINT "1. 179,331 X-Y Coordinate Pairs"
  LOCATE 7, 25: PRINT "2. 109,992 X-Y Coordinate Pairs"
  LOCATE 8, 25: PRINT "3. 27,393 X-Y Coordinate Pairs"
  LOCATE 9, 25: PRINT "4. 14,867 X-Y Coordinate Pairs"
  LOCATE 10, 25: PRINT "5. 5,365 X-Y Coordinate Pairs"
  LOCATE 12, 18
  PRINT "Select Option (1-5) to Change Database Level"
  LOCATE 13, 18
  INPUT "    or 0 to Return to Previous Menu: ", Menu%
' 
  SELECT CASE Menu%
'   
    CASE 0
      ExitMenu% = TRUE
'
    CASE 1 TO 5
      G.MapDataLevel = Menu%
'   
    CASE ELSE
      PRINT CHR$(7)
'
  END SELECT '{ Menu. }
'
  LOOP UNTIL ExitMenu%
'
END SUB '{ DataPointsMenu. }

SUB DrawGrid
'
  G.LastModCrtX = 0
  G.LastModCrtY = 0
  G.ColorVal = G.GridColor

  LatRange% = 90     '
  LatDetail% = 5     ' Since Latitude Lines are straight no need to change
  LongDetail! = .25  ' For Faster Screen Draws this variable can be increased
  '                    For "Publication" Quality vector Graphics try .1#
  '                    (slows screen draws and large increase
  '                    in plot file size)
'
'
' > If G.LongOption is 1 - All Longitude lines converge at pole
'   If G.LongOption is 2 - Only draw longitude lines to within 5 Degrees
'                          of the pole (Default value)
'
  SELECT CASE G.LongOption

    CASE IS = 1
      LatStop% = 90
'
    CASE IS = 2
      LatStop% = 85
' 
  END SELECT '{ G.LongOption. }
'
  CALL MakeLongitudeLines(LatStop%, LongDetail!)
  CALL MakeLatitudeLines(LatRange%, LatDetail%)
'
END SUB '{ DrawGrid. }

SUB DrawOutline STATIC
'
  G.Outline = TRUE
  G.ColorVal = G.GridColor
  G.LastModCrtX = 0
 
  PolyLine% = TRUE
  Increment# = DEG2RAD / 4
   
  G.Lambda = -DEG180
  G.Lambda2 = -DEG160
  CALL LongitudeFOR(-DEG90, 0, Increment#)
  G.Lambda2 = -DEG100
  CALL LongitudeFOR(0, DEG90, Increment#)
'
' ==================
' Insert 1 Longitude
' ==================
'
  G.Lambda = -DEG10
  G.Lambda2 = -DEG100
  CALL LongitudeFOR(DEG90, DEG60, -Increment#)
'
' =================================
' Insert 1 Latitude (Bottom Border)
' =================================
'
  G.Phi = DEG60
  G.Lambda2 = -DEG100
  G.Lambda = -DEG10
'
  DO
    CALL DrawProjection(PolyLine%)
    G.Lambda = G.Lambda - Increment#
    IF G.Lambda < -DEG40 THEN EXIT DO
  LOOP
'
' =================================
'
' =================================
'
  G.Lambda = -DEG40
  G.Lambda2 = -DEG100
  CALL LongitudeFOR(DEG60, 0, -Increment#)
  G.Lambda2 = DEG30
  CALL LongitudeFOR(0, DEG60, Increment#)
'
' =================================
' Insert 2 Latitude (Bottom Border)
' =================================
'
  G.Phi = DEG60
  G.Lambda2 = DEG30
  G.Lambda = -DEG40

  DO
    CALL DrawProjection(PolyLine%)
    G.Lambda = G.Lambda - Increment#
    IF G.Lambda < -DEG50 THEN EXIT DO
  LOOP
'
' ==================
' Insert 2 Longitude
' ==================
'
  G.Lambda = -DEG50
  G.Lambda2 = DEG30
  CALL LongitudeFOR(DEG60, DEG90, Increment#)
'
' ==================
' Insert 3 Longitude
' ==================
'
  G.Lambda = -DEG160
  G.Lambda2 = DEG30
  CALL LongitudeFOR(DEG90, DEG50, -Increment#)
'
' =================================
' Insert 3 Latitude (Bottom Border)
' =================================
'
  G.Phi = DEG50
  G.Lambda2 = DEG30
  G.Lambda = -DEG160

  DO
    CALL DrawProjection(PolyLine%)
    G.Lambda = G.Lambda - Increment#
    IF G.Lambda < -DEG180 THEN EXIT DO
  LOOP

' ==================
'
' ==================
'
  G.Lambda = DEG180
  G.Lambda2 = DEG30
  CALL LongitudeFOR(DEG50, 0, -Increment#)
  G.Lambda2 = DEG140
  CALL LongitudeFOR(0, -DEG90, -Increment#)

  G.Lambda = DEG80
  G.Lambda2 = DEG140
  CALL LongitudeFOR(-DEG90, 0, Increment#)
  G.Lambda2 = DEG20
  CALL LongitudeFOR(0, -DEG90, -Increment#)

  G.Lambda = -DEG20
  G.Lambda2 = DEG20
  CALL LongitudeFOR(-DEG90, 0, Increment#)
  G.Lambda2 = -DEG60
  CALL LongitudeFOR(0, -DEG90, -Increment#)
 
  G.Lambda = -DEG100
  G.Lambda2 = -DEG60
  CALL LongitudeFOR(-DEG90, 0, Increment#)
  G.Lambda2 = -DEG160
  CALL LongitudeFOR(0, -DEG90, -Increment#)
   
  PolyLine% = FALSE
  CALL DrawProjection(PolyLine%)
 
  G.Outline = FALSE '*
'
END SUB '{ DrawOutline. }

SUB DrawProjection (PolyLine%) STATIC
'
IF G.ExtensionLobe1 THEN
  G.Outline = FALSE
  LongDeg# = CSNG(G.Lambda * RAD2DEG)
 
  IF LongDeg# >= -40 AND LongDeg# <= -10 THEN
    IF CSNG(G.Phi * RAD2DEG) >= 60 THEN
      G.Lambda2 = -DEG100
      G.Outline = TRUE
    END IF
  END IF

  IF LongDeg# >= -180 AND LongDeg# <= -160 THEN
    IF CSNG(G.Phi * RAD2DEG) >= 50 THEN
      G.Lambda2 = DEG30
      G.Outline = TRUE
    END IF
  END IF
  IF G.Outline = FALSE THEN EXIT SUB
END IF

IF G.ExtensionsLobe2 THEN
  G.Outline = FALSE
  LongDeg# = CSNG(G.Lambda * RAD2DEG)
  IF LongDeg# >= -50 AND LongDeg# <= -40 THEN
    IF CSNG(G.Phi * RAD2DEG) >= 60 THEN
      G.Lambda2 = DEG30
      G.Outline = TRUE
    END IF
  END IF
  IF G.Outline = FALSE THEN EXIT SUB
END IF
       
CALL Goode(G.Lambda, CRTRADIUS, G.CrtX, G.CrtY)
       
IF G.PlotFile THEN
  CALL Goode(G.Lambda, PLOTRADIUS, PlotX#, PlotY#)
  PlotX# = ((25.4# * PlotX#) / .025#) '+ PltXCenter#
  PlotY# = ((25.4# * PlotY#) / .025#) '+ PltYCenter#
END IF
'
G.ModCrtX = FIX((G.CrtX * ASPECT) + XCENTER)
G.ModCrtY = CINT(YCENTER) - FIX(G.CrtY)
'
IF PolyLine% = FALSE THEN G.Visible = FALSE
'
' > This statement eliminates the stray lines going from top to bottom
' > for the Transverse Mercator and Cassini Projections
'
IF ABS(G.ModCrtY - G.LastModCrtY) > 120 THEN G.Visible = FALSE
'
' > This statement eliminates the stray lines for the Azimuthal Projections
'
IF ABS(G.LastModCrtX - G.ModCrtX) > 120 THEN G.Visible = FALSE
       
IF G.LastModCrtX < -2000 OR G.ModCrtX < -2000 THEN G.Visible = FALSE
        
IF G.Visible THEN
  Y2# = (G.ModCrtY - G.LastModCrtY) * (G.ModCrtY - G.LastModCrtY)
  X2# = (G.ModCrtX - G.LastModCrtX) * (G.ModCrtX - G.LastModCrtX)
  Distance# = Raise#(X2# + Y2#, .5)
  IF ABS(Distance#) > 20 THEN
    G.Visible = FALSE
  END IF
END IF
        
IF G.PlotFile THEN
  IF G.Visible THEN
    PRINT #1, "PD"; STR$(CINT(PlotX#)); ","; STR$(CINT(PlotY#)); ";"
  ELSE
    PRINT #1, "PU"; STR$(CINT(PlotX#)); ","; STR$(CINT(PlotY#)); ";"
  END IF
END IF
       
IF G.Visible THEN
  LINE (G.LastModCrtX, G.LastModCrtY)-(G.ModCrtX, G.ModCrtY), G.ColorVal
END IF

G.Visible = TRUE
G.LastModCrtX = G.ModCrtX
G.LastModCrtY = G.ModCrtY
'
END SUB ' { DrawProjection. }

SUB FeaturesMenu
'
ExitMenu% = FALSE
'
DO
  CLS
'
  LOCATE 2, 19
  PRINT "** Turn Geographical Features On or Off **"
'   
  LOCATE 5, 22: PRINT "1. Coastlines are";
  IF G.CoastSW THEN
    PRINT " On "
  ELSE
    PRINT " Off"
  END IF
'   
  LOCATE 6, 22: PRINT "2. Islands are";
  IF G.IslandSW THEN
    PRINT " On "
  ELSE
    PRINT " Off"
  END IF
'   
  LOCATE 7, 22: PRINT "3. Lakes are";
  IF G.LakeSW THEN
    PRINT " On "
  ELSE
    PRINT " Off"
  END IF
'   
  LOCATE 8, 22: PRINT "4. Rivers are";
  IF G.RiverSW THEN
    PRINT " On "
  ELSE
    PRINT " Off"
  END IF
'   
  LOCATE 9, 22: PRINT "5. Country Borders are";
  IF G.BorderSW THEN
    PRINT " On "
  ELSE
    PRINT " Off"
  END IF
'  
  LOCATE 10, 22: PRINT "6. U.S. State Borders are";
  IF G.StateSW THEN
    PRINT " On "
  ELSE
    PRINT " Off"
  END IF
'   
  LOCATE 11, 22: PRINT "7. Canadian Province Borders are";
  IF G.ProvinceSW THEN
    PRINT " On "
  ELSE
    PRINT " Off"
  END IF
' 
  LOCATE 12, 22: PRINT "8. Austrailian State Borders are";
  IF G.AustSW THEN
    PRINT " On "
  ELSE
    PRINT " Off"
  END IF
'   
  LOCATE 13, 22: PRINT "9. Mexican State Borders are";
  IF G.AustSW THEN
    PRINT " On "
  ELSE
    PRINT " Off"
  END IF
'
  LOCATE 16, 23: PRINT "Select Option (1-9) to toggle Feature"
'
  LOCATE 17, 37: PRINT "- Or -"
'
  LOCATE 18, 24: INPUT "0 to Return to Previous Menu: ", Menu%
'   
  SELECT CASE Menu%
'     
    CASE 0
      ExitMenu% = TRUE
'
    CASE 1
      IF G.CoastSW THEN
        G.CoastSW = FALSE
      ELSE
        G.CoastSW = TRUE
      END IF
'
    CASE 2
      IF G.IslandSW THEN
        G.IslandSW = FALSE
      ELSE
        G.IslandSW = TRUE
      END IF
'
    CASE 3
      IF G.LakeSW THEN
        G.LakeSW = FALSE
      ELSE
        G.LakeSW = TRUE
      END IF
'   
    CASE 4
      IF G.RiverSW THEN
        G.RiverSW = FALSE
      ELSE
        G.RiverSW = TRUE
      END IF
'   
    CASE 5
      IF G.BorderSW THEN
        G.BorderSW = FALSE
      ELSE
        G.BorderSW = TRUE
      END IF
'  
    CASE 6
      IF G.StateSW THEN
        G.StateSW = FALSE
      ELSE
        G.StateSW = TRUE
      END IF
'
    CASE 7
      IF G.ProvinceSW THEN
        G.ProvinceSW = FALSE
      ELSE
        G.ProvinceSW = TRUE
      END IF
'     
    CASE 8
      IF G.AustSW THEN
        G.AustSW = FALSE
      ELSE
        G.AustSW = TRUE
      END IF
'   
    CASE 9
      IF G.MexicoSW THEN
        G.MexicoSW = FALSE
      ELSE
        G.MexicoSW = TRUE
      END IF
'
    CASE ELSE
      PRINT CHR$(7)
'
  END SELECT '{ Menu. }
'
  LOOP UNTIL ExitMenu%
'
END SUB '{ FeaturesMenu. }

SUB GetFullCoords (LonI#, LatI#)
' Used by GetPKDData subroutine
'
STATIC Temp#
STATIC Temp2#
'
' Longitude Degree1
CALL ReadInfile(Byte%)
Temp# = Byte%
Temp# = Temp# * 7200!
'
' Longitude Degree2
CALL ReadInfile(Byte%)
Temp2# = Byte%
Temp# = Temp# + (Temp2# * 3600!)
'
' Longitude Minutes
CALL ReadInfile(Byte%)
Temp2# = Byte%
Temp# = Temp# + (Temp2# * 60!)
'
' Longitude Seconds
CALL ReadInfile(Byte%)
Temp2# = Byte%
Temp# = Temp# + Temp2#
LonI# = Temp#
'
' Latitude Degrees
Temp# = 0
Temp2# = 0
CALL ReadInfile(Byte%)
Temp# = Byte%
Temp# = Temp# * 3600!
'
' Latitude Minutes
CALL ReadInfile(Byte%)
Temp2# = Byte%
Temp# = Temp# + (Temp2# * 60!)
'
' Latitude Seconds
CALL ReadInfile(Byte%)
Temp2# = Byte%
Temp# = Temp# + Temp2#
LatI# = Temp#
'
END SUB '{ GetFullCoords. }

SUB GetMapData (FileName$) STATIC
'
G.Visible = FALSE
G.LastModCrtX = 0

OPEN "C:\WORK\" + FileName$ FOR RANDOM AS #2 LEN = LEN(PNTData)

TotalRecords& = LOF(2) / LEN(PNTData)

FOR RecordCounter& = 1 TO TotalRecords&

  GET #2, RecordCounter&, PNTData

  IF PNTData.Header >= G.MapDataLevel THEN
 
    IF PNTData.Header > 5 THEN
      G.ColorVal = PolyLineColor%(PNTData.Header)
      PolyLine% = FALSE
    ELSE
      PolyLine% = TRUE
    END IF

    G.Phi = PNTData.Lat * MIN2RAD
    G.Lambda = PNTData.Lon * MIN2RAD
   
    CALL DrawProjection(PolyLine%)
 
  END IF

NEXT RecordCounter&

CLOSE #2

END SUB '{ GetMapData. }

SUB GetMP1Data
' This module contains both the GetMP1Data and ParseWord Subroutines.
' They are not hooked into the program, but are provided in case the user
' wishes to install them and create his or her own coordinate database.
'
' An *.MP1 file is nothing more than an ASCII file that can be
' created with a Text editor containing  Latitude in Decimal Degrees
' then Longitude in Decimal Degrees separated by a comma or a space.
' Comments can also be included on each line as long as they are set
' off by an apostrophe (') and are at the end of the line.
'
' The first set of coordinates in the file are understood to be the beginning
' of the first PolyLine%. A new PolyLine% is indicated by a blank line at the
' beginning of the series of coordinates making up the new line.
'
' OPEN "C:EXAMPLE.PRN" FOR INPUT AS #1
'
'    G.Visible = False
'    G.LastModCRTX = 0
'    SEEK #1, 1
'
'   WHILE NOT EOF(1)
'
'     LINE INPUT #1, MP1Rec$
'
'     CALL ParseWord(MP1Rec$, TLat$)
'     LatR = VAL(TLat$) * Deg2Rad
'
'     CALL ParseWord(MP1Rec$, TLong$)
'     LongR = VAL(TLong$) * Deg2Rad
'
'     IF VAL(TLong$) = 0 AND VAL(TLat$) = 0 THEN
'       PolyLine% = False
'       LINE INPUT #1, MP1Rec$
'
'       CALL ParseWord(MP1Rec$, TLat$)
'       LatR = VAL(TLat$) * Deg2Rad
'
'       CALL ParseWord(MP1Rec$, TLong$)
'       LongR = VAL(TLong$) * Deg2Rad
'
'     ELSE
'       PolyLine% = True
'     END IF
'     CALL DrawProjection(PolyLine%)

'   WEND
'
'END SUB '{ GetMP1Data. }
'
' SUB ParseWord (PointLine$, Coord$) STATIC
' Subroutine to Parse text for *.MP1 Format
'
'   Sep$ = " ,"
'   Coord$ = ""
'   PointLine$ = RTRIM$(LTRIM$(PointLine$))
'   LenPointLine% = LEN(PointLine$)
'   IF PointLine$ = "" OR PointLine$ = "'" THEN
'     EXIT SUB
'   END IF
'   FOR Cnt1% = 1 TO LenPointLine%
'     IF INSTR(Sep$, MID$(PointLine$, Cnt1%, 1)) = 0 THEN
'       EXIT FOR
'     END IF
'   NEXT Cnt1%
'   FOR Cnt2% = Cnt1% TO LenPointLine%
'     IF INSTR(Sep$, MID$(PointLine$, Cnt2%, 1)) THEN ' = 0
'       EXIT FOR
'     END IF
'   NEXT Cnt2%
'   FOR Cnt3% = Cnt2% TO LenPointLine%
'     IF INSTR(Sep$, MID$(PointLine$, Cnt3%, 1)) = 0 THEN
'       EXIT FOR
'     END IF
'   NEXT Cnt3%
'   IF Cnt1% > LenPointLine% THEN
'     PointLine$ = ""
'     EXIT SUB
'   END IF
'   IF Cnt2% > LenPointLine% THEN
'     Coord$ = MID$(PointLine$, Cnt1%)
'     PointLine$ = ""
'     EXIT SUB
'   END IF
'     Coord$ = MID$(PointLine$, Cnt1%, Cnt2% - Cnt1%)
'     IF Cnt3% > LenPointLine% THEN
'       PointLine$ = ""
'     ELSE
'       PointLine$ = MID$(PointLine$, Cnt3%)
'     END IF
'
' END SUB ' { ParseWord. }
END SUB '{ GetMP1Data. }

SUB GetNextCoords (LonI AS DOUBLE, LatI AS DOUBLE)
' Used by GetPKDData subroutine
'
'Extract Longitude Delta
CALL ReadInfile(Byte%)
LonI = LonI + Byte%
'
'Extract Latitude Delta
CALL ReadInfile(Byte%)
LatI = LatI + Byte%
'
END SUB '{GetNextCoords}

SUB GetPKDData

' extracts data from *.PKD files
' This uses the South America Database Only

' G.Visible = False
' PreviousScreenX# = 0#

' Infile$(1) = "SAC1.PKD"
' Infile$(2) = "SAC2.PKD"
' Infile$(3) = "SAC3.PKD"
' Infile$(4) = "SAC4.PKD"
' Infile$(5) = "SAC7.PKD"
' Infile$(6) = "SAC8.PKD"
' Infile$(7) = "SAC9.PKD"
' Infile$(8) = "SAC13.PKD"
' Infile$(9) = "SAC14.PKD"
' Infile$(10) = "SAC10.PKD"
' Infile$(11) = "SAB01.PKD"
' Infile$(12) = "SAB2.PKD"
' Infile$(13) = "SAB3.PKD"
' Infile$(14) = "SAR1.PKD"
' Infile$(15) = "SAR2.PKD"
' Infile$(16) = "SAR3.PKD"
' Infile$(17) = "SAR4.PKD"
' Infile$(18) = "SAR5.PKD"
' Infile$(19) = "SAR06.PKD"
' Infile$(20) = "SAR7.PKD"
' Infile$(21) = "SAR8.PKD"
' Infile$(22) = "SAR10.PKD"

' FOR Infil% = 1 TO 22
' OPEN "C:\SA\" + Infile$(Infil%) FOR BINARY AS #1
'
' DO
' CALL ReadInfile(FeatureType)

' IF MID$(Infile$(Infil%), 3, 1) = "C" THEN G.ColorVal = 0
' IF MID$(Infile$(Infil%), 3, 1) = "B" THEN G.ColorVal = 4
' IF MID$(Infile$(Infil%), 3, 1) = "R" THEN G.ColorVal = 1
'
' PointsInLine& = PointCount&(PtCnt&)
'
' CALL GetFullCoords(LonI, LatI) ' - Max Place Holder (Max not used in program)
'
' CALL GetFullCoords(LonI, LatI) ' - Min Place Holder (Min not used in Program)
'
' PolyLine% = False

' CALL GetFullCoords(LonI, LatI) ' - Use First Coordinate

' LonR = ConvertCoordToDecDeg#(LonI)
' LatR = ConvertCoordToDecDeg#(LatI)

' G.Phi = LatR * Deg2Rad
' G.Lambda = LonR * Deg2Rad
' CALL DrawProjection(PolyLine%)
'
' PointsInLine& = PointsInLine& - 1
'
' PolyLine% = True
' FOR LoopCount = 1 TO PointsInLine&
'
' CALL GetNextCoords(LonI, LatI) '- Use next coordinates
' LonR = ConvertCoordToDecDeg#(LonI)
' LatR = ConvertCoordToDecDeg#(LatI)
 
' G.Phi = LatR * Deg2Rad
' G.Lambda = LonR * Deg2Rad
' CALL DrawProjection(PolyLine%)
'
' NEXT LoopCount
'
' LOOP UNTIL Index = LOF(1)
'
' Index = 0
' CLOSE #1
' NEXT Infil%
'
END SUB '{ GetPKDData. }

SUB Goode (DeltaLambda#, Radius#, XCoord#, YCoord#) STATIC
'
' When drawing the outline and the extensions there are a number
' of duplications that would be drawn over each other if the following
' groups of IF-THEN statements were used. It was easier to define the
' necessary Lobe Central Longitude (G.Lambda2) values in the Outline
' and DrawProjection (extensions) subroutines and use the Outline
' variable as a flag to skip over the following IF-THENs.
 
  IF G.Outline THEN
    GOTO XYPlot
  END IF
 
IF G.Phi >= 0 THEN
  IF (G.Lambda > -DEG180) AND (G.Lambda <= -DEG40) THEN
    G.Lambda2 = -DEG100
    GOTO XYPlot
  END IF

  IF (G.Lambda > -DEG40) AND (G.Lambda <= DEG180) THEN
    G.Lambda2 = DEG30
    GOTO XYPlot
  END IF
END IF

  IF (G.Lambda >= -DEG180) AND (G.Lambda <= -DEG100) THEN
    G.Lambda2 = -DEG160
    GOTO XYPlot
  END IF

  IF (G.Lambda >= -DEG100) AND (G.Lambda <= -DEG20) THEN
    G.Lambda2 = -DEG60
    GOTO XYPlot
  END IF

  IF (G.Lambda >= -DEG20) AND (G.Lambda <= DEG80) THEN
    G.Lambda2 = DEG20
    GOTO XYPlot
  END IF

  IF (G.Lambda >= DEG80) AND (G.Lambda <= DEG180) THEN
    G.Lambda2 = DEG140
    GOTO XYPlot
  END IF
'
' ----------------------------------
'
XYPlot:
' The DeltaLambda# value is computed at the top of the DrawProjection
' subroutine to place the Longitude (G.Lambda#) Value into the correct
' quadrant of the map in relation to the center of the full map. It is
' used here to establish the same Longitude relationship for the lobe.
 
  Lambda1# = Normalize#(DeltaLambda#, G.Lambda2)

  IF ABS(G.Phi) < MERGEPOINT THEN ' (40.73666# * Deg2Rad)
    ' Sinusoidal Projection
    XCoord# = Radius# * Lambda1# * COS(G.Phi)
    YCoord# = Radius# * G.Phi
  ELSE
'   Part of the MollweideFormula# Function
'   Placed here to reduce amount of calculations
    SinPhi# = DEG180 * SIN(G.Phi)
'
'   This subroutine uses Newton-Raphson iteration to derive the Theta# value
'
'   Mollweide Projection
    Start# = G.Phi * .5
    DeltaThetaPrime# = Start#
    DO
      FirstGuess# = MollweideFormula#(DeltaThetaPrime#, SinPhi#)
      DeltaThetaPrime# = DeltaThetaPrime# + DTPINCR#
'     MollweideFormula# is a user defined function - use The Menu Bar - View
'     Menu to find it.
      SecondGuess# = (MollweideFormula#(DeltaThetaPrime#, SinPhi#) - FirstGuess#) / DTPINCR#
      Start# = Start# - FirstGuess# / SecondGuess#
      DeltaThetaPrime# = Start#
    LOOP WHILE ABS(MollweideFormula#(DeltaThetaPrime#, SinPhi#)) >= TOLERANCE#
'
    Theta# = DeltaThetaPrime# * .5
'
    XCoord# = Radius# * XCONST * Lambda1# * COS(Theta#)
    YCoord# = Radius# * (SQRT2 * SIN(Theta#) - MOLLRFACTOR * Sign#(G.Phi))
'
  END IF
'
' In addition to the extensive use of IF-THEN statements to define the
' Boundries and Central Longitudes of each lobe, the TRICK to
' Interrupting and recentering a projection is in the following
' line of code. As you know the Homolosine consists of 2 projections that
' are merged at 40 Deg. 44 Min. and 11.89 Sec.(40.73666# Decimal Deg.).
' The X Coordinate formula of the center projection (Sinusoidal) is copied
' and modified by making the G.Phi# (Latitude) value always equal to 0 (Cosine
' of 0 is 1) and using the central longitude (G.Lambda2#) of the lobe the
' Current Longitude (G.Lambda#) value is within. This value is then added to
' the Normal X coordinate output of both projections.
'
'     Normal X OUT  | Modified X coord of center Projection
  XCoord# = XCoord# + (G.Lambda2 * Radius#)
   
' This line of code is used to turn off PolyLine%s that cross into different
' lobes. It could be better (When crossing the equator)!
 
  IF (PreviousPhi# > 0 AND G.Phi < 0) OR (PreviousPhi# < 0 AND G.Phi > 0) THEN
    G.Visible = TRUE
  ELSE
    IF (PreviousLambda2# <> G.Lambda2) THEN
      G.Visible = FALSE
    END IF
  END IF
'
  PreviousPhi# = G.Phi
  PreviousLambda2# = G.Lambda2
'
END SUB '{ Goode. }

SUB LatLongEntry (Title$, Note$)
'
  CLS
 
  IF Title$ <> "" THEN
    Col = Center%(LEN(Title$))
    LOCATE 2, Col: PRINT Title$
  END IF
'
  IF Note$ <> "" THEN
    Col = Center%(LEN(Note$))
    LOCATE 4, Col: PRINT Note$
  END IF
'
   LOCATE CSRLIN + 1, 20
'
   INPUT "Do you want Grid Lines (Y/N)? ", Answer$
   Answer$ = UCASE$(Answer$)
  
   IF Answer$ = "Y" OR Answer$ = "" THEN
     G.Grid = TRUE
   ELSE
     G.Grid = FALSE
   END IF

'
   LOCATE CSRLIN + 1, 19
   INPUT "Send Output to Plot File (Y/N)? ", Answer$
   Answer$ = UCASE$(Answer$)
  
   IF Answer$ = "Y" THEN
     G.PlotFile = TRUE
   ELSE
     G.PlotFile = FALSE
   END IF

'
END SUB '{ LatLongEntry. }

SUB LatLongMenu
'
ExitMenu% = FALSE

DO
  CLS
'
  LOCATE 2, 20
  PRINT "** Modify Latitude/ Longitude Display **"
'
  SELECT CASE G.LongOption
   
    CASE 1
      LOCATE 4, 17
      PRINT "Currently all Longitude lines converge at Pole"
          
    CASE 2
      LOCATE 4, 11
      PRINT "Currently all Longitude lines end 5 Degrees away from Pole"
          
  END SELECT '{ G.LongOption. }
        
  LOCATE 6, 12
  PRINT "1. Change Latitude increment (Currently"; G.LatStep; "Degrees)"
 
  LOCATE 7, 12
  PRINT "2. Change Longitude increment (Currently"; G.LongStep; "Degrees)"
 
  LOCATE 8, 12
  PRINT "3. All Longitude lines converge at Pole"
 
  LOCATE 9, 12
  PRINT "4. Longitude lines end 5 Degrees away from Pole"
 
  LOCATE 11, 14
  INPUT "Select Option (1-4) or 0 to Return to Previous Menu: ", Menu%
'
  SELECT CASE Menu%
'    
    CASE 0
      ExitMenu% = TRUE
'
    CASE 1
      LOCATE 12, 1: PRINT SPACE$(80)
      LOCATE 13, 1: PRINT SPACE$(80)
     
      LOCATE 12, 20
      INPUT "Change Latitude Increment to (5 to 90): ", Increment%
     
      IF Increment% >= 5 AND Increment% <= 90 THEN
        G.LatStep = Increment%
      ELSE
        PRINT CHR$(7)
      END IF
'
    CASE 2
      LOCATE 12, 1: PRINT SPACE$(80)
      LOCATE 13, 1: PRINT SPACE$(80)
     
      LOCATE 12, 19
      INPUT "Change Longitude Increment to (5 to 90): ", Increment%
     
      IF Increment% >= 5 AND Increment% <= 90 THEN
        G.LongStep = Increment%
      ELSE
        PRINT CHR$(7)
      END IF
'   
    CASE 3
      G.LongOption = 1
'      
    CASE 4
      G.LongOption = 2
'
    CASE ELSE
      PRINT CHR$(7)
'
  END SELECT '{ Menu. }
'
  LOOP UNTIL ExitMenu%
'
END SUB '{ LatLongMenu. }

FUNCTION LN# (n#) STATIC
'
IF n# > 0 THEN
  LN# = LOG(n#)
ELSE
  PRINT "Error: LN#(n#) Where n# <= 0# "
  SYSTEM
END IF
'
END FUNCTION ' { LN#. }

SUB LongitudeFOR (Begin#, Finish#, Incr#) STATIC
'
  PolyLine% = TRUE
 
  FOR LatGrid# = Begin# TO Finish# STEP Incr#
    G.Phi = LatGrid#
    CALL DrawProjection(PolyLine%)
  NEXT LatGrid#
'
END SUB '{ LongitudeFOR. }

SUB MakeLatitudeLines (LatRange%, LatDetail%)
'
  G.Outline = TRUE
  FOR LatGrid% = LatRange% TO -LatRange% STEP -1
   
    IF ABS(LatGrid%) < 90 THEN
   
      Even% = (LatGrid% MOD G.LatStep = 0)
      IF Even% THEN
        G.Phi = LatGrid% * DEG2RAD
        PolyLine% = FALSE
        CALL DrawProjection(PolyLine%)
        PolyLine% = TRUE

        IF LatGrid% <= 0 THEN
'
'       > Southern Lobe 1
'
          G.Lambda2 = -DEG160
          PolyLine% = FALSE
          CALL DrawProjection(PolyLine%)
          PolyLine% = TRUE
     
          FOR LongGrid% = -180 TO -100 STEP 5
            G.Lambda = LongGrid% * DEG2RAD
            CALL DrawProjection(PolyLine%)
          NEXT LongGrid%
'
'       > Southern Lobe 2
'
          G.Lambda2 = -DEG60
          PolyLine% = FALSE
          CALL DrawProjection(PolyLine%)
          PolyLine% = TRUE
     
          FOR LongGrid% = -100 TO -20 STEP 5
            G.Lambda = LongGrid% * DEG2RAD
            CALL DrawProjection(PolyLine%)
          NEXT LongGrid%
'
'       > Southern Lobe 3
'
          G.Lambda2 = DEG20
          PolyLine% = FALSE
          CALL DrawProjection(PolyLine%)
          PolyLine% = TRUE
     
          FOR LongGrid% = -20 TO 80 STEP 5
            G.Lambda = LongGrid% * DEG2RAD
            CALL DrawProjection(PolyLine%)
          NEXT LongGrid%
'
'       > Southern Lobe 4
'
          G.Lambda2 = DEG140
          PolyLine% = FALSE
          CALL DrawProjection(PolyLine%)
          PolyLine% = TRUE
     
          FOR LongGrid% = 80 TO 180 STEP 5
            G.Lambda = LongGrid% * DEG2RAD
            CALL DrawProjection(PolyLine%)
          NEXT LongGrid%
     
        ELSE
'
'       > Northern Lobe 1
'
          G.Lambda2 = -DEG100
          IF LatGrid% < 60 THEN
            PolyLine% = FALSE
            CALL DrawProjection(PolyLine%)
            PolyLine% = TRUE
       
            FOR LongGrid% = -180 TO -40 STEP 5
              G.Lambda = LongGrid% * DEG2RAD
              CALL DrawProjection(PolyLine%)
            NEXT LongGrid%
          ELSE
            PolyLine% = FALSE
            CALL DrawProjection(PolyLine%)
            PolyLine% = TRUE
       
            FOR LongGrid% = -180 TO -10 STEP 5
              G.Lambda = LongGrid% * DEG2RAD
              CALL DrawProjection(PolyLine%)
            NEXT LongGrid%
          END IF
'
'       > Northern Lobe 2
'
          G.Lambda2 = DEG30
          IF LatGrid% < 50 THEN
            PolyLine% = FALSE
            CALL DrawProjection(PolyLine%)
            PolyLine% = TRUE
       
            FOR LongGrid% = -40 TO 180 STEP 5
              G.Lambda = LongGrid% * DEG2RAD
              CALL DrawProjection(PolyLine%)
            NEXT LongGrid%
          END IF

          IF LatGrid% >= 50 AND LatGrid% < 60 THEN
            PolyLine% = FALSE
            CALL DrawProjection(PolyLine%)
            PolyLine% = TRUE
       
            FOR LongGrid% = -40 TO 180 STEP 5
              G.Lambda = LongGrid% * DEG2RAD
              CALL DrawProjection(PolyLine%)
            NEXT LongGrid%
       
            FOR LongGrid% = -180 TO -160 STEP 5
              G.Lambda = LongGrid% * DEG2RAD
              CALL DrawProjection(PolyLine%)
            NEXT LongGrid%
          END IF
     
          IF LatGrid% >= 60 THEN
            PolyLine% = FALSE
            CALL DrawProjection(PolyLine%)
            PolyLine% = TRUE
       
            FOR LongGrid% = -50 TO 180 STEP 5
              G.Lambda = LongGrid% * DEG2RAD
              CALL DrawProjection(PolyLine%)
            NEXT LongGrid%
       
            FOR LongGrid% = -180 TO -160 STEP 5
              G.Lambda = LongGrid% * DEG2RAD
              CALL DrawProjection(PolyLine%)
            NEXT LongGrid%
          END IF
        END IF
      END IF
    END IF

  NEXT LatGrid%
  G.Outline = FALSE
'
END SUB

SUB MakeLongitudeLines (LatRange%, LongDetail!)
'
' ===========================================
' Extension 3 (Right side of Northern Lobe 2)
' ===========================================
'
  G.Outline = TRUE
  G.Lambda2 = DEG30
 
  FOR LongGrid% = -160 TO -180 STEP -1
    Even% = (LongGrid% MOD G.LongStep = 0)
    IF Even% THEN
      IF LongGrid% >= -180 AND LongGrid% < -160 THEN
        G.Lambda = LongGrid% * DEG2RAD
        PolyLine% = FALSE
'
        FOR LatGrid! = LatRange% TO 50 STEP -LongDetail!
          G.Phi = LatGrid! * DEG2RAD
          CALL DrawProjection(PolyLine%)
          PolyLine% = TRUE
        NEXT LatGrid!
      END IF
   END IF
  NEXT LongGrid%
 
  G.Outline = FALSE
'
' ===============================================
' Longitude Lines from 180 degrees to -40 Degrees
' ===============================================
'
  FOR LongGrid% = 180 TO -40 STEP -1
   
    IF LongGrid% < 180 THEN
      Even% = (LongGrid% MOD G.LongStep = 0)
      IF Even% THEN
        IF LongGrid% = -40 THEN
          G.Lambda2 = DEG30
          G.Outline = TRUE
        ELSE
          G.Outline = FALSE
        END IF

        G.Lambda = LongGrid% * DEG2RAD
        PolyLine% = FALSE
'
        North# = LatRange% * DEG2RAD
        South# = -LatRange% * DEG2RAD
    
        IF LatRange% = 85 THEN
          IF G.Lambda = DEG30 THEN
            North# = DEG90
          END IF
 
          IF G.Lambda = DEG140 THEN
            South# = -DEG90
          END IF

          IF G.Lambda = DEG20 THEN
            South# = -DEG90
          END IF
        END IF
     
        IF LongGrid% = -40 THEN
          South# = DEG60
        END IF
       
        IF LongGrid% = 80 OR LongGrid% = -20 THEN
          South# = 0
        END IF
       
        CALL LongitudeFOR(North#, South#, -LongDetail! * DEG2RAD)
      END IF
    END IF
  NEXT LongGrid%
'
' ===========
' Extension 2 (Left side of Northern Lobe 2)
' ===========
'
  G.Outline = TRUE
  G.Lambda2 = DEG30#

  FOR LongGrid% = -40 TO -50 STEP -1
    Even% = (LongGrid% MOD G.LongStep = 0)
    IF Even% THEN
      IF LongGrid% > -50 AND LongGrid% < -40 THEN
        G.Lambda = LongGrid% * DEG2RAD
        PolyLine% = FALSE
'
        FOR LatGrid! = LatRange% TO 60 STEP -LongDetail!
          G.Phi = LatGrid! * DEG2RAD
          CALL DrawProjection(PolyLine%)
          PolyLine% = TRUE
        NEXT LatGrid!
      END IF
    END IF
  NEXT LongGrid%
'
' ===========================================
' Extension 1 (Right side of Northern Lobe 1)
' ===========================================
'
  G.Outline = TRUE
  G.Lambda2 = -DEG100

  FOR LongGrid% = -10 TO -40 STEP -1
    Even% = (LongGrid% MOD G.LongStep = 0)
    IF Even% THEN
      IF LongGrid% > -40 AND LongGrid% < -10 THEN
        G.Lambda = LongGrid% * DEG2RAD
        PolyLine% = FALSE
'
        FOR LatGrid! = LatRange% TO 60 STEP -LongDetail!
          G.Phi = LatGrid! * DEG2RAD
          CALL DrawProjection(PolyLine%)
          PolyLine% = TRUE
        NEXT LatGrid!
      END IF
    END IF
'
  NEXT LongGrid%
  G.Outline = FALSE
'
' ==============================================
' Longitude Lines from 0 degrees to -180 Degrees
' ==============================================
'
  FOR LongGrid% = -40 TO -180 STEP -1
  
    IF LongGrid% = -180 THEN EXIT FOR ' no real need to redraw -180

    Even% = (LongGrid% MOD G.LongStep = 0)
    IF Even% THEN
      IF LongGrid% <= -40 THEN
        G.Lambda = LongGrid% * DEG2RAD
        PolyLine% = FALSE
'
        North# = LatRange% * DEG2RAD
        South# = -LatRange% * DEG2RAD
     
        IF LatRange% = 85 THEN
          IF G.Lambda = -DEG100 THEN
            North# = DEG90
          END IF
  
          IF G.Lambda = -DEG60 THEN
            South# = -DEG90
          END IF

          IF G.Lambda = -DEG160 THEN
            South# = -DEG90
          END IF
        END IF
      
        IF LongGrid% = -100 THEN
          South# = 0
        END IF
       
        CALL LongitudeFOR(North#, South#, -LongDetail! * DEG2RAD)
   
      END IF
    END IF
'
  NEXT LongGrid%
'
END SUB '{ MakeLongitudeLines. }

FUNCTION MollweideFormula# (ThetaPrime#, MPhi#) STATIC
' From John Snyder's "Map Projections -- A Working Manual"
' Used by the Newton-Raphson iteration in the Mollweide portion
' of the Map subroutine
'
  MollweideFormula# = -(ThetaPrime# + SIN(ThetaPrime#) - MPhi#) / (1 + COS(ThetaPrime#))
'
END FUNCTION '{ MollweideFormula#. }

FUNCTION Normalize# (LambdaVal#, Lambda0Val#) STATIC
'
' This subroutine is responsible for placing the Longitude (Lambda)
' value into the correct part of the map in relation to the selected
' Central Longitude (Lambda0) of the map.
'
LambdaDiff# = LambdaVal# - Lambda0Val#

DO WHILE ABS(LambdaDiff#) > DEG180
  IF LambdaDiff# < 0 THEN
    LambdaDiff# = LambdaDiff# + DEG360
  ELSE
    LambdaDiff# = LambdaDiff# - DEG360
  END IF
LOOP

Normalize# = LambdaDiff#
'
END FUNCTION ' { Normalize#. }

SUB PlotFileColorMenu
'
ExitMenu% = FALSE
'
DO
  CLS
'
  LOCATE 2, 11
  PRINT "** Change Plot File Pen Colors of Geographical Features **"
  LOCATE 4, 33: PRINT "Pen Color Codes"
  LOCATE 6, 29: PRINT "1 - Black  5 - Magenta"
  LOCATE 7, 29: PRINT "2 - Blue   6 - Yellow"
  LOCATE 8, 29: PRINT "3 - Red    7 - Cyan"
  LOCATE 9, 29: PRINT "4 - Green  8 - Brown"
        
  LOCATE 12, 27: PRINT "1. Grid Pen is:"; G.GridPen
  LOCATE 13, 27: PRINT "2. Coast Pen is:"; G.CoastPen
  LOCATE 14, 27: PRINT "3. Country Border Pen is:"; G.BorderPen
  LOCATE 15, 27: PRINT "4. Island Pen is:"; G.IslandPen
  LOCATE 16, 27: PRINT "5. State Border Pen is:"; G.StatePen
  LOCATE 17, 27: PRINT "6. Lake Pen is:"; G.LakePen
  LOCATE 18, 27: PRINT "7. River Pen is:"; G.RiverPen
  LOCATE 20, 14
  INPUT "Select Option (1-7) or 0 to Return to Main Menu: ", Menu%
    
  SELECT CASE Menu%
'
    CASE 0
      ExitMenu% = TRUE

    CASE 1
      LOCATE 20, 1: PRINT SPACE$(80)
      LOCATE 20, 26
      INPUT "Change Grid Pen to (1-8):", NewPen%
      IF NewPen% >= 1 AND NewPen% < 9 THEN
        G.GridPen = NewPen%
      ELSE
        PRINT CHR$(7)
      END IF
'
    CASE 2
      LOCATE 20, 1: PRINT SPACE$(80)
      LOCATE 20, 23
      INPUT "Change Coastline Pen to (1-8):", NewPen%
      IF NewPen% >= 1 AND NewPen% < 9 THEN
        G.CoastPen = NewPen%
      ELSE
        PRINT CHR$(7)
      END IF
'
    CASE 3
      LOCATE 20, 1: PRINT SPACE$(80)
      LOCATE 20, 21
      INPUT "Change Country Border Pen to (1-8):", NewPen%
      IF NewPen% >= 1 AND NewPen% < 9 THEN
        G.BorderPen = NewPen%
      ELSE
        PRINT CHR$(7)
      END IF
'
    CASE 4
      LOCATE 20, 1: PRINT SPACE$(80)
      LOCATE 20, 25
      INPUT "Change Island Pen to (1-8):", NewPen%
      IF NewPen% >= 1 AND NewPen% < 9 THEN
        G.IslandPen = NewPen%
      ELSE
        PRINT CHR$(7)
      END IF
'
    CASE 5
      LOCATE 20, 1: PRINT SPACE$(80)
      LOCATE 20, 22
      INPUT "Change State Border Pen to (1-8):", NewPen%
      IF NewPen% >= 1 AND NewPen% < 9 THEN
        G.StatePen = NewPen%
      ELSE
        PRINT CHR$(7)
      END IF
'
    CASE 6
      LOCATE 20, 1: PRINT SPACE$(80)
      LOCATE 20, 26
      INPUT "Change Lake Pen to (1-8):", NewPen%
      IF NewPen% >= 1 AND NewPen% < 9 THEN
        G.LakePen = NewPen%
      ELSE
        PRINT CHR$(7)
      END IF
'
    CASE 7
      LOCATE 20, 1: PRINT SPACE$(80)
      LOCATE 20, 26
      INPUT "Change River Pen to (1-8):", NewPen%
      IF NewPen% >= 1 AND NewPen% < 9 THEN
        G.RiverPen = NewPen%
      ELSE
        PRINT CHR$(7)
      END IF
'
    CASE ELSE
      PRINT CHR$(7)

  END SELECT '{ Menu. }
'
LOOP UNTIL ExitMenu%
'
END SUB '{ PlotFileColorMenu. }

FUNCTION PointCount& (PtCnt&)
' Used by GetPKDData subroutine
'
  CALL ReadInfile(Byte%)
  PtCnt& = Byte% * 100
  CALL ReadInfile(Byte%)
  PtCnt& = PtCnt& + Byte%
  PointCount& = PtCnt&
'
END FUNCTION '{ PointCount&. }

FUNCTION PolyLineColor% (PolyLineHeader%) STATIC
'
SELECT CASE PolyLineHeader%
'
  CASE 1000 TO 1999
'   Coast
    PolyLineColor% = G.CoastColor
    PlotPen$ = LTRIM$(STR$(G.CoastPen))
'
  CASE 2000 TO 2999
'   Country Borders
    PolyLineColor% = G.BorderColor
    PlotPen$ = LTRIM$(STR$(G.BorderPen))
       
  CASE 3000 TO 3999
'   Canadian Provinces
    PolyLineColor% = G.StateColor
    PlotPen$ = LTRIM$(STR$(G.StatePen))
      
  CASE 4000 TO 4999
'   U.S. State borders
    PolyLineColor% = G.StateColor
    PlotPen$ = LTRIM$(STR$(G.StatePen))
       
  CASE 5000 TO 5999
'   Islands
    PolyLineColor% = G.IslandColor
    PlotPen$ = LTRIM$(STR$(G.IslandPen))
       
  CASE 6000 TO 6999
'   Lakes
    PolyLineColor% = G.LakeColor
    PlotPen$ = LTRIM$(STR$(G.LakePen))
       
  CASE 7000 TO 7999
'   Rivers
    PolyLineColor% = G.RiverColor
    PlotPen$ = LTRIM$(STR$(G.RiverPen))
        
  CASE 8000 TO 8999
'   Australian States
    PolyLineColor% = G.StateColor
    PlotPen$ = LTRIM$(STR$(G.StatePen))
'
END SELECT '{ PlineHeader. }
   
IF G.PlotFile THEN
  IF G.LastPlotPen <> PlotPen$ THEN
    PRINT #1, "SP" + PlotPen$ + ";"
  END IF

  G.LastPlotPen = PlotPen$
END IF
'
END FUNCTION '{ PolyLineColor%. }

FUNCTION Raise# (n#, Power#)
' Raise a number to a power
' (even negative numbers raised to a non-integer power)
'
IF n# = 0 THEN
  IF Power# = 0 THEN
    Raise# = 1
  ELSE
    Raise# = 0
  END IF
ELSE
  Raise# = Sign#(n#) * EXP(Power# * LN#(ABS(n#)))
END IF
'
END FUNCTION '{ Raise. }

SUB ReadInfile (Byte%)
STATIC Rec AS STRING * 1
' Used by GetPKDData subroutine
'
 Index = Index + 1 'Record Count
 GET #1, , Rec
 Byte% = ASC(Rec)
'
' Convert to Signed Char
 IF Byte% > 127 THEN
   Byte% = Byte% - 256
 END IF
'
END SUB '{ ReadInFile. }

FUNCTION Round# (n#, PowerOfTen%) STATIC
'
pTen# = 10 ^ PowerOfTen%
RTemp# = INT(n# / pTen# + .5) * pTen#
Temp$ = STR$(RTemp#)
Temp$ = MID$(Temp$, 1, ABS(PowerOfTen%) + 4)
Round# = VAL(Temp$)
'
END FUNCTION

FUNCTION Sec# (n#)
'
Cosine# = COS(n#)
IF ABS(Cosine#) <= 0# THEN
  PRINT "Error: Sec#(n#) where n# ="; n#
  PRINT "       Cosine# ="; Cosine#
  SYSTEM
ELSE
  Sec# = (1 / Cosine#)
END IF

END FUNCTION

FUNCTION Sign# (n#)
' Return -1 if n# < 0, or +1 if n# >= 0
'
IF n# = 0 THEN
  Sign# = 1
ELSE
  Sign# = ABS(n#) / n#
END IF
'
END FUNCTION '{ Sign. }

