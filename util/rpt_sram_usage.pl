#! /usr/bin/perl -w

my $fn_log = $ARGV[0];
my ($bss_size,$bss_vma) = (0,0);
my ($sec_no,$sec_name,$sec_size,$sec_vma,$sec_lma) = ( 0, 0, 0, 0);
my $KB = 1024;
my $sram_base = hex("0x2000000");
my $stack_size = 4* $KB;	## all kinds of stack ##

my $sram_total = 192 * $KB;
my $sram_in_use = 0;
my $sram_free = 0;


sub dump_rpt()
{
		$sram_free = $sram_total - $sram_in_use -$stack_size;
		printf("\n-------------------------SRAM report-------------------------\n");

		printf("sram total  -----------> %5d KB\n", ($sram_total / $KB ) );	
		printf("sram in use -----------> %5d KB\n", ($sram_in_use / $KB) );
		printf("sram free   -----------> %5d KB\n", ($sram_free / $KB)   );
}		 

#################################
#								#
#			Main				#
#								#
#################################

printf("<----------------------check sram usage---------------------->(read %s by %s)\n",$fn_log,$0);

open(FIN,"$fn_log") || die "cannot open $fn_log";

$sram_in_use = 0; 

printf("Idx Name          Size      VMA       LMA\n");
while(<FIN>)
{
	chomp;
    #0 startup       00000600  03000000  03000000  00008000  2**2
    #1 prog_in_flash 00019f24  03002000  03002000  00012000  2**2
	#2 prog_in_sram  00003aac  02000000  0301bf24  00030000  2**2
	#3 .ARM          00000008  02003aac  0301f9d0  00033aac  2**2
    #4 .data         00000c0c  02003ab8  0301f9e0  00033ab8  2**3
    #5 .bss          00008d10  020046d0  030205f8  000346c4  2**3
    #6 .boot_data    00000010  02024000  03000600  0000c000  2**2
	#7 .boot_bss     000001c0  02024010  03000610  0000c010  2**2

	if(/(\d+)\s+(\S+)\s+(\S+)\s+(02......)/)
	{
		($sec_no,$sec_name,$sec_size,$sec_vma,$sec_lma) = ($1,$2,$3,$4,$5);
		$sram_in_use += hex($sec_size); 
		printf("%s\n",$_);		
		#printf("sram_in_use=0x%x\n",$sram_in_use);
	}
}
close(FIN);

dump_rpt();
