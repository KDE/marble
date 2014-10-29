#!/usr/bin/perl

# This file is part of the Marble Virtual Globe.
#
# This program is free software licensed under the GNU LGPL. You can
# find a copy of this license in LICENSE.txt in the top directory of
# the source code.
#
# Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>


print "//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <hardaker\@users.sourceforge.net>
//

";

print "    // Generated automatically by $0\n";

foreach my $c ('0'..'9') {
    print "    m_dstCallDigits['$c']          = $c;\n";
    print "    m_dstCallSouthEast['$c']       = true;\n";
    print "    m_dstCallLongitudeOffset['$c'] = 0;\n";
    print "    m_dstCallMessageBit['$c']      = 0;\n\n";
}

foreach my $c ('A'..'J') {
    print "    m_dstCallDigits['$c']          = " . (ord($c)-ord('A')) . ";\n";
    print "    m_dstCallMessageBit['$c']      = 2;\n\n";    # 1(Custom)
}

    print "    m_dstCallDigits['K']          = 0;\n";     # technically ' '
    print "    m_dstCallMessageBit['K']      = 2;\n\n";   # 1(Custom)

    print "    m_dstCallDigits['L']          = 0;\n";     # technically ' '
    print "    m_dstCallMessageBit['L']      = 0;\n";
    print "    m_dstCallLongitudeOffset['L'] = 0;\n";
    print "    m_dstCallSouthEast['L']       = true;\n\n";  

foreach my $c ('P'..'Y') {
    print "    m_dstCallDigits['$c']          = " . (ord($c)-ord('P')) . ";\n";
    print "    m_dstCallSouthEast['$c']       = false;\n";
    print "    m_dstCallLongitudeOffset['$c'] = 100;\n";
    print "    m_dstCallMessageBit['$c']      = 1;\n\n";    # 1 (Standard)
}


    print "    m_dstCallDigits['Z']          = 0;\n";     # technically ' '
    print "    m_dstCallMessageBit['Z']      = 1;\n";     # 1 (Standard)
    print "    m_dstCallLongitudeOffset['Z'] = 100;\n";
    print "    m_dstCallSouthEast['Z']       = false;\n"; 

print "\n\n";

foreach my $c (118..127) {
    print "    m_infoFieldByte1[$c] = $c-1128;\n";
}

foreach my $c (38..127) {
    print "    m_infoFieldByte1[$c] = $c-28;\n";
}

foreach my $c (108..117) {
    print "    m_infoFieldByte1[$c] = $c-8;\n";
}

foreach my $c (110..179) {
    print "    m_infoFieldByte1[$c] = $c-8;\n";
}

print "\n\n";

print "    m_standardMessageText[0] = \"M0: Off Duty\";\n";
print "    m_standardMessageText[1] = \"M1: En Route\";\n";
print "    m_standardMessageText[2] = \"M2: In Service\";\n";
print "    m_standardMessageText[3] = \"M3: Returning\";\n";
print "    m_standardMessageText[4] = \"M4: Committed\";\n";
print "    m_standardMessageText[5] = \"M5: Special\";\n";
print "    m_standardMessageText[6] = \"M6: Priority\";\n";
print "    m_standardMessageText[7] = \"M7: EMERGENCY\";\n\n";

for (my $i = 0; $i <= 7; $i++) {
    print "    m_customMessageText[$i] = \"Custom-$i\";\n";
}
print "    m_customMessageText[7] = \"M7: EMERGENCY\";\n";

print "\n";

foreach my $chr (ord('!') .. ord('~')) {
    printf ("    m_pixmaps[QPair<QChar, QChar>('/','%c')] = " .
	    "\"aprs/primary/%02d.png\";\n", $chr, $chr-ord('!'));
    printf ("    m_pixmaps[QPair<QChar, QChar>('\\\\','%c')] = " .
	    "\"aprs/secondary/%02d.png\";\n", $chr, $chr-ord('!'));
}
