#!/usr/bin/perl

use LWP::Simple;
use Image::Magick;

print 'downloading image from server...' . "\n";
$response = getstore('http://xplanet.sourceforge.net/clouds/clouds_4096.jpg', 'clouds.jpg');
die 'error downloading image' unless is_success($response);

$clouds = Image::Magick->new();
$clouds->ReadImage('clouds.jpg');

print 'resizing source image...' . "\n";
$clouds->Resize(width=>'2700', height=>'1350');

sub tile {
	my ($x, $y, $level) = @_;
	
	$tile = $clouds->Clone();
	$tile->Crop(geometry=>'675x675+' . ($x * 675) . '+' . ($y * 675));
	
	$xstr = '00000' . $x;
	$ystr = '00000' . $y;
	
	mkdir($level . '/' . $ystr);
	$fname = $level . '/' . $ystr . '/' . $ystr . '_' . $xstr . '.jpg';
	print 'saving ' . $fname . '...' . "\n";
	$tile->Write($fname);
}

# level 1
mkdir('1');
for($x = 0; $x < 4; $x++) {
	for($y = 0; $y < 2; $y++) {
		tile($x, $y, '1');
	}
}

print 'resizing source image...' . "\n";
$clouds->Resize(width=>'1350', height=>'675');

# level 0
mkdir('0');
for($x = 0; $x < 2; $x++) {
	for($y = 0; $y < 1; $y++) {
		tile($x, $y, '0');
	}
}

# create expiration.txt
open(EXP, '>', 'expiration.txt');
print EXP 'Timestamp: ' . time . "\n";
print EXP 'Expiration: 10800' . "\n";
close(EXP);

