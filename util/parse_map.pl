#! /usr/bin/perl -w

my $fn_map = "contiki-cabrio-e.map";

open(FIN,$fn_map) || die "cannot open $fn_map";

my %hash_objs = {};

while(<FIN>)
{
	chomp;
	#contiki-cabrio-e.a(flash_api.o)
	if(/^contiki-cabrio-e.a\((\S+)\)/)
	{
		#printf "$_ \n";
		#printf("---->%s<--------- \n",$1); 
		$hash_objs{$1} = 1;
	}
}
 
my $loop_i = 0;
foreach my $item (sort keys %hash_objs)
{
	printf("<%d>$item\n",$loop_i++);
}
