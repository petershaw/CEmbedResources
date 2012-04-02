#!/usr/bin/perl

# 
# File:   c_builder.pl
# Author: Kris Wolff
# Twitter: @peter_shaw
#
# Created on March, 2012
# 
# 
# ----------------------------------------------------------------------------
# / embed resources in a static-c-library                                    /
#

use warnings;

use Cwd;
use Digest::MD5 qw(md5 md5_hex md5_base64);
use MIME::Base64::Perl;
use Data::Dumper;

my $directory;
my $library;
my $options;
my $outputfile;

print "************************************************************\n";
print "* c_builder.pl    - convert a directory                    *\n";
print "*                   into a .c/.h implementation            *\n";
print "*                   and build a static-library to embet in *\n";
print "*                   in your project.                       *\n"; 
print "************************************************************\n";
print "\n";

($directory, $library) = @ARGV;
if($ARGV[2]){
    $options = $ARGV[2];
}
if(!$directory || $directory eq ''){
    print "Use this tool with at least the argument of the directory\n";
    print "html_builder.pl <directory> [libraryname] [options]\n";
    die();
}
chomp($directory);
if($library){
    chomp($library);
}
if(!$library || $library eq ''){
    my $curTimeStamp = time;
    $library = 'libcembedresources_'.$curTimeStamp.".a";
}

if(! -d $directory){
    print "The directory '". $directory ."' does not exists\n";
    die();
}

my $workdir = getcwd;
chdir($directory);
my $result_c;
my $result_h;
my $tepl_header;
my $tepl_implementation;
my @funcs;

my $date = localtime(time);

processDirectory('.');
chdir($workdir);

print "\n";
print "************************************************************\n";
print "*** Generate files                                       ***\n";
print "\n";

my $anzFuncs = @funcs;
print "Read ". $anzFuncs ." files.\n";
print "Write header file...";
print " open template...";
open(TEPL, "<". $workdir."/templates/header") or die("Can not read template for headerfile.\n");
    $result_h = join("", <TEPL>);
close(TEPL);
print " replacing...";
$result_h =~ s/(NUM_OF_RESOURCES) \(([0-9]+)\)/$1 \($anzFuncs\)/sg;
print " save!\n";
open(HEADER, ">". $workdir."/build/cer_resourcebundle.h") or die("can not write headerfile.\n");
    print HEADER $result_h;
close(HEADER);

print "Write implementation file...";
print " open template...";
open(TEPL, "<". $workdir."/templates/implementation") or die("Can not read template for implementation.\n");
    $result_c = join("", <TEPL>);
close(TEPL);
print " parsing...";
my %TEPL;
while($result_c =~ /\<loop name=\"([a-z]+)\"\>(.+?)(\<\/loop\>)/isg){
    $TEPL{$1} = $2;
}
#$result_c =~ s/\<loop name=\"([a-z]+)\"\>(.+?)(\<\/loop\>)/____$1____/isg;
print "construct...\n";
my $funccount = 0;
my %replaceWith;
foreach my $f(@funcs){
    $funccount = $funccount +1;
    my $construct = $TEPL{'construct'};
    print "\t". $f->{'resourceName'} ."\n";
    $construct =~ s/_add_resouce_N/_add_resouce_$funccount/sg;
    $construct =~ s/\*content\s+=\s+\"\"/*content = \"\\\n$f->{'content'}\"/sg;
    $construct =~ s/resourceName\s+=\s+\"\"/resourceName = \"$f->{'resourceName'}\"/sg;
    $construct =~ s/resourceSize\s+=\s+0/resourceSize = \"$f->{'resourceSize'}\"/sg;
    $construct =~ s/resourceIdentifyer\s+=\s+\"00000000000000000000000000000000\"/resourceIdentifyer = \"$f->{'resourceIdentifyer'}\"/sg;
    $replaceWith{'construct'} .= $construct;
}
print "add... ";
for(my $i=1;$i<=$funccount;$i++){
    my $add = $TEPL{'add'};
    print $i." ";
    $add =~ s/_add_resouce_N/_add_resouce_$i/sg;
    $replaceWith{'add'} .= $add;
}
print "\n";
print "Write implementation file...\n";
$result_c =~ s/\<loop name=\"([a-z]+)\"\>(.+?)(\<\/loop\>)/$replaceWith{$1}/isg;
open(IMPL, ">". $workdir."/build/cer_resourcebundle.c") or die("can not write implementation file.\n");
print IMPL $result_c;
close(IMPL);

print "Write identifyer index...\n";
open(IDX, ">". $workdir."/build/identifyer.idx") or die("Can not write identifyer index.\n");
    print IDX "THIS IS A AUTOMATED GENERATED INDEX OF FILE THAT ARE INCLUDED IN THIS RESOURCE PACKAGE\n";
    print IDX "--------------------------------------------------------------------------------------\n";
    print IDX "\n";
    foreach my $f(@funcs){
        print IDX $f->{'resourceIdentifyer'}." -> ". $f->{'resourceName'} ." (". ($f->{'resourceSize'} /1024) .") kb)\n";
    }
close(IDX);

print "\n";
print "************************************************************\n";
print "*** Build phase                                          ***\n";
print "\n";

chdir($workdir."/build");

print "compile... ";
my $gccOut = `gcc -c -I. -bundle CEmbedResources.c cer_resourcebundle.c base64.c`;
print $gccOut;

print " archive... ";
my $arOut = `ar -cvq $library CEmbedResources.o cer_resourcebundle.o base64.o`;

print "\n";
print "************************************************************\n";
print "Make Package.\n";
my $packName = $library;
$packName =~ s/\.a$//sg;
$outTar = `tar -cvHf $packName.tar CEmbedResources.h cer_resourcebundle.h $library`;
if($options =~ /-addimplementation/isg){
    print "Add implementation into tarball\n";
    $outTar = `tar -rf $packName.tar cer_resourcebundle.c`;
    print $outTar;   
}
`mv $library $packName.tar identifyer.idx ../dist`;

unlink("cer_resourcebundle.c"); 
unlink("cer_resourcebundle.h"); 
unlink("cer_resourcebundle.o");
unlink("CEmbedResources.o");
unlink("base64.o");

print "\n";
print "************************************************************\n";
print "done.\n";

# ------------------------------------------------------------------------------

sub processDirectory {
    my $directory = shift;
    my $result;
    
    opendir(my $DIR, $directory) or die "Can't open directory: ". $directory ."\n". $! ."\n";
    while( my $entry = readdir $DIR ){
        next if $entry =~ /^\./;
        my $fullpath = $directory."/".$entry;
        if(-d $fullpath){
            &processDirectory($fullpath);
        } else {
            if(-e $fullpath){
                print "Precessing $fullpath\n";
                my %info;
                my $fullpath_cut = $fullpath; # remove first dot taht it matched with uri
                $fullpath_cut =~ s/^\.//isg;
                my $identifyer = uc(md5_hex($fullpath_cut));
                
                my $content;
                my $filesize = -s $fullpath;
                open(DAT, $fullpath) or die("Can't open file: ". $fullpath .".\n". $! ."\n");
                    $content = join('', <DAT>);
                close(DAT);
                my $encode = encode_base64($content);
                $encode =~ s/\n/\\\n/isg;

                $info{'resourceName'} = $fullpath_cut;
                $info{'resourceIdentifyer'} = $identifyer;
                $info{'resourceSize'} = $filesize;
                $info{'content'} = $encode;
                push(@funcs, {%info});
            }
        }
    }
    closedir($DIR);
    return $result;
}