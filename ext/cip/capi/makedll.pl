#!/usr/bin/perl
# File makedll.pl created by Joshua Wills at 12:45:07 on Fri Sep 19 2003. 


my $curdir = ".";
if ($ARGV[0]) {
    $curdir = $ARGV[0];
}

#constants
my $INT = 0;
my $VOID = 1;
my $STRING = 2;

#functions to ignore in parsing ecmdClientCapi.H because they don't get implemented in the dll, client only functions in ecmdClientCapi.C
my @ignores = qw( ecmdLoadDll ecmdUnloadDll ecmdCommandArgs ecmdQueryTargetConfigured main);
my $ignore_re = join '|', @ignores;

# These are functions that should not be auto-gened into ecmdClientCapiFunc.C hand created in ecmdClientCapi.C
my @no_gen = qw( ecmdQueryConfig ecmdQuerySelected );
my $no_gen_re = join '|', @no_gen;

my @dont_flush_sdcache = qw( Query Cache Output Error Spy );
my $dont_flush_sdcache_re = join '|', @dont_flush_sdcache;
 
my $printout;
my @enumtable;

open IN, "${curdir}/ecmdClientCapi.H" or die "Could not find ecmdClientCapi.H: $!\n";

open OUT, ">${curdir}/ecmdDllCapi.H" or die $!;
print OUT "/* The following has been auto-generated by makedll.pl */\n";

print OUT "#ifndef ecmdDllCapi_H\n";
print OUT "#define ecmdDllCapi_H\n\n";

print OUT "#include <inttypes.h>\n";
print OUT "#include <vector>\n";
print OUT "#include <string>\n";
print OUT "#include <ecmdStructs.H>\n";
print OUT "#include <ecmdReturnCodes.H>\n";
print OUT "#include <ecmdDataBuffer.H>\n\n\n";

print OUT "extern \"C\" {\n\n";

print OUT "/* Dll Common load function - verifies version */\n";
print OUT "int dllLoadDll (const char * i_version, int debugLevel);\n";
print OUT "/* Dll Specific load function - used by Cronus/GFW to init variables/object models*/\n";
print OUT "int dllInitDll ();\n\n";
print OUT "/* Dll Common unload function */\n";
print OUT "int dllUnloadDll ();\n";
print OUT "/* Dll Specific unload function - deallocates variables/object models*/\n";
print OUT "int dllFreeDll();\n\n";
print OUT "/* Dll Common Command Line Args Function*/\n";
print OUT "int dllCommonCommandArgs(int*  io_argc, char** io_argv[]);\n";
print OUT "/* Dll Specific Command Line Args Function*/\n";
print OUT "int dllSpecificCommandArgs(int*  io_argc, char** io_argv[]);\n\n";


#parse file spec'd by $ARGV[0]
while (<IN>) {

    if (/^(int|std::string|void)/) {
	
	next if (/$ignore_re/o);

	my $type_flag = $INT;
	$type_flag = $VOID if (/^void/);
	$type_flag = $STRING if (/^std::string/);

	chomp; chop;  
	my ($func, $args) = split /\(|\)/ , $_;

	my ($type, $funcname) = split /\s+/, $func;
	my @argnames = split /,/ , $args;

        #remove the default initializations
        foreach my $i (0..$#argnames) {
            if ($argnames[$i] =~ /=/) {
              $argnames[$i] =~ s/=.*//;
            }
        }
        $" = ",";


        my $enumname;
        my $orgfuncname = $funcname;

        if ($funcname =~ /ecmd/) {

           $funcname =~ s/ecmd//;

           $enumname = "ECMD_".uc($funcname);

           $funcname = "dll".$funcname;
        }
        else {

           $enumname = "ECMD_".uc($funcname);
           $funcname = "dll".ucfirst($funcname);
        }

	push @enumtable, $enumname;

        print OUT "$type $funcname(@argnames); \n\n";

	next if (/$no_gen_re/o);

        $printout .= "$type $orgfuncname(@argnames) {\n\n";

	
	$printout .= "  $type rc;\n\n" unless ($type_flag == $VOID);


        #Put the debug stuff here
        if (!($orgfuncname =~ /ecmdOutput/)) {
          $printout .= "#ifndef ECMD_STRIP_DEBUG\n";
          $printout .= "  if (ecmdClientDebug > 1) {\n";
          $printout .= "    std::string printed = \"ECMD DEBUG ($orgfuncname) : Entering\\n\"; ecmdOutput(printed.c_str());\n";
          $printout .= "  }\n";
          $printout .= "#endif\n\n";
        }

	unless (/$dont_flush_sdcache_re/o) {
	    $printout .= "  int flushrc = ecmdFlushRingCache();\n";
	    $printout .= "  if (flushrc) {\n";
	    if ($type_flag == $STRING) {
		$printout .= "     return ecmdGetErrorMsg(flushrc);\n";
	    }
	    elsif ($type_flag == $INT) {
		$printout .= "     return flushrc;\n";
	    }
	    else { #type is VOID
		$printout .= "     return;\n";
	    }

	    $printout .= "  }\n\n";
	}
	
	$printout .= "#ifdef ECMD_STATIC_FUNCTIONS\n\n";

	$printout .= "  rc = " unless ($type_flag == $VOID);

	$" = " ";
       
	if ($type_flag == $VOID) {
	    $printout .= "  ";
	}

	$printout .= $funcname . "(";

	my $argstring;
	my $typestring;
	foreach my $curarg (@argnames) {

	    my @argsplit = split /\s+/, $curarg;

	    my @typeargs = @argsplit[0..$#argsplit-1];
	    $tmptypestring = "@typeargs";

	    my $tmparg = $argsplit[-1];
	    if ($tmparg =~ /\[\]$/) {
		chop $tmparg; chop $tmparg;
		$tmptypestring .= "[]";
	    }

	    $typestring .= $tmptypestring . ", ";
	    $argstring .= $tmparg . ", ";
	}

	chop ($typestring, $argstring);
	chop ($typestring, $argstring);

	$printout .= $argstring . ");\n\n";
	    
	$printout .= "#else\n\n";
	
	
	$printout .= "  if (dlHandle == NULL) {\n";
	if ($type_flag == $STRING) {
	    $printout .= "     return \"ECMD_DLL_UNINITIALIZED\";\n";
	}
	elsif ($type_flag == $INT) {
	    $printout .= "     return ECMD_DLL_UNINITIALIZED;\n";
	}
	else { #type is VOID
	    $printout .= "     return;\n";
	}

	$printout .= "  }\n\n";

	$printout .= "  if (DllFnTable[$enumname] == NULL) {\n";
	$printout .= "     DllFnTable[$enumname] = (void*)dlsym(dlHandle, \"$funcname\");\n";

	$printout .= "     if (DllFnTable[$enumname] == NULL) {\n";

        $printout .= "       fprintf(stderr,\"$funcname : Unable to find $funcname function, must be an invalid DLL - program aborting\\n\"); \n";
        $printout .= "       exit(ECMD_DLL_INVALID);\n";

	$printout .= "     }\n";
	
	$printout .= "  }\n\n";

	$printout .= "  $type (*Function)($typestring) = \n";
	$printout .= "      ($type(*)($typestring))DllFnTable[$enumname];\n\n";

	$printout .= "  rc = " unless ($type_flag == $VOID);
	$printout .= "   (*Function)($argstring);\n\n" ;
	
	$printout .= "#endif\n\n";


        #Put the debug stuff here
        if (!($orgfuncname =~ /ecmdOutput/)) {
          $printout .= "#ifndef ECMD_STRIP_DEBUG\n";
          $printout .= "  if (ecmdClientDebug > 1) {\n";
          $printout .= "    std::string printed = \"ECMD DEBUG ($orgfuncname) : Exiting\\n\"; ecmdOutput(printed.c_str());\n";
          $printout .= "  }\n";
          $printout .= "#endif\n\n";
        }

	$printout .= "  return rc;\n\n" unless ($type_flag == $VOID);

	$printout .= "}\n\n";

    }

}
close IN;

print OUT "} //extern C\n\n";
print OUT "#endif\n";
print OUT "/* The previous has been auto-generated by makedll.pl */\n";

close OUT;  #ecmdDllCapi.H

open OUT, ">${curdir}/ecmdClientEnums.H" or die $!;

print OUT "/* The following has been auto-generated by makedll.pl */\n\n";
print OUT "#ifndef ecmdClientEnums_H\n";
print OUT "#define ecmdClientEnums_H\n\n";
print OUT "#ifndef ECMD_STATIC_FUNCTIONS\n";


push @enumtable, "ECMD_COMMANDARGS"; # This function is handled specially because it is renamed on the other side

push @enumtable, "ECMD_NUMFUNCTIONS";
$" = ",\n";
print OUT "/* These are used to lookup cached functions symbols */\n";
print OUT "typedef enum {\n@enumtable\n} ecmdFunctionIndex_t;\n\n";
print OUT "#endif\n\n";
print OUT "#endif\n";
$" = " ";

close OUT;  #ecmdClientEnums.H

open OUT, ">${curdir}/ecmdClientCapiFunc.C" or die $!;

print OUT "/* The following has been auto-generated by makedll.pl */\n\n";
print OUT "#include <stdio.h>\n\n";
print OUT "#include <ecmdClientCapi.H>\n";
print OUT "#include <ecmdClientEnums.H>\n";
print OUT "#include <ecmdDllCapi.H>\n\n\n";
print OUT "#ifndef ECMD_STATIC_FUNCTIONS\n";
print OUT "\n#include <dlfcn.h>\n\n";

print OUT "void * dlHandle = NULL;\n";
print OUT "void * DllFnTable[ECMD_NUMFUNCTIONS];\n";

print OUT "#endif\n\n\n";

print OUT "#ifndef ECMD_STRIP_DEBUG\n";
print OUT "extern int ecmdClientDebug;\n";
print OUT "#endif\n\n\n";



print OUT $printout;

print OUT "/* The previous has been auto-generated by makedll.pl */\n";

close OUT;  #ecmdClientCapiFunc.C
