#! /usr/bin/perl -w

#############################
#							#
#	list source code owner 	#
#							#	
#############################

my @arr_tim = (
"smartConf.c",
"smartConf.h",
"wep.c",
"wep.h",
"airkiss.h",
"airkiss.c",
"smartComm.c",
"smartComm.h",
"userSmart.c"
 );

my @arr_jim = (
"flash_api.c",
"drv_flash.c",
"ssid_recv.c",
"ssid_recv.h",
"hw_tbl.c",
"hw_tbl.h",
"cfs-coffee.c",
"debug-uart.c",
"cpu.c",
"irq.c",
"bsp.c",
"drv_mbox.c",
"console.c",
"ssv_lib.h",
"contiki-main.c",
"ssv6200_common.h",
"ssv_types.h",
"process.c",
"xmem.c",
"boot-main.c",
"fence.c",
"irq.h",
"ssv_lib.c",
"pk_buf_test.c",
"hw_init.c",
"serial.c",
"drv_phy.c",
"nullradio.c",
"nullradio.h",
"ssvradio.c",
"ssvradio.h",
"ssv6200_rx.c",
"ssv6200_rx.h",
"ssv6200_tx.c",
"soft_ap.c",
"soft_ap.h",
"dhcps.c",
"shell-ping.c",
"dbg-printf.c",
"drv_pktbuf.c",
"ssv_config.h",
"bsp_info.c"
);

my @arr_jonathan = (
"i2c.c",
"i2c.h",
"EAPOL.c",
"wpa.c",
"simple-udp.c",
"tcpdump.c",
"tcpip.c",
"clock.c",
"rtimer-arch.c",
"drv_pmu.c",
"hmac_sha1.c",
"sha1.c",
"collect.c",
"collect-neighbor.c",
"simple-udp.c",
"uaodv.c",
"ieee80211_mgmt.h",
"systemconf.c",
"timetable.c",
"dhcp_process.c",
"uip_arp.c",
"ieee80211_mac.c",
"ieee80211_mac.h",
"ieee80211_main.h",
"ieee80211_rdc.c",
"ieee80211_main.c",
"ieee80211_net.c",
"ieee80211_mgmt.c",
"cxmac.c",
"xmac.c",
"lpp.c",
"nullrdc.c",
"contikimac.c",
"dhcpc.c",
"uip-fw.c",
"uip-over-mesh.c",
"uip-split.c",
"uip.c",
"uip_arp.h",
"uip-icmp6.c",
"hdr_80211.h",
"pwm.c",
"hmac_md5.c",
"hash_alg.c",
"uip-udp-packet.c",
"wep.c",
"wep.h",
"crypthw.c",
"crypthw.h"

 );

my @arr_ivan = (
"ssv6060-main.c",
"Cabrio-main.c",
"Cabrio-main.c",
"resolv.c",
"dbg-atcmd-api.c"
);


my @hash_owner = {};
my @hash_result = {};

#############################################
#	Description:							#
#		Build hash to find owner easily 	#
#											#	
#############################################
sub build_hash
{
	foreach $item (@arr_tim){
		$hash_owner{$item} = "tim";
	}
	foreach $item (@arr_jim){
		$hash_owner{$item} = "jim";
	}
	foreach $item (@arr_jonathan){
		$hash_owner{$item} = "jonathan";
	}
	foreach $item (@arr_ivan){
		$hash_owner{$item} = "ivan";
	}
}

#########################################################################################################
#	Description																							#
#		Main routine																					#
#	Example:																							#
#		../../core/net/ieee80211_bss/compiler.h:273:0: warning: "ENABLE" redefined [enabled by default]	#
#																										#
#########################################################################################################

sub find_owner
{
	my $fn_src = $_[0];
	my $owner = "";

	#printf " src = $fn_src\n";

	if(exists $hash_owner{$fn_src})
	{
		$owner = $hash_owner{$fn_src};
	} else {
		$owner = "unknown";
	}
	
	return $owner; 
}

#############################################
#	Description:							#
#		store messages into hash 			#
#											#	
#############################################
sub store_msg
{
	my ($owner,$lmsg) = ($_[0],$_[1]);			

	#in-fact all warning messages are stored in one variable!!!
	$hash_result{$owner} .= "$lmsg\n";
}

#####################################################################
#	Description:													#
#		walk through the hash and dump all stored messages 			#
#																	#	
##################################################################### 
sub dump_msg
{
	my $cur_owner = "";
	foreach my $owner (sort keys %hash_result)
	{
		if( ($cur_owner eq "") || ($owner ne $cur_owner) ){
			$cur_owner = $owner; 
			printf "----------------------------owner: %s---------------------------\n",$owner; 
		}

		printf"%s\n",$hash_result{$owner}; 
	}
}

#####################################################################
#	Description:													#
#		parse the compile.log and find owners			 			#
#																	#	
##################################################################### 
sub parse_log
{
	my $fn_log = "compile.log";
	open(FIN_LOG,"$fn_log") || die "cannot open $fn_log";
		
	while(<FIN_LOG>)
	{
		unless(/warning/)
		{
			next;		
		}

		chomp;
		$lmsg = $_;
		#printf "xxx:$_\n";
		if(/^.*\/(\S+):(\d+):(\d+):\s+warning/){
			$fn_src= $1;
			#printf "fn_src=$fn_src\n";	
			$owner = find_owner($fn_src);			
			#printf "aa:owner=$owner\n";
			store_msg($owner,$lmsg);
		}
		#printf "yyy:$_\n";
	}
			
	close(FIN_LOG);
}

#####################################
#									#
#				Main				#
#									#
#####################################

#build relation hash first!!!	Thus,we can use "file-name" as key to see who's the owner
build_hash();

parse_log();

dump_msg();


