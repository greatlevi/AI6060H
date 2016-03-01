#! /usr/bin/perl -w
my $fn_map = $ARGV[0];
my $hash_func = { }; 

#################################################
# Description									#
#   Dump report about all functions in sram		#
#												#
#################################################

sub dump_rpt
{
	printf "            --------------------------<Lists of FUNCTIONS in SRAM>--------------------------\n";
	my $prv_val = 0;
	my $cur_val = 0;
	my $delta = 0;
	my $total_text = 0;

	foreach my $key (sort { $a <=> $b } keys %hash_func) {

			$cur_val = $key;		

			if($prv_val ne 0) 
			{
				$delta = $cur_val - $prv_val;
				printf "                              -> delta= %d\n", $delta;
				$total_text += $delta;
			}

			#printf(" (cur_val,prv_val,delta)=(0x%x,0x%x,0x%x)\n",$cur_val,$prv_val,$delta);
			printf "            0x%x %s\n", $key, $hash_func{$key};

			$prv_val = $cur_val;		
	}

	printf("total_text = %d KB\n",$total_text / 1024);
}


#################################
#								#
#			Main				#
#								#
#################################

#printf("<----------------------check sram usage---------------------->(by %s)\n",$0);

open(FIN,"$fn_map") || die "cannot open $fn_map";


#find keep_in_sram first!!
while(<FIN>)
{
	chomp;
	if(/keep_in_sram/){
		last;
	}
}


while(<FIN>)
{
	chomp;
	#0x0200020c                process_exit


	
	if(/rodata/) { last; }
	if(/keep_in_sram/) { next; }
	


	if(/0x(\S+)\s+(\S+)/){
		my ($func_addr,$func_name) = ($1,$2); 
		$hash_func{hex($func_addr)} = $func_name;
	
		#printf "($1,$2) $_ \n";

	}


	#printf("$_ \n");	
}

close(FIN);

dump_rpt();
