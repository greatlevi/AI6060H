#! /usr/bin/perl -w

my $fn_map = $ARGV[0];

my @arr_func_all = ( 
	"text"
);

my @arr_func_run_once = ( 
#-------------AT COMMAND--------------
	"At_Initialize",
	"At_Reboot",
	"At_GetVersion",
	"At_GetManufactureInfo",
	"At_SetWifiConfig",
	"At_GetConfigure",
	"At_SetIfConfigure",
	"At_GetNetStatus",
	"At_NetScan",
	"At_Connect",
	"At_Send",
	"At_Disconnect",
	"At_SetGpio",
	"At_WriteGpio",
	"At_ReadGpio",
	"At_ParserRecv",
	"At_PowerOn",
	"At_RX",
	"At_TX",
	"At_PowerSaving",
	"At_SetPWM",
	"At_ParserInitial",
	"At_ParserInd",
	"At_Parser",
#-------------DHCP--------------
	"dhcp_configure",
	"dhcpc_configured",
	"dhcpc_unconfigured",
	"dhcpc_init",
	"dhcpc_appcall",
	"dhcpc_request",
#-------------PRINTF--------------
	"write_str",
	"printf",
	"fill_space",
	"format_str_v",
	"format_str",
	"dbg_send_bytes",
	"dbg_putchar",
	"dbg_blocking_putchar"

);

my $func_total_size = 0;
my $func_size = 0;
my $func_name = "";

my $hash_func= { };

my $key_found= 0;
my $size_found = 0;

sub chk_keyword_found
{
	$line = $_[0];
	$_ = $line;

	#printf "chk:$line  \n";	

	foreach $item (@arr_func_run_once){
		#printf "item=$item\n";
		if(/$item/)
		{
			printf "$item found\n";
		}
	}
}

sub chk_all_text
{

	open(FIN,"$fn_map") || die "cannot open $fn_map";
	
	while(<FIN>)
	{
		# *(.text.copy_step1)
		if(/\(.text.*\)/){
			next;
		}
		chomp;
	
		if($key_found==1){
	
				if(/0x\S+\s+(\S+)/)
				{
					$func_size = $1;
					$func_total_size += hex($func_size);
					#printf "(%s,%s)\n",$func_name,$func_size;
					$hash_func{$func_name} = hex($func_size);
					$key_found = 0;
					#exit(0);
				}
		}
	
	
		if(/text/){
			#chk_keyword_found($_);		
	
	#.text.At_Initialize
	#	0x020005d8        0x4 Cabrio-main.co
	#	0x020005d8            At_Initialize
	
	#.text.At_RX    0x02000ed4       0x38 Cabrio-main.co
	#               0x02000ed4       At_RX 
	
			foreach $item (@arr_func_all){
				if(/$item.(\S+)/)
				{
					$func_name = $1;
					$key_found = 1;
					if(/0x\S+\s+(\S+)/){
						$func_size = $1;
						$func_total_size += hex($func_size);
						#printf "(%s,%s)\n",$func_name,$func_size;
						$hash_func{$func_name} = hex($func_size);
						$key_found = 0;
					}
	
				}
			}#foreach

#.text.strtol   0x0200e690       0x28 libc.a(lib_a-strtol.     o)
#				0x0200e690                strtol
#.text          0x0200e6b8        0x0 lib_a-impure.     o)

			if(/.text\s+(0x\S+)\s+(0x\S+)/){
				$func_size = $2;
				#printf "%s: %s\n", $func_name,$func_size;	
				$hash_func{$func_name} += hex($func_size);
			}

		}#if
			
	}#while
	
	close(FIN);
}


sub chk_run_once
{
	open(FIN,"$fn_map") || die "cannot open $fn_map";
	
	while(<FIN>)
	{
		chomp;
	
		if($key_found==1){
				if(/0x\S+\s+(\S+)/)
				{
					$func_size = $1;
					$func_total_size += hex($func_size);
					#printf "(%s,%s)\n",$func_name,$func_size;
					$key_found = 0;
					#exit(0);
				}
		}
	
	
		if(/text/){
			#chk_keyword_found($_);		
	
	#.text.At_Initialize
	#	0x020005d8        0x4 Cabrio-main.co
	#	0x020005d8            At_Initialize
	
	#.text.At_RX    0x02000ed4       0x38 Cabrio-main.co
	#               0x02000ed4       At_RX 
	
			foreach $item (@arr_func_run_once){
				#printf "item=$item\n";
				if(/$item/)
				{
					$func_name = $item;
					$key_found = 1;
					if(/0x\S+\s+(\S+)/){
						$func_size = $1;
						$func_total_size += hex($func_size);
						#printf "(%s,%s)\n",$func_name,$func_size;
						$key_found = 0;
					}
	
				}
			}#foreach
	
	
		}
			
	}#while
	
	close(FIN);
}

sub dump_all_rpt
{
	#$printf "\n\nRUN_ALL: code size = 0x%x, %.2f(KB)\n",$func_total_size,$func_total_size/1024; 
	printf "###################################### Code Size ######################################\n";

	foreach my $key (sort { $hash_func{$a} <=> $hash_func{$b} } keys %hash_func) {
			printf "%50s %8d (Bytes)\n", $key, $hash_func{$key};
	}
		
	printf "--------------------------------------------------------------------------------------\n";	
	printf "Total code size = 0x%x, %.2f(KB) \n\n\n", $func_total_size, ($func_total_size/1024); 
}

sub dump_once_rpt
{
	printf "--------------------------------------------------------------------------------------\n";	
	printf "\n\n run-once code size = 0x%x, %.2f(KB)\n",$func_total_size,$func_total_size/1024; 
		
}

#################################
#								#
#			Main				#
#								#
#################################
#printf("<----------------------check run-once text---------------------->(by %s)\n",$0);

($func_size,$func_total_size) = (0,0);
chk_all_text();
dump_all_rpt();


#($func_size,$func_total_size) = (0,0);
#chk_run_once();
#dump_once_rpt();


