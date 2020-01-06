#!/bin/bash
tar xzvf testdata.tar.gz
find `pwd` -iname "2019*Mobile*" | sort > dir-Mobile.txt
find `pwd` -iname "2019*Unicom*" | sort > dir-Unicom.txt
find `pwd` -iname "2019*Teleco*" | sort > dir-Telecom.txt
