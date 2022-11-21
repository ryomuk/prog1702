#!/usr/bin/perl

while(<>){
    if(m/ ([01]{8}) /){
	$data=oct('0b'.$1);
	#printf("%02X\n", $data);
	printf("%c", $data);
    }
}
