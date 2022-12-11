#!/bin/bash

init()
{
	legacy_compiler=$1
	ruc_vm=$2
	test_dir=./tests 

	wait_for=1

	pass=0
	fail=0
	timeout=0

	ulimit -s 81920
}

build()
{
	if [[ $legacy_compiler = "--build" ]] ; then
		sh ./scripts/build_legacy.sh
		legacy_compiler=../compiler/ruc
		ruc_vm=../vm/ruc-vm
	fi

	cd src
	`$legacy_compiler main.ruc>/dev/null 2>/dev/null`
	if ! [[ $? -eq 0 ]]; then
		exit 21
	fi
	cd ..
}

test()
{
	cd $test_dir
	cp ../src/keywords.txt keywords.txt
	for code in ./*.c
	do
		cp $code main.ruc
		out=`timeout $wait_for $ruc_vm ../src/export.txt >$code.out 2>$code.out`

		case $? in
			0)
				echo -e "\x1B[1;32m build passing \x1B[1;39m: $code"
				let pass++
				;;
			124)
				echo -e "\x1B[1;34m build timeout \x1B[1;39m: $code"
				let timeout++
				;;
			*)
				echo -e "\x1B[1;31m build failing \x1B[1;39m: $code"
				let fail++
				;;
		esac
	done


	if ! [[ -z $full_out ]] ; then
		echo
	fi

	echo -e "\x1B[1;39m pass = $pass, fail = $fail, timeout = $timeout"
}

main()
{
	cd `dirname $0`/..
	init $@

	build
	test

	if [[ fail != 0 || timeout != 0 ]] ; then
		exit 1
	fi

	exit 0
}

main $@
