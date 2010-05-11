#!/usr/bin/perl

# This file is part of the Marble Desktop Globe.
#
# This program is free software licensed under the GNU LGPL. You can
# find a copy of this license in LICENSE.txt in the top directory of
# the source code.
#
# Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>


print "
//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

";

print "    // Generated automatically by $0\n";

foreach my $c ('0'..'9') {
    print "    DstcallDigits['$c']          = $c;\n";
    print "    DstcallSouthEast['$c']       = true;\n";
    print "    DstcallLongitudeOffset['$c'] = 0;\n";
    print "    DstcallMessageBit['$c']      = 0;\n\n";
}

foreach my $c ('A'..'J') {
    print "    DstcallDigits['$c']          = " . (ord($c)-ord('A')) . ";\n";
    print "    DstcallMessageBit['$c']      = 2;\n\n";    # 1(Custom)
}

    print "    DstcallDigits['K']          = 0;\n";     # technically ' '
    print "    DstcallMessageBit['K']      = 2;\n\n";   # 1(Custom)

    print "    DstcallDigits['L']          = 0;\n";     # technically ' '
    print "    DstcallMessageBit['L']      = 0;\n";
    print "    DstcallLongitudeOffset['L'] = 0;\n";
    print "    DstcallSouthEast['L']       = true;\n\n";  

foreach my $c ('P'..'Y') {
    print "    DstcallDigits['$c']          = " . (ord($c)-ord('P')) . ";\n";
    print "    DstcallSouthEast['$c']       = false;\n";
    print "    DstcallLongitudeOffset['$c'] = 100;\n";
    print "    DstcallMessageBit['$c']      = 1;\n\n";    # 1 (Standard)
}


    print "    DstcallDigits['Z']          = 0;\n";     # technically ' '
    print "    DstcallMessageBit['Z']      = 1;\n";     # 1 (Standard)
    print "    DstcallLongitudeOffset['Z'] = 100;\n";
    print "    DstcallSouthEast['Z']       = false;\n"; 

print "\n\n";

foreach my $c (118..127) {
    print "    InfoFieldByte1[$c] = $c-1128;\n";
}

foreach my $c (38..127) {
    print "    InfoFieldByte1[$c] = $c-28;\n";
}

foreach my $c (108..117) {
    print "    InfoFieldByte1[$c] = $c-8;\n";
}

foreach my $c (110..179) {
    print "    InfoFieldByte1[$c] = $c-8;\n";
}

print "\n\n";

print "    StandardMessageText[0] = \"M0: Off Duty\";\n";
print "    StandardMessageText[1] = \"M1: En Route\";\n";
print "    StandardMessageText[2] = \"M2: In Service\";\n";
print "    StandardMessageText[3] = \"M3: Returning\";\n";
print "    StandardMessageText[4] = \"M4: Committed\";\n";
print "    StandardMessageText[5] = \"M5: Special\";\n";
print "    StandardMessageText[6] = \"M6: Priority\";\n";
print "    StandardMessageText[7] = \"M7: EMERGENCY\";\n\n";

for (my $i = 0; $i <= 7; $i++) {
    print "    CustomMessageText[$i] = \"Custom-$i\";\n";
}
print "    CustomMessageText[7] = \"M7: EMERGENCY\";\n";


foreach my $chr (ord('!') .. ord('~')) {
    printf ("pixmaps[QPair<QChar, QChar>('/','%c')] = " .
	    "\"bitmaps/aprs-primary/%03d.png\";\n", $chr, $chr-ord('!'));
    printf ("pixmaps[QPair<QChar, QChar>('\\\\','%c')] = " .
	    "\"bitmaps/aprs-secondary/%03d.png\";\n", $chr, $chr-ord('!'));
}
#!/usr/bin/perl

# This file is part of the Marble Desktop Globe.
#
# This program is free software licensed under the GNU LGPL. You can
# find a copy of this license in LICENSE.txt in the top directory of
# the source code.
#
# Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>


print "
//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

";

print "    // Generated automatically by $0\n";

foreach my $c ('0'..'9') {
    print "    DstcallDigits['$c']          = $c;\n";
    print "    DstcallSouthEast['$c']       = true;\n";
    print "    DstcallLongitudeOffset['$c'] = 0;\n";
    print "    DstcallMessageBit['$c']      = 0;\n\n";
}

foreach my $c ('A'..'J') {
    print "    DstcallDigits['$c']          = " . (ord($c)-ord('A')) . ";\n";
    print "    DstcallMessageBit['$c']      = 2;\n\n";    # 1(Custom)
}

    print "    DstcallDigits['K']          = 0;\n";     # technically ' '
    print "    DstcallMessageBit['K']      = 2;\n\n";   # 1(Custom)

    print "    DstcallDigits['L']          = 0;\n";     # technically ' '
    print "    DstcallMessageBit['L']      = 0;\n";
    print "    DstcallLongitudeOffset['L'] = 0;\n";
    print "    DstcallSouthEast['L']       = true;\n\n";  

foreach my $c ('P'..'Y') {
    print "    DstcallDigits['$c']          = " . (ord($c)-ord('P')) . ";\n";
    print "    DstcallSouthEast['$c']       = false;\n";
    print "    DstcallLongitudeOffset['$c'] = 100;\n";
    print "    DstcallMessageBit['$c']      = 1;\n\n";    # 1 (Standard)
}


    print "    DstcallDigits['Z']          = 0;\n";     # technically ' '
    print "    DstcallMessageBit['Z']      = 1;\n";     # 1 (Standard)
    print "    DstcallLongitudeOffset['Z'] = 100;\n";
    print "    DstcallSouthEast['Z']       = false;\n"; 

print "\n\n";

foreach my $c (118..127) {
    print "    InfoFieldByte1[$c] = $c-1128;\n";
}

foreach my $c (38..127) {
    print "    InfoFieldByte1[$c] = $c-28;\n";
}

foreach my $c (108..117) {
    print "    InfoFieldByte1[$c] = $c-8;\n";
}

foreach my $c (110..179) {
    print "    InfoFieldByte1[$c] = $c-8;\n";
}

print "\n\n";

print "    StandardMessageText[0] = \"M0: Off Duty\";\n";
print "    StandardMessageText[1] = \"M1: En Route\";\n";
print "    StandardMessageText[2] = \"M2: In Service\";\n";
print "    StandardMessageText[3] = \"M3: Returning\";\n";
print "    StandardMessageText[4] = \"M4: Committed\";\n";
print "    StandardMessageText[5] = \"M5: Special\";\n";
print "    StandardMessageText[6] = \"M6: Priority\";\n";
print "    StandardMessageText[7] = \"M7: EMERGENCY\";\n\n";

for (my $i = 0; $i <= 7; $i++) {
    print "    CustomMessageText[$i] = \"Custom-$i\";\n";
}
print "    CustomMessageText[7] = \"M7: EMERGENCY\";\n";


foreach my $chr (ord('!') .. ord('~')) {
    printf ("pixmaps[QPair<QChar, QChar>('/','%c')] = " .
	    "\"bitmaps/aprs-primary/%03d.png\";\n", $chr, $chr-ord('!'));
    printf ("pixmaps[QPair<QChar, QChar>('\\\\','%c')] = " .
	    "\"bitmaps/aprs-secondary/%03d.png\";\n", $chr, $chr-ord('!'));
}
